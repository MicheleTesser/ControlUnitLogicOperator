#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include "ControlUnitLogicOperator.h"
#include "lib/board_dbc/can2.h"
#include "linux_board/linux_board.h"
#include "src/board_conf/id_conf.h"
#include "src/GIEI/giei.h"
#include "src/GIEI/power_maps/power_maps.h"
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <stdint.h>
#include <unistd.h>

extern struct __GIEI{
    time_var_microseconds sound_start_at;
    float limit_power;
    float limit_pos_torque;
    float limit_neg_torque;
    float limit_regen;
    float limit_max_speed;
    float settings_power_repartition;

    uint8_t activate_torque_vectoring :1;
}GIEI;


static int init_core_0(void* args __attribute_maybe_unused__){
    main_cpu_x(0);
    return 0;
}

static int init_core_1(void* args __attribute_maybe_unused__){
    main_cpu_x(1);
    return 0;
}

static int init_core_2(void* args __attribute_maybe_unused__){
    main_cpu_x(2);
    return 0;
}

static int test_default_active_maps(void)
{
    const struct power_map_settings expected_power = {
        .power_kw = 77,
        .torque_pos = 21
    };
    const struct regen_map_settings expected_regen = {
        .regen_scale = 0,
        .max_neg_torque = 0
    };
    const struct tv_repartition_map expected_repartition ={
        .repartition = 50,
        .torque_vectoring_on = 1,
    };
    const struct car_map* maps = giei_get_active_maps();

    uint8_t cmp_pow = memcmp(maps->power, &expected_power, sizeof(expected_power));
    uint8_t cmp_regen = memcmp(maps->regen, &expected_regen, sizeof(expected_regen));
    uint8_t cmp_rep = 
        !(maps->repartition->repartition == expected_repartition.repartition) ||
        !(maps->repartition->torque_vectoring_on == expected_repartition.torque_vectoring_on);

    if (cmp_pow || cmp_regen || cmp_rep) {
        FAILED("wrong default maps");
    }else{
        PASSED("default maps ok");
    }

    return 0;
}

static void check_power_map(uint8_t MAP_NUM, float KW, float TORQUE_POS)
{
    const struct power_map_settings map ={
        .power_kw = KW, 
        .torque_pos = TORQUE_POS,
    };
    can_obj_can2_h_t o;
    CanMessage mex;
    o.can_0x064_Map.power = MAP_NUM;
    mex.message_size = pack_message_can2(&o, CAN_ID_MAP, &mex.full_word);
    mex.id = CAN_ID_MAP;

    hardware_write_can(CAN_MODULE_GENERAL, &mex);
    sleep(1);
    raise_interrupt(CAN_MODULE_GENERAL);

    const struct car_map* curr_map = giei_get_active_maps();
    printf("checking test map :%d\t", MAP_NUM );
    if(!memcmp(curr_map->power, &map, sizeof(map))){
        PASSED("Power Map setted correctly");
    }else{
        FAILED("Power Map setted wrongly");
    }
}

static void check_regen_map(uint8_t MAP_NUM, float REGEN_SCALE, float TORQUE_NEG)
{
    const struct regen_map_settings map ={
        .regen_scale = REGEN_SCALE, 
        .max_neg_torque = TORQUE_NEG,
    };
    can_obj_can2_h_t o;
    CanMessage mex;
    o.can_0x064_Map.regen = MAP_NUM;
    mex.message_size =pack_message_can2(&o, CAN_ID_MAP, &mex.full_word);
    mex.id = CAN_ID_MAP;

    hardware_write_can(CAN_MODULE_GENERAL, &mex);
    sleep(1);
    raise_interrupt(CAN_MODULE_GENERAL);

    const struct car_map* curr_map = giei_get_active_maps();
    printf("checking test map :%d\t", MAP_NUM );
    if(!memcmp(curr_map->regen, &map, sizeof(map))){
        PASSED("Regen Map setted correctly");
    }else{
        FAILED("Regen Map setted wrongly");
    }
}

static void check_repartition_map(uint8_t MAP_NUM, float REPARTITION, float TV_ON)
{
    const struct tv_repartition_map map ={
        .repartition = REPARTITION, 
        .torque_vectoring_on = TV_ON,
    };
    can_obj_can2_h_t o;
    CanMessage mex;
    o.can_0x064_Map.torque_rep = MAP_NUM;
    mex.message_size =pack_message_can2(&o, CAN_ID_MAP, &mex.full_word);
    mex.id = CAN_ID_MAP;

    hardware_write_can(CAN_MODULE_GENERAL, &mex);
    sleep(1);
    raise_interrupt(CAN_MODULE_GENERAL);

    const struct car_map* curr_map = giei_get_active_maps();
    printf("checking test map :%d\t", MAP_NUM );
    if((curr_map->repartition->repartition == map.repartition) && 
            (curr_map->repartition->torque_vectoring_on == map.torque_vectoring_on)){
        PASSED("Repartition Map setted correctly");
    }else{
        FAILED("Repartition Map setted wrongly");
    }
}

static int test_change_power_map(void)
{
    check_power_map(0, 77, 21);
    check_power_map(1, 75, 20);
    check_power_map(2, 70, 18);
    check_power_map(3, 60, 16);
    check_power_map(4, 50, 15);
    check_power_map(5, 40, 15);
    check_power_map(6, 35, 13);
    check_power_map(7, 30, 13);
    check_power_map(8, 15, 12);
    check_power_map(9, 10, 10);

    return 0;
}

static int test_change_regen_map(void)
{
    check_regen_map(0, 0, 0);
    check_regen_map(1, 20, -8);
    check_regen_map(2, 30, -10);
    check_regen_map(3, 40, -12);
    check_regen_map(4, 50, -15);
    check_regen_map(5, 60, -17);
    check_regen_map(6, 70, -18);
    check_regen_map(7, 80, -19);
    check_regen_map(8, 90, -20);
    check_regen_map(9, 100, -21);

    return 0;
}

static int test_change_repartition_map(void)
{
    check_repartition_map(0, 50, 1);
    check_repartition_map(1, 100, 0);
    check_repartition_map(2, 82, 18);
    check_repartition_map(3, 80, 20);
    check_repartition_map(4, 78, 22);
    check_repartition_map(5, 75, 25);
    check_repartition_map(6, 70, 30);
    check_repartition_map(7, 60, 40);
    check_repartition_map(8, 50, 50);
    check_repartition_map(9, 50, 50);

    return 0;
}

int main(void)
{
    int8_t err=0;
    if(create_virtual_chip() <0){
        err--;
        goto end;
    }

    thrd_t core_0;
    thrd_t core_1;
    thrd_t core_2;
    thrd_create(&core_0, init_core_0, NULL);
    thrd_create(&core_1, init_core_1, NULL);
    thrd_create(&core_2, init_core_2, NULL);
    sleep(2);

    test_default_active_maps();
    test_change_power_map();
    test_change_regen_map();
    test_change_repartition_map();
    if(DEBUG_GIEI_check_limits(10, 10, -21, 50, 0)){
        PASSED("GIEI limit setted ok");
    }else{
        FAILED("GIEI limit not setted properly");
    }

end:
    print_SCORE();
    return err;
}
