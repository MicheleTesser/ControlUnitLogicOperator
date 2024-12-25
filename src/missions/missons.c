#include "./missons.h"

static enum MISSIONS current_mission = NONE;

uint8_t update_current_mission(const enum MISSIONS mission)
{
    current_mission = mission;
    return 0;
}
enum MISSIONS get_current_mission(void)
{
    return current_mission;
}
