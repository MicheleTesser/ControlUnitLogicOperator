#ifndef __HARDWARE_TEST_LIB__
#define __HARDWARE_TEST_LIB__

#include <stdint.h>

typedef struct __attribute__((aligned(2))){
  const uint8_t private_data[4];
}TestManager_h;

void
test_manager_init(TestManager_h* const restrict self)__attribute__((__nonnull__(1)));

void 
test_passed(TestManager_h* const restrict self, const char* const test)
  __attribute__((__nonnull__(1,2)));

void
teste_failed(TestManager_h* const restrict self, const char* const test)
  __attribute__((__nonnull__(1,2)));

void
test_manager_print_result(const TestManager_h* const restrict self)
__attribute__((__nonnull__(1)));

#define check_condition(test_manafer, bool_exp, exp_descr)\
  bool_exp? test_passed(test_manafer, exp_descr): test_failed(test_manafer, exp_descr);

#endif // !__HARDWARE_TEST_LIB__
