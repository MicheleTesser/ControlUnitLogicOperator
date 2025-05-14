#include "score_lib/test_lib.h"
#include "src/cores/core_0/feature/driver_input/driver_input.h"
#include "linux_board/linux_board.h"
#include "car_component/car_component.h"
#include "src/cores/core_utility/core_utility.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
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
  CarMissionReader_h* mission_reader;
  SharedMessageOwner_h* shared_memory;

  int8_t run;
};

typedef struct{
  DriverInput_h* driver_input;
  ExternalBoards_t* external_boards;
}TestInput;

static int driver_loop(void* args)
{
  struct ThInput* input = args;

  while(input->run)
  {
    car_mission_reader_update(input->mission_reader);
    giei_driver_input_update(input->driver_input);
    shared_message_owner_update(input->shared_memory);
  }
  return 0;
}

static int test_throttle(TestInput* input){
  int8_t err=0;
  uint8_t throttle_value = 40;
  float throttle =0;

  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_THROTTLE, throttle_value);

  wait_milliseconds(get_tick_from_millis(200));
  throttle = giei_driver_input_get(input->driver_input, THROTTLE);

  if (throttle == throttle_value) {
    PASSED("throttle value setted correctly");
  }else{
    FAILED("throttle value set fail");
    printf("expected: %d, gived: %f\n",throttle_value, throttle);
    err--;
  }

  throttle_value = 67;
  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_THROTTLE, throttle_value);
  wait_milliseconds(get_tick_from_millis(200));

  throttle = giei_driver_input_get(input->driver_input, THROTTLE);
  if (throttle == throttle_value) {
    PASSED("throttle value upadte correctly");
  }else{
    FAILED("throttle value update fail");
    printf("expected: %d, gived: %f\n",throttle_value, throttle);
    err--;
  }

  return err;
}

static int test_brake(TestInput* input){
  int8_t err=0;
  uint8_t brk_value = 42;
  float brake =0;

  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_BRAKE, brk_value);
  wait_milliseconds(get_tick_from_millis(200));

  brake = giei_driver_input_get(input->driver_input, BRAKE);

  if (brake == brk_value) {
    PASSED("brake value setted correctly");
  }else{
    FAILED("brake value set fail");
    printf("expected: %d, gived: %f\n",brk_value, brake);
    err--;
  }

  brk_value = 30;
  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_BRAKE, brk_value);
  wait_milliseconds(get_tick_from_millis(200));
  brake = giei_driver_input_get(input->driver_input, BRAKE);

  if (brake == brk_value) {
    PASSED("brake value upadte correctly");
  }else{
    FAILED("brake value update fail");
    printf("expected: %d, gived: %f\n",brk_value, brake);
    err--;
  }

  return err;
}

static int test_steering_wheel(TestInput* input)
{
  int8_t err=0;
  uint8_t steering_value = 10;
  float stw =0;

  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_STEERING_ANGLE, steering_value);
  wait_milliseconds(get_tick_from_millis(200));

  stw = giei_driver_input_get(input->driver_input, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("steering value setted correctly");
  }else{
    FAILED("steering value set fail");
    printf("expected: %d, gived: %f\n",steering_value, stw);
    err--;
  }

  steering_value = 88;
  atc_pedals_steering_wheel(&input->external_boards->atc, ATC_STEERING_ANGLE, steering_value);
  wait_milliseconds(get_tick_from_millis(200));
  stw = giei_driver_input_get(input->driver_input, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("steering value upadte correctly");
  }else{
    FAILED("steering value update fail");
    printf("expected: %d, gived: %f\n",steering_value, stw);
    err--;
  }

  return err;
}

static int test_throttle_dv(TestInput* input)
{
  int8_t err=0;
  uint8_t throttle_value = 40;
  float throttle =0;

  embedded_system_set_dv_input(
      &input->external_boards->embedded_system,
      DV_INPUT_THROTTLE,
      throttle_value);

  wait_milliseconds(get_tick_from_millis(50));
  throttle = giei_driver_input_get(input->driver_input, THROTTLE);

  if (throttle == throttle_value) {
    PASSED("dv throttle value setted correctly");
  }else{
    FAILED("dv throttle value set fail");
    err--;
  }
  printf("expected: %d, gived: %f\n",throttle_value, throttle);

  throttle_value = 67;

  embedded_system_set_dv_input(
      &input->external_boards->embedded_system,
      DV_INPUT_THROTTLE,
      throttle_value);
  wait_milliseconds(get_tick_from_millis(50));

  throttle = giei_driver_input_get(input->driver_input, THROTTLE);
  if (throttle == throttle_value) {
    PASSED("dv throttle value upadte correctly");
  }else{
    FAILED("dv brake value update fail");
    err--;
  }
  printf("expected: %d, gived: %f\n",throttle_value, throttle);

  return err;
}

static int test_brake_dv(TestInput* input)
{
  int8_t err=0;
  uint8_t brk_value = 42;
  float brake =0;

  embedded_system_set_dv_input(
      &input->external_boards->embedded_system,
      DV_INPUT_BRAKE,
      brk_value);
  wait_milliseconds(get_tick_from_millis(50));

  brake = giei_driver_input_get(input->driver_input, BRAKE);

  if (brake == brk_value) {
    PASSED("dv brake value setted correctly");
  }else{
    FAILED("dv brake value set fail");
    err--;
  }
  printf("expected: %d, gived: %f\n",brk_value, brake);

  brk_value = 30;

  embedded_system_set_dv_input(
      &input->external_boards->embedded_system,
      DV_INPUT_BRAKE,
      brk_value);
  wait_milliseconds(get_tick_from_millis(50));

  brake = giei_driver_input_get(input->driver_input, BRAKE);

  if (brake == brk_value) {
    PASSED("dv brake value upadte correctly");
  }else{
    FAILED("dv brake value update fail");
    err--;
  }
  printf("expected: %d, gived: %f\n",brk_value, brake);

  return err;
}

static int test_steering_wheel_dv(TestInput* input){
  int8_t err=0;
  uint8_t steering_value = 10;
  float stw =0;

  embedded_system_set_dv_input(
      &input->external_boards->embedded_system,
      DV_INPUT_STEERING_ANGLE,
      steering_value);
  wait_milliseconds(get_tick_from_millis(50));

  stw = giei_driver_input_get(input->driver_input, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("dv steering value setted correctly");
  }else{
    FAILED("dv steering value set fail");
    err--;
  }
  printf("expected: %d, gived: %f\n",steering_value, stw);

  steering_value = 88;

  embedded_system_set_dv_input(
      &input->external_boards->embedded_system,
      DV_INPUT_STEERING_ANGLE,
      steering_value);

  wait_milliseconds(get_tick_from_millis(200));
  stw = giei_driver_input_get(input->driver_input, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("dv steering value upadte correctly");
  }else{
    FAILED("dv brake value update fail");
    err--;
  }
  printf("expected: %d, gived: %f\n",steering_value, stw);

  return err;
}

int main(void)
{
  ExternalBoards_t external_boards = {0};
  SharedMessageOwner_h shared_memory = {0};

  CarMissionReader_h mission_reader = {0};
  DriverInput_h o_driver={0};

  int8_t err=0;
  thrd_t driver=0;

  struct ThInput input = {
    .driver_input = &o_driver,
    .mission_reader = &mission_reader,
    .shared_memory = &shared_memory,

    .run=1,
  };

  TestInput t_input ={
    .driver_input = &o_driver,
    .external_boards = &external_boards,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(shared_message_owner_init(&shared_memory), "shared_memory");
  INIT_PH(car_mission_reader_init(&mission_reader), "car mission reader");
  INIT_PH(driver_input_init(&o_driver, &mission_reader), "driver input");

  thrd_create(&driver, driver_loop, &input);

  steering_wheel_select_mission(&external_boards.steering_wheel, CAR_MISSIONS_HUMAN);
  wait_milliseconds(get_tick_from_millis(50));
  test_throttle(&t_input);
  test_brake(&t_input);
  test_steering_wheel(&t_input);

  pcu_start_embedded(&external_boards.pcu);
  steering_wheel_select_mission(&external_boards.steering_wheel, CAR_MISSIONS_DV_EBS_TEST);
  wait_milliseconds(get_tick_from_millis(50));
  test_throttle_dv(&t_input);
  test_brake_dv(&t_input);
  test_steering_wheel_dv(&t_input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  input.run=0;
  thrd_join(driver,NULL);

  hardware_can_node_debug_print_status();
  stop_external_boards(&external_boards);
  stop_thread_can();

end:
  print_SCORE();
  return err;
}
