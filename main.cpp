#include <iostream>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdint>
#include <cassert>

#define LOWEST_PREC (-254)
#define HIGHEST_PREC (255)

struct Token
{
  enum Type
  {
    Plus,
    Minus,
    Mult,
    Pow,

    Integer,
    Open_Paren,
    Closed_Paren,

    End_Of_File
  };

  Type type;
  const char *begin;
  size_t size;

  int32_t precedence() const
  {
    switch (type)
    {
    case Token::Plus:
      return 1;
    case Token::Minus:
      return 1;
    case Token::Mult:
      return 2;
    case Token::Pow:
      return 3;
    default:
      return LOWEST_PREC - 1;
    }
  }

  int32_t is_right_assoc() const
  {
    return type == Pow;
  }

  int32_t to_integer() const
  {
    assert(type == Integer);

    int32_t value = 0;

    for (size_t i = 0; i < size; i++)
      value = value * 10 + (begin[i] - '0');

    return value;
  }
};

struct Tokenizer
{
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
    static struct
    {
      const char *const string;
      uint32_t size;
      Token::Type const type;
    } const tokens[] = {
      { "+", 1, Token::Plus },
      { "-", 1, Token::Minus },
      { "*", 1, Token::Mult },
      { "^", 1, Token::Pow },
      { "(", 1, Token::Open_Paren },
      { ")", 1, Token::Closed_Paren }
    };

    while (std::isspace(*at))
      at++;

    const char *const begin = at;

    if (*at == '\0')
    {
      return { Token::End_Of_File, begin, 0 };
    }
    else if (std::isdigit(*at))
    {
      while (std::isdigit(*++at))
        ;

      return { Token::Integer, begin, size_t(at - begin) };
    }
    else
    {
      for (size_t i = 0; i < sizeof (tokens) / sizeof (*tokens); i++)
      {
        auto &token = tokens[i];
        if (!std::strncmp(token.string, at, token.size))
        {
          at += token.size;
          return { token.type, begin, token.size };
        }
      }

      assert(false && "ERROR: invalid token.\n");
    }
  }
};

int32_t apply(Token::Type type, int32_t left, int32_t right)
{
  switch (type)
  {
  case Token::Plus:
    return left + right;
  case Token::Minus:
    return left - right;
  case Token::Mult:
    return left * right;
  case Token::Pow:
    return std::pow(left, right);
  default:
    std::cerr << "ERROR: invalid type: " << type << '\n';
    std::exit(EXIT_FAILURE);
  }
}

int32_t parse_level(int32_t level, Tokenizer &tokenizer)
{
  Token token = tokenizer.next_token();
  int32_t left;

  switch (token.type)
  {
  case Token::Open_Paren:
  {
    left = parse_level(LOWEST_PREC, tokenizer);

    token = tokenizer.next_token();
    assert(token.type == Token::Closed_Paren);
  } break;
  case Token::Integer:
    left = token.to_integer();
    break;
  case Token::Minus:
    left = -parse_level(HIGHEST_PREC, tokenizer);
    break;
  default:
    assert(false && "failed to read expression.");
  }

  token = tokenizer.next_token();
  int32_t curr_level = token.precedence();
  int32_t prev_level = std::numeric_limits<int32_t>::max();

  while (curr_level >= level && curr_level < prev_level)
  {
    if (token.is_right_assoc())
    {
      left = apply(
        token.type, left, parse_level(curr_level, tokenizer)
        );
      token = tokenizer.next_token();
    }
    else
    {
      // This else shouldn't be necessary, since all tokens of
      // precedence "curr_level" will be consumed by right
      // associative operator from the branch above.
      while (token.precedence() == curr_level)
      {
        left = apply(
          token.type, left, parse_level(curr_level + 1, tokenizer)
          );
        token = tokenizer.next_token();
      }
    }

    prev_level = curr_level;
    curr_level = token.precedence();
  }

  tokenizer.putback_token(token);

  return left;
}

void parse_expr(const char *expr)
{
  Tokenizer tokenizer = { expr, { }, 0 };

  std::cout << "Result: "
            << parse_level(0, tokenizer)
            << '\n';

  Token token = tokenizer.next_token();

  if (token.type != Token::End_Of_File)
  {
    std::cout << "Failed to parse expression at `"
              << token.begin
              << "`.\n";
  }
}

int main(int argc, char **argv)
{
  assert(argc == 2);

  parse_expr(argv[1]);
}
