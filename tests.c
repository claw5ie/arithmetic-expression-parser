#include "srdparser.c"

#include <string.h>
#include <stddef.h>

typedef struct
{
  int expected;
  const char *expr;
} Test;

Test tests[]
  = { {       234, "234" },
      {        10, "2+8" },
      {        -4, "1-2-3" },
      {         6, "2+2*2" },
      {         8, "(2+2)*2" },
      {         6, "2*2+2" },
      {         8, "2*(2+2)" },
      { 134217728, "2^3^3" },
      {      4096, "(2^3)^4" },
      {       -75, "2^2-3^4+2^-3+2" },
      {         1, "--2+-3+-(-(2))" },
      {       -90, "2+2*2-2^3*3*(2+2)" } };

void
assert_equals(Test test)
{
  printf("* Running test...");

  at = test.expr;
  int actual = parse_expr();

  ptrdiff_t expected_length = strlen(test.expr);
  ptrdiff_t actual_length = at - test.expr;

  if (actual == test.expected && expected_length == actual_length)
    puts(" Ok.");
  else
    {
      printf(" Failed."
             "\n  - Expected:   %i"
             "\n  - Actual:     %i"
             "\n  - Expression: \'%s\'\n",
             test.expected,
             actual,
             test.expr);

      actual_length += 17;

      while (actual_length-- > 0)
        putchar(' ');

      puts("^");
    }
}

int
main(void)
{
  for (size_t i = 0; i < sizeof(tests) / sizeof(*tests); i++)
    assert_equals(tests[i]);
}
