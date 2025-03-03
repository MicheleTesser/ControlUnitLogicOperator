#include "score_lib/test_lib.h"
#include "src/cores/core_1/feature/log/telemetry/json_builder/json_builder.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

//public

void test_init_json(void)
{
  Json_h json = {0};
  int err = json_init(&json);
  if(err<0)
  {
    FAILED("json init failed with error code: ");
    printf("%d\n",err);
  }
  else if (strcmp("{ }", json_get(&json)))
  {
    FAILED("json created but not correctly");
  }
  else
  {
    PASSED("json created correctly");
  }

  printf("given: %s, expected: %s\n", json_get(&json), "{ }");

  json_destroy(&json);
}

void test_push_one_element(void)
{
  Json_h json = {0};
  float brake = 10.0f;
  int err=0;

  json_init(&json);

  if((err = json_push_element(&json, "brake", brake))<0)
  {
    FAILED("json push one element failed when pushing brake with error: ");
    printf("%d\n",err);
  }

  const char expected[]= "{\"brake\":10.00}";
  if (strcmp(json_get(&json), expected))
  {
    FAILED("json push one element ok but output is not what expected: ");
    printf("given: %s, expected: %s\n", json_get(&json), expected);
  }
  else
  {
    PASSED("json push one element ok: ");
    printf("given: %s, expected: %s\n", json_get(&json), expected);
  }

  json_destroy(&json);
}

void test_push_multiple_elements(void)
{
  Json_h json = {0};
  float brake = 10.0f;
  float throttle = 56.78f;
  int err=0;

  json_init(&json);

  if((err = json_push_element(&json, "brake", brake))<0)
  {
    FAILED("push muliple elements failed when pushing brake with error: ");
    printf("%d\n",err);
  }

  if((err = json_push_element(&json, "throttle", throttle))<0)
  {
    FAILED("push mulipl elements failed when pushing throttle with error: ");
    printf("%d\n",err);
  }

  const char expected[]= "{\"brake\":10.00,\"throttle\":56.78}";
  if (strcmp(json_get(&json), expected))
  {
    FAILED("json push muliple elements ok but output is not what expected: ");
    printf("given: %s, expected: %s\n", json_get(&json), expected);
  }
  else
  {
    PASSED("json push muliple elements ok: ");
    printf("given: %s, expected: %s\n", json_get(&json), expected);
  }

  json_destroy(&json);
}

int main(void)
{

  test_init_json();
  test_push_one_element();
  test_push_multiple_elements();

  printf("tests finished\n");
  print_SCORE();
  return 0;
}
