#include "sdc_circuit.h"
#include "../src/lib/raceup_board/raceup_board.h"
#include "../external_gpio.h"

#include <stdint.h>
#include <threads.h>

typedef enum
{
  SDC_AIR_1,
  SDC_AIR_2,
  SDC_SCS,
  SDC_AS_NODE,
  SDC_RES, //NOTE: u must define DV in order to use this node!!!

  __NUM_OF_SDC_CIRCUITS_NODES__
}SdcCircuitsNode;

static struct
{
  GpioRead_h m_sdc_nodes[__NUM_OF_SDC_CIRCUITS_NODES__];
  thrd_t thrd;
  uint8_t closed:1;
  uint8_t run:1;
}SDC_CIRCUIT;


static int _sdc_circuit_update(void* arg __attribute__((__unused__)))
{
  while (SDC_CIRCUIT.run)
  {
    uint8_t closed =1;
    for (SdcCircuitsNode node=0; node<__NUM_OF_SDC_CIRCUITS_NODES__; node++)
    {
      closed &= gpio_read_state(&SDC_CIRCUIT.m_sdc_nodes[node]);
    }
    SDC_CIRCUIT.closed = closed;
  }

  return 0;
}


//public

int8_t sdc_init(void)
{
#define INIT_GPIO(name, id,err)\
  if (hardware_init_read_permission_gpio(&SDC_CIRCUIT.m_sdc_nodes[name], id)<0)\
  {\
    goto err;\
  }
  int8_t err=0;

  INIT_GPIO(SDC_AIR_1, GPIO_AIR_PRECHARGE_INIT, init_air_1_failed);
  INIT_GPIO(SDC_AIR_2, GPIO_AIR_PRECHARGE_DONE, init_air_2_failed);
  INIT_GPIO(SDC_SCS, GPIO_SCS, init_scs_failed);
  INIT_GPIO(SDC_AS_NODE, GPIO_AS_NODE, init_as_node_failed);
  INIT_GPIO(SDC_RES, GPIO_RES_SDC, init_res_sdc_failed);

  thrd_create(&SDC_CIRCUIT.thrd, _sdc_circuit_update, NULL);

  return err;

init_res_sdc_failed:
  err--;
init_as_node_failed:
  err--;
init_scs_failed:
  err--;
init_air_2_failed:
  err--;
init_air_1_failed:
  err--;

  return err;
}

int8_t sdc_is_closed(void)
{
  return SDC_CIRCUIT.closed;
}

void sdc_stop(void)
{
  SDC_CIRCUIT.run=0;
}
