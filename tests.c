#include <stdio.h>
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
assert_equals(int (*parse_expr)(const char *), Test test)
{
  printf("* Running test...");

  int actual = parse_expr(test.expr);

  if (actual == test.expected)
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

      size_t spaces_to_insert = strlen(test.expr) + 17;

      while (spaces_to_insert-- > 0)
        putchar(' ');

      puts("^");
    }
}

int
srd_parse_expr(const char *str);

int
rd_parse_expr(const char *str);

int
main(void)
{
  for (size_t i = 0; i < sizeof(tests) / sizeof(*tests); i++)
    assert_equals(srd_parse_expr, tests[i]);

  putchar('\n');
  putchar('\n');

  for (size_t i = 0; i < sizeof(tests) / sizeof(*tests); i++)
    assert_equals(rd_parse_expr, tests[i]);
}
