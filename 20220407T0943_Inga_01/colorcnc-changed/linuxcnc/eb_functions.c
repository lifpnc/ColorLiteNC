


int eb_unfill_read32(uint8_t wb_buffer[20]) {
    int buffer;
    uint32_t intermediate;
    memcpy(&intermediate, &wb_buffer[16], sizeof(intermediate));
    intermediate = be32toh(intermediate);
    memcpy(&buffer, &intermediate, sizeof(intermediate));
    return buffer;
}

int eb_fill_readwrite32(uint8_t wb_buffer[20], uint32_t data, uint32_t address, int is_read) {
    memset(wb_buffer, 0, 20);
    wb_buffer[0] = 0x4e;	// Magic byte 0
    wb_buffer[1] = 0x6f;	// Magic byte 1
    wb_buffer[2] = 0x10;	// Version 1, all other flags 0
    wb_buffer[3] = 0x44;	// Address is 32-bits, port is 32-bits
    wb_buffer[4] = 0;		// Padding
    wb_buffer[5] = 0;		// Padding
    wb_buffer[6] = 0;		// Padding
    wb_buffer[7] = 0;		// Padding

    // Record
    wb_buffer[8] = 0;		// No Wishbone flags are set (cyc, wca, wff, etc.)
    wb_buffer[9] = 0x0f;	// Byte enable

    if (is_read) {
        wb_buffer[10] = 0;  // Write count
        wb_buffer[11] = 1;	// Read count
        data = htobe32(csr_addr(address));
        memcpy(&wb_buffer[16], &data, sizeof(data));
    }
    else {
        wb_buffer[10] = 1;	// Write count
        wb_buffer[11] = 0;  // Read count
        address = htobe32(csr_addr(address));
        memcpy(&wb_buffer[12], &address, sizeof(address));

        data = htobe32(data);
        memcpy(&wb_buffer[16], &data, sizeof(data));
    }
    return 20;
}

int eb_fill_write32(uint8_t wb_buffer[20], uint32_t data, uint32_t address) {
    return eb_fill_readwrite32(wb_buffer, data, address, 0);
}

int eb_fill_read32(uint8_t wb_buffer[20], uint32_t address) {
    return eb_fill_readwrite32(wb_buffer, 0, address, 1);
}

int eb_send(struct eb_connection *conn, const void *bytes, size_t len) {
    if (conn->is_direct) {
#ifdef UDP_RAW
// todo: from conn
        int destIp = inet_addr ( "192.168.1.50" );
	int destPort = 1234;

        return udp_raw_send(conn, destIp, destPort, bytes, len);
#else
        return sendto(conn->fd, bytes, len, 0, conn->addr->ai_addr, conn->addr->ai_addrlen);
#endif
    }
    return write(conn->fd, bytes, len);
}

int eb_recv(struct eb_connection *conn, void *bytes, size_t max_len) {
    if (conn->is_direct)
        return recvfrom(conn->read_fd, bytes, max_len, 0, NULL, NULL);
    return read(conn->fd, bytes, max_len);
}

void eb_write32(struct eb_connection *conn, uint32_t val, uint32_t addr) {
    uint8_t raw_pkt[20];
    eb_fill_write32(raw_pkt, val, addr);
    eb_send(conn, raw_pkt, sizeof(raw_pkt));
}

uint32_t eb_read32(struct eb_connection *conn, uint32_t addr) {
    uint8_t raw_pkt[20];
    eb_fill_read32(raw_pkt, addr);

    eb_send(conn, raw_pkt, sizeof(raw_pkt));

    int count = eb_recv(conn, raw_pkt, sizeof(raw_pkt));
    if (count != sizeof(raw_pkt)) {
        fprintf(stderr, "colorcnc eb_read32: unexpected read length: %d\n", count);
        return -1;
    }
    return eb_unfill_read32(raw_pkt);
}

struct eb_connection *eb_connect(const char *addr, const char *port, int is_direct) {

    struct addrinfo hints_rx;
    struct addrinfo hints_tx;
    struct addrinfo* res_rx = 0;
    struct addrinfo* res_tx = 0;
    int err;
    int sock;

    struct eb_connection *conn = malloc(sizeof(struct eb_connection));
    if (!conn) {
        rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: couldn't allocate memory for eb_connection");
        return NULL;
    }

    memset(&hints_rx, 0, sizeof(hints_rx));
    hints_rx.ai_family = AF_INET;
    hints_rx.ai_socktype = is_direct ? SOCK_DGRAM : SOCK_STREAM;
    hints_rx.ai_protocol = is_direct ? IPPROTO_UDP : IPPROTO_TCP;
    hints_rx.ai_flags = AI_ADDRCONFIG;
    err = getaddrinfo(addr, port, &hints_rx, &res_rx);
    if (err != 0) {
        rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: failed to resolve remote socket address (err=%d / %s)\n", err, gai_strerror(err));
        free(conn);
        return NULL;
    }


    memset(&hints_tx, 0, sizeof(hints_tx));
    hints_tx.ai_family = AF_INET;
#ifdef UDP_RAW
    hints_tx.ai_socktype = is_direct ? SOCK_RAW : SOCK_STREAM;
    hints_tx.ai_protocol = is_direct ? IPPROTO_RAW : IPPROTO_TCP;
    hints_tx.ai_flags = AI_ADDRCONFIG;
    err = getaddrinfo(addr, NULL, &hints_tx, &res_tx);
#else
    hints_tx.ai_socktype = is_direct ? SOCK_DGRAM : SOCK_STREAM;
    hints_tx.ai_protocol = is_direct ? IPPROTO_UDP : IPPROTO_TCP;
    hints_tx.ai_flags = AI_ADDRCONFIG;
    err = getaddrinfo(addr, port, &hints_tx, &res_tx);
#endif
    if (err != 0) {
        rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: failed to resolve remote socket address (err=%d / %s)\n", err, gai_strerror(err));
        free(conn);
        return NULL;
    }

    conn->is_direct = is_direct;

    if (is_direct) {
        // Rx half
        struct sockaddr_in si_me;

        memset((char *) &si_me, 0, sizeof(si_me));
        si_me.sin_family = res_rx->ai_family;
        si_me.sin_port = ((struct sockaddr_in *)res_rx->ai_addr)->sin_port;
        si_me.sin_addr.s_addr = htobe32(INADDR_ANY);

        int rx_socket;
        if ((rx_socket = socket(res_rx->ai_family, res_rx->ai_socktype, res_rx->ai_protocol)) == -1) {
            rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: Unable to create Rx socket: %s\n", strerror(errno));
            freeaddrinfo(res_rx);
            free(conn);
            return NULL;
        }
        if (bind(rx_socket, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
            rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: Unable to bind Rx socket to port: %s\n", strerror(errno));
            close(rx_socket);
            freeaddrinfo(res_rx);
            free(conn);
            return NULL;
        }
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000; //FIRST_PACKETS 
		err = setsockopt(rx_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		if (err < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: could NOT to set setsockopt for tx\n");
            free(conn);
            return NULL;
		}

        // Tx half
//fprintf(stderr,"colorcnc debug: Perparing tx socket\n");
        int tx_socket = socket(res_tx->ai_family, res_tx->ai_socktype, res_tx->ai_protocol);
        if (tx_socket == -1) {
            rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: Unable to create socket: %s\n", strerror(errno));
            close(rx_socket);
            close(tx_socket);
            freeaddrinfo(res_tx);
            rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: unable to create socket: %s\n", strerror(errno));
            free(conn);
            return NULL;
        }

		timeout.tv_usec = SEND_TIMEOUT_US;
		err = setsockopt(tx_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
		if (err < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: could NOT to set setsockopt timeout for tx\n");

                    free(conn);
                    return NULL;
		}

#if 0
	        // inga 2.4.22 try seting the prio
	        int sendPrio = 7 ; // 1 low, 7 high
		err = setsockopt(tx_socket, SOL_SOCKET, SO_PRIORITY, &sendPrio, sizeof(sendPrio));
		if (err < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: could NOT to set setsockopt sendprio for tx\n");


                        free(conn);
                        return NULL;
		}
#endif

	        // inga 2.4.22 try seting the TOS to "immediatly"
	        int optval = 0x08 ; // immediadlty
		err = setsockopt(tx_socket, IPPROTO_IP, IP_TOS, &optval, sizeof(optval));
		if (err < 0) {
			rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: could NOT to set setsockopt TOS for tx\n");


                        free(conn);
                        return NULL;
		}

        conn->read_fd = rx_socket;
        conn->fd = tx_socket;
        conn->addr = res_rx;
	
fprintf(stderr,"colorcnc debug: Initial socket created\n");

    }
    else {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            rtapi_print_msg(RTAPI_MSG_ERR, "colorcnc: failed to create socket: %s\n", strerror(errno));
            freeaddrinfo(res_rx);
            free(conn);
            return NULL;
        }

        int connection = connect(sock, res_rx->ai_addr, res_rx->ai_addrlen);
        if (connection == -1) {
            close(sock);
            freeaddrinfo(res_rx);
            rtapi_print_msg(RTAPI_MSG_ERR,"colorcnc: unable to create socket: %s\n", strerror(errno));
            free(conn);
            return NULL;
        }

        conn->fd = sock;
        conn->addr = res_rx;
    }

    return conn;
}

void eb_disconnect(struct eb_connection **conn) {
    if (!conn || !*conn)
        return;

    freeaddrinfo((*conn)->addr);
    close((*conn)->fd);
    if ((*conn)->read_fd)
        close((*conn)->read_fd);
    free(*conn);
    *conn = NULL;
    return;
}
