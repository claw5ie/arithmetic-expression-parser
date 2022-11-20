#include <ctype.h>
#include <assert.h>

static const char *at;

static
int
parse_top_level();

static
int
parse_base()
{
  int base = 0;

  if (*at == '(')
    {
      ++at;
      base = parse_top_level();
      assert(*at == ')');
      ++at;
    }
  else if (*at == '-')
    {
      ++at;
      return -parse_base();
    }
  else if (isdigit(*at))
    {
      do
        base = 10 * base + (*at++ - '0');
      while (isdigit(*at));
    }
  else
    assert(0);

  return base;
}

int
powi(int base, int exponent);

static
int
parse_exp()
{
  int left = parse_base();

  if (*at == '^')
    {
      ++at;
      left = powi(left, parse_exp());
    }

  return left;
}

static
int
parse_mult()
{
  int left = parse_exp();

  while (*at == '*')
    {
      ++at;
      left *= parse_exp();
    }

  return left;
}

static
int
parse_top_level()
{
  int left = parse_mult();
  char op = *at;

  while (op == '+' || op == '-')
    {
      ++at;
      int right = parse_mult();

      if (op == '+')
        left += right;
      else
        left -= right;

      op = *at;
    }

  return left;
}

int
rd_parse_expr(const char *str)
{
  at = str;
  int left = parse_top_level();
  assert(*at == '\0' && "couldn't parse entire expression");

  return left;
}
