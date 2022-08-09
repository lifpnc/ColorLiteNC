
// included from colorcnc.c

void firstRun(data_t *port)
{
	__u8 wb_wr_buffer[MESSAGE_BUFFER_SIZE];
	__u8 wb_rd_buffer[MESSAGE_BUFFER_SIZE];
	hStartTime = time(NULL);

	//fill etherbone packet headers
	eb_fill_readwrite32(wb_wr_buffer, 0, 0x0, 1);
	wb_wr_buffer[11]=2;
	*(uint32_t*)&wb_wr_buffer[12]=htobe32(0); //own address
	*(uint32_t*)&wb_wr_buffer[16]=htobe32(csr_addr(CSR_WALLCLOCK_OFFSET)); //data address
	*(uint32_t*)&wb_wr_buffer[20]=htobe32(csr_addr(CSR_WALLCLOCK_OFFSET+4)); //data address



	// #################  TRANSMIT DATA  ###############################################################

	// check cnc magic 
	wb_wr_buffer[11] = 2;
	*(uint32_t*)&wb_wr_buffer[16]=htobe32(csr_addr(CSR_MAGIC_OFFSET)); //data address
	*(uint32_t*)&wb_wr_buffer[20]=htobe32(csr_addr(CSR_MAGIC_OFFSET+4)); //data address

	for ( int tryCount = 5*10, first=1; tryCount >= 0; --tryCount, first = 0)
	{

		
	    
	    if ( first || tryCount % 10 == 0) {
		eb_send(device_data->eb, wb_wr_buffer, 24);
	    }
	    
	    int rcMagic = eb_recv(device_data->eb, wb_rd_buffer, sizeof(wb_rd_buffer));

	    if ( rcMagic < 0) {
		if ( tryCount > 0) {
			usecSleep(10000);
			continue; // nochmal
		}
		fprintf(stderr,"\n\nERROR: colorcnc: cannot read magic from card. Terminating\n******\n\n");
		exit(0);
	    }
	    //dump_buffer(wb_rd_buffer, rcMagic, "Received initial data", 0);
	    
	    if (!check_magic(wb_rd_buffer, 16, 1))  // offset 16: First payload word
	    {
		    if ( tryCount > 0 ) continue; // probably missed initial packet that shows up here
		    fprintf(stderr, "\n\nERROR: colorcnc: the is no magic in this card, got %d, expected %d, exit!\n", wb_rd_buffer[16], MAGIC_CNC);
		    exit(1);
	    }
	    break;
	}
}


// #################  RECEIVE DATA  ###############################################################

int getInputData(data_t *port, long period)
{
	int requestSize = 0;
	int rc;
	
	rc = requestInputData(port, &requestSize);
        if ( rc >= 0) 
	{
	    rc = readInputData(port, period, requestSize);
	}
	return rc;
}

#define RD_BUFFER_HEADER_LEN              16
#define RD_BUFFER_STEP_POS_OFFSET         RD_BUFFER_HEADER_LEN
#define RD_BUFFER_WALLCLOCK_OFFSET        RD_BUFFER_STEP_POS_OFFSET + STEP_POS_SIZE
#define RD_BUFFER_GPIO_IN_OFFSET          RD_BUFFER_WALLCLOCK_OFFSET + WALLCLOCK_SIZE  
#define RD_BUFFER_WATCHDOG_CHECK_OFFSET   RD_BUFFER_GPIO_IN_OFFSET + GPIO_IN_SIZE
#define RD_BUFFER_MAGIC_OFFSET            RD_BUFFER_WATCHDOG_CHECK_OFFSET + WATCHDOG_SIZE
#define RD_BUFFER_DEBUG_DATA              RD_BUFFER_MAGIC_OFFSET + MAGIC_SIZE

int requestInputData(data_t *port, int *requestSize_out)
{
	int num_records_to_read;
	__u32 tmp_data;
	__u8 wb_wr_buffer[MESSAGE_BUFFER_SIZE];
    
        memset(wb_wr_buffer,0,sizeof(wb_wr_buffer));
        //fill etherbone packet headers
        eb_fill_readwrite32(wb_wr_buffer, 0, 0x0, 1);
        wb_wr_buffer[10]=0;	 //number of registers to write+(slave) start address

        tmp_data=htobe32(0); //fill addresses to read
        memcpy((void*)&wb_wr_buffer[12],(void*)&tmp_data,4);
        //store stepgen parameters, apply time and gpios

	// Leseadressen aufsagen...
	int pos1 = RD_BUFFER_HEADER_LEN;
	
        for (int i=0;i<steppers_count;i++)
        {
	    // double words
            tmp_data=htobe32(csr_addr(CSR_STEP_POSITION_OFFSET+i*8)); //fill addresses to read
            memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	    pos1 += 4;
            tmp_data=htobe32(csr_addr(CSR_STEP_POSITION_OFFSET+i*8+4)); //fill addresses to read
            memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	    pos1 += 4;
        }
        tmp_data=htobe32(csr_addr(CSR_WALLCLOCK_OFFSET)); //fill addresses to read
        memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	pos1 += 4;
        tmp_data=htobe32(csr_addr(CSR_WALLCLOCK_OFFSET+4)); //fill addresses to read
        memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	pos1 += 4;
        for (int i=0;i<(di_count+31)/32;i++)
        {
            tmp_data=htobe32(csr_addr(CSR_GPIO_IN_OFFSET+i*4)); //fill addresses to read
            memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	    pos1 += 4;
        }

	// debug: Watchdog zurücklesen
        tmp_data=htobe32(csr_addr(CSR_WATCHDOG_OFFSET)); //fill addresses to read
        memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	pos1 += 4;
	
	// Die Magic zur Sicherheit nochmals ANS ENDE anfügen gegen Crash-Pakete)
        tmp_data=htobe32(csr_addr(CSR_MAGIC_OFFSET)); //fill addresses to read
        memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	pos1 += 4;
        tmp_data=htobe32(csr_addr(CSR_MAGIC_OFFSET+4)); //fill addresses to read
        memcpy((void*)&wb_wr_buffer[pos1],&tmp_data,4);
	pos1 += 4;

	num_records_to_read = (pos1-RD_BUFFER_HEADER_LEN) / 4;
        wb_wr_buffer[11]=num_records_to_read;	 //number of registers to read+(master) start address



/* debug */
dump_buffer(wb_wr_buffer, pos1, "colorcnc debug: request input data", 0);

    // #################  TRANSMIT DATA  ###############################################################
        int res=eb_send(device_data->eb, wb_wr_buffer, pos1);
	if ( res < 0) {
            rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: cannot send data request message: %s\n", strerror(errno));
	    return -1;
	}
	
	*requestSize_out = pos1;
	
	return res;
}

int readInputData(data_t *port, long period, int expectedLength)
{
        __u8 wb_wr_buffer[MESSAGE_BUFFER_SIZE];
    __u8 wb_rd_buffer[MESSAGE_BUFFER_SIZE];

    __u32 tmp1=0, tmp2=0;


/*debug*/
long tsr1 = timestampMsec();


        int count = eb_recv(device_data->eb, wb_rd_buffer, sizeof(wb_rd_buffer));
/*debug*/
long tsr2 = timestampMsec();

        if (count <0) {
                //(24+num_records_to_read*4)) {
	    ++hErrorCountTotal;
	    ++hErrorCountCont;
	    if ( hErrorCountTotal > MAX_ERROR_TOTAL || hErrorCountCont > MAX_ERROR_CONT ) {
		    rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: too many receive errors: %s\n", strerror(errno));
		    return -1;
	    }
            //fprintf(stderr, "colorcnc update_port: unexpected read length: %d after %ld msec\r", count, (tsr2-tsr1));
            fprintf(stderr, "colorcnc update_port: Connection broken after %ld sec, errorCount %ld ***, try again\n", (hLastReceiveTime - hStartTime), hErrorCountTotal );
	    return 0;
        }
	else if ( count != expectedLength) // request and reply packet have the same size
	{
	    rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc update port: requested port data packet size mismatch %d / %d (ignored)\n", count, expectedLength);
	    return 0; // ignore
	}

        //end receive process
        else {
		

	    if (!check_magic(wb_rd_buffer, RD_BUFFER_MAGIC_OFFSET, 1))
	    {
		    dump_buffer(wb_rd_buffer, count, "colorcnc - no magic in received data packet. ignored.", 1);
		    ++hErrorCountTotal;
		    ++hErrorCountCont;
		    if ( hErrorCountTotal > MAX_ERROR_TOTAL || hErrorCountCont > MAX_ERROR_CONT ) 
		    {
			rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: too many receive errors. exit.\n\n");
			return -1; // exit
		    }
		    return 0; // ignore
	    }
	    
	    hErrorCountCont = 0;  // reset continuos error count
	
/* debug */
//fprintf(stderr, "colorcnc debug: recv %d bytes reply after %ld msec\n", count, ( tsr2-tsr1));
dump_buffer(wb_rd_buffer, count, "colorcnc debug: received input data", 0);

        //parse received data
            ///////////// read digital inputs /////////////////////////////////////////////
                //pack_mainR_t *t=(pack_mainR_t *)&wb_rd_buffer[16];
                //16+6*4+8
                //16+6*8+8
                tmp1=htobe32(*(__u32*)&wb_rd_buffer[RD_BUFFER_GPIO_IN_OFFSET]);
                //tmp1=htobe32(t->gpios_in);
                tmp2 = 0x01;
                for (int i=0 ; i < di_count ; i++) {
                    if (port->digital_in[i])
                        *(port->digital_in[i]) = (tmp1 & tmp2) ? 1:0 ;
                    if (port->digital_in_n[i])
                        *(port->digital_in_n[i]) = (tmp1 & tmp2) ? 0:1 ;
                    tmp2 <<= 1;
                }

	port->board_wallclock_old=port->board_wallclock;
	port->board_wallclock=htobe64(*(__u64*)&wb_rd_buffer[RD_BUFFER_WALLCLOCK_OFFSET]);//htobe64(t->board_wallclock);
	*(port->board_wallclock_msb)=htobe32(*(__u32*)&wb_rd_buffer[RD_BUFFER_WALLCLOCK_OFFSET]);
	*(port->board_wallclock_lsb)=htobe32(*(__u32*)&wb_rd_buffer[RD_BUFFER_WALLCLOCK_OFFSET+4]);
	if (port->board_wallclock>port->board_wallclock_old)
		port->t_period_real=(port->board_wallclock-port->board_wallclock_old)*(20.0e-9);//time between measurements
	else
		port->t_period_real=fperiod;
		
	

	// debug: last time something was received
	hLastReceiveTime = time(NULL);
	
    //////////////////////////////////////////////////////////////////////////////
    ////////////////////STEP dir section///////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    ///////////// read s/d count /////////////////////////////////////////////

            for (int i=0; i<steppers_count;i++)
            {
                int64_t count;
                double fcount;
                int64_t newlow;
                int reset;
                //ppdata = read32();
                reset = stepgen_reset(i);

                newlow = (htobe64(*(__u64*)&wb_rd_buffer[i*8+RD_BUFFER_STEP_POS_OFFSET])) & MASK;
// debug
if (i==0) 
{
    int64_t other = (htobe64(*(__u64*)&wb_rd_buffer[i*8+RD_BUFFER_STEP_POS_OFFSET]));
    if ( newlow != other) {
	printf("colorcnc debug: newlow %" PRIx64 " vs %" PRIx64 "\n", newlow, other);
    }
}
		port->rcvd_speed[i]=newlow;
                count = extend(port->last_count[i], newlow, W+F);
                stepgen_velocity_fb(i) = ((double)(count - port->last_count[i])) / ((double) stepgen_scale(i)) / (port->t_period_real) / (1 << F);
                port->last_count[i] = count;
                if(reset) port->reset_count[i] = count;
                fcount = (count - port->reset_count[i]) * 1. / (1<<F);
                stepgen_counts(i) = fcount;
                stepgen_position_fb(i) = fcount / stepgen_scale(i);
            }

             /*   port->board_wallclock=htobe64(*(__u64*)&wb_rd_buffer[16+6*8]);//htobe64(t->board_wallclock);
                *(port->board_wallclock_msb)=htobe32(*(__u32*)&wb_rd_buffer[16+6*8]);
                *(port->board_wallclock_lsb)=htobe32(*(__u32*)&wb_rd_buffer[16+6*8+4]);
                */
//debug
int tmpwdc=htobe32(*(__u32*)&wb_rd_buffer[RD_BUFFER_WATCHDOG_CHECK_OFFSET]);
if (tmpwdc < 100000) {
        rtapi_print_msg(RTAPI_MSG_WARN,"colorcnc: Watchdog low value received: %d\n", tmpwdc);
	dump_buffer(wb_rd_buffer, count, "colorcnc debug: received input data", 1);
}
//debug
///uint64_t tmpdebug=htobe64(*(__u64*)&wb_rd_buffer[16+6*8+8+4+4]);
//	if (tmpdebug != 0) fprintf(stderr, "colorcnc: debug data received: %ld\n", tmpdebug);


        }

	// IF we missed a paket earlier with timeout AND this packet arrives later, there can be a queue of packet.
	// Test here if anoter packet is ready ( no delay) and discard that packet to avoid such a queue.
	discard_pending_packet(device_data->eb);
}
