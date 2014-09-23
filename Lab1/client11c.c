/*
** client11c.c -- a datagram "client" sending 100,000 packets
**					Tracks the round trip time and reports the highest, lowest and average
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define SERVERPORT "10014"    // the port users will be connecting to
#define PORT "10014" // 10010 + GID
#define MAXMESSAGE 1024
#define PACKET_LENGTH_SIZE 2
#define PACKET_SEQUENCE_NUMBER_SIZE 4
#define PACKET_TIMESTAMP_SIZE 8
#define MAXDATASIZE 1038 // max number of bytes we can get at once 

struct Packet
{
          uint16_t length;
          uint32_t sequence_number;
          uint64_t timestamp;
          char message[MAXMESSAGE];
};

void build_packet(struct Packet *pack, uint32_t sequence_number_in, char *message[]);
void build_packet_from_socket(struct Packet *pack, char recived_data[], int data_length);
void print_packet(struct Packet *pack);
void build_string_from_packet(struct Packet *pack,char *buffer_out[]);


int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    if (argc != 3) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }
 	
 	int mypid = fork();
 	
 	if( 0 == mypid )
 	{
	 	//Child
	 	
	 	int count = 1, next_sequence_number=1;
	 	char buf[MAXDATASIZE];
	 	unsinged long roundtrip_high=0, roundtrip_low=0, roundtrip_avg=0;
	 	while (count < 100000)
	 	{
	 	
	 	    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	 	    	perror("recv");
	 	    	exit(1);
	 	    }
	 	    
	 	    buf[numbytes] = '\0';
	 	    
	 	    printf("client: received: '%s'\n",buf);
	 	
	 		struct Packet packet_recieved;
	 		build_packet_from_socket(&packet_recieved,buf,numbytes);
	 		print_packet(&packet_recieved);
	 		
	 		if(next_sequence_number = packet_recieved.sequence_number)
	 		{
		 		unsigned long roundtrip_time= (time(NULL) * 100)- packet_recieved.timestamp;
		 		if (roundtrip_time > roundtrip_high)
		 			roundtrip_high = roundtrip_time;
		 		else if (roundtrip_time < roundtrip_low)
		 			roundtrip_low = roundtrip_time;
		 		rounttrip_avg += roundtrip_time;
		 		
		 		count++;
		 	}
		 	else
		 	{
		 		
		 	}
	 	}
	 	//round trip times
	 	
	 	printf("Round trip time: %lu ms\n", roundtriptime);
 	}
 	else
 	{
 	    //Parent
 		int count = 1;
 		char *count_buffer;
 		count_buffer = (char *) malloc (20 + 1);
 		while (count < 100000)
 		{
	 		struct Packet packet_sent;
	 		sprintf(count_buffer, "%d", count);
	 		build_packet(&packet_sent,count,&count_buffer);
	 		print_packet(&packet_sent);
	 		char *buffer;
	 		build_string_from_packet(&packet_sent,&buffer);
	 		
	 		if ((numbytes = sendto(sockfd, buffer, (packet_sent.length + 3), 0,p->ai_addr, p->ai_addrlen)) == -1) {
	 		    perror("client to server: sendto");
	 		    exit(1);
	 		}
 		
 		
 			count++;
 		}
 	}	
	
    freeaddrinfo(servinfo);
        
    close(sockfd);

    return 0;
}

void build_packet(struct Packet *pack, uint32_t sequence_number_in, char *message[])
{
	pack->sequence_number = sequence_number_in;
	pack->timestamp  = time(NULL) * 1000;
	strcpy(pack->message, *message); 
	pack->length = strlen(*message) + PACKET_LENGTH_SIZE+PACKET_TIMESTAMP_SIZE+PACKET_SEQUENCE_NUMBER_SIZE;
}
void build_packet_from_socket(struct Packet *pack, char recived_data[], int data_length)
{
//	memcpy((void*)pack->length, recived_data, PACKET_LENGTH_SIZE);
//	memcpy((void*)pack->timestamp, recived_data+PACKET_LENGTH_SIZE,PACKET_LENGTH_SIZE);
//	memcpy((void*)pack->sequence_number, recived_data+PACKET_LENGTH_SIZE+PACKET_LENGTH_SIZE,PACKET_SEQUENCE_NUMBER_SIZE);
//	memcpy((void*)pack->message, recived_data +PACKET_LENGTH_SIZE+PACKET_LENGTH_SIZE+PACKET_SEQUENCE_NUMBER_SIZE,data_length -(PACKET_LENGTH_SIZE+PACKET_TIMESTAMP_SIZE+PACKET_SEQUENCE_NUMBER_SIZE));
}
void build_string_from_packet(struct Packet *pack,char *buffer_out[])
{
	*buffer_out = (char*)malloc(pack->length+3);
	snprintf(*buffer_out,pack->length + 3,"%u%u%llu%s",pack->length,pack->sequence_number,pack->timestamp,pack->message);	
}
void print_packet(struct Packet *pack)
{
	printf("Packet ======= \n");
	printf("  Length: %d \n", pack->length);
	printf("  Sequence Number: %d \n", pack->sequence_number);
	printf("  Timestamp: %llu \n", pack->timestamp);
	printf("  Message: %s \n\n", pack->message);
}