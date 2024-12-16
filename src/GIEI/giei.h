#ifndef __CAR_GIEI__
#define __CAR_GIEI__
/*
 * Generic Inverter Engine Interface (GIEI) is a generic interface for the engines used in the car
 * it contains all the function needed to:
 *  - initialize
 *  - control
 *  the engine used. 
 *
 *  It does not depend on the type of engine used and has to stay in this way.
 */

#include "../lib/raceup_board/raceup_board.h"
#include "./power_maps/power_maps.h"
#include <stdint.h>

struct init_args;
struct GIEI_status;

int8_t GIEI_initialize(const struct init_args* const restrict init_args);
int8_t GIEI_get_all_data(const uint32_t data_type, const struct GIEI_status* restrict o_buffer, 
        const uint32_t buffer_size);

int8_t GIEI_recv_data(const CanMessage* const restrict mex);

uint8_t GIEI_check_running_condition(void);

#endif // !__CAR_GIEI__
