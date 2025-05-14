#include "score_lib/test_lib.h"
#include "src/cores/core_utility/shared_message/shared_message.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "../../../lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "../../../lib/board_dbc/dbc/out_lib/can3/can3.h"
#pragma GCC diagnostic pop
#include "linux_board/linux_board.h"
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

#define _check_condition(bool_exp, cond_str)\
{\
  (bool_exp)?PASSED("passed: "cond_str):FAILED("failed: "cond_str);\
}

typedef struct{
  thrd_t thread_id;
  uint8_t run;
}CoreThread;

typedef struct{
  volatile const uint8_t* const core_run;
}CoreInput;

typedef struct{
  ExternalBoards_t* external_boards;
}TestInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
  }
  return 0;
}

//public

union SharedMessageOwner_h_t_conv{
  SharedMessageOwner_h* const restrict hidden;
  struct shared_message_owner_t* const restrict clear;
};


void test_double_message_owner(void)
{
  SharedMessageOwner_h second_owner = {0};
  _check_condition(shared_message_owner_init(&second_owner) == -1, "Second message owner creation attempt")
}

void test_shared_message_implementation(
  SharedMessageOwner_h* owner,
  SharedMessageReader_h* reader_0,
  SharedMessageReader_h* reader_1,
  struct CanMailbox* p_mailbox_send)
{
  uint64_t message_0 = 0, message_1 = 1;

  hardware_mailbox_send(p_mailbox_send, (uint64_t)42);
  wait_milliseconds(get_tick_from_micros(100));
  shared_message_owner_update(owner);
  shared_message_read(reader_0, &message_0);
  shared_message_read(reader_1, &message_1);
  _check_condition(message_0 == 42 && message_1 == 42, "both readers can read messages")

  hardware_mailbox_send(p_mailbox_send, (uint64_t)720);
  wait_milliseconds(get_tick_from_micros(100));
  hardware_mailbox_send(p_mailbox_send, (uint64_t)721);
  wait_milliseconds(get_tick_from_micros(100));
  shared_message_owner_update(owner);
  shared_message_read(reader_0, &message_0);
  _check_condition(message_0 == 721 && message_1 == 42, "reader updates only when necessary")

  int8_t result_0, result_1;
  result_0 = shared_message_read(reader_0, &message_0);
  hardware_mailbox_send(p_mailbox_send, (uint64_t)721);
  wait_milliseconds(get_tick_from_micros(100));
  shared_message_owner_update(owner);
  result_1 = shared_message_read(reader_0, &message_0);
  _check_condition(result_0 == 0 && result_1 == 1, "timestamps work")
}


int main(void)
{
  SharedMessageOwner_h message_owner = {0};
  SharedMessageReader_h message_reader_0 = {0}, message_reader_1 = {0};
  struct CanNode* p_can_node = NULL;
  struct CanMailbox* p_can_mailbox = NULL;

  CoreThread core_thread={.run=1};
  CoreInput input =
  {
    .core_run = &core_thread.run,
  };

  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");

  if(shared_message_owner_init(&message_owner) < 0)
  {
    goto end;
  }

  test_double_message_owner();

  ACTION_ON_CAN_NODE(CAN_GENERAL, p_can_node){
    p_can_mailbox = hardware_get_mailbox(
      p_can_node,
      SEND_MAILBOX,
      CAN_ID_IMU1,
      (uint16_t)~0,
      message_dlc_can2(CAN_ID_IMU1));
  }
  if(!p_can_mailbox)
  {
    goto end;
  }

  if(shared_message_reader_init(&message_reader_0, SHARED_MEX_IMU1) < 0){
    FAILED("Could not initialize reader 0");
    goto end;
  }
  if(shared_message_reader_init(&message_reader_1, SHARED_MEX_IMU1) < 0)
  {
    FAILED("Could not initialize reader 1");
    goto end;
  }

  test_shared_message_implementation(&message_owner, &message_reader_0, &message_reader_1, p_can_mailbox);

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);

  stop_thread_can();
end:
  print_SCORE();
  return 0;
}
