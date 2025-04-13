#ifndef __VIRTUAL_ETHERNET__
#define __VIRTUAL_ETHERNET__

#include <stdint.h>
#include "./board_architecture.h"

typedef struct{
  char* addr;
  uint16_t port;
}IpAddrIpV4Port;

typedef struct{
  const void* raw_data;
  const uint16_t data_length;
}UdpIpv4Mex;

#if ARCH == 64
typedef __attribute__((aligned(8))) struct {
  const uint8_t private_data[16];
}EthernetNodeIpv4_h;
#elif ARCH == 32
typedef __attribute__((aligned(4))) struct {
  const uint8_t private_data[16];
}EthernetNodeIpv4_h;
#else
#endif

int8_t
hardware_ethernet_udp_init(
    EthernetNodeIpv4_h* const restrict self,
    const IpAddrIpV4Port* const addr);

extern int8_t
hardware_ethernet_udp_send(const EthernetNodeIpv4_h* const restrict self,
    const UdpIpv4Mex* const restrict data)__attribute__((__nonnull__(1,2)));

extern void 
hardware_ethernet_udp_free(EthernetNodeIpv4_h* self)__attribute__((__nonnull__(1)));

#endif // !__VIRTUAL_ETHERNET__
