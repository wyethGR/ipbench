#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CONNECTIONS 16

struct pollfd fds[MAX_CONNECTIONS];

int
main(int argc, char *argv[]){
	int i, s, b;
	int snd, rcv;
	struct sockaddr_in6 my_addr;
	struct sockaddr_in6 client_addr;
	socklen_t addrlen;
	uint16_t port;
	uint32_t message_size;
	char *buffer;

	/* parse the options */
	if(argc!=3){
		printf("usage: echo <port> <message size in bytes>\n");
		return -1;
	}

	port = atoi(argv[1]);
	message_size = atoi(argv[2]);

	assert(port>0);
	assert(message_size>0);

	printf("using port %d, message size %d\n", port, message_size);

	/* allocate the buffer */
	buffer = malloc(message_size);
	assert(buffer!=NULL);

	/* initialise the polling list */
	for(i=0; i<MAX_CONNECTIONS; i++){
		fds[i].fd = -1;
		fds[i].events = 0;
		fds[i].revents = 0;
	}

	/* set up wildcard address with desired port */
	memset(&my_addr, 0, sizeof(struct sockaddr_in6));
	my_addr.sin6_family = AF_INET6;
	my_addr.sin6_addr   = in6addr_any;
	my_addr.sin6_port   = htons(port);

	s = socket(AF_INET6, SOCK_DGRAM, 0);
	assert(s>=0);

	b = bind(s, (struct sockaddr*)&my_addr, sizeof(struct sockaddr_in6));
	assert(b==0);

	for(;;){
		addrlen = sizeof(client_addr);
		rcv = recvfrom(s, buffer, message_size, 0, (struct sockaddr *) &client_addr,
        &addrlen);
		assert(rcv >= 0);

		while(rcv > 0){
			snd = sendto(s, buffer, rcv, 0, (struct sockaddr *) &client_addr,
          addrlen);
			assert(snd >= 0);

			rcv -= snd;
		}
	}
}
