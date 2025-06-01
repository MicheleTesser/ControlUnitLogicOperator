#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_utility/core_utility.h"
#include "src/lib/DPS/dps.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  SytemSettingOwner_h* system_settings;
  DpsMaster_h* dps_master;
}CoreInput;

static int _core_thread_fun(void* arg) __attribute__((__unused__));
static int _core_thread_fun(void* arg)
{
  CoreInput* core_input = arg;
  while (*core_input->core_run)
  {
    system_settings_update(core_input->system_settings);
  }
  return 0;
}


static struct CanMailbox* MAILBOX_SEND_DPS_MASTER = NULL;
static struct CanMailbox* MAILBOX_RECV_DPS_MASTER = NULL;
static int8_t _can_send_fun(const DpsCanMessage* const restrict mex)
{
  return hardware_mailbox_send(MAILBOX_SEND_DPS_MASTER, mex->full_word);
}

static int _dps_update_fun(void* arg)
{
  CoreInput* core_input = arg;
  CanMessage mex = {0};
  while (*core_input->core_run)
  {
    if (hardware_mailbox_read(MAILBOX_RECV_DPS_MASTER, &mex))
    {
      DpsCanMessage dps_mex = 
      {
        .id = (uint16_t) mex.id,
        .dlc = mex.message_size,
        .full_word = mex.full_word,
      };
      dps_master_check_mex_recv(core_input->dps_master, &dps_mex);
    }
  }
  return 0;
}

void _check_value_var(DpsMaster_h* const restrict dps_master, uint8_t expected)
{
  VarRecord var_val = {0};
  dps_master_refresh_value_var_all(dps_master, 0);
  sleep(1);
  if(dps_master_get_value_var(dps_master, 0, 0, &var_val)<0)
  {
    FAILED("failed getting value of var");
  }

  if (var_val.value != expected)
  {
    FAILED("invalid default value of CXST");
  }
  else
  {
    PASSED("valid default value of CXST");
  }
  printf("expected: %d, given: %d\n", expected, var_val.value);
}

//public

void test_discovery(DpsMaster_h* const restrict dps_master)
{
  dps_master_new_connection(dps_master);
  sleep(1);
  BoardListInfo* boards = dps_master_list_board(dps_master);

  if (!boards || boards->board_num != 1)
  {
    FAILED("invalid board num: ");
  }
  else
  {
    PASSED("valid board num: ");
  }
  printf("expected: %d, given: %d\n", 1, boards->board_num);

  for (uint8_t i=0; i<boards->board_num; i++)
  {
    dps_master_request_info_board(dps_master, boards->boards[i].id, REQ_VAR);
    sleep(1);
  }

  VarListInfo* vars = dps_master_list_vars(dps_master, 0);
  if (vars->var_num != 2)
  {
    FAILED("invalid num of system_settings: ");
  }
  else
  {
    PASSED("valid num of system_settings: ");
  }
  printf("given: %d, expected: %d\n", vars->var_num, 1);

  for (uint8_t i=0; i<vars->var_num; i++)
  {
    dps_master_refresh_value_var_all(dps_master, 0);
    sleep(1);
  }

  uint8_t cxst_val = 0;
  _check_value_var(dps_master, cxst_val);

  cxst_val = 1;
  dps_master_update_var(dps_master, 0, 0, &cxst_val, sizeof(cxst_val));
  _check_value_var(dps_master, cxst_val);

  cxst_val = 2;
  dps_master_update_var(dps_master, 0, 0, &cxst_val, sizeof(cxst_val));
  _check_value_var(dps_master, cxst_val);

  free(vars);
  free(boards);
}

int main(void)
{
  SytemSettingOwner_h system_settings = {0};
  DpsMaster_h dps_master ={0};

  CoreThread core_thread={.run=1};
  CoreThread dps_thread={.run=1};
  CoreInput input =
  {
    .core_run = &core_thread.run,
    .system_settings = &system_settings,
    .dps_master = &dps_master,
  };
  struct CanNode* can_node = hardware_init_new_external_node(CAN_GENERAL);

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  MAILBOX_RECV_DPS_MASTER = hardware_get_mailbox_single_mex(
      can_node,
      RECV_MAILBOX,
      CAN_ID_DPSSLAVEMEX,
      (uint16_t) message_dlc(CAN_ID_DPSSLAVEMEX));

  MAILBOX_SEND_DPS_MASTER = hardware_get_mailbox_single_mex(
      can_node,
      SEND_MAILBOX,
      CAN_ID_DPSMASTERMEX,
      (uint16_t) message_dlc(CAN_ID_DPSMASTERMEX));

  INIT_PH(system_settings_init(&system_settings), "system_settings");
  INIT_PH(dps_master_init(&dps_master, CAN_ID_DPSMASTERMEX, CAN_ID_DPSSLAVEMEX, _can_send_fun), "dps master");

  thrd_create(&dps_thread.thread_id, _dps_update_fun, &input);
  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_discovery(&dps_master);


  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);

  hardware_free_mailbox_can(&MAILBOX_RECV_DPS_MASTER);
  hardware_free_mailbox_can(&MAILBOX_SEND_DPS_MASTER);
  hardware_init_new_external_node_destroy(can_node);
  stop_thread_can();
  dps_master_destroy(&dps_master);
end:
  print_SCORE();
  return 0;
}
