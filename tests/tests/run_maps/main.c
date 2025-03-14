#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wconversion"
#include "lib/board_dbc/dbc/out_lib/can2/can2.h"
#pragma GCC diagnostic pop 
#include "linux_board/linux_board.h"
#include "./car_component/car_component.h"
#include "src/cores/core_0/feature/maps/maps.h"

#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

#define INIT_PH(init_exp, module_name)\
  if ((init_exp)<0)\
  {\
    FAILED("failed init "module_name);\
    goto end;\
  }

struct MapsData{
  float power_kw;
  float torque_pos;
  float torque_neg;
  float regen_scale;
  float repartition;
  float tv_enable;
};

typedef struct CoreThread{
  thrd_t thread_id;
  uint8_t run;
}CoreThread;

typedef struct CoreInput{
  DrivingMaps_h* maps;

  volatile const uint8_t* const core_run;
}CoreInput;

typedef struct{
  DrivingMaps_h* maps;
  ExternalBoards_t* ExternalBoards_t;

}TestInput;


static int _core_thread_fun(void* arg)__attribute_maybe_unused__;
static int _core_thread_fun(void* arg)
{
  CoreInput* const core_input = arg;
  time_var_microseconds t=0;


  while (*core_input->core_run)
  {
    ACTION_ON_FREQUENCY(t, 1 MILLIS)
    {
      driving_map_update(core_input->maps);
    }
  }
  return 0;
}

static void _get_data(DrivingMaps_h* maps, struct MapsData *data)
{
  data->power_kw=driving_map_get_parameter(maps, POWER_KW);
  data->torque_pos=driving_map_get_parameter(maps, MAX_POS_TORQUE);
  data->torque_neg=driving_map_get_parameter(maps, MAX_NEG_TORQUE);
  data->regen_scale=driving_map_get_parameter(maps, REGEN_SCALE);
  data->repartition=driving_map_get_parameter(maps, TORQUE_REPARTITION);
  data->tv_enable=driving_map_get_parameter(maps, TV_ON);
}

static void _print_map_status(struct MapsData* data)
{
  printf("power_kw: %f\t",data->power_kw);
  printf("pos_torque : %f\t",data->torque_pos);
  printf("neg_torque : %f\t",data->torque_neg);
  printf("regen_scale : %f\t",data->regen_scale);
  printf("tv_enable: %f\t",data->tv_enable);
  printf("repartition : %f\n",data->repartition);
}



static void _check_power_map(TestInput* t_input, uint8_t MAP_NUM, float KW, float TORQUE_POS)
{
  struct MapsData data={0};

  steering_wheel_select_map(&t_input->ExternalBoards_t->steering_wheel, MAPS_TYPE_POWER, MAP_NUM);
  wait_milliseconds(75 MILLIS);

  _get_data(t_input->maps, &data);
  printf("checking test map :%d\t", MAP_NUM );
  if(data.power_kw == KW && data.torque_pos == TORQUE_POS){
    PASSED("Power Map setted correctly");
  }else{
    FAILED("Power Map setted wrongly");
  }
}

static void _check_regen_map(TestInput* t_input, uint8_t MAP_NUM, float REGEN_SCALE, float TORQUE_NEG)
{
  struct MapsData data={0};

  steering_wheel_select_map(
      &t_input->ExternalBoards_t->steering_wheel, MAPS_TYPE_REGEN, MAP_NUM);
  wait_milliseconds(75 MILLIS);

  _get_data(t_input->maps, &data);
  printf("checking test map :%d\t", MAP_NUM );
  if(data.regen_scale == REGEN_SCALE && data.torque_neg == TORQUE_NEG){
    PASSED("Regen Map setted correctly");
  }else{
    FAILED("Regen Map setted wrongly");
  }
}

static void _check_repartition_map(TestInput* t_input, uint8_t MAP_NUM, float REPARTITION, float TV_ON)
{
  struct MapsData data={0};

  steering_wheel_select_map(&t_input->ExternalBoards_t->steering_wheel, MAPS_TYPE_TV_REPARTITION, MAP_NUM);
  wait_milliseconds(75 MILLIS);

  _get_data(t_input->maps, &data);
  printf("checking test map :%d\t", MAP_NUM );
  if(data.repartition == REPARTITION && data.tv_enable == TV_ON){
    PASSED("Repartition Map setted correctly");
  }else{
    FAILED("Repartition Map setted wrongly");
  }
}

//public

static int test_default_active_maps(TestInput* t_input)
{
  struct MapsData data={0};
  _get_data(t_input->maps, &data);

  struct MapsData expected_data = {
    .power_kw =77,
    .torque_pos =21,
    .regen_scale=0,
    .torque_neg =0,
    .tv_enable = 1,
    .repartition=0.50f,
  };

  if (!memcmp(&data, &expected_data, sizeof(data))) {
    PASSED("default maps ok");
  }else{
    FAILED("wrong default maps");
    printf("expected: ");
    _print_map_status(&expected_data);
    printf("given: ");
    _print_map_status(&data);
  }

  return 0;
}

static int test_change_power_map(TestInput* t_input)
{
  _check_power_map(t_input, 0, 77, 21);
  _check_power_map(t_input, 1, 75, 20);
  _check_power_map(t_input, 2, 70, 18);
  _check_power_map(t_input, 3, 60, 16);
  _check_power_map(t_input, 4, 50, 15);
  _check_power_map(t_input, 5, 40, 15);
  _check_power_map(t_input, 6, 35, 13);
  _check_power_map(t_input, 7, 30, 13);
  _check_power_map(t_input, 8, 15, 12);
  _check_power_map(t_input, 9, 10, 10);

  return 0;
}

static int test_change_regen_map(TestInput* t_input)
{
  _check_regen_map(t_input, 0, 0, 0);
  _check_regen_map(t_input, 1, 20, -8);
  _check_regen_map(t_input, 2, 30, -10);
  _check_regen_map(t_input, 3, 40, -12);
  _check_regen_map(t_input, 4, 50, -15);
  _check_regen_map(t_input, 5, 60, -17);
  _check_regen_map(t_input, 6, 70, -18);
  _check_regen_map(t_input, 7, 80, -19);
  _check_regen_map(t_input, 8, 90, -20);
  _check_regen_map(t_input, 9, 100, -21);

  return 0;
}

static int test_change_repartition_map(TestInput* t_input)
{
  _check_repartition_map(t_input, 0, 0.50, 1);
  _check_repartition_map(t_input, 1, 1.0f, 0);
  _check_repartition_map(t_input, 2, 0.82f, 0);
  _check_repartition_map(t_input, 3, 0.80f, 0);
  _check_repartition_map(t_input, 4, 0.78f, 0);
  _check_repartition_map(t_input, 5, 0.75f, 0);
  _check_repartition_map(t_input, 6, 0.70f, 0);
  _check_repartition_map(t_input, 7, 0.60f, 0);
  _check_repartition_map(t_input, 8, 0.50f, 0);
  _check_repartition_map(t_input, 9, 0.50f, 0);

  return 0;
}

int main(void)
{
  ExternalBoards_t external_boards = {0};
  CoreThread core_thread={.run=1};
  DrivingMaps_h maps={0};
  TestInput t_input = 
  {
    .ExternalBoards_t = &external_boards,
    .maps = &maps,
  };

  CoreInput input __attribute_maybe_unused__= {
    .maps = &maps,

    .core_run = &core_thread.run,
  };

  INIT_PH(hardware_init_can(CAN_INVERTER, _1_MBYTE_S_), "can inverter");
  INIT_PH(hardware_init_can(CAN_GENERAL, _500_KBYTE_S_), "can general");
  INIT_PH(hardware_init_can(CAN_DV, _500_KBYTE_S_), "can dv");
  INIT_PH(create_virtual_chip(), "virtual chip gpio");

  INIT_PH(start_external_boards(&external_boards), "external_boards");

  INIT_PH(driving_maps_init(&maps), "driver maps");

  thrd_create(&core_thread.thread_id, _core_thread_fun, &input);

  test_default_active_maps(&t_input);
  test_change_power_map(&t_input);
  test_change_regen_map(&t_input);
  test_change_repartition_map(&t_input);

  printf("tests finished\n");

  printf("stopping debug core\n");
  core_thread.run=0;
  thrd_join(core_thread.thread_id, NULL);

  stop_external_boards(&external_boards);
  stop_thread_can();

end:
  print_SCORE();
  return 0;
}
