#include "parser.c"

#include <string.h>

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
  printf("* running test...");

  at = test.expr;
  int actual = parse_expr();

  int expected_len = strlen(test.expr);
  int actual_len = at - test.expr;

  if (actual == test.expected)
    {
      if (*at != '\0' || expected_len != actual_len)
        goto report_error;
      else
        printf(" Ok.\n");
    }
  else
    goto report_error;

  return;

 report_error:
  printf(" Failed."
         "\n  - Expected:   %i"
         "\n  - Actual:     %i"
         "\n  - Expression: \'%s\'\n",
         test.expected,
         actual,
         test.expr);

  actual_len += 17;

  while (actual_len-- > 0)
    putchar(' ');

  puts("^");
}

int
main(void)
{
  for (size_t i = 0; i < sizeof(tests) / sizeof(*tests); i++)
    assert_equals(tests[i]);
}
