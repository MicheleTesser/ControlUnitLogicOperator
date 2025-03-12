#include "../raceup_board/components/ethernet.h"
#include <stdint.h>
#include <string.h>

struct EthernetNodeIpv4_t
{
  struct udp_pcb * node;
  ip4_addr_t addr;
  uint16_t port;
};

union EthernetNodeIpv4_h_t_conv{
  EthernetNodeIpv4_h* const hidden;
  struct EthernetNodeIpv4_t* const clear;
};

union EthernetNodeIpv4_h_t_conv_const{
  const EthernetNodeIpv4_h* const hidden;
  const struct EthernetNodeIpv4_t* const clear;
};

#ifdef DEBUG
char __assert_size_ethernet_node[(sizeof(EthernetNodeIpv4_h)==sizeof(struct EthernetNodeIpv4_t))?1:-1];
char __assert_align_ethernet_node[(_Alignof(EthernetNodeIpv4_h)==_Alignof(struct EthernetNodeIpv4_t))?1:-1];
#endif /* ifdef DEBUG */

//public

int8_t
hardware_ethernet_udp_init(EthernetNodeIpv4_h* const restrict self,
    const IpAddrIpV4Port* const addr)
{
  union EthernetNodeIpv4_h_t_conv conv = {self};
  struct EthernetNodeIpv4_t* const p_self = conv.clear;

  memset(p_self, 0, sizeof(*p_self));
  //                                int ip4addr_aton(const char *cp, ip4_addr_t *addr);

  if(!ip4addr_aton(addr->addr, &p_self->addr))
  {
    return -1;
  }
  p_self->port = addr->port;
  p_self->node = udp_new();

  return -2 * (p_self->node == NULL);
  
}

extern int8_t
hardware_ethernet_udp_send(const EthernetNodeIpv4_h* const restrict self,
    const IpAddrIpV4Port* const restrict addr,
    const UdpIpv4Mex* const restrict data)
{
  const union EthernetNodeIpv4_h_t_conv_const conv = {self};
  const struct EthernetNodeIpv4_t* const p_self = conv.clear;
  struct pbuf p = {0};

  // udp_sendto     (struct udp_pcb *pcb, struct pbuf *p,
  //                                const ip_addr_t *dst_ip, u16_t dst_port);


  return udp_sendto(p_self->node,&p, &p_self->addr, &p_self->port);
}

extern void 
hardware_ethernet_udp_free(EthernetNodeIpv4_h** self)
{
  memset(*self, 0, sizeof(**self));
}
