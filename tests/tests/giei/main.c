#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_0/feature/giei/giei.h"
#include "src/cores/core_0/feature/engines/engines.h"
#include "src/cores/core_0/feature/engines/amk/amk.h"
#include "src/cores/core_0/feature/driver_input/driver_input.h"
#include "src/cores/core_0/feature/maps/maps.h"
#include "src/cores/core_0/feature/imu/imu.h"
#include "src/cores/core_utility/emergency_module/emergency_module.h"
#include "car_component/car_component.h"

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

typedef struct CoreThread{
  thrd_t thread_id;
  uint8_t run;
}CoreThread;

typedef struct CoreInput{
  Giei_h* giei;
  EngineType* engine;
  DrivingMaps_h* maps;
  DriverInput_h* driver;
  Imu_h* imu;
  EmergencyNode_h* emergency_node;
  Gpio_h* ts;
  Gpio_h* rf;
  GpioRead_h* rtd_sound;

  Atc_h* atc;
  Pcu_h* pcu;
  EmulationAmkInverter_h* amk_emulation;

  volatile const uint8_t* const core_run;
}CoreInput;

static int _core_thread_fun(void* arg)__attribute_maybe_unused__;
static int _core_thread_fun(void* arg)
{
  CoreInput* const core_input = arg;
  while (*core_input->core_run)
  {
    driver_input_update(core_input->driver);
    driving_map_update(core_input->maps);
    imu_update(core_input->imu);
    inverter_update(core_input->engine);
    giei_update(core_input->giei);
  }
  return 0;
}

static inline void _check_status_rtd(Giei_h* const self,
    const EmergencyNode_h* const emergency_read,
    const GpioRead_h* rtd_sound,
    const enum RUNNING_STATUS expected_running_status,
    const uint8_t expected_emergency_status,
    const uint8_t expected_rtd_sound_status)
{
  enum RUNNING_STATUS status = GIEI_check_running_condition(self);
  uint8_t emergency_status= EmergencyNode_is_emergency_state(emergency_read);
  uint8_t rtd_sound_status = gpio_read_state(rtd_sound);

  if (status != expected_running_status ||
      emergency_status != expected_emergency_status ||
      rtd_sound_status != expected_rtd_sound_status)
  {
    FAILED("status is not what expected");
  }
  else
  {
    PASSED("correct status");
  }
  printf("expected_running_status: %d, given: %d\t",expected_running_status, status);
  printf("expected_emergency_status: %d, given: %d\n",expected_emergency_status, emergency_status);
  printf("expected_rtd_sound_status: %d, given: %d\n",expected_rtd_sound_status, rtd_sound_status);
  printf("-------------------------------\n");
}

//public

static void test_giei_rtd(CoreInput* const input) __attribute_maybe_unused__;
static void test_giei_rtd(CoreInput* const input)
{
  car_amk_inverter_reset(input->amk_emulation);

  FOR_EACH_ENGINE(engine)
  {
    car_amk_inverter_set_attribute(input->amk_emulation, SYSTEM_READY, engine, 1);
  }

  printf("switching to human driver\n");
  driver_input_change_driver(input->driver, DRIVER_HUMAN);

  printf("initial giei status\n");
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, SYSTEM_OFF, 0,0);

  printf("starting precharge: SYSTEM_OFF -> SYSTEM_PRECAHRGE\n");
  gpio_set_low(input->ts);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, SYSTEM_PRECAHRGE, 0 ,0);

  printf("after 1 second still in precharge: SYSTEM_PRECAHRGE -> SYSTEM_PRECAHRGE\n");
  wait_milliseconds(1 SECONDS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, SYSTEM_PRECAHRGE, 0, 0);

  printf("after 5 seconds precharge completed: SYSTEM_PRECAHRGE -> TS_READY\n");
  wait_milliseconds(8 SECONDS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, TS_READY, 0, 0);

  printf("activating rf with brake pedal at 5 percentage in manual mode from TS_READY -> TS_READY\n");
  atc_pedals_steering_wheel(input->atc, ATC_BRAKE, 5);
  gpio_set_low(input->rf);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, TS_READY, 0, 0);

  printf("activating rf with brake pedal at 25 percentage in manual mode from TS_READY -> RUNNING\n");
  atc_pedals_steering_wheel(input->atc, ATC_BRAKE, 25);
  gpio_set_low(input->rf);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, RUNNING, 0, 1);

  printf("waiting 2 seconds sound still on\n");
  wait_milliseconds(2 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, RUNNING, 0, 1);

  printf("waiting 2 seconds sound off\n");
  wait_milliseconds(2 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, RUNNING, 0, 0);

  printf("disabling rf in manual mode from RUNNING -> TS_READY\n");
  gpio_set_high(input->rf);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, TS_READY, 0, 0);

  printf("disabling rf in manual mode from TS_READY -> RUNNING \n");
  gpio_set_low(input->rf);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, RUNNING, 0, 0);

  printf("emergency shutdown hv: RUNNING -> SYSTEM_OFF and raise emergency\n");
  car_amk_inverter_emergency_shutdown(input->amk_emulation);
  wait_milliseconds(1 SECONDS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, SYSTEM_OFF, 1, 0);

  printf("deactivating rf after emergency raised: SYSTEM_OFF -> SYSTEM_OFF and emergency resolved\n");
  gpio_set_high(input->rf);
  gpio_set_high(input->ts);
  wait_milliseconds(500 MILLIS);
  _check_status_rtd(input->giei, input->emergency_node, input->rtd_sound, SYSTEM_OFF, 0, 0);
}

int main(void)
{
  Giei_h giei = {0};
  EngineType engines = {0};
  AmkInverter_h amk = {0};
  DriverInput_h driver = {0};
  DrivingMaps_h maps = {0};
  Imu_h imu = {0};
  CoreThread core_thread={.run=1};
  EmergencyNode_h emergency_read = {0};
  Gpio_h ts={0};
  Gpio_h rf ={0};
  GpioRead_h rtd_sound_read = {0};
  EmulationAmkInverter_h amk_inverter_emulation = {0};
  Atc_h atc={0};
  Pcu_h pcu={0};

  CoreInput input __attribute_maybe_unused__= {
    .giei = &giei,
    .imu =&imu,
    .engine = &engines,
    .maps = &maps,
    .driver = &driver,
    .emergency_node = &emergency_read,
    .ts = &ts,
    .rf = &rf,
    .atc = &atc,
    .pcu = &pcu,
    .amk_emulation = &amk_inverter_emulation,
    .rtd_sound = &rtd_sound_read,

    .core_run = &core_thread.run,
  };

  INIT_PH(EmergencyNode_class_init(), "emergency module class init");
  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");
  INIT_PH(hardware_init_gpio(&ts, GPIO_TS_BUTTON), "ts gpio");
  INIT_PH(hardware_init_gpio(&rf, GPIO_RTD_BUTTON), "rf gpio");
  INIT_PH(hardware_init_read_permission_gpio(&rtd_sound_read, GPIO_RTD_SOUND), "rtd sound gpio");

  //HACK: I do not know why but if you init the amk_module before the pcu the pcu breaks and 
  //the send mailbox for PCURFACK reset itself. I Do not know why for now.
  INIT_PH(car_amk_inverter_class_init(&amk_inverter_emulation), "amk emulation");
  INIT_PH(pcu_init(&pcu), "pcu emulation");
  INIT_PH(atc_start(&atc), "atc emulation");

  INIT_PH(driver_input_init(&driver), "driver input");
  INIT_PH(driving_maps_init(&maps), "driver maps");
  INIT_PH(imu_init(&imu), "imu");
  INIT_PH(amk_module_init(&amk, &driver, &engines), "amk module");
  INIT_PH(EmergencyNode_init(&emergency_read), "emergency instance");
  INIT_PH(giei_init(&giei, &engines, &driver, &maps, &imu), "giei module");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_giei_rtd(&input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);
  printf("stopping pcu\n");
  pcu_stop(&pcu);
  printf("stopping atc\n");
  atc_stop(&atc);
  printf("stopping inverter\n");
  car_amk_inverter_stop(&amk_inverter_emulation);
  printf("stopping can module\n");
  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
