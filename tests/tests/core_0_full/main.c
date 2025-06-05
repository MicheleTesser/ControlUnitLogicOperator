#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_0/feature/feature.h"
#include "src/cores/core_0/feature/engines/engines.h"
#include "src/cores/core_utility/core_utility.h"

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

typedef struct{
  thrd_t thread_id;
  uint8_t run;
}CoreThread;

typedef struct{
  volatile const uint8_t* const core_run;

  Core0Feature_h* core_0_feature;
  AsNode_h* as_node;
  CarMissionReader_h* mission_reader;
}CoreInput;

typedef struct{
  ExternalBoards_t* external_boards;

  Gpio_h* rf;
  GpioRead_h* rtd_sound;
  EmergencyNode_h* emergency_read;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
    car_mission_reader_update(core_input->mission_reader);
    as_node_update(core_input->as_node);
    core_0_feature_update(core_input->core_0_feature);
    core_0_feature_compute_power(core_input->core_0_feature);
    errno_trace_print(CORE_0);
    errno_trace_clear(CORE_0);
  }
  return 0;
}

static void _check_status_rtd(
    const TestInput* t_input,
    const enum RUNNING_STATUS expected_running_status,
    const uint8_t expected_emergency_status,
    const uint8_t expected_rtd_sound_status)
{
  enum RUNNING_STATUS status = global_running_status_get();
  int8_t emergency_status= EmergencyNode_is_emergency_state(t_input->emergency_read);
  int8_t rtd_sound_status = gpio_read_state(t_input->rtd_sound);

  if (emergency_status <0)
  {
    FAILED("error emergency_status");
  }

  if (rtd_sound_status<0)
  {
    FAILED("error rtd_sound_status");
  }


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
  printf("expected_emergency_status: %d, given: %d\t",expected_emergency_status, emergency_status);
  printf("expected_rtd_sound_status: %d, given: %d\n",expected_rtd_sound_status, rtd_sound_status);
  printf("-------------------------------\n");
}

//public

void test_initial_status(TestInput* t_input)__attribute_maybe_unused__;
void test_initial_status(TestInput* t_input __attribute_maybe_unused__)
{
  _check_status_rtd(t_input, SYSTEM_OFF, 0, 0);
}

void test_rtd_seq(TestInput* t_input)__attribute_maybe_unused__;
void test_rtd_seq(TestInput* t_input __attribute_maybe_unused__)
{
  printf("switching to human driver\n");
  pcu_stop_embedded(&t_input->external_boards->pcu);
  steering_wheel_select_mission(&t_input->external_boards->steering_wheel, CAR_MISSIONS_HUMAN);
  wait_milliseconds(get_tick_from_millis(200));

  FOR_EACH_ENGINE(engine)
  {
    car_amk_inverter_set_attribute(&t_input->external_boards->amk_inverter, SYSTEM_READY, (uint8_t) engine, 1);
  }

  printf("starting precharge: SYSTEM_OFF -> SYSTEM_PRECAHRGE\n");
  bms_hv_start_button(&t_input->external_boards->bms_hv);
  wait_milliseconds(get_tick_from_millis(500));
  _check_status_rtd(t_input, SYSTEM_PRECAHRGE, 0 ,0);
}

int main(void)
{
  ExternalBoards_t external_boards = {0};
  Core0Feature_h core_0_feature = {0};
  EmergencyNode_h emergency_read = {0};
  CarMissionReader_h mission_reader = {0};
  AsNode_h as_node = {0};

  Gpio_h rf ={0};
  GpioRead_h rtd_sound_read = {0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .core_run = &core_thread.run,

    .mission_reader = &mission_reader,
    .as_node = &as_node,
    .core_0_feature = &core_0_feature,
  };

  TestInput t_input = {
    .external_boards = &external_boards,

    .rf = &rf,
    .rtd_sound = &rtd_sound_read,
    .emergency_read = &emergency_read,
  };

  INIT_PH(create_virtual_chip(), "virtual chip gpio");
  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(hardware_init_gpio(&rf, GPIO_RTD_BUTTON), "rf gpio");
  INIT_PH(hardware_init_read_permission_gpio(&rtd_sound_read, GPIO_RTD_ASSI_SOUND), "rtd sound gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(car_mission_reader_init(&mission_reader), "mission reader");
  INIT_PH(as_node_init(&as_node, &mission_reader), "as_node");
  INIT_PH(core_0_feature_init(&core_0_feature), "core_0_feature");
  INIT_PH(EmergencyNode_init(&emergency_read),"emergency_read");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  printf("tests finished\n");

  test_initial_status(&t_input);
  test_rtd_seq(&t_input);

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);

  stop_external_boards(&external_boards);
  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
