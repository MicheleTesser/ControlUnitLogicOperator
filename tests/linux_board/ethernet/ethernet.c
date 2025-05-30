#include "ethernet.h"

#include <stdio.h>
#include <string.h>
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <netinet/in.h>
#include <netinet/udp.h>

//private

struct EthernetNodeIpv4_t
{
  int socket;
  struct sockaddr_in servaddr;
};

union EthernetNodeIpv4_h_t_conv{
  EthernetNodeIpv4_h* const hidden;
  struct EthernetNodeIpv4_t* const clear;
};

union EthernetNodeIpv4_h_t_conv_const{
  const EthernetNodeIpv4_h* const hidden;
  const struct EthernetNodeIpv4_t* const clear;
};

//public

int8_t hardware_ethernet_udp_init(EthernetNodeIpv4_h* const restrict self,
    const IpAddrIpV4Port* const addr)
{
  union EthernetNodeIpv4_h_t_conv conv = {self};
  struct EthernetNodeIpv4_t* const p_self = conv.clear;

  // Create a UDP Socket 
  p_self->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (p_self->socket < 0)
  {
    return -1;
  }

  p_self->servaddr.sin_family = AF_INET;
  p_self->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  p_self->servaddr.sin_port = htons(addr->port);

  return 0;
}

int8_t hardware_ethernet_udp_send(const EthernetNodeIpv4_h* const restrict self,
    const UdpIpv4Mex* const restrict data)
{
  const union EthernetNodeIpv4_h_t_conv_const conv = {self};
  const struct EthernetNodeIpv4_t* const p_self = conv.clear;

  return (int8_t) sendto(p_self->socket, data->raw_data, data->data_length, 0,
      (struct sockaddr*) &p_self->servaddr, sizeof(p_self->servaddr));
}

void hardware_ethernet_udp_free(EthernetNodeIpv4_h* self)
{
  union EthernetNodeIpv4_h_t_conv conv = {self};
  struct EthernetNodeIpv4_t*  p_self = conv.clear;
  close(p_self->socket);
  memset(self, 0, sizeof(*self));
}
