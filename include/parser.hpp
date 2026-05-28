#include <lexer.hpp>
#include <stdbool.h>
#include <memory>

#ifndef PARSER_HPP
#define PARSER_HPP

struct Node {
  Token value;
  bool isError = false;
  Type op;

  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
};

class Parser {
private:
  std::vector<Token> listOfTokens;
  unsigned long long index = 0;
  Type endblockArray[5] = {Type::KW_END, Type::KW_ELSE,
                          Type::KW_ELSEIF, Type::KW_UNTIL,
                          Type::END_OF_FILE};
  Token peek();
  Token peekNext();
  Token advance();
  bool  check(Type type);
  bool  match(Type type);
  void  expect(Type type);

  bool endblock();
  int  get_lbp(); 
  std::unique_ptr<Node> nud(); 
  std::unique_ptr<Node> parse_expr(int min_lbp);

  /*
   * funcitons to parse every keyword by using pratt (will be used in parse_stat method):
   * parse_local(), parse_if() ...
  */

public:
  void parse_block();
  void parse_stat();

  Parser(std::vector<Token> VectorOfTokens) : listOfTokens(VectorOfTokens) {}
};

#endif
