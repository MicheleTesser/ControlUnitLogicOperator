#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_2/feature/DV/ebs/ebs.h"

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

typedef struct {
  DvEbs_h* ebs;

  volatile const uint8_t* const core_run;
}CoreInput;

typedef struct{
  ExternalBoards_t* external_boards;
  Gpio_h* ts_button;
  Gpio_h* rf_button;

  DvEbs_h* ebs;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
    ebs_update(core_input->ebs);
  }
  return 0;
}

#define _check_status(bool_exp, check_str)\
  if (bool_exp)\
  {\
    PASSED("consisten state: "check_str);\
  }\
  else\
  {\
    FAILED("inconsisten state: "check_str);\
  }


//public

void test_ebs_initial_state(TestInput* t_input)
{
  _check_status(!ebs_on(t_input->ebs), "initial state ebs: inactive");
}

void test_ebs_test_activation_of_ebs_human_driver(TestInput* t_input)
{
  printf("human driver mode\n");
  asb_reset(&t_input->external_boards->asb);
  asb_set_parameter(&t_input->external_boards->asb, SYSTEM_CHECK, 1);
  steering_wheel_select_mission(&t_input->external_boards->steering_wheel, CAR_MISSIONS_HUMAN);

  while (car_amk_inverter_precharge_status(&t_input->external_boards->amk_inverter)!=PRECHARGE_FINISHED)
  {
    car_amk_inverter_force_precharge_status(&t_input->external_boards->amk_inverter);
    wait_milliseconds(5 SECONDS);
  }

  _check_status(!ebs_on(t_input->ebs), "precharge done and ebs is currently off");
  _check_status(
      ebs_asb_consistency_check(t_input->ebs) == EBS_NO,
      "started consistency check of ebs");

  wait_milliseconds(1 SECONDS);

  enum ASB_INTEGRITY_CHECK_RESULT consistency = ebs_asb_consistency_check(t_input->ebs);
  _check_status(consistency == EBS_NO,"ebs consistency still not done");
  printf("expected: %d, given %d\n", EBS_NO, consistency);
  gpio_set_high(t_input->ts_button);
}

void test_ebs_test_activation_of_ebs_dv_driver(TestInput* t_input)
{
  printf("dv driver mode\n");
  asb_reset(&t_input->external_boards->asb);
  asb_set_parameter(&t_input->external_boards->asb, SYSTEM_CHECK, 1);
  car_amk_inverter_reset(&t_input->external_boards->amk_inverter);

  steering_wheel_select_mission(&t_input->external_boards->steering_wheel, CAR_MISSIONS_DV_EBS_TEST);
  asb_set_parameter(&t_input->external_boards->asb, CURR_MISSION, CAR_MISSIONS_DV_EBS_TEST);
  while (car_amk_inverter_precharge_status(&t_input->external_boards->amk_inverter)!=PRECHARGE_FINISHED)
  {
    car_amk_inverter_force_precharge_status(&t_input->external_boards->amk_inverter);
    wait_milliseconds(5 SECONDS);
  }

  asb_set_parameter(&t_input->external_boards->asb, INTEGRITY_CHECK_STATUS, 1);
  wait_milliseconds(500 MILLIS);

  _check_status(ebs_on(t_input->ebs), "precharge done and ebs is currently on");
  _check_status(
      ebs_asb_consistency_check(t_input->ebs) == EBS_NO,
      "started consistency check of ebs");

  wait_milliseconds(3 SECONDS);

  enum ASB_INTEGRITY_CHECK_RESULT consistency = ebs_asb_consistency_check(t_input->ebs);
  _check_status(consistency == EBS_OK,"ebs consistency done with success");
  printf("expected: %d, given %d\n", EBS_OK, consistency);

  EbsPhaes_t current_asb_phase = asb_current_phase(&t_input->external_boards->asb);
  _check_status(current_asb_phase == EbsPhase_3, "consistency done and asb received the ack from MCU");
  printf("expected: %d, given %d\n", EbsPhase_3, current_asb_phase);
}

int main(void)
{
  Gpio_h ts_button = {0};
  Gpio_h rtd_button= {0};

  ExternalBoards_t external_boards = {0};
  DvEbs_h ebs = {0};

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .ebs = &ebs,

    .core_run = &core_thread.run,
  };

  TestInput t_input = {
    .ts_button = &ts_button,
    .rf_button = &rtd_button,
    .external_boards = &external_boards,
    .ebs = &ebs,
  };

  INIT_PH(hardware_init_gpio(&ts_button, GPIO_TS_BUTTON),"ts button");
  INIT_PH(hardware_init_gpio(&rtd_button, GPIO_RTD_BUTTON),"rf button");
  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(ebs_class_init(&ebs), "ebs");

  pcu_start_embedded(&external_boards.pcu);

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_ebs_initial_state(&t_input);
  test_ebs_test_activation_of_ebs_human_driver(&t_input);
  test_ebs_test_activation_of_ebs_dv_driver(&t_input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);

  hardware_can_node_debug_print_status();
  stop_external_boards(&external_boards);
  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
