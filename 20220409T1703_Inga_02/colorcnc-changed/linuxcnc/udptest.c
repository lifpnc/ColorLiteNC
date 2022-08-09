// included into colorcnc.c

#include <netinet/udp.h>	//Provides declarations for udp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <arpa/inet.h>

// based on https://www.binarytides.com/raw-udp-sockets-c-linux/


// TODO: DST ADR aus conn nehmen
static const char * sourceIp = "192.168.1.10";
//static const char * destIp = "192.168.1.50";
static int sourcePort = 4242;
//static int destPort = 1234;

static int packetId = 54320;

unsigned short csum(unsigned short *ptr,int nbytes);


int udp_raw_send(struct eb_connection *conn, int destAddr, int destPort, const void *payload, size_t payloadLen)
{

//LL_DEBUG"udptest debug: enter udp_raw_send\n");

	//Datagram to represent the packet
	char datagram[MESSAGE_BUFFER_SIZE + sizeof(struct iphdr) + sizeof(struct udphdr)];
	char source_ip[32] , *pseudogram;

        // zero out packet buffer
        memset(datagram, 0, sizeof(datagram));
        
	if ( payloadLen > (sizeof(datagram) - sizeof(struct iphdr) - sizeof(struct udphdr))) 
	{
		LL_ERROR("udptest: payload data length %d exceeds maximum lenght\n", payloadLen);
		exit(1);
	}
	
        struct iphdr *iph = (struct iphdr*) datagram;
   
   	//UDP header
	struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));
   
   	struct pseudo_header psh;

	struct sockaddr_in raw_sin;
	raw_sin.sin_family = AF_INET;
	raw_sin.sin_port = htons(destPort);
	raw_sin.sin_addr.s_addr = destAddr; //inet_addr (destIp);


  
    	//Fill in the IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0; //(40 << 2)+0;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + payloadLen;
	iph->id = htonl (++packetId);	//Id of this packet
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;		//Set to 0 before calculating checksum
	iph->saddr = inet_addr ( sourceIp );	//Spoof the source ip address
	iph->daddr = destAddr; //inet_addr ( destIp );
	
//LL_DEBUG("udptest debug: iph done\n");
	
	//Ip checksum
	iph->check = csum ((unsigned short *) datagram, iph->tot_len);

//LL_DEBUG("udptest debug: csum done\n");
	
	//UDP header
	udph->source = htons (sourcePort);
	udph->dest = htons (destPort);
	udph->len = htons(8 + payloadLen);	//tcp header size
	udph->check = 0;	//leave checksum 0 now, filled later by pseudo header
 
//LL_DEBUG("udptest debug: udph done\n");

	char * dataPart = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
	memcpy(dataPart, payload, payloadLen);
	
 	//Now the UDP checksum using the pseudo header
	psh.source_address = inet_addr( source_ip );
	psh.dest_address = destPort;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_UDP;
	psh.udp_length = htons(sizeof(struct udphdr) + payloadLen );

//LL_DEBUG("udptest debug: psh done\n");
	
	int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + payloadLen;
	pseudogram = malloc(psize);

	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + payloadLen);
	
	udph->check = csum( (unsigned short*) pseudogram , psize);

//LL_DEBUG("udptest debug: checksum done\n");
	
	//loop if you want to flood :)
	//while (1)
	{
		
//LL_DEBUG("udp raw socket: %d, adr = %d, port = %d\n", conn->fd, raw_sin.sin_addr, raw_sin.sin_port);
//dump_buffer(datagram, iph->tot_len, "udp raw data",1);
		//Send the packet
		if (sendto (conn->fd, datagram, iph->tot_len ,	0, (struct sockaddr *) &raw_sin, sizeof (raw_sin)) < 0)
		{
			perror("udptest: sendto failed");
		}
		//Data send successfully
		else
		{
			//LL_DEBUG("udptest: Packet Send. Length : %d \n" , iph->tot_len);
		}
	}
//LL_DEBUG("udptest debug: exit udp_raw_send\n");
	
	return 0;   
} 


/*
	Generic checksum calculation function
*/
unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}
