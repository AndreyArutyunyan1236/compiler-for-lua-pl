#include <any>
#include <string>
#include <vector>
#include <unordered_map>

enum class Type {
  KW_LOCAL, // Keywords
  KW_IF,
  KW_THEN,
  KW_ELSE,
  KW_END,
  KW_FUNCTION,
  KW_RETURN,
  KW_WHILE,
  KW_FOR,
  KW_DO,
  KW_REPEAT,
  KW_UNTIL,
  KW_NIL,
  KW_TRUE,
  KW_FALSE,
  KW_AND,
  KW_OR,
  KW_NOT,
  KW_IN,

  LIT_INT, // Literals
  LIT_FLOAT,
  LIT_STRING,

  IDENT, // Identificator
  
  PLUS, // Operatiors
  MINUS,
  STAR,
  SLASH,
  DOUBLE_SLASH,
  PERCENT,
  CARET,
  CONCAT,
  HASH,
  EQUAL,
  EQUAL_EQUAL,
  NOT_EQUAL,
  LESS,
  GREATER,
  LESS_EQUAL,
  GREATER_EQUAL,
  L_PAREN,
  R_PAREN,
  L_BRACE,
  R_BRACE,
  L_BRACKET,
  R_BRACKET,
  COMMA,
  DOT,
  COLON,
  COLON_COLON,
  SEMICOLON,

  END_OF_FILE, // Specefic
  ERROR
};

struct Vect2 { int x = 1; int y = 1; };

struct Token { 
  Type type; 
  std::any value;
  Vect2 position;
};

class Lexer {
private:
  std::string sourceCode;
  int index = 0, x = 1, y = 1;


  char peek() {
    if (index >= sourceCode.size()) return '\0';
    return sourceCode[index];
  }

  char peekNext() {
    if (index+1 >= sourceCode.size()) return '\0';
    return sourceCode[index+1];
  }

  char advance() {
    char c = sourceCode[index];
    
    if (c == '\n') {
      x = 1;
      y++;
    } 
    else x++;

    index++;
    return c;
  }

  bool match(char expected) {
    if (peek() != expected) return false;
    
    advance();
    return true;
  }

  void skipWhiteSpace() {
    while (index < sourceCode.size()) {
      char c = peek();

      if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
        advance();

      else break;
    }
  }

public:
  std::unordered_map<char, Type> map = {
    {'+', Type::PLUS},
    {'-', Type::MINUS},
    {'*', Type::STAR},
    {';', Type::SEMICOLON},
  };

  Token nextToken() {
    skipWhiteSpace();

    Type type;


    if (map.count(peek())) {
      type = map[peek()]; 

      advance();
      return Token{.type = type};

    } else {
      switch (peek()) {
        case '=':
          advance();
          if (match('=')) type = Type::EQUAL_EQUAL;
          else type = Type::EQUAL;
          return Token{.type = type};

        case '/':
          advance();
          if (match('/')) type = Type::DOUBLE_SLASH;
          else type = Type::SLASH;
          return Token{.type = type};

        case '<':
          advance();
          if (match('=')) type = Type::LESS_EQUAL;
          else type = Type::LESS;
          return Token{.type = type};

        case '>':
          advance();
          if (match('=')) type = Type::GREATER_EQUAL;
          else type = Type::GREATER;
          return Token{.type = type};

        case ':':
          advance();
          if (match(':')) type = Type::COLON_COLON;
          else type = Type::COLON;
          return Token{.type = type};

        default:
          return Token{.type = Type::ERROR}; } 
    }

    return Token{.type = type};
  }

  std::vector<Token> tokenize() { }
};
