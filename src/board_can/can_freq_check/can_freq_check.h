#ifndef __CAR_CAN_FREQ_CHECK__
#define __CAR_CAN_FREQ_CHECK__

#include "../../lib/raceup_board/raceup_board.h"
#include <stdint.h>

typedef int8_t (*fault_manager)(void);

int8_t can_freq_class_init(const uint16_t amount_of_ids);
int8_t can_freq_add_mex_freq(const uint16_t can_id, const time_var_microseconds freq, 
        const fault_manager fault_fun);
int8_t can_freq_update_freq(const uint16_t can_id, const time_var_microseconds now);
int8_t can_freq_check_faults(void);

#endif // !__CAR_CAN_FREQ_CHECK__
