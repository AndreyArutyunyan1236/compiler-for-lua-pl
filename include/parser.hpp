#include <lexer.hpp>
#include <stdbool.h>

#ifndef PARSER_HPP
#define PARSER_HPP

class Parser {
private:
  std::vector<Token> listOfTokens;
  unsigned long long index = 0;
  Type endblockArray[5] = {Type::KW_END, Type::KW_ELSE,
                          Type::KW_ELSEIF, Type::KW_UNTIL,
                          Type::END_OF_FILE};
  Token peek();
  Token advance();
  bool check(Type type);
  bool match(Type type);
  void expect(Type type);

  /*
   * funcitons to parse every keyword by using pratt (will be used in parse_stat method):
   * parse_local(), parse_if() ...
  */

  bool is_block_end();

public:
  void parse_block();
  void parse_stat();
};

#endif
