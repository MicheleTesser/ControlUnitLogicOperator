#include "ethernet.h"

#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include<unistd.h> 
#include<stdlib.h>

//private

struct EthernetNodeIpv4_t
{
  int socket;
  struct sockaddr_in servaddr;
};

//public

extern EthernetNodeIpv4_t*
hardware_ethernet_udp_init(const IpAddrIpV4Port* const restrict addr)
{
  EthernetNodeIpv4_t* node = calloc(1, sizeof(*node));

  if (!node)
  {
    return NULL;
  }
  // Create a UDP Socket 
  node->socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (node->socket < 0)
  {
    free(node);
    return NULL;
  }

  node->servaddr.sin_addr.s_addr = addr->addr;
  node->servaddr.sin_port = htons(addr->port);
  node->servaddr.sin_family = AF_INET;

  return node;
}

extern int8_t
hardware_ethernet_udp_send(const EthernetNodeIpv4_t* const restrict self,
    const UdpIpv4Mex* const restrict data)
{
  return sendto(self->socket, data->raw_data, data->data_length, 0,
      (struct sockaddr*) &self->servaddr, sizeof(self->servaddr));
}

extern void 
hardware_ethernet_udp_free(EthernetNodeIpv4_t** self)
{
  close((*self)->socket);
  free(*self);
  *self=NULL;
}
