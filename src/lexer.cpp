#include "lexer.hpp"

char Lexer::peek() {
  if (index >= sourceCode.size()) return '\0';
  return sourceCode[index];
}
char Lexer::peekNext() {
  if (index+1 >= sourceCode.size()) return '\0';
  return sourceCode[index+1];
}
char Lexer::advance() {
  char c = sourceCode[index];

  if (c == '\n') {
    x = 1;
    y++;
  } 
  else x++;
  index++;
  return c;
}
bool Lexer::match(char expected) {
  if (peek() != expected) return false;

  advance();
  return true;
}
void Lexer::skipWhiteSpace() {
  while (index < sourceCode.size()) {
    char c = peek();
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
      advance();
    else break;
  }
}
// dont forget about value of token
Token Lexer::nextToken() {
  skipWhiteSpace();
  Type type;
  if (map.count(peek())) { // if exists in map
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

std::vector<Token> Lexer::tokenize() { 
  std::vector<Token> VectorOfTokens;
  while (peek() != '\0') {
    VectorOfTokens.push_back(nextToken());
  } 
  return VectorOfTokens;
}
