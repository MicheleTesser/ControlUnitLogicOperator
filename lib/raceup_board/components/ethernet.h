#ifndef __VIRTUAL_ETHERNET__
#define __VIRTUAL_ETHERNET__

#include <stdint.h>

typedef struct{
  uint32_t addr;
  uint16_t port;
}IpAddrIpV4Port;

typedef struct{
  const void* raw_data;
  const uint16_t data_length;
}UdpIpv4Mex;

typedef struct EthernetNodeIpv4_t EthernetNodeIpv4_t;

extern EthernetNodeIpv4_t*
hardware_ethernet_udp_init(const IpAddrIpV4Port* const restrict addr)
  __attribute__((__nonnull__(1)));

extern int8_t
hardware_ethernet_udp_send(const EthernetNodeIpv4_t* const restrict self,
    const UdpIpv4Mex* const restrict data)__attribute__((__nonnull__(1,2)));

extern void 
hardware_ethernet_udp_free(EthernetNodeIpv4_t** self)__attribute__((__nonnull__(1)));

#endif // !__VIRTUAL_ETHERNET__
