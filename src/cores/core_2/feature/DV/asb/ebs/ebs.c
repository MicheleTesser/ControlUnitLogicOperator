#include "ebs.h"
#include "../../../../../../lib/raceup_board/raceup_board.h"
#include <stdint.h>
#include <string.h>

struct DvEbs_t{
  struct CanMailbox* ebs_mailbox;
  uint8_t ebs_on;
};

union DvEbs_h_t_conv{
  DvEbs_h* const restrict hidden;
  struct DvEbs_t* const restrict clear;
};

union DvEbs_h_t_conv_const{
  const DvEbs_h* const restrict hidden;
  const struct DvEbs_t* const restrict clear;
};

//private

#if DEBUG
uint8_t __assert_size_dv_ebs[(sizeof(DvEbs_h) == sizeof(struct DvEbs_t))? 1:-1];
#endif

int8_t ebs_class_init(DvEbs_h* const restrict self)
{
  union DvEbs_h_t_conv conv = {self};
  struct DvEbs_t* const restrict p_self= conv.clear;

  memset(p_self, 0, sizeof(*p_self));
  //TODO: init ebs mailbox

  return 0;
}

int8_t ebs_update(DvEbs_h* const restrict self)
{
  union DvEbs_h_t_conv conv = {self};
  struct DvEbs_t* const restrict p_self __attribute__((__unused__)) = conv.clear;
  //TODO: update data of ebs through mailbox

  return 0;
}

int8_t ebs_on(const DvEbs_h* const restrict self)
{
  const union DvEbs_h_t_conv_const conv = {self};
  const struct DvEbs_t* const restrict p_self= conv.clear;

  return  p_self->ebs_on;
  
}
