#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>

#define LOWEST_PREC (-127)
#define HIGHEST_PREC (127)

static
int
prec_of_token(char op)
{
  switch (op)
    {
    case '+':
    case '-':
      return 0;
    case '*':
    case '/':
      return 1;
    case '^':
      return 2;
    default:
      return LOWEST_PREC - 1;
    }
}

static
bool
is_token_right_assoc(char ch)
{
  return ch == '^';
}

int
powi(int base, int exponent);

static
int
apply(char op, int left, int right)
{
  switch (op)
    {
    case '+':
      return left + right;
    case '-':
      return left - right;
    case '*':
      return left * right;
    case '/':
      return left / right;
    case '^':
      return powi(left, right);
    default:
      assert(false && "Invalid operator");
    }
}

static
int
parse_top_level();

static
int
parse_prec(int prec_limit);

static const char *at;

static
int
parse_base()
{
  switch (*at)
    {
    case '(':
      {
        ++at;
        int val = parse_top_level();
        assert(*at == ')');
        ++at;

        return val;
      }
    case '-':
      ++at;
      return -parse_prec(HIGHEST_PREC);
    default:
      if (isdigit(*at))
        {
          int val = 0;

          do
            val = 10 * val + (*at++ - '0');
          while (isdigit(*at));

          return val;
        }
      else
        assert(false && "Expected expression");
    }
}

static
int
parse_prec(int prec_limit)
{
  int left = parse_base();

  char op = *at;
  int prev_prec = INT_MAX, curr_prec = prec_of_token(op);

  // If "op" is not an operator, it must have smaller precedence than
  // the lowest possible precedence of any operator.
  // In that way, current precedence will always be smaller than
  // the precedence limit and the left-hand side will be returned
  // (this loop won't run).
  while (curr_prec >= prec_limit && curr_prec < prev_prec)
    {
      // When entering one of the conditions, it is guaranteed -
      // - or, atleast, should be - that precedence of the current
      // token (which should be an operator) is the same as
      // "curr_prec", so no need to check it.
      if (is_token_right_assoc(op))
        {
          ++at;
          left = apply(op, left, parse_prec(curr_prec));
          op = *at;
        }
      else
        {
          do
            {
              ++at;
              left = apply(op, left, parse_prec(curr_prec + 1));
              op = *at;
            }
          while (curr_prec == prec_of_token(op));
        }

      prev_prec = curr_prec;
      curr_prec = prec_of_token(op);
    }

  return left;
}

static
int
parse_top_level()
{
  return parse_prec(LOWEST_PREC);
}

int
srd_parse_expr(const char *str)
{
  at = str;
  int res = parse_top_level();
  assert(*at == '\0' && "couldn't parse entire expression");

  return res;
}
