#include "dv_driver_input.h"
#include "../../../core_utility/driver_input_reader/driver_input_reader.h"
#include <stdint.h>
#include <string.h>

struct DvDriverInput_t{
  DriverInputReader_h o_driver_input_reader;
};

union DvDriverInput_h_t_conv{
    DvDriverInput_h* const restrict hidden;
    struct DvDriverInput_t* const restrict clear;
};

union DvDriverInput_h_t_conv_const{
    const DvDriverInput_h* const restrict hidden;
    const struct DvDriverInput_t* const restrict clear;
};

#ifdef DEBUG
char __assert_size_dv_driver_input[(sizeof(DvDriverInput_h) == sizeof(struct DvDriverInput_t))? 1:-1];
char __assert_align_dv_driver_input[(_Alignof(DvDriverInput_h) == _Alignof(struct DvDriverInput_t))? 1:-1];
#endif // DEBUG

int8_t dv_driver_input_init(DvDriverInput_h* const restrict self )
{
    union DvDriverInput_h_t_conv conv = {self};
    struct DvDriverInput_t* const restrict p_self = conv.clear;

    memset(p_self, 0, sizeof(*p_self));
    if(driver_input_reader_init(&p_self->o_driver_input_reader)<0)
    {
      return -1;
    }

    return 0;
}

int8_t dv_driver_input_update(DvDriverInput_h* const restrict self )
{
    union DvDriverInput_h_t_conv conv = {self};
    struct DvDriverInput_t* const restrict p_self = conv.clear;

    if(driver_input_reader_update(&p_self->o_driver_input_reader)<0)
    {
      return -1;
    }

    return 0;
}

float dv_driver_input_get_brake(const DvDriverInput_h* const restrict self )
{
    union DvDriverInput_h_t_conv_const conv = {self};
    const struct DvDriverInput_t* const restrict p_self = conv.clear;
    
    float brake_human = 
      driver_input_reader_get(&p_self->o_driver_input_reader, DRIVER_HUMAN, BRAKE);

    float brake_dv = 
      driver_input_reader_get(&p_self->o_driver_input_reader, DRIVER_EMBEDDED, BRAKE);

    return (brake_dv > brake_human)? brake_dv:brake_human;

}
