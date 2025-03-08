#ifndef __VIRTUAL_ETHERNET__
#define __VIRTUAL_ETHERNET__

#include <stdint.h>

enum EthernetNodeType{
  ETHERNET_NODE=0,

  __NUM_OF_ETHERNET_NODES__
};

typedef struct{
  uint32_t addr;
  uint16_t port;

}IpAddrIpV4Port;

typedef struct EthernetNodeIpv4_h EthernetNodeIpv4_h;

typedef struct{
  const void* const raw_data;
  const uint16_t data_length;
}UdpIpv4Mex;

extern EthernetNodeIpv4_h*
hardware_ethernet_udp_init(const enum EthernetNodeType node,
    const IpAddrIpV4Port* const restrict addr);

extern int8_t
hardware_ethernet_udp_send(const EthernetNodeIpv4_h* const restrict self,
    const UdpIpv4Mex* const restrict data)__attribute__((__nonnull__(1,2)));

extern int8_t
hardware_ethernet_udp_free(EthernetNodeIpv4_h** self)__attribute__((__nonnull__(1)));

#endif // !__VIRTUAL_ETHERNET__
