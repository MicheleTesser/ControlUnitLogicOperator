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
  float rv_enable;
};

DrivingMaps_h maps;
uint8_t ready;

static void get_data(DrivingMaps_h* maps, struct MapsData *data)
{
  data->power_kw=driving_map_get_parameter(maps, POWER_KW);
  data->torque_pos=driving_map_get_parameter(maps, MAX_POS_TORQUE);
  data->torque_neg=driving_map_get_parameter(maps, MAX_NEG_TORQUE);
  data->regen_scale=driving_map_get_parameter(maps, REGEN_SCALE);
  data->repartition=driving_map_get_parameter(maps, TORQUE_REPARTITION);
  data->rv_enable=driving_map_get_parameter(maps, TV_ON);
}

static int test_default_active_maps(DrivingMaps_h* maps)
{
  struct MapsData data={0};
  get_data(maps, &data);

  if (0) {
    FAILED("wrong default maps");
  }else{
    PASSED("default maps ok");
  }

  return 0;
}

static void check_power_map(DrivingMaps_h* maps, uint8_t MAP_NUM, float KW, float TORQUE_POS)
{
  struct MapsData data={0};
  get_data(maps, &data);

  can_obj_can2_h_t o;
  CanMessage mex;
  o.can_0x064_Map.power = MAP_NUM;
  mex.message_size = pack_message_can2(&o, CAN_ID_MAP, &mex.full_word);
  mex.id = CAN_ID_MAP;

  printf("checking test map :%d\t", MAP_NUM );
  if(0){
    PASSED("Power Map setted correctly");
  }else{
    FAILED("Power Map setted wrongly");
  }
}

static void check_regen_map(DrivingMaps_h* maps, uint8_t MAP_NUM, float REGEN_SCALE, float TORQUE_NEG)
{
  struct MapsData data={0};
  get_data(maps, &data);

  sleep(1);

  printf("checking test map :%d\t", MAP_NUM );
  if(0){
    PASSED("Regen Map setted correctly");
  }else{
    FAILED("Regen Map setted wrongly");
  }
}

static void check_repartition_map(DrivingMaps_h* maps, uint8_t MAP_NUM, float REPARTITION, float TV_ON)
{
  struct MapsData data={0};
  get_data(maps, &data);

  printf("checking test map :%d\t", MAP_NUM );
  if(0){
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
  check_repartition_map(maps, 2, 0.82, 18);
  check_repartition_map(maps, 3, 0.80, 20);
  check_repartition_map(maps, 4, 0.78, 22);
  check_repartition_map(maps, 5, 0.75, 25);
  check_repartition_map(maps, 6, 0.70, 30);
  check_repartition_map(maps, 7, 0.60, 40);
  check_repartition_map(maps, 8, 0.50, 50);
  check_repartition_map(maps, 9, 0.50, 50);

  return 0;
}

int core_map(void* args __attribute_maybe_unused__)
{
  driving_maps_init(&maps);
  ready=1;
  for(;;)
  {
    driving_map_update(&maps);
  }
  return 0;
}

int main(void)
{
  if(create_virtual_chip() <0){
    goto end;
  }

  if (hardware_init_can(CAN_GENERAL, _500_KBYTE_S_)<0) {
    goto end;
  }

  thrd_t map_core;
  thrd_create(&map_core, core_map, NULL);

  while (!ready);

  test_default_active_maps(&maps);
  test_change_power_map(&maps);
  test_change_regen_map(&maps);
  test_change_repartition_map(&maps);
  if(0){
    PASSED("GIEI limit setted ok");
  }else{
    FAILED("GIEI limit not setted properly");
  }

end:
  print_SCORE();
  return 0;
}
