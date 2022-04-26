#include <iostream>
#include <cstdint>
#include <cassert>
#include <cmath>

/*
  Scratch pad...

  Level0 : Level1 ... (+/- Level1)*
         | Level1

  AddOrDiff : + Level1 AddOrDiff
            | - Level1 AddOrDiff
            | eps

  Level1 : Level1 * Level2
         | Level1 / Level2
         | Level2

  Level2 : Number
         | (Level0)
         | -Level2

*/

struct Tokenizer
{
  struct Token
  {
    enum Type
    {
      OR,
      AND,
      EQUAL,
      DIFFERENT,
      LESS,
      LESS_EQUAL,
      GREATER,
      GREATER_EQUAL,
      PLUS,
      MINUS,
      MULT,
      DIVIDE,
      MOD,
      POWER,
      NEGATION,
      LPAREN,
      RPAREN,
      INTEGER,

      UNKNOWN_TOKEN,
      END_OF_FILE
    };

    Type type;
    const char *begin;
    size_t size;

    uint32_t prec_level() const
    {
      switch (type)
      {
      case OR:
        return 0;
      case AND:
        return 1;
      case EQUAL:
      case DIFFERENT:
        return 2;
      case LESS:
      case LESS_EQUAL:
      case GREATER:
      case GREATER_EQUAL:
        return 3;
      case PLUS:
      case MINUS:
        return 4;
      case MULT:
      case DIVIDE:
      case MOD:
        return 5;
      case POWER:
        return 6;
      case NEGATION:
      case LPAREN:
      case RPAREN:
      case INTEGER:
        return 7;
      default:
        // Nothing to see here.
        ;
      }

      return uint32_t(-1);
    }

    bool is_right_assoc() const
    {
      return type == POWER;
    }

    int32_t to_integer() const
    {
      assert(type == INTEGER);

      int32_t value = 0;

      for (size_t i = 0; i < size; i++)
        value = value * 10 + (begin[i] - '0');

      return value;
    }
  };

  const char *at;
  Token buffer[4];
  uint32_t count;

  Token next_token()
  {
    assert(count < 4);

    if (count > 0)
      return buffer[--count];
    else
      return next_token_aux();
  }

  void putback_token(const Token &token)
  {
    assert(count < 4);

    buffer[count++] = token;
  }

private:
  Token next_token_aux()
  {
    while (std::isspace(*at))
      at++;

    const char *const begin = at;

    at++;

    switch (begin[0])
    {
    case '|':
      if (*at == '|')
      {
        at++;
        return { Token::OR, begin, 1 };
      }
      break;
    case '&':
      if (*at == '&')
      {
        at++;
        return { Token::AND, begin, 1 };
      }
      break;
    case '=':
      if (*at == '=')
      {
        at++;
        return { Token::EQUAL, begin, 1 };
      }
      break;
    case '!':
      if (*at == '=')
      {
        at++;
        return { Token::DIFFERENT, begin, 1 };
      }
      else
      {
        return { Token::NEGATION, begin, 1};
      }
      break;
    case '<':
      if (*at == '=')
      {
        at++;
        return { Token::LESS_EQUAL, begin, 1 };
      }
      else
      {
        return { Token::LESS, begin, 1 };
      }
      break;
    case '>':
      if (*at == '=')
      {
        at++;
        return { Token::GREATER_EQUAL, begin, 1 };
      }
      else
      {
        return { Token::GREATER, begin, 1 };
      }
      break;
    case '^':
      return { Token::POWER, begin, 1 };
    case '+':
      return { Token::PLUS, begin, 1 };
    case '-':
      return { Token::MINUS, begin, 1 };
    case '*':
      return { Token::MULT, begin, 1 };
    case '/':
      return { Token::DIVIDE, begin, 1 };
    case '%':
      return { Token::MOD, begin, 1 };
    case '(':
      return { Token::LPAREN, begin, 1 };
    case ')':
      return { Token::RPAREN, begin, 1 };
    case '\0':
      return { Token::END_OF_FILE, begin, 1 };
    default:
      // Nothing to see here.
      ;
    }

    if (std::isdigit(begin[0]))
    {
      while (std::isdigit(*at))
        at++;

      return { Token::INTEGER, begin, size_t(at - begin) };
    }
    else
    {
      return { Token::UNKNOWN_TOKEN, begin, 1 };
    }
  }
};

int32_t apply(Tokenizer::Token::Type type, int32_t left, int32_t right)
{
  switch (type)
  {
  case Tokenizer::Token::OR:
    return left || right;
  case Tokenizer::Token::AND:
    return left && right;
  case Tokenizer::Token::EQUAL:
    return left == right;
  case Tokenizer::Token::DIFFERENT:
    return left != right;
  case Tokenizer::Token::LESS:
    return left < right;
  case Tokenizer::Token::LESS_EQUAL:
    return left <= right;
  case Tokenizer::Token::GREATER:
    return left > right;
  case Tokenizer::Token::GREATER_EQUAL:
    return left >= right;
  case Tokenizer::Token::PLUS:
    return left + right;
  case Tokenizer::Token::MINUS:
    return left - right;
  case Tokenizer::Token::MULT:
    return left * right;
  case Tokenizer::Token::DIVIDE:
    return left / right;
  case Tokenizer::Token::MOD:
    return left % right;
  case Tokenizer::Token::POWER:
    return std::pow(left, right);
  default:
    // Hello there!
    ;
  }

  assert(false);

  return -1;
}

int32_t parse_level(uint32_t level, Tokenizer &tokenizer)
{
  if (level < 7)
  {
    int32_t left = parse_level(level + 1, tokenizer);
    Tokenizer::Token token = tokenizer.next_token();

    if (token.prec_level() == level && token.is_right_assoc())
      return apply(token.type, left, parse_level(level, tokenizer));

    while (token.prec_level() == level)
    {
      left = apply(token.type, left, parse_level(level + 1, tokenizer));
      token = tokenizer.next_token();
    }

    tokenizer.putback_token(token);

    return left;
  }
  else
  {
    Tokenizer::Token token = tokenizer.next_token();

    switch (token.type)
    {
    case Tokenizer::Token::LPAREN:
    {
      int32_t left = parse_level(0, tokenizer);

      token = tokenizer.next_token();
      assert(token.type == Tokenizer::Token::RPAREN);

      return left;
    }
    case Tokenizer::Token::MINUS:
      return -parse_level(level, tokenizer);
    case Tokenizer::Token::NEGATION:
      return !parse_level(level, tokenizer);
    case Tokenizer::Token::INTEGER:
      return token.to_integer();
    default:
      assert(false);
    }
  }
}

int32_t parse_expr(const char *expr)
{
  Tokenizer tokenizer = { expr, { }, 0 };

  int32_t res = parse_level(0, tokenizer);

  Tokenizer::Token token = tokenizer.next_token();

  assert(token.type == Tokenizer::Token::END_OF_FILE);

  return res;
}

int main(int argc, char **argv)
{
  assert(argc == 2);

  std::cout << parse_expr(argv[1]) << '\n';
}
