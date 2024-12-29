#include "asb.h"
#include "ebs/ebs.h"
#include <stdint.h>

int8_t asb_class_init(void)
{
    ebs_class_init();
    return 0;
}

int8_t asb_consistency_check(void)
{
    return 0;
}

