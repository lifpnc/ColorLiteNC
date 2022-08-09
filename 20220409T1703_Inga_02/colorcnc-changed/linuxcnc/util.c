
// included from colorcnc.c

void wait_for_tx_buffer_empty(struct eb_connection *conn)
{
    int tx_queue_len = 0, first = 1;
    int tx_queue_max_len = 0;
    int peekCount = 0;
    long time1 = timestampUsec();
    
    while ( first || tx_queue_len > 0) {
	    if (tx_queue_len > 0) usecSleep(10);
	    int rc = ioctl(conn->fd, TIOCOUTQ, &tx_queue_len);
	    if ( rc < 0) {
		    perror("colorcnc: cannot ioctl?");
		    exit(1);
	    }
	    if ( tx_queue_len > tx_queue_max_len ) tx_queue_max_len = tx_queue_len;
#ifdef STAT_COUNT
	    if (/*first &&*/ tx_queue_len > 0) addSideCount(&stat, 1);
#endif
	    first = 0;
	    ++peekCount;

#ifdef UDP_RAW   
	    if ( tx_queue_len > 0 && peekCount == 1 ) {
	        // test: trigger send event with another packet ( to lala land )
	        int destIp = inet_addr ( "192.168.1.10" );
		int destPort = 42;
		char dummyPayload[1] = {'X'};
	        //udp_raw_send(conn, destIp, destPort,  &dummyPayload, sizeof(dummyPayload));
	    }
#endif
    }
    
    long time2 = timestampUsec();
    long dura = time2 - time1;
    if (dura > 300) {
	    //LL_DEBUG("waited for socket tx_queue to clear for %ld usec, %d samples, max len %d\n", dura, peekCount, tx_queue_max_len);
    }
    
}

// https://stackoverflow.com/questions/38071732/how-to-check-if-udp-packet-received-in-c-linux
void discard_pending_packet(struct eb_connection *conn)
{
	
    int retval;
    fd_set rfds;
    // NO wait
    struct timeval tv = {0,0};

    __u8 buffer[MESSAGE_BUFFER_SIZE];

    if (!conn->is_direct) {
	return;
    }

    FD_ZERO(&rfds);
    FD_SET(conn->fd, &rfds);

    retval = select(1, &rfds, NULL, NULL, &tv);

    if (retval == -1)
    {
        perror("select()");
        exit(1);
    }
    
    if (retval > 0) 
    {
	// read data and ignore
	int rc = eb_recv(conn, buffer, sizeof(buffer));
	    LL_PRINT("found qeued packet. Ignore that data, size %d\n", rc);
    }
	
	
}

int check_magic( unsigned char * buffer, int offset, int complain)
{
    int version = buffer[offset];
    if (version != MAGIC_CNC) {
	    if (complain) LL_ERROR("there is no magic in this card, got %d, expected %d, exit!\n", version, MAGIC_CNC);
	    return 0;
    }
    int inputs = buffer[offset+1];
    if (inputs != di_count) {
	    if (complain) LL_ERROR("magic input count mismatch, got %d, expected %d, exit!\n", inputs, di_count);
	    return 0;
    }
    int outputs = buffer[offset+2];
    if (outputs != do_count) {
	    if (complain) LL_ERROR("magic output count mismatch, got %d, expected %d, exit!\n", outputs, do_count);
	    return 0;
    }
    int steppercount = buffer[offset+3];
    if (steppercount != steppers_count) {
	    if (complain) LL_ERROR("magic steppers count mismatch, got %d, expected %d, exit!\n", steppercount, steppers_count);
	    return 0;
    }
    int pwmcount = buffer[offset+4];
    if (pwmcount != pwm_count) {
	    if (complain) LL_ERROR("magic pwm count mismatch, got %d, expected %d, exit!\n", pwmcount, pwm_count);
	    return 0;
    }

    return 1;
}


uint32_t csr_addr(uint32_t relativeAddr) /* convert csr relative addr to real csr adr */
{
	return relativeAddr + CSR_BASE;
}

void dump_buffer(unsigned char * buffer, int len, char * description, int force)
{
	if (force || maxDumps > 0 )
	{
		char buffer[1000]; buffer[0] = '\0';
		char temp[20];
		if (description) {
			strncat(buffer, description, sizeof(buffer)-1);
		}
		for (int i = 0; i < len; ++i)
		{
			if ( i % 8 == 0) { rtapi_snprintf(temp, sizeof(temp), "\n%03d",i); strncat(buffer, temp, sizeof(buffer)-1); }
			else if ( i % 4 == 0) { strncat(buffer, " ", sizeof(buffer)-1); }
			rtapi_snprintf(temp, sizeof(temp), " 0x%02x", buffer[i]); strncat(buffer, temp, sizeof(buffer)-1); 
		}
		strncat(buffer, "\n\n", sizeof(buffer)-1);
		LL_DEBUG_PLAIN(buffer);
		--maxDumps;
	} 
}

unsigned long timestampMsec()
{
	struct timeval timestamp;
	
	gettimeofday(&timestamp, NULL);
	
	return(timestamp.tv_sec * 1000 + ( timestamp.tv_usec/1000));
}

unsigned long timestampUsec()  // this can roll over! for debug only
{
	struct timeval timestamp;
	
	gettimeofday(&timestamp, NULL);
	
	return(timestamp.tv_sec * 1000000 + ( timestamp.tv_usec));
}

// sleep for some microseconds.
// this method will allow for rescheduling
void usecSleep(long usec)
{
long ts1=timestampUsec();
	
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = usec;
	select(0, NULL, NULL, NULL, &tv);
	
long ts2=timestampUsec();
long dura = ts2 - ts1; // can roll...
if ( dura > 0 && dura > usec + 100) {
	LL_DEBUG("overslept %ld instad of %ld usec\n", dura, usec);
}

}
