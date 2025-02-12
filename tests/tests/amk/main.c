#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include <stdint.h>
#include <stdio.h>
#include "./car_component/car_component.h"
#include "src/cores/core_0/feature/engines/engines.h"
#include "src/cores/core_0/feature/engines/amk/amk.h"
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

struct ThInput {
  DriverInput_h* driver_input;
  EngineType* engine_input;
};

static int run=1;
static int core_update(void* args)
{
  struct ThInput* input = args;
  time_var_microseconds t =0;

  while(run)
  {
    if ((timer_time_now() - t) > 1 SECONDS)
    {
      driver_input_update(input->driver_input);
      inverter_update(input->engine_input);
    }
  }
  return 0;
}

static void test_initial_status(EngineType* self)
{
  int8_t rtd = engine_rtd_procedure(self);
  if (rtd != SYSTEM_OFF)
  {
    FAILED("initial rtd status is wrong");
    printf("given %d, expected: %d\n", rtd, SYSTEM_OFF);
  }
  PASSED("initial rtd status is SYSTEM_OFF");
}

#define CHECK_STATUS_RTD(self, expected)\
{\
  enum RUNNING_STATUS status = engine_rtd_procedure(self);\
  if (status != expected)\
  {\
    FAILED("status is not what expected");\
    printf("expected: %d, given: %d\n",expected, status);\
  }\
  else\
  {\
    PASSED("correct status:");\
    printf("expected: %d, given: %d\n",expected, status);\
  }\
}

static void test_start_precharge(EngineType* self, EmulationAmkInverter_h* inverter)
{
  Gpio_h ts={0};

  car_amk_inverter_reset(inverter);
  if (hardware_init_gpio(&ts, GPIO_TS_BUTTON)<0)
  {
    FAILED("ts gpio init failed");
    return;
  }

  FOR_EACH_ENGINE(engine)
  {
    car_amk_inverter_set_attribute(inverter, SYSTEM_READY, engine, 1);
  }

  printf("system ready but precharge not yet started: ");
  CHECK_STATUS_RTD(self, SYSTEM_OFF);

  gpio_set_low(&ts);
  wait_milliseconds(100 MILLIS);

  printf("system ready and precharge started: ");
  CHECK_STATUS_RTD(self, SYSTEM_PRECAHRGE);

  wait_milliseconds(1 SECONDS);

  printf("still system ready and precharge after 1 second: ");
  CHECK_STATUS_RTD(self, SYSTEM_PRECAHRGE);

  wait_milliseconds(6 SECONDS);

  printf("still system ready and precharge completed -> TS_READY: ");
  CHECK_STATUS_RTD(self, TS_READY);

}


int main(void)
{
  struct EmulationAmkInverter_h amk_inverter_emulation = {0};
  AmkInverter_h amk={0};
  EngineType engine = {0};
  DriverInput_h driver_input = {0};
  thrd_t core=0;

  if (hardware_init_can(CAN_INVERTER, _1_MBYTE_S_)<0)
  {
    FAILED("failed init can inverter");
    goto end;
  }


  if (hardware_init_can(CAN_GENERAL, _500_KBYTE_S_)<0)
  {
    FAILED("failed init can general");
    goto end;
  }

  if (hardware_init_can(CAN_DV, _500_KBYTE_S_)<0)
  {
    FAILED("failed init can dv");
    goto end;
  }

  if (hardware_init_interrupt()<0)
  {
    FAILED("failed init interrupt");
    goto end;
  }

  if(create_virtual_chip() <0)
  {
    FAILED("failed init virtual chip");
    goto end;
  }

  if (driver_input_init(&driver_input)<0)
  {
    FAILED("failed init driver input");
    goto end;
  }


  if(car_amk_inverter_class_init(&amk_inverter_emulation,"culo_can_0")<0){
    FAILED("amk emulator init failed");
    goto end;
  }
  if (amk_module_init(&amk, &driver_input, &engine)<0)
  {
    FAILED("failed init amk module");
    goto end;
  }


  struct ThInput input = {
    .engine_input = &engine,
    .driver_input = &driver_input,
  };


  thrd_create(&core, core_update, &input);

  test_initial_status(&engine);
  test_start_precharge(&engine, &amk_inverter_emulation);

  run=0;
  thrd_join(core,NULL);
  printf("stopping inverter\n");
  car_amk_inverter_stop(&amk_inverter_emulation);
  printf("stopping can module\n");
  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
