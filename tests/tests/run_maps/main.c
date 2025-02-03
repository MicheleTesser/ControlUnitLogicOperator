#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "lib/board_dbc/dbc/out_lib/can2/can2.h"
#include "linux_board/linux_board.h"
#include "src/cores/core_0/feature/maps/maps.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

struct MapsData{
  float power_kw;
  float torque_pos;
  float torque_neg;
  float regen_scale;
  float repartition;
  float tv_enable;
};


static void get_data(DrivingMaps_h* maps, struct MapsData *data)
{
  data->power_kw=driving_map_get_parameter(maps, POWER_KW);
  data->torque_pos=driving_map_get_parameter(maps, MAX_POS_TORQUE);
  data->torque_neg=driving_map_get_parameter(maps, MAX_NEG_TORQUE);
  data->regen_scale=driving_map_get_parameter(maps, REGEN_SCALE);
  data->repartition=driving_map_get_parameter(maps, TORQUE_REPARTITION);
  data->tv_enable=driving_map_get_parameter(maps, TV_ON);
}

static int test_default_active_maps(DrivingMaps_h* maps)
{
  struct MapsData data={0};
  get_data(maps, &data);

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
  }

  return 0;
}

static void check_power_map(DrivingMaps_h* maps, uint8_t MAP_NUM, float KW, float TORQUE_POS)
{
  struct MapsData data={0};
  can_obj_can2_h_t o={0};
  CanMessage mex={0};

  o.can_0x064_Map.power= MAP_NUM;
  mex.message_size = pack_message_can2(&o, CAN_ID_MAP, &mex.full_word);
  mex.id = CAN_ID_MAP;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
      hardware_write_can(can_node, &mex);
  })

  wait_milliseconds(2 MILLIS);
  get_data(maps, &data);
  printf("checking test map :%d\t", MAP_NUM );
  if(data.power_kw == KW && data.torque_pos == TORQUE_POS){
    PASSED("Power Map setted correctly");
  }else{
    FAILED("Power Map setted wrongly");
  }
}

static void check_regen_map(DrivingMaps_h* maps, uint8_t MAP_NUM, float REGEN_SCALE, float TORQUE_NEG)
{
  struct MapsData data={0};
  can_obj_can2_h_t o={0};
  CanMessage mex={0};

  o.can_0x064_Map.regen= MAP_NUM;
  mex.message_size = pack_message_can2(&o, CAN_ID_MAP, &mex.full_word);
  mex.id = CAN_ID_MAP;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
      hardware_write_can(can_node, &mex);
  })

  wait_milliseconds(2 MILLIS);
  get_data(maps, &data);
  printf("checking test map :%d\t", MAP_NUM );
  if(data.regen_scale == REGEN_SCALE && data.torque_neg == TORQUE_NEG){
    PASSED("Regen Map setted correctly");
  }else{
    FAILED("Regen Map setted wrongly");
  }
}

static void check_repartition_map(DrivingMaps_h* maps, uint8_t MAP_NUM, float REPARTITION, float TV_ON)
{
  struct MapsData data={0};
  can_obj_can2_h_t o={0};
  CanMessage mex={0};

  o.can_0x064_Map.torque_rep= MAP_NUM;
  mex.message_size = pack_message_can2(&o, CAN_ID_MAP, &mex.full_word);
  mex.id = CAN_ID_MAP;
  ACTION_ON_CAN_NODE(CAN_GENERAL,{
      hardware_write_can(can_node, &mex);
  })
  wait_milliseconds(2 MILLIS);
  get_data(maps, &data);
  printf("checking test map :%d\t", MAP_NUM );
  if(data.repartition == REPARTITION && data.tv_enable == TV_ON){
    PASSED("Repartition Map setted correctly");
  }else{
    FAILED("Repartition Map setted wrongly");
  }
}

static int test_change_power_map(DrivingMaps_h* maps)
{
  check_power_map(maps, 0, 77, 21);
  check_power_map(maps, 1, 75, 20);
  check_power_map(maps, 2, 70, 18);
  check_power_map(maps, 3, 60, 16);
  check_power_map(maps, 4, 50, 15);
  check_power_map(maps, 5, 40, 15);
  check_power_map(maps, 6, 35, 13);
  check_power_map(maps, 7, 30, 13);
  check_power_map(maps, 8, 15, 12);
  check_power_map(maps, 9, 10, 10);

  return 0;
}

static int test_change_regen_map(DrivingMaps_h* maps)
{
  check_regen_map(maps, 0, 0, 0);
  check_regen_map(maps, 1, 20, -8);
  check_regen_map(maps, 2, 30, -10);
  check_regen_map(maps, 3, 40, -12);
  check_regen_map(maps, 4, 50, -15);
  check_regen_map(maps, 5, 60, -17);
  check_regen_map(maps, 6, 70, -18);
  check_regen_map(maps, 7, 80, -19);
  check_regen_map(maps, 8, 90, -20);
  check_regen_map(maps, 9, 100, -21);

  return 0;
}

static int test_change_repartition_map(DrivingMaps_h* maps)
{
  check_repartition_map(maps, 0, 0.50, 1);
  check_repartition_map(maps, 1, 1.0f, 0);
  check_repartition_map(maps, 2, 0.82, 0);
  check_repartition_map(maps, 3, 0.80, 0);
  check_repartition_map(maps, 4, 0.78, 0);
  check_repartition_map(maps, 5, 0.75, 0);
  check_repartition_map(maps, 6, 0.70, 0);
  check_repartition_map(maps, 7, 0.60, 0);
  check_repartition_map(maps, 8, 0.50, 0);
  check_repartition_map(maps, 9, 0.50, 0);

  return 0;
}

int run=1;
int core_map(void* args)
{
  driving_maps_init(args);
  while(run)
  {
    driving_map_update(args);
  }
  return 0;
}

int main(void)
{
  DrivingMaps_h maps={0};
  thrd_t map_core={0};

  if(create_virtual_chip() <0){
    goto end;
  }

  if (virtual_can_manager_init()<0)
  {
    goto end;
  }

  if (hardware_init_can(CAN_GENERAL, _500_KBYTE_S_)<0) {
    goto end;
  }

  thrd_create(&map_core, core_map, &maps);

  wait_milliseconds(5 MILLIS);


  test_default_active_maps(&maps);
  test_change_power_map(&maps);
  test_change_regen_map(&maps);
  test_change_repartition_map(&maps);

  run=0;
  thrd_join(map_core,NULL);
end:
  print_SCORE();
  return 0;
}
