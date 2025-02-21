#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "./car_component/car_component.h"
#include "src/cores/core_0/feature/engines/engines.h"
#include "src/cores/core_0/feature/engines/amk/amk.h"
#include "src/cores/core_utility/emergency_module/emergency_module.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define INIT_PH(init_exp, module_name)\
  if ((init_exp)<0)\
  {\
    FAILED("failed init "module_name);\
    goto end;\
  }

struct ThInput {
  DriverInput_h* driver_input;
  EngineType* engine_input;
  CarMissionReader_h* mission_reader;

  int8_t run;
};

typedef struct{
  ExternalBoards_t* external_boards;
  DriverInput_h* driver_input;
  EngineType* engine_input;
  EmergencyNode_h* emergency_node;
  Gpio_h* ts;
  Gpio_h* rf;
}TestInput;

static int core_update(void* args)
{
  struct ThInput* input = args;
  time_var_microseconds t =0;

  while(input->run)
  {
    ACTION_ON_FREQUENCY(t, 50 MILLIS)
    {
      car_mission_reader_update(input->mission_reader);
      giei_driver_input_update(input->driver_input);
      inverter_update(input->engine_input);
    }
  }
  return 0;
}

static inline void _check_status_rtd(EngineType* self,const enum RUNNING_STATUS expected)
{
  enum RUNNING_STATUS status = engine_rtd_procedure(self);
  if (status != expected)
  {
    FAILED("status is not what expected");
    printf("expected: %d, given: %d\n",expected, status);
  }
  else
  {
    PASSED("correct status:");
    printf("expected: %d, given: %d\n",expected, status);
  }
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


static void test_start_precharge(EngineType* self, TestInput* input)
{
  steering_wheel_select_mission(&input->external_boards->steering_wheel, CAR_MISSIONS_HUMAN);
  car_amk_inverter_reset(&input->external_boards->amk_inverter);

  FOR_EACH_ENGINE(engine)
  {
    car_amk_inverter_set_attribute(&input->external_boards->amk_inverter, SYSTEM_READY, engine, 1);
  }

  printf("system ready but precharge not yet started: ");
  _check_status_rtd(self, SYSTEM_OFF);

  gpio_set_low(input->ts);
  wait_milliseconds(500 MILLIS);

  printf("system ready and precharge started: ");
  _check_status_rtd(self, SYSTEM_PRECAHRGE);

  wait_milliseconds(1 SECONDS);

  printf("still system ready and precharge after 1 second: ");
  _check_status_rtd(self, SYSTEM_PRECAHRGE);

  wait_milliseconds(10 SECONDS);

  printf("still system ready and precharge completed -> TS_READY: ");
  _check_status_rtd(self, TS_READY);

  printf("activating rf with brake pedal at 5 percentage in manual mode from TS_READY -> TS_READY: ");
  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_BRAKE, 5);
  gpio_set_low(input->rf);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(self, TS_READY);

  printf("activating rf with brake pedal at 25 percentage in manual mode from TS_READY -> RUNNING: ");
  wait_milliseconds(500 MILLIS);
  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_BRAKE, 25);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(self, RUNNING);

  printf("disabling rf in manual mode from RUNNING -> TS_READY: ");
  gpio_set_high(input->rf);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(self, TS_READY);

  printf("reactivating rf with brake pedal at 25 percentage in manual mode from TS_READY -> RUNNING: ");
  gpio_set_low(input->rf);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(self, RUNNING);

  printf("emergency shutdown hv: RUNNING -> SYSTEM_OFF and raise emergency ");
  car_amk_inverter_emergency_shutdown(&input->external_boards->amk_inverter);
  wait_milliseconds(1 SECONDS);
  _check_status_rtd(self, SYSTEM_OFF);
  if(EmergencyNode_is_emergency_state(input->emergency_node))
  {
    PASSED("emergency on recognized after shutdown hv with rf active");
  }
  else
  {
    FAILED("emergency on not recognized after shutdown hv with rf active");
  }

  printf("deactivating rf after emergency raised: SYSTEM_OFF -> SYSTEM_OFF and emergency resolved ");
  gpio_set_high(input->rf);
  gpio_set_high(input->ts);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(self, SYSTEM_OFF);
  if(!EmergencyNode_is_emergency_state(input->emergency_node))
  {
    PASSED("emergency resolved after shutdown hv with rf deactivated");
  }
  else
  {
    FAILED("emergency not resolved after shutdown hv with rf deactivated");
  }
}


int main(void)
{
  ExternalBoards_t external_boards = {0};

  DriverInput_h driver_input = {0};
  EngineType engine = {0};
  CarMissionReader_h mission_reader = {0};

  AmkInverter_h amk={0};
  thrd_t core=0;
  Gpio_h ts={0};
  Gpio_h rf ={0};
  EmergencyNode_h read_emergecy;
  struct ThInput input = {
    .engine_input = &engine,
    .driver_input = &driver_input,
    .mission_reader = &mission_reader,

    .run=1,
  };

  TestInput test_input = {
    .engine_input = &engine,
    .driver_input = &driver_input,
    .rf = &rf,
    .ts = &ts,
    .emergency_node = &read_emergecy,
    .external_boards = &external_boards,
  };

  INIT_PH(EmergencyNode_class_init(), "emergency module class init");
  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");
  INIT_PH(hardware_init_gpio(&ts, GPIO_TS_BUTTON), "ts gpio");
  INIT_PH(hardware_init_gpio(&rf, GPIO_RTD_BUTTON), "rf gpio");
  
  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(car_mission_reader_init(&mission_reader), "car mission reader");
  INIT_PH(driver_input_init(&driver_input, &mission_reader), "driver input");
  INIT_PH(EmergencyNode_init(&read_emergecy), "emergency instance");
  INIT_PH(amk_module_init(&amk, &driver_input, &engine), "amk module");

  thrd_create(&core, core_update, &input);

  test_initial_status(&engine);
  test_start_precharge(&engine, &test_input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  input.run=0;
  thrd_join(core,NULL);

  stop_external_boards(&external_boards);
  stop_thread_can();

end:
  print_SCORE();
  return 0;
}
