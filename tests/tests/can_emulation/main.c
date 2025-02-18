#include "score_lib/test_lib.h"
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

typedef struct CoreThread{
  thrd_t thread_id;
  uint8_t run;
}CoreThread;

typedef struct CoreInput{
  volatile const uint8_t* const core_run;
}CoreInput;

static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
  }
  return 0;
}

//public

void test_comunication_only_internal_mailbox(void)
{
  struct CanMailbox* int_mail_1 = NULL;
  struct CanMailbox* int_mail_2 = NULL;
  struct CanMailbox* int_mail_3 = NULL;
  struct CanMailbox* int_mail_4 = NULL;

  struct CanNode* p_node = NULL;
  for (p_node =NULL;;
      (!p_node?
       (p_node = hardware_init_can_get_ref_node(CAN_INVERTER)):
       (hardware_init_can_destroy_ref_node(&p_node)))
      ){}\

}

int main(void)
{
  CoreThread core_thread={.run=1};

  CoreInput input =
  {
    .core_run = &core_thread.run,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);
end:
  print_SCORE();
  return 0;
}
