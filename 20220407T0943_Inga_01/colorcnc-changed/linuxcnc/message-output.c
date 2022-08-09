
// included from colorcnc.c



int sendOutputData(data_t* port, long period)
{
       __u32 tmp_data;
	__u8 num_records_to_write;
	
	int rate, maxrate = MAX_STEP_RATE;
	double fmax;
	int stepspace_ticks = stepgen_stepspace/COLORCNC_SPEED_NS;
	int steplen_ticks = stepgen_steplen/COLORCNC_SPEED_NS;
	int dirtime_ticks = stepgen_dirtime/COLORCNC_SPEED_NS;

	pack_mainW_t send_payload;

	__u8 wb_wr_buffer[MESSAGE_BUFFER_SIZE];


        if(steplen_ticks > TMAX)
        {
            steplen_ticks = TMAX;
            rtapi_print_msg(RTAPI_MSG_ERR,"Requested step length %dns decreased to %dns due to hardware limitations\n", stepgen_steplen, TMAX * COLORCNC_SPEED_NS);
            stepgen_steplen = TMAX * COLORCNC_SPEED_NS;
        }

        if(dirtime_ticks > TMAX)
        {
            dirtime_ticks = TMAX;
            rtapi_print_msg(RTAPI_MSG_ERR,"Requested direction change time %dns decreased to %dns due to hardware limitations\n", stepgen_dirtime, TMAX * COLORCNC_SPEED_NS);
            stepgen_dirtime = TMAX * COLORCNC_SPEED_NS;
        }

        // Speed limits come from several sources
        // First limit: step waveform timings
        fmax = 1. / COLORCNC_SPEED / (2 + steplen_ticks + stepspace_ticks);
        port->fmax_limit1=fmax;
        // Second limit: highest speed command
      /*  if(fmax > COLORCNC_FREQ / (2<<speedrange))
            {fmax = COLORCNC_SPEED * (2<<speedrange);
            port->fmax_limit2=fmax;
            }
        // Third limit: max sign-extenable counts per period
        if(fmax > MAXDELTA / fperiod / (1<<speedrange))
            {fmax = MAXDELTA / fperiod / (1<<speedrange);
            port->fmax_limit3=fmax;
            }
        */
        for(int i=0; i<steppers_count; i++) 
	{
            //fetch new position command from motion control task
            double new_position_cmd = stepgen_position_cmd(i);
            //calculate required speed
            double v;
            if (port->stepgen_mode[steppers_count])
             {v = new_position_cmd - port->old_position_cmd[i];}
            else
             {v = stepgen_velocity_cmd(i);}
            //estimate position error
            double est_err = stepgen_position_fb(i) + port->old_velocity_cmd[i] * fperiod - new_position_cmd;
            double actual_max;
            //absolute value of scale
            double scale_abs = fabs(stepgen_scale(i));

            v = v - debug_2 * est_err / fperiod;
            if(v > 0) v = v + .5/scale_abs;
            else if(v < 0) v = v - .5/scale_abs;

            //store position command
            port->old_position_cmd[i] = new_position_cmd;
            //store (corrected?) velocity command
            port->old_velocity_cmd[i] = v;
            actual_max = fmax / scale_abs;
            //implement boundaries on velocity
            if(stepgen_maxvel(i) < 0) stepgen_maxvel(i) = -stepgen_maxvel(i);
            if(stepgen_maxvel(i) != 0 && stepgen_maxvel(i) > actual_max) 
	    {
                static int message_printed[6] = {0,0,0,0,0,0};
                //notify user about limit of velocity
                if(!message_printed[i]) {
                    rtapi_print_msg(RTAPI_MSG_ERR,
                        "Requested step rate %dHz decreased to %dHz due to hardware or timing limitations\n",
                        (int)(stepgen_maxvel(i) * scale_abs),
                        (int)(fmax));
                    message_printed[i] = 1;
                }
                stepgen_maxvel(i) = actual_max;
            }

            if(stepgen_maxvel(i) == 0) {
                if(v < -actual_max) v = -actual_max;
                if(v > actual_max) v = actual_max;
            } else {
                if(v < -stepgen_maxvel(i)) v = -stepgen_maxvel(i);
                if(v > stepgen_maxvel(i)) v = stepgen_maxvel(i);
            }
            //velocity expressed in parts of 1
            rate = v * stepgen_scale(i) * ONE * COLORCNC_SPEED / (1<<speedrange);

            if(rate > maxrate) rate = maxrate;
            if(rate < -maxrate) rate = -maxrate;

            if(!(*(port->enable_dr))) rate = 0;

            send_payload.steppers[i].velocity=rate;
            port->stepgen_rate[i]=rate;

        }

        send_payload.steptime=steplen_ticks;
        send_payload.dirtime=dirtime_ticks;

        for (int i=0;i<pwm_count;i++)
        {
            send_payload.pwm[i].pwm_period=*(port->pwm_period[i]);
            send_payload.pwm[i].pwm_width=*(port->pwm_width[i]);
        }

        __u32 tmp=1;
        send_payload.gpios_out=0;
        for (int i=0;i<do_count;i++)
        {
            if (port->digital_out[i]) {
            send_payload.gpios_out|= *(port->digital_out[i])?tmp:0;
            }
            tmp<<=1;
        }
 
        send_payload.apply_time=port->board_wallclock+(BOARD_CLK*SERVO_PERIOD)/1000000ULL;

        send_payload.steppers_ctrlword=0;
        send_payload.steppers_ctrlword|=*(port->enable_dr)?1:0;

	/***** FILL send packet **************/
	
        memset((void*)wb_wr_buffer,0,sizeof(wb_wr_buffer));
        //fill etherbone packet headers
        eb_fill_readwrite32(wb_wr_buffer, 0, 0x0, 1);

        num_records_to_write=1+(sizeof(send_payload))/sizeof(__u32);  // Watchdog extra

        wb_wr_buffer[10]=num_records_to_write;   //number of registers to write + (slave) start address
        wb_wr_buffer[11]=0;   //number of registers to write + (slave) start address

        tmp_data=htobe32(csr_addr(CSR_WRITE_START)); //fill addresses to start writing
        memcpy((void*)&wb_wr_buffer[12],(void*)&tmp_data,4);
        //store stepgen parameters, apply time and gpios
        __u64 tmp_data64;
        __u32* tmp_ptr=(__u32*)&send_payload;

	// Watchdog first
        tmp_data=htobe32( WATCHDOG_MSEC* 50000); //watchdog, 100 ms, decrease @ 50MHz, 
        memcpy((void*)&wb_wr_buffer[16],(void*)&tmp_data,4);
	
        for (int i=0;i<num_records_to_write-1;i++)
        {
            if (i==((int)(((void*)&send_payload.apply_time)-((void*)&send_payload)))/sizeof(__u32))
            {
                tmp_data64=htobe64(*(__u64*)(&tmp_ptr[i])); //fill data to write
                memcpy((void*)&wb_wr_buffer[16+4+i*4],(void*)&tmp_data64,8);
                i++;
            }
            else
            {
                tmp_data=htobe32(tmp_ptr[i]); //fill data to write
                memcpy((void*)&wb_wr_buffer[16+4+i*4],(void*)&tmp_data,4);
            }
        }
	
	int debug_force_output_data_dump = 0;
//	if (send_payload.steppers[0].velocity != 0) {
//	  fprintf(stderr,"send stp: velo %d, dirtime %u, steptime %u\n", 
//	    send_payload.steppers[0].velocity, send_payload.dirtime, send_payload.steptime);
//	  debug_force_output_data_dump = 1;
//	}

/* debug */
dump_buffer(wb_wr_buffer, num_records_to_write*4+16, "colorcnc debug: sending output data", debug_force_output_data_dump);


    // #################  TRANSMIT DATA  ###############################################################
        int res=eb_send(device_data->eb, wb_wr_buffer, num_records_to_write*4+16);
    // #################################################################################################

        if (res < 0) {
            rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: cannot send output data: %s\n", strerror(errno));
	    return -1;
	}
	
	return res;
}
