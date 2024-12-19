#ifndef __GIEI_ENGINES_COMMON__
#define __GIEI_ENGINES_COMMON__

enum ENGINES {
    FRONT_LEFT = 0,     //INFO: Front Left: Status Values: [0x283,0x285] SetPoint: 0x184
    FRONT_RIGHT = 1,    //INFO: Front Right: Status Values: [0x284,0x286] SetPoint: 0x185
    REAR_LEFT = 2,      //INFO: Rear Left: Status Values: [0x287,0x289] SetPoint: 0x188
    REAR_RIGHT = 3,     //INFO: Rear Right: Status Values: [0x288,0x28A] SetPoint: 0x189
};                              

enum RUNNING_STATUS{
    SYSTEM_OFF = 0,
    SYSTEM_PRECAHRGE,
    RUNNING,
};

#endif // !__GIEI_ENGINES_COMMON__
