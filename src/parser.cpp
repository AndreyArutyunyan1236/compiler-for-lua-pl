#include <parser.hpp>
#include <iostream>

Token Parser::peek() {
  if (index >= listOfTokens.size()) return Token{.type = Type::END_OF_FILE};  
  return listOfTokens[index];
}

Token Parser::advance() {
  if (index >= listOfTokens.size()) return Token{.type = Type::END_OF_FILE};  
  return listOfTokens[index++];
}

bool Parser::check(Type type) {
  if (peek().type == type) return true;
  return false;
}

bool Parser::match(Type type) {
  if (peek().type == type) {
    advance();
    return true;
  }
  return false;
}

void Parser::expect(Type type) {
  if (peek().type == type) {
    advance();
    return;
  }
  throw std::runtime_error("Runtime Error: Expected different token at line " + std::to_string(peek().position.y));
}

bool Parser::is_block_end() {
  for (Type type : endblockArray) {
    if (peek().type == type) return true;
  }
  return false;
}

void Parser::parse_block() {
  while (!is_block_end()) { parse_stat(); }
}

void Parser::parse_stat() {
  switch (peek().type) {
    case Type::KW_LOCAL:
      // parse_local();
      advance();
      break;
    case Type::KW_IF:
      // parse_if();
      advance();
      break;
    case Type::KW_WHILE:
      advance();
      break;
    case Type::KW_FOR:
      advance();
      break;
    default:
      advance();
      break;
  } 
}
