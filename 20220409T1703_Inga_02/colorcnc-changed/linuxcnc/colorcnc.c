/*
 * V0.1 Initial release  by romanetz
 * V 0.2 Modified by inga: added magic for version check, port count check, watchdog
 * V 0.3 enforce packet separation as colorlight tends to crash when packet come too close,
 *       added statistics and performance optimizations
 * V 0.4 the step counts where generated wrong by the driver. 
 *       The input processing could terminate unexpectedly
 *       Some cleanup.
*/

#include "rtapi_ctype.h"	/* isspace() */
#include "rtapi.h"			/* RTAPI realtime OS API */
#include "rtapi_app.h"		/* RTAPI realtime module decls */
#include "hal.h"			/* HAL public API decls */
#include <linux/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include "colorcnc.h"
#include "etherbone.h"
#include "rtapi_math64.h"
#include "rtapi_math.h"

#if defined(__FreeBSD__)
#include <sys/endian.h>
#else
#include <endian.h>
#endif
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>  // inga timestamp


//#define Max_pause 16777215

/* module information */
MODULE_AUTHOR("romanetz");
MODULE_DESCRIPTION("colorcnc board driver");
MODULE_LICENSE("GPL");


// --------------------------------------

// Inga 4.4.22
//
// added functionality
//
// support for a watchdog on the colorlight card. Stepgen will be disabled when the watchdog expireds.
//
// testing colorlight config
// the colorlight will send a magic and the number of inputs, outputs, stepgens and pwm configured.
// the driver will not start it number doesnt match these numbers
// TODO: use these numbers in the driver.
//
// packet separation
// the litex core, which is used for ethernet communication on the colorlight card, seems to 
// require a time distance between two incoming packet from the net. 
// Packets too close to each other ( seen 9 usec) can make the card unresponsive.
// The driver therefore waits until tx data is really send onto the wire and wais some extra time.
//
// use a different realtime scheduler in for the driver
// the default schedure is SCHED_FIFO. This scheduler seems to be responsible for rare delays 
// of a millisecond or more. Using SCHED_RR is better and gives stable execution times below 400 us.
//
// reorder of data send and receive.
// the driver will now first check for pending packets in the output queue (sidecount in statistic)
//   then send a request to receive input data
//   then received the data
//   and last but not least send the output data
// the new order avoids sending two packets close to each other. 
// Normally, the write packet has 800 us to leave the queue- If not, the next cycle will wait until the queue is cleared.
// The avarage process time for one cycle is now down to 160 usec on a RPI4. 

// I have split the source into multiple files. 
// colorcnc.c includes these c files as I have no idea how to otherwise compile multiple C files into one .so



// some option for testing


#define STAT_COUNT 100000   // provide statitics every x calls. comment to disable.

int maxDumps = 0;  // debug, set > 0 to see the first x packets send and recvd

#define CHANGE_SCHED  // use Round Robin scheduler. avg 157us < 400us on an otherwise idle RPI4. 

//#define WANT_CORE_FILE

// ----------------------------------------

//#define UDP_RAW  // use raw udp sockets (test)

#ifdef UDP_RAW
#include "udptest.h"
#endif

#ifdef STAT_COUNT
#include "stat.h"
static struct runtimeStat stat;
#endif


/***********************************************************************
*                STRUCTURES AND GLOBAL VARIABLES                       *
************************************************************************/
static char *board_ip="192.168.1.50";
static unsigned char board_mac_addr[6] = { 0x10, 0xe2, 0xd5, 0x00, 0x00, 0x00};
static char *board_network_interface = "eth0"; // unfortunately required for setting static arp

RTAPI_MP_ARRAY_STRING(board_ip, 1, "ip address of ethernet board");

static int speedrange=0;
RTAPI_MP_INT(speedrange, "Speed range 0..3");

#define COLORCNC_SPEED_NS (20)
#define COLORCNC_SPEED    (COLORCNC_SPEED_NS * 1e-9)
#define COLORCNC_FREQ     (1e9 / COLORCNC_SPEED_NS)
#define TMAX           ((1<<10)-1)

#define W 32
#define F 30
//#define MODULO ((1LLU<<(W+F))-1)
#define MASK ((1LLU<<(W+F))-1)
#define MAXDELTA (MASK/2)

#define ONE (1<<F)
#define MAX_STEP_RATE (1<<(F-1))

static data_t *device_data;

/* other globals */
static int comp_id;		/* component ID */
static int num_ports;		/* number of ports configured */



time_t hStartTime = 0;
time_t hLastReceiveTime = 0;
long hErrorCountTotal = 0;
long hErrorCountCont = 0;
#define MAX_ERROR_TOTAL 5000
#define MAX_ERROR_CONT 5



/***********************************************************************
*                  LOCAL FUNCTION DECLARATIONS                         *
************************************************************************/
/* These is the functions that actually do the I/O
   everything else is just init code
*/
static void update_port(void *arg, long period);



/***********************************************************************
*                       Include support modules                        *
************************************************************************/

// Include C coding. Not nice but I have no other way to use several C modules for one hal module.


#include "eb_functions.c"

#include "util.c"

#ifdef UDP_RAW
// include c file. How to split into real module with halcompile?
#include "udptest.c"
#endif

#ifdef STAT_COUNT
#include "stat.c"
#endif


/***********************************************************************
*                       INIT AND EXIT CODE                             *
************************************************************************/

int rtapi_app_main(void)
{
    char name[HAL_NAME_LEN + 1];
    int retval,n,i,r ;


#ifdef WANT_CORE_FILE
// inga 4.5.22 allow core files
// also required: 
// sudo -i 
//   echo 2 >/proc/sys/fs/suid_dumpable
    struct rlimit lim;
    lim.rlim_cur = RLIM_INFINITY;
    lim.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &lim);
    prctl(PR_SET_DUMPABLE, 1);
#endif
    
    num_ports = 1;
    n = 0;

    /* STEP 1: initialise the driver */
    comp_id = hal_init(driver_NAME);
    if (comp_id < 0) {
    	rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: hal_init() failed\n");
    	goto fail0;
    }

    /* STEP 2: allocate shared memory for to_hal data */
    int bytesNeeded = num_ports * sizeof(data_t);
    device_data = hal_malloc(num_ports * sizeof(data_t));
    if (device_data == 0) {
    	rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: hal_malloc() failed\n");
		r = -1;
		goto fail0;
    }
    

//###################################################
///// INIT ETH BOARD ; OPEN SOCKET
//###################################################
    device_data->eb = eb_connect(board_ip, "1234", 1);

    if (!device_data->eb)
    {
        rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: failed to connect to board\n");
        goto fail1;
    }

  	device_data->first_run = 1;
	//device_data->block = 0; //local block
//######################################################
//######### EXPORT SIGNALS, PIN, FUNCTION
//######################################################

//////////////////////
//////////////////////
//////////////////////
/* Export IO pin's */

//########################## INPUTS ###############################
    /* export write only HAL pin's for the input bit */
    for ( i=0; i<di_count;i++) {
		retval = hal_pin_bit_newf(HAL_OUT, &(device_data->digital_in[i]),comp_id, "colorcnc.%d.pins.pin-%02d-in", 1, i);
      	if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: ERROR: port %d var export failed with err=%i\n", i + 1,retval);
			r = -1;
			goto fail1;
		}

      	retval = hal_pin_bit_newf(HAL_OUT, &(device_data->digital_in_n[i]),comp_id, "colorcnc.%d.pins.pin-%02d-in-n", 1, i);
      	if (retval < 0) {
        	rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: ERROR: port %d var export failed with err=%i\n", i + 1,retval);
        	r = -1;
        	goto fail1;
      	}

    }


//########################## OUTPUTS ###############################
    /* export read only HAL pin's for the output bit */
    for ( i=0; i<do_count;i++) {
		retval = hal_pin_bit_newf(HAL_IN, &(device_data->digital_out[i]),comp_id, "colorcnc.%d.pins.pin-%02d-out", 1, i);

		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: port %d var export failed with err=%i\n", i + 1,retval);
			r = -1;
			goto fail1;
		}
    }


///////////////////////////
///////////////////////////
///////////////////////////
/* export read only HAL pin's for the control bit */

	retval = hal_pin_bit_newf(HAL_IN, &(device_data->enable_dr),comp_id, "colorcnc.%d.enable_drive", 1);

	if (retval < 0) {
		rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: enable_dr var export failed with err=%i\n",retval);
		r = -1;
		goto fail1;
	}

	retval = hal_pin_u32_newf(HAL_OUT, &(device_data->board_wallclock_lsb),comp_id, "colorcnc.%d.board_wallclock_lsb", 1);

	if (retval < 0) {
		rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: board_wallclock var export failed with err=%i\n",retval);
		r = -1;
		goto fail1;
	}

	retval = hal_pin_u32_newf(HAL_OUT, &(device_data->board_wallclock_msb),comp_id, "colorcnc.%d.board_wallclock_msb", 1);

	if (retval < 0) {
		rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: board_wallclock var export failed with err=%i\n",retval);
		r = -1;
		goto fail1;
	}


	/*retval = hal_pin_bit_newf(HAL_OUT, &(device_data->WD_lock),comp_id, "colorcnc.%d.WD_lock", 1);

	if (retval < 0) {
		rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: enable_dr var export failed with err=%i\n",retval);
		r = -1;
		goto fail1;
	}
*/
/////////////////////////////
/////////////////////////////
/////////////////////////////
/* export encoder signal */
/*
	for ( i=0; i<=(NUM_chanal-1);i++) {
		// encoder_count
		retval = hal_pin_float_newf(HAL_OUT, &(device_data-> enccounts[i]), comp_id, "colorcnc.%d.feedback.encoder%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err encoder=%i\n", retval);
			r = -1;
			goto fail1;
		}
        // encoder_scale
		retval = hal_pin_float_newf(HAL_IN, &(device_data-> encscale[i]), comp_id, "colorcnc.%d.feedback.enc_scale%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err enc_scale=%i\n", retval);
			r = -1;
			goto fail1;
		}
		*device_data-> encscale[i] = 1;

        // index
		retval = hal_pin_bit_newf(HAL_IO, &(device_data->index_en[i]),comp_id, "colorcnc.%d.feedback.index_en%01d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: port %d var export failed with err=%i\n", n + 1,retval);
			r = -1;
			goto fail1;
		}
        // encoder_velocity
		retval = hal_pin_float_newf(HAL_IN, &(device_data-> encvel[i]), comp_id, "colorcnc.%d.feedback.enc_vel%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err enc_velocity=%i\n", retval);
			r = -1;
			goto fail1;
		}
	}

*/

//////////////////////////////
/////////////////////////////
/////////////////////////////
	/* export control drive STEP_DIR*/

	for ( i=0; i<steppers_count;i++) {

       r = hal_pin_float_newf(HAL_IN, &(device_data->stepgen_position_cmd[i]), comp_id, "colorcnc.%d.stepgen.%01d.position-cmd", 1, i);
        if(r != 0) return r;
       r = hal_pin_float_newf(HAL_IN, &(device_data->stepgen_velocity_cmd[i]), comp_id, "colorcnc.%d.stepgen.%01d.velocity-cmd", 1, i);
        if(r != 0) return r;

        r = hal_pin_float_newf(HAL_OUT, &(device_data->stepgen_velocity_fb[i]), comp_id, "colorcnc.%d.stepgen.%01d.velocity-fb", 1, i);
        if(r != 0) return r;

        r = hal_pin_float_newf(HAL_OUT, &(device_data->stepgen_position_fb[i]), comp_id, "colorcnc.%d.stepgen.%01d.position-fb", 1, i);
        if(r != 0) return r;

        r = hal_pin_s32_newf(HAL_OUT, &(device_data->stepgen_counts[i]), comp_id, "colorcnc.%d.stepgen.%01d.counts", 1, i);
        if(r != 0) return r;

        r = hal_pin_bit_newf(HAL_IN, &(device_data->stepgen_enable[i]), comp_id, "colorcnc.%d.stepgen.%01d.enable", 1, i);
        if(r != 0) return r;

        r = hal_pin_bit_newf(HAL_IN, &(device_data->stepgen_reset[i]), comp_id, "colorcnc.%d.stepgen.%01d.reset", 1, i);
        if(r != 0) return r;

        r = hal_param_float_newf(HAL_RW, &(device_data->stepgen_scale[i]), comp_id, "colorcnc.%d.stepgen.%01d.scale", 1, i);
        if(r != 0) return r;
        device_data->stepgen_scale[i] = 1.0;

        r = hal_param_float_newf(HAL_RW, &(device_data->stepgen_maxvel[i]), comp_id, "colorcnc.%d.stepgen.%01d.maxvel", 1, i);
        if(r != 0) return r;
        device_data->stepgen_maxvel[i] = 0;

        r = hal_param_bit_newf(HAL_RW, &(device_data->stepgen_mode[i]), comp_id, "colorcnc.%d.stepgen.%01d.mode", 1, i);
        if(r != 0) return r;
        device_data->stepgen_mode[i] = 0;

        r = hal_param_s32_newf(HAL_RO, &(device_data->stepgen_rate[i]), comp_id, "colorcnc.%d.stepgen.%01d.rate", 1, i);
        if(r != 0) return r;
        

			/* feed back step/dir */
/*		retval = hal_pin_float_newf(HAL_OUT, &(device_data-> position[i]), comp_id, "colorcnc.%d.stepgen.position%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err stepgen=%i\n", retval);
			r = -1;
			goto fail1;
		}
*/

			/* DIRC step/dir */
	/*	retval = hal_pin_bit_newf(HAL_IN, &(device_data->invert_dir[i]), comp_id, "colorcnc.%d.step_dir.invert_dir%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err invert_dir=%i\n", retval);
			r = -1;
			goto fail1;
		}
		*device_data->invert_dir[i] = 0;
    */

	/*	retval = hal_pin_float_newf(HAL_OUT, &(device_data-> velocity[i]), comp_id, "colorcnc.%d.stepgen.velocity%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err stepgen velocity=%i\n", retval);
			r = -1;
			goto fail1;
		}
*/

    }

            r = hal_param_float_newf(HAL_RO, &(device_data->fmax_limit1), comp_id, "colorcnc.%d.stepgen.fmax_limit1", 1);
        if(r != 0) return r;
            r = hal_param_float_newf(HAL_RO, &(device_data->fmax_limit2), comp_id, "colorcnc.%d.stepgen.fmax_limit2", 1);
        if(r != 0) return r;
            r = hal_param_float_newf(HAL_RO, &(device_data->fmax_limit3), comp_id, "colorcnc.%d.stepgen.fmax_limit3", 1);
        if(r != 0) return r;
            r = hal_param_float_newf(HAL_RO, &(device_data->t_period_real), comp_id, "colorcnc.%d.t_period_real", 1);
        if(r != 0) return r;



	        retval = hal_param_bit_newf(HAL_RW, &(device_data->stepgen_step_polarity), comp_id, "colorcnc.%d.stepgen.step-polarity", 1);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err step-polarity=%i\n", retval);
			r = -1;
			goto fail1;
		}

		retval = hal_param_u32_newf(HAL_RW, &(device_data-> stepgen_steplen), comp_id, "colorcnc.%d.stepgen.steplen", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err steptime=%i\n", retval);
			r = -1;
			goto fail1;
		}

		retval = hal_param_u32_newf(HAL_RW, &(device_data-> stepgen_dirtime), comp_id, "colorcnc.%d.stepgen.dirtime", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err dirtime=%i\n", retval);
			r = -1;
			goto fail1;
		}

		for ( i=0; i<pwm_count;i++) {
			retval = hal_pin_u32_newf(HAL_IN, &(device_data-> pwm_width[i]), comp_id, "colorcnc.%d.pwm.width%d", 1, i);
			if (retval < 0) {
				rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err steptime=%i\n", retval);
				r = -1;
				goto fail1;
			}
			retval = hal_pin_u32_newf(HAL_IN, &(device_data-> pwm_period[i]), comp_id, "colorcnc.%d.pwm.period%d", 1, i);
			if (retval < 0) {
				rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err dirtime=%i\n", retval);
				r = -1;
				goto fail1;
			}
		}
		
/*	else if(device_data->PWM_board_num == 1)
{

	/////////////////////////////
	// export control drive PWM


	for ( i=0; i<=(NUM_chanal-1);i++) {
		retval = hal_pin_float_newf(HAL_IN, &(device_data-> dcontrol[i]), comp_id, "colorcnc.%d.PWM.dcontrol%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err dcontrol=%i\n", retval);
			r = -1;
			goto fail1;
		}

		retval = hal_pin_float_newf(HAL_IN, &(device_data-> outscale[i]), comp_id, "colorcnc.%d.PWM.out_scale%d", 1, i);
		if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR:  err out_scale=%i\n", retval);
			r = -1;
			goto fail1;
		}
	}

}
*/
//////////////////////////////
/////////////////////////////
/////////////////////////////
// Export TESTS signls
/*

	retval = hal_pin_u32_newf(HAL_OUT, &(device_data->test),comp_id, "colorcnc.%d.TEST", 1);
      	if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: ERR: port %d TEST var export failed with err=%i\n", n + 1,retval);
			r = -1;
			goto fail1;
		}
	retval = hal_pin_u32_newf(HAL_OUT, &(device_data->testi),comp_id, "colorcnc.%d.TEST_INT", 1);
      	if (retval < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: ERR: port %d TEST var export failed with err=%i\n", n + 1,retval);
			r = -1;
			goto fail1;
		}

	*device_data->testi = 0;
*/
////////////////////////////////////////
//// UPDATE  export function
////////////////////////////////////////

    /* STEP 4: export function */
    rtapi_snprintf(name, sizeof(name), "colorcnc.%d.update", n + 1);
    retval = hal_export_funct(name, update_port, device_data, 1, 0,comp_id);
    if (retval < 0) {
		rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: ERROR: port %d write funct export failed\n", n + 1);
		r = -1;
		goto fail1;
    }


    rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: installed driver for %d card(s)\n", num_ports);
    hal_ready(comp_id);
    
    return 0;


//####### ERROR ############

fail1:

	eb_disconnect(&device_data->eb);

fail0:
    hal_exit(comp_id);
return r;

}
//####### EXIT ############
void rtapi_app_exit(void)
{
	eb_disconnect(&device_data->eb );
	hal_exit(comp_id);
}


#undef fperiod
#define fperiod (period * 1e-9)
#undef stepgen_position_cmd
#define stepgen_position_cmd(i) (0+*(port->stepgen_position_cmd[i]))
#undef stepgen_velocity_cmd
#define stepgen_velocity_cmd(i) (0+*(port->stepgen_velocity_cmd[i]))
#undef stepgen_velocity_fb
#define stepgen_velocity_fb(i) (*(port->stepgen_velocity_fb[i]))
#undef stepgen_position_fb
#define stepgen_position_fb(i) (*(port->stepgen_position_fb[i]))
#undef stepgen_counts
#define stepgen_counts(i) (*(port->stepgen_counts[i]))
#undef stepgen_enable
#define stepgen_enable(i) (0+*(port->stepgen_enable[i]))
#undef stepgen_reset
#define stepgen_reset(i) (0+*(port->stepgen_reset[i]))
#undef stepgen_scale
#define stepgen_scale(i) (port->stepgen_scale[i])
#undef stepgen_maxvel
#define stepgen_maxvel(i) (port->stepgen_maxvel[i])
#undef stepgen_step_polarity
#define stepgen_step_polarity (port->stepgen_step_polarity)
#undef stepgen_steplen
#define stepgen_steplen (port->stepgen_steplen)
#undef stepgen_stepspace
#define stepgen_stepspace (port->stepgen_stepspace)
#undef stepgen_dirtime
#define stepgen_dirtime (port->stepgen_dirtime)
#undef debug_0
#define debug_0 (port->debug_0)
#undef debug_1
#define debug_1 (port->debug_1)
#undef debug_2
#define debug_2 (port->debug_2)
#undef debug_3
#define debug_3 (port->debug_3)




/**************************************************************
###############################################################
* REALTIME PORT WRITE FUNCTION                                *
###############################################################
**************************************************************/
void update_port_inner(void *arg, long period);


void update_port(void *arg, long period)
{
#ifdef WANT_CORE_FILE
	// inga 4.5.22 allow core files
	// somewhat difficult on suid programs
// also required: 
// sudo -i 
//   echo 2 >/proc/sys/fs/suid_dumpable
    struct rlimit lim;
    lim.rlim_cur = RLIM_INFINITY;
    lim.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &lim);
    prctl(PR_SET_DUMPABLE, 1);
#endif

//LL_DEBUG("enter update_port\n");


    if(((data_t*)arg)->first_run)
    {
#ifdef STAT_COUNT
        initMeasure("updatePort dura", &stat, STAT_COUNT, 1, 5000);
#endif
    }

#ifdef STAT_COUNT
    enterMeasure(&stat);
#endif
    
#ifdef CHANGE_SCHED	
    struct sched_param temp_params, orig_params;
    int orig_policy = 0;

    pthread_t this_thread = pthread_self();
    
    int ret = pthread_getschedparam(this_thread, &orig_policy, &orig_params);
    if (ret < 0) perror ("pthread_getschedparam failed!");
    
    temp_params.sched_priority = sched_get_priority_max(SCHED_RR);
    ret = pthread_setschedparam(this_thread, SCHED_RR, &temp_params);
    if (ret < 0) perror("cannot change priority and scheduler");
#endif


    update_port_inner(arg, period);


#ifdef CHANGE_SCHED
    ret = pthread_setschedparam(this_thread, orig_policy, &orig_params);
    if (ret < 0) perror("cannot change back priority and scheduler");
#endif

#ifdef STAT_COUNT
    exitMeasure(&stat);
#endif

//LL_DEBUG("exit update_port\n");

}


void update_port_inner(void *arg, long period){

    data_t *port = arg;
    int rc;
    
    if(port->first_run)
    {
	firstRun(port);
	port->first_run=0;
 
    }
    else
    {

        // inga, 01.04.22 : wait for the socket tx_queue to become empty. 
	// This is essential as the colorlight card crashes when two packets come close to each other.
	// This prevents crashes in the litex eth core. 
	// Also turn of mDNS request from linux to the colorlight card. (avahi-daemon)
	wait_for_tx_buffer_empty(port->eb);

	// REQUEST INPUT DATA
	rc = getInputData(port, period);
	if (rc < 0) {
		//hal_exit(comp_id);  // no hal_exit from realtime task
		rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: terminating after input error\n");
		exit(1);
	}

	// SEND OUTPUT DATA
	rc = sendOutputData(port, period);
	if (rc < 0) {
		//hal_exit(comp_id);
		rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: terminating after output error\n");
		exit(1);
	}
    }


}

// message handling in separate modules

#include "message-input.c"
#include "message-output.c"






