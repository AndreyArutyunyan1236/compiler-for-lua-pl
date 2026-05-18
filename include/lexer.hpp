#include <any>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdbool.h>
#include <cctype>

#ifndef LEXER_HPP
#define LEXER_HPP

enum class Type {
  KW_LOCAL, KW_IF, KW_THEN, KW_ELSE, KW_END,
  KW_FUNCTION, KW_RETURN, KW_WHILE,
  KW_FOR, KW_DO, KW_REPEAT, KW_UNTIL, KW_NIL,
  KW_TRUE, KW_FALSE, KW_AND, KW_OR, KW_NOT, KW_IN,

  LIT_INT, // Literals
  LIT_FLOAT, LIT_STRING,

  IDENT, // Identificator
  
  PLUS, // Operatiors
  MINUS, STAR, SLASH, DOUBLE_SLASH, PERCENT,
  CARET, CONCAT, HASH, EQUAL, EQUAL_EQUAL,
  NOT_EQUAL, LESS, GREATER, LESS_EQUAL,
  GREATER_EQUAL, L_PAREN, R_PAREN, L_BRACE,
  R_BRACE, L_BRACKET, R_BRACKET, COMMA,
  COLON, COLON_COLON, SEMICOLON,

  END_OF_FILE, // Specefic
  ERROR
};


struct Vect2 { int x; int y; };

struct Token { 
  Type type; 
  std::any value;
  Vect2 position;
};

class Lexer {
private:
  std::string sourceCode;
  unsigned long long int index = 0;
  int x = 1, y = 1;

std::unordered_map<char, Type> operationMap = {
    {'+', Type::PLUS},
    {'-', Type::MINUS},
    {'*', Type::STAR},
    {'%', Type::PERCENT},
    {'^', Type::CARET},
    {'#', Type::HASH},
    {'(', Type::L_PAREN},
    {')', Type::R_PAREN},
    {'{', Type::L_BRACE},
    {'}', Type::R_BRACE},
    {'[', Type::L_BRACKET},
    {']', Type::R_BRACKET},
    {',', Type::COMMA},
    {';', Type::SEMICOLON},
    {'\0', Type::END_OF_FILE},
};

  char numbersArray[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  char peek(); 
  char peekNext(); 
  char advance(); 
  bool match(char expected); 
  void skipWhiteSpace(); 

public:
  Lexer(std::string source) : sourceCode(source), index(0), x(1), y(1) {}
  Token nextToken(); 
  std::vector<Token> tokenize(); 
};

#endif
