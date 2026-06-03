#include <lexer.hpp>
#include <stdbool.h>
#include <memory>

#ifndef PARSER_HPP
#define PARSER_HPP

struct Node {
  Token value;
  Token className;
  bool isLocal = false;
  Type op;


  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  std::unique_ptr<Node> condition;

  std::vector<std::unique_ptr<Node>> arrayElements;
  std::vector<std::unique_ptr<Node>> Args;
  std::vector<std::unique_ptr<Node>> body;
  std::vector<std::unique_ptr<Node>> elseifs;
  std::vector<std::unique_ptr<Node>> elseBody;
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
  bool checkNext(Type type); 
  void  expect(Type type);
  [[noreturn]] void throwError();

  bool endblock();
  int  get_lbp(); 
  std::unique_ptr<Node> nud(); 
  std::unique_ptr<Node> parse_expr(int min_lbp);

  std::unique_ptr<Node> parse_if();
  std::unique_ptr<Node> parse_elseif();
  std::unique_ptr<Node> parse_while();
  std::unique_ptr<Node> parse_for();
  std::unique_ptr<Node> parse_local();
  std::unique_ptr<Node> parse_function(bool isLocal);
  std::unique_ptr<Node> parse_ident();
  std::unique_ptr<Node> parse_method(Token className, bool isLocal); 

public:
  std::vector<std::unique_ptr<Node>> parse_block(); 
  std::unique_ptr<Node> parse_stat();

  Parser(std::vector<Token> VectorOfTokens) : listOfTokens(VectorOfTokens) {}
};

#endif
