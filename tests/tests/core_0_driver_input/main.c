#include "score_lib/test_lib.h"
#include "src/cores/core_0/feature/driver_input/driver_input.h"
#include "lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "lib/board_dbc/dbc/out_lib/can3/can3.h"
#include "linux_board/linux_board.h"

#include <string.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

uint8_t run=1;
static int driver_loop(void* args)
{
  while(driver_input_init(args)<0);

  while(run)
  {
    driver_input_update(args);
  }
  return 0;
}

static int test_throttle(DriverInput_h* driver){
  int8_t err=0;
  uint8_t throttle_value = 40;
  can_obj_can2_h_t mex={0};
  CanMessage mex_c={0};
  float throttle =0;

  mex.can_0x053_Driver.no_implausibility =1;
  mex.can_0x053_Driver.throttle = throttle_value;
  mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
  mex_c.id = CAN_ID_DRIVER;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
      hardware_write_can(can_node, &mex_c);
  });

  sleep(1);
  throttle = driver_input_get(driver, THROTTLE);

  if (throttle == throttle_value) {
    PASSED("throttle value setted correctly");
  }else{
    FAILED("throttle value set fail");
    err--;
  }

  throttle_value = 67;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));
  mex.can_0x053_Driver.no_implausibility =1;
  mex.can_0x053_Driver.throttle = throttle_value;
  mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
  mex_c.id = CAN_ID_DRIVER;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
      hardware_write_can(can_node, &mex_c);
  });
  sleep(1);

  throttle = driver_input_get(driver, THROTTLE);
  if (throttle == throttle_value) {
    PASSED("throttle value upadte correctly");
  }else{
    FAILED("brake value update fail");
    err--;
  }

  return err;
}

static int test_brake(DriverInput_h* driver){
  int8_t err=0;
  uint8_t brk_value = 42;
  can_obj_can2_h_t mex={0};
  CanMessage mex_c={0};
  float brake =0;

  mex.can_0x053_Driver.no_implausibility =1;
  mex.can_0x053_Driver.brake = brk_value;
  mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
  mex_c.id = CAN_ID_DRIVER;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
    hardware_write_can(can_node, &mex_c);
  })
  sleep(1);
  brake = driver_input_get(driver, BRAKE);

  if (brake == brk_value) {
    PASSED("brake value setted correctly");
  }else{
    FAILED("brake value set fail");
    err--;
  }

  brk_value = 30;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));
  mex.can_0x053_Driver.no_implausibility =1;
  mex.can_0x053_Driver.brake = brk_value;
  mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
  mex_c.id = CAN_ID_DRIVER;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
    hardware_write_can(can_node, &mex_c);
  })

  sleep(1);
  brake = driver_input_get(driver, BRAKE);

  if (brake == brk_value) {
    PASSED("brake value upadte correctly");
  }else{
    FAILED("brake value update fail");
    err--;
  }

  return err;
}

static int test_steering_wheel(DriverInput_h* driver){
  int8_t err=0;
  uint8_t steering_value = 10;
  can_obj_can2_h_t mex;
  CanMessage mex_c;
  float stw =0;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));
  mex.can_0x053_Driver.steering = steering_value;
  mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
  mex_c.id = CAN_ID_DRIVER;
  ACTION_ON_CAN_NODE(CAN_GENERAL, {
    hardware_write_can(can_node, &mex_c);
  })
  sleep(1);

  stw = driver_input_get(driver, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("steering value setted correctly");
  }else{
    FAILED("steering value set fail");
    err--;
  }

  steering_value = 88;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));
  mex.can_0x053_Driver.steering = steering_value;
  mex_c.message_size = pack_message_can2(&mex, CAN_ID_DRIVER, &mex_c.full_word);
  mex_c.id = CAN_ID_DRIVER;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
    hardware_write_can(can_node, &mex_c);
  })

  sleep(1);
  stw = driver_input_get(driver, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("steering value upadte correctly");
  }else{
    FAILED("brake value update fail");
    err--;
  }

  return err;
}

static int test_throttle_dv(DriverInput_h* driver){
  int8_t err=0;
  uint8_t throttle_value = 40;
  can_obj_can3_h_t mex={0};
  CanMessage mex_c={0};
  float throttle =0;

  mex.can_0x07d_DV_Driver.Throttle= throttle_value;
  mex_c.id = CAN_ID_DV_DRIVER;
  mex_c.message_size = pack_message_can3(&mex, mex_c.id, &mex_c.full_word);
  ACTION_ON_CAN_NODE(CAN_DV,{
      hardware_write_can(can_node, &mex_c);
  });

  sleep(1);
  throttle = driver_input_get(driver, THROTTLE);

  if (throttle == throttle_value) {
    PASSED("dv throttle value setted correctly");
  }else{
    FAILED("dv throttle value set fail");
    err--;
  }

  throttle_value = 67;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));

  mex.can_0x07d_DV_Driver.Throttle = throttle_value;
  mex_c.id = CAN_ID_DV_DRIVER;
  mex_c.message_size = pack_message_can3(&mex, mex_c.id, &mex_c.full_word);
  ACTION_ON_CAN_NODE(CAN_DV,{
      hardware_write_can(can_node, &mex_c);
  });
  sleep(1);

  throttle = driver_input_get(driver, THROTTLE);
  if (throttle == throttle_value) {
    PASSED("dv throttle value upadte correctly");
  }else{
    FAILED("dv brake value update fail");
    err--;
  }

  return err;
}

static int test_brake_dv(DriverInput_h* driver){
  int8_t err=0;
  uint8_t brk_value = 42;
  can_obj_can3_h_t mex={0};
  CanMessage mex_c={0};
  float brake =0;

  mex.can_0x07d_DV_Driver.Brake = brk_value;
  mex_c.id = CAN_ID_DV_DRIVER;
  mex_c.message_size = pack_message_can3(&mex, mex_c.id, &mex_c.full_word);
  ACTION_ON_CAN_NODE(CAN_DV,{
    hardware_write_can(can_node, &mex_c);
  })
  sleep(1);
  brake = driver_input_get(driver, BRAKE);

  if (brake == brk_value) {
    PASSED("dv brake value setted correctly");
  }else{
    FAILED("dv brake value set fail");
    err--;
  }

  brk_value = 30;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));

  mex.can_0x07d_DV_Driver.Brake = brk_value;
  mex_c.id = CAN_ID_DV_DRIVER;
  mex_c.message_size = pack_message_can3(&mex, mex_c.id, &mex_c.full_word);
  ACTION_ON_CAN_NODE(CAN_DV,{
    hardware_write_can(can_node, &mex_c);
  })

  sleep(1);
  brake = driver_input_get(driver, BRAKE);

  if (brake == brk_value) {
    PASSED("dv brake value upadte correctly");
  }else{
    FAILED("dv brake value update fail");
    err--;
  }

  return err;
}

static int test_steering_wheel_dv(DriverInput_h* driver){
  int8_t err=0;
  uint8_t steering_value = 10;
  can_obj_can3_h_t mex;
  CanMessage mex_c;
  float stw =0;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));
  mex.can_0x07d_DV_Driver.Steering_angle= steering_value;
  mex_c.id = CAN_ID_DV_DRIVER;
  mex_c.message_size = pack_message_can3(&mex, mex_c.id, &mex_c.full_word);
  ACTION_ON_CAN_NODE(CAN_DV, {
    hardware_write_can(can_node, &mex_c);
  })
  sleep(1);

  stw = driver_input_get(driver, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("dv steering value setted correctly");
  }else{
    FAILED("dv steering value set fail");
    err--;
  }

  steering_value = 88;
  memset(&mex, 0, sizeof(mex));
  memset(&mex_c, 0, sizeof(mex_c));
  mex.can_0x07d_DV_Driver.Steering_angle= steering_value;
  mex_c.id = CAN_ID_DV_DRIVER;
  mex_c.message_size = pack_message_can3(&mex, mex_c.id, &mex_c.full_word);
  ACTION_ON_CAN_NODE(CAN_DV,{
    hardware_write_can(can_node, &mex_c);
  })

  sleep(1);
  stw = driver_input_get(driver, STEERING_ANGLE);

  if (stw == steering_value) {
    PASSED("dv steering value upadte correctly");
  }else{
    FAILED("dv brake value update fail");
    err--;
  }

  return err;
}

int main(void)
{
  int8_t err=0;
  thrd_t driver=0;
  DriverInput_h o_driver={0};

  hardware_init_can(CAN_GENERAL, _500_KBYTE_S_);
  hardware_init_can(CAN_DV, _500_KBYTE_S_);
  if(create_virtual_chip() <0){
    err--;
    goto end;
  }

  if (virtual_can_manager_init()<0) {
    goto end;
  }

  thrd_create(&driver, driver_loop, &o_driver);
  sleep(3);

  driver_input_change_driver(&o_driver, DRIVER_HUMAN);
  test_throttle(&o_driver);
  test_brake(&o_driver);
  test_steering_wheel(&o_driver);

  driver_input_change_driver(&o_driver, DRIVER_EMBEDDED);
  test_throttle_dv(&o_driver);
  test_brake_dv(&o_driver);
  test_steering_wheel_dv(&o_driver);

end:
  run=0;
  print_SCORE();
  return err;
}
