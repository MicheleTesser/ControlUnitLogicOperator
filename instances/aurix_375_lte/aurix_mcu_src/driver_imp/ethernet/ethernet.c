#include "../../src/lib/raceup_board/components/ethernet.h"

int8_t hardware_ethernet_udp_init(
    EthernetNodeIpv4_h* const restrict self __attribute__((__unused__)),
    const IpAddrIpV4Port* const addr __attribute__((__unused__)))
{
  return -1;
}

int8_t hardware_ethernet_udp_send(const EthernetNodeIpv4_h* const restrict self __attribute__((__unused__)),
    const UdpIpv4Mex* const restrict data __attribute__((__unused__)))
{
  return -1;
}

void  hardware_ethernet_udp_free(EthernetNodeIpv4_h* self __attribute__((__unused__)))
{
  return;
}
