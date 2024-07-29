#include "latency.h"

/* the socket */
static int s;

static int
get_addr(char *hostname, uint16_t port, int socktype, struct addrinfo *addr)
{
  struct addrinfo hints, *result, *entry;
  int err;
  char port_str[128] = {0};

  // Set our hints
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET6;
  hints.ai_socktype = socktype;
  hints.ai_flags = AI_NUMERICSERV;

  // With AI_NUMERICSERV our port number must be a string
  sprintf(port_str, "%d", port);

  err = getaddrinfo(hostname, port_str, &hints, &result);
  if (err) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
    freeaddrinfo(result);
    return -1;
  }

  for(entry = result; entry != NULL; result = result->ai_next) {
    if (entry->ai_family == AF_INET6) {
      memcpy(addr, entry, sizeof(struct addrinfo));
      break;
    }
  }

  freeaddrinfo(result);
  return 0;
}


int tcp_setup_socket(char *hostname, int port, char *args)
{
	int err;
	struct addrinfo addr = {0};

	if (get_addr(hostname, port, SOCK_STREAM, &addr) == -1) {
		fprintf(stderr, "Can not contact target!\n");
		return -1;
	}

	s = socket(AF_INET6, SOCK_STREAM, 0);
	if (s == -1) {
		fprintf(stderr, "Can not create socket (%s).\n", strerror(errno));
		return -1;
	}

    if (args != NULL) {
        if(setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, args, strlen(args)) < 0) {
            fprintf(stderr, "Can not set socket option: (%s)\n", strerror(errno));
            return -1;
        }
    }

	err = connect(s, addr.ai_addr, addr.ai_addrlen);
	if (err != 0) {
		fprintf(stderr, "Can not connect (%s).\n", strerror(errno));
		return -1;
	}

	return s;
}

int udp_setup_socket(char *hostname, int port, char *args)
{
	int err;
	struct addrinfo addr = {0};

	if (get_addr(hostname, port, SOCK_DGRAM, &addr) == -1) {
		fprintf(stderr, "Can not contact target!\n");
		return -1;
	}

	s = socket(AF_INET6, SOCK_DGRAM, 0);
	if (s == -1) {
		fprintf(stderr, "Can not create socket (%s).\n", strerror(errno));
		return -1;
	}

    if (args != NULL) {
        if(setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, args, strlen(args)) < 0) {
            fprintf(stderr, "Can not set socket option: (%s)\n", strerror(errno));
            return -1;
        }
    }

	err = connect(s, addr.ai_addr, addr.ai_addrlen);
	if (err != 0) {
		fprintf(stderr, "Can not connect (%s).\n", strerror(errno));
		return -1;
	}

	return s;
}


/* ip based packet sending functions */
int ip_setup_packet(char **buf, int size)
{
	*buf = malloc(size);
	assert (buf != NULL);
	return 0;
}

int ip_fill_packet(void *buf, uint64_t seq)
{
	*((uint64_t*)buf) = seq;
	return 0;
}

int ip_read_packet(void *buf, uint64_t *seq)
{
	*seq = *((uint64_t*)buf);
	return 0;
}

int ip_send_packet(void *buf, int size, int flags)
{
	return send(s, buf, size, flags);
}

int ip_recv_packet(void *buf, int size, int flags)
{
	return recv(s, buf, size, flags);
}
