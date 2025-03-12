#include "test_lib.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

//private

#define Color_Red "\33[31m" 
#define Color_Green "\33[32m" 
#define Color_end "\33[0m" // To flush out prev settings

struct TestManager_t
{
  uint16_t passed;
  uint16_t failed;
};

union TestManager_h_t_conv{
  TestManager_h* const hidden;
  struct TestManager_t* const clear;
};

union TestManager_h_t_conv_const{
  const TestManager_h* const hidden;
  const struct TestManager_t* const clear;
};

#ifdef DEBUG
char __assert_size_test_manager[(sizeof(TestManager_h)==sizeof(struct TestManager_t))?+1:-1];
char __assert_size_test_manager[(_Alignof(TestManager_h)==_Alignof(struct TestManager_t))?+1:-1];
#endif /* ifdef DEBUG */

//public

void test_manager_init(TestManager_h* const restrict self)
{
  memset(self, 0, sizeof(*self));
}

void 
passed(TestManager_h* const restrict self, const char* const test)
{
  union TestManager_h_t_conv conv = {self};
  struct TestManager_t* const p_self = conv.clear;

  printf(Color_Green "%s" Color_end "\n",test);
  p_self->passed++;
}

void
failed(TestManager_h* const restrict self, const char* const test)
{
  union TestManager_h_t_conv conv = {self};
  struct TestManager_t* const p_self = conv.clear;

  printf(Color_Red "%s" Color_end "\n",test);
  p_self->failed++;
}

void
test_manager_print_result(const TestManager_h* const restrict self)
{
  const union TestManager_h_t_conv_const conv = {self};
  const struct TestManager_t* const p_self = conv.clear;
    printf("========================================\n");
    printf("passed %d\n",p_self->passed);
    printf("failed %d\n",p_self->failed);
    printf("========================================\n");
}
