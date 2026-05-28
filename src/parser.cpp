#include <parser.hpp>
#include <stdexcept>

Token Parser::peek() {
  if (index >= listOfTokens.size()) return Token{.type = Type::END_OF_FILE};  
  return listOfTokens[index];
}

Token Parser::peekNext() {
  if (index + 1 >= listOfTokens.size()) return Token{.type = Type::END_OF_FILE};  
  return listOfTokens[index + 1];
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

bool Parser::endblock() {
  for (Type type : endblockArray) {
    if (peek().type == type) return true;
  }
  return false;
}

void Parser::parse_block() {
  while (!endblock()) { parse_stat(); }
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

int Parser::get_lbp() {
  switch (peek().type) {
    case Type::EQUAL:         return 10;
    case Type::VERTICAL_BAR:  return 20;
    case Type::AMPERSAND:     return 30;
    case Type::EQUAL_EQUAL:   return 40;
    case Type::NOT_EQUAL:     return 40;
    case Type::GREATER:       return 50;
    case Type::GREATER_EQUAL: return 50;
    case Type::LESS:          return 50;
    case Type::LESS_EQUAL:    return 50;
    case Type::PLUS:          return 60;
    case Type::MINUS:         return 60;
    case Type::SLASH:         return 70;
    case Type::STAR:
      if (peekNext().type == Type::STAR) return 80;
      return 70;

    default: return 0;
  }
}


std::unique_ptr<Node> Parser::nud() {
  if (peek().type == Type::IDENT   ||
      peek().type == Type::LIT_INT ||
      peek().type == Type::LIT_FLOAT) 
  {
    Token value = peek();
    advance();

    return std::make_unique<Node>(Node{.value = value});
  }
  else if (peek().type == Type::L_PAREN) {
    advance();
    std::unique_ptr<Node> ParenExpr = parse_expr(0); 
    expect(Type::R_PAREN);
    
    return ParenExpr;
  }

  return std::make_unique<Node>(Node{.isError = true});
}


std::unique_ptr<Node> Parser::parse_expr(int min_lbp) {
  std::unique_ptr<Node> left = nud(); 
  
  while (true) {
    if (left->isError) break;
 
    int lbp = get_lbp();
    if (lbp <= min_lbp) break;

    Type op = peek().type;

    if (op == Type::STAR && peekNext().type == Type::STAR) {
      advance(); advance();

      std::unique_ptr<Node> right = parse_expr(lbp - 1); 
      left = std::make_unique<Node>(Node{
        .op = op,
        .left = std::move(left),
        .right = std::move(right),
      });

      continue;
    }

    advance();

    std::unique_ptr<Node> right = parse_expr(lbp); 
    left = std::make_unique<Node>(Node{
      .op = op,
      .left = std::move(left),
      .right = std::move(right),
    });
  }

  return left;
}
