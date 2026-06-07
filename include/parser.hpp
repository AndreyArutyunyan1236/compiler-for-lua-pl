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

  std::unordered_map<Type, std::string> TokenToStrMAP = {
    {Type::KW_IF,          "'if'"},
    {Type::KW_ELSEIF,      "'else if'"},
    {Type::KW_ELSE,        "'else'"},
    {Type::KW_LOCAL,       "'local'"},
    {Type::KW_THEN,        "'then'"},
    {Type::KW_END,         "'end'"},
    {Type::KW_FUNCTION,    "'function'"},
    {Type::KW_RETURN,      "'return'"},
    {Type::KW_WHILE,       "'while'"},
    {Type::KW_FOR,         "'for'"},
    {Type::KW_DO,          "'do'"},
    {Type::KW_REPEAT,      "'repeat'"},
    {Type::KW_UNTIL,       "'until'"},
    {Type::KW_NIL,         "'nil'"},
    {Type::KW_IN,          "'in'"},
    {Type::KW_TRUE,        "'true'"},
    {Type::KW_FALSE,       "'false'"},
    {Type::KW_AND,         "'and'"},
    {Type::KW_OR,          "'or'"},
    {Type::KW_NOT,         "'not'"},
    {Type::IDENT,          "'identificator'"},
    {Type::CALLEDFUNCTION, "'function invocation'"},
    {Type::PLUS,           "'+'"},
    {Type::MINUS,          "'-'"},
    {Type::STAR,           "'*'"},
    {Type::PERCENT,        "'%'"},
    {Type::CARET,          "'^'"},
    {Type::HASH,           "'#'"},
    {Type::L_PAREN,        "'('"},
    {Type::R_PAREN,        "')'"},
    {Type::L_BRACE,        "'{'"},
    {Type::R_BRACE,        "'}'"},
    {Type::L_BRACKET,      "'['"},
    {Type::R_BRACKET,      "']'"},
    {Type::COMMA,          "','"},
    {Type::SEMICOLON,      "';'"},
    {Type::COLON_COLON,    "':'"},
    {Type::EQUAL,          "'='"},
    {Type::EQUAL_EQUAL,    "'=='"},
    {Type::NOT_EQUAL,      "'!='"},
    {Type::LESS,           "'<'"},
    {Type::GREATER,        "'>'"},
    {Type::LESS_EQUAL,     "'<='"},
    {Type::GREATER_EQUAL,  "'>='"},
    {Type::SLASH,          "'/'"},
    {Type::AMPERSAND,      "'&'"},
    {Type::VERTICAL_BAR,   "'|'"},
    {Type::LIT_INT,        "'integer literal'"},
    {Type::LIT_STRING,     "'string literal'"},
    {Type::LIT_FLOAT,      "'float literal'"},
    {Type::END_OF_FILE,    "'end of file'"},
  };

  Token peek();
  Token peekNext();
  Token advance();
  bool  check(Type type);
  bool checkNext(Type type); 
  void  expect(Type type);
  [[noreturn]] void throwError(Type expected);
   
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
