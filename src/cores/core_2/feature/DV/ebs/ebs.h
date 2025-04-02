#ifndef __DV_EBS__
#define __DV_EBS__

#include <stdint.h>

typedef struct __attribute__((aligned(8))){
  const uint8_t private_data[80];
}DvEbs_h;

//INFO: copied from can2.dbc message 104 response_status
enum ASB_INTEGRITY_CHECK_RESULT
{
  EBS_SUCCESS=0,
  EBS_FAILURE,
  EBS_ERROR,
  EBS_NOT_YET_DONE, //added in here only

  __NUM_OF_ASB_INTEGRITY_CHECK_RESULT__
};

int8_t ebs_class_init(DvEbs_h* const restrict self)__attribute__((__nonnull__(1)));
int8_t ebs_update(DvEbs_h* const restrict self)__attribute__((__nonnull__(1)));
enum ASB_INTEGRITY_CHECK_RESULT ebs_asb_consistency_check(DvEbs_h* const restrict self)__attribute__((__nonnull__(1)));
int8_t ebs_on(const DvEbs_h* const restrict self)__attribute__((__nonnull__(1)));
int8_t ebs_activated(const DvEbs_h* const restrict self)__attribute__((__nonnull__(1)));

#endif // !__DV_EBS__
