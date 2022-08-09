#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#include "etherbone.h"

static struct eb_connection *eb;

uint32_t csr_read_simple(unsigned long addr) {
    return eb_read32(eb, addr);
}

void csr_write_simple(uint32_t val, unsigned long addr) {
    eb_write32(eb, val, addr);
}

#include "generated/csr.h"
#include <string.h>

//Структура содержащая время задержки
struct timespec tw = {0,125000000};
//Структура, в которую будет помещен остаток времени
//задержки, если задержка будет прервана досрочно
struct timespec tr;

int main(int argc, char **argv) {
    eb = eb_connect("192.168.0.50", "1234", 1);
    if (!eb) {
        fprintf(stderr, "Couldn't connect\n");
        exit(1);
    }

    // You can now access registers from csr.h.  E.g.:
    //fprintf(stderr, "Version: %d\n", version_major_read());
	volatile unsigned char wb_wr_buffer[16+100*4];
	volatile unsigned char wb_rd_buffer[16+100*4];
    while(1)
    {
    //printf("velocity 1: %d",MMIO_inst_velocity1_read());
    //eb_read32(eb, addr)
    eb_fill_readwrite32(wb_wr_buffer, 0, 0x0, 1);
    wb_wr_buffer[11]=3;
    unsigned int tmp_data;
    for (int i=0;i<2;i++)
    {
		tmp_data=htobe32(0x70+i*4);
		memcpy((void*)&wb_wr_buffer[20+i*4],&tmp_data,4);
		//(void)tmp_data;
	}    
    eb_send(eb, wb_wr_buffer, 28);

    int count = eb_recv(eb, wb_rd_buffer, sizeof(wb_rd_buffer));
    if (count <0) {
        fprintf(stderr, "unexpected read length: %d\n", count);
        return -1;
    }
    //return eb_unfill_read32(wb_rd_buffer);
    
    fprintf(stdout, "timestamp: %lu\n", htobe64(*(unsigned long long*)&wb_rd_buffer[0x0+20]));
    //fprintf(stdout, "digital inputs: %u\n", htobe32(*(unsigned long*)&wb_rd_buffer[0xa4+16]));
    nanosleep(&tw,&tr);
    }
    return 0;
}
