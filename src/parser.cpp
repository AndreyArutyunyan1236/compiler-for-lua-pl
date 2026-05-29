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

std::unique_ptr<Node> Parser::parse_local() {
  advance();

  if (peek().type != Type::IDENT) {
    if (peek().type == Type::KW_FUNCTION) {
     return parse_function(true);
    }
    else throw std::runtime_error("Runtime Error: Expected identificator at line " + std::to_string(peek().position.y));
  }

  Token name = peek();
  advance();

  std::unique_ptr<Node> expr = nullptr;
  if (peek().type == Type::EQUAL) {
    advance();
    expr = parse_expr(0);
  }

  return std::make_unique<Node>(Node{
    .value = name,
    .op = Type::KW_LOCAL,
    .right = std::move(expr),
  });
}

std::unique_ptr<Node> Parser::parse_while() {
  std::unique_ptr<Node> kwWhile = std::make_unique<Node>(Node{.op = Type::KW_WHILE});
  advance();

  std::unique_ptr<Node> condition = parse_expr(0); 
  std::vector<std::unique_ptr<Node>> body = parse_block();
  expect(Type::KW_END);

  return std::make_unique<Node>(Node{
    .op = kwWhile->op,
    .condition = std::move(condition),
    .body = std::move(body), 
  });
}

std::unique_ptr<Node> Parser::parse_for() {
  std::unique_ptr<Node> kwFor = std::make_unique<Node>(Node{.op = Type::KW_FOR});
  advance();

  std::unique_ptr<Node> condition = parse_expr(0); 
  std::vector<std::unique_ptr<Node>> body = parse_block();
  expect(Type::KW_END);

  return std::make_unique<Node>( Node{
    .op = kwFor->op,
    .condition = std::move(condition),
    .body = std::move(body), 
  });
}

std::unique_ptr<Node> Parser::parse_if() {
  std::unique_ptr<Node> kwIf = std::make_unique<Node>( Node{ .op = Type::KW_IF }); 
  advance();

  std::unique_ptr<Node> condition = parse_expr(0); 

  if (peek().type == Type::KW_THEN) {
    advance();
  } else throw std::runtime_error("Runtime Error: Expected different keyoword (then) at line " + std::to_string(peek().position.y));

  std::vector<std::unique_ptr<Node>> body = parse_block(); 

  std::vector<std::unique_ptr<Node>> elseifs;
  while (peek().type == Type::KW_ELSEIF) {
    elseifs.push_back(parse_elseif());
  }

  std::vector<std::unique_ptr<Node>> elseBody;
  while (peek().type == Type::KW_ELSE) {
    advance();
    elseBody = parse_block();
  }

  expect(Type::KW_END);

  return std::make_unique<Node>(Node{
    .op = kwIf->op,
    .condition = std::move(condition),
    .body = std::move(body), 
    .elseifs = std::move(elseifs),
    .elseBody = std::move(elseBody),
  });
}

std::unique_ptr<Node> Parser::parse_elseif() {
  std::unique_ptr<Node> kwelseif = std::make_unique<Node>( Node{ .op = Type::KW_ELSEIF }); 
  advance();

  std::unique_ptr<Node> condition = parse_expr(0);

  if (peek().type == Type::KW_THEN) {
    advance();
  } else {
    throw std::runtime_error("Runtime Error: Expected different keyoword (then) at line " + std::to_string(peek().position.y));
  }

  std::vector<std::unique_ptr<Node>> body = parse_block(); 

  return std::make_unique<Node>(Node{
    .op = kwelseif->op,
    .condition = std::move(condition),
    .body = std::move(body), 
  });
}

std::unique_ptr<Node> Parser::parse_function(bool isLocal) {
  std::unique_ptr<Node> kwFunc = std::make_unique<Node>(Node{ .op = Type::KW_FUNCTION }); 
  advance();
  
  if (peek().type != Type::IDENT) {
    throw std::runtime_error("Runtime Error: Expected identificator at line " + std::to_string(peek().position.y));
  }
  Token funcName = peek();
  advance();

  std::vector<std::unique_ptr<Node>> Args;
  if (peek().type == Type::L_PAREN) {
    advance();

    while (peek().type != Type::R_PAREN) {
      if (peek().type != Type::IDENT) {
        throw std::runtime_error("Runtime Error: Expected identificator at line " + std::to_string(peek().position.y));
      }

      Args.push_back(std::make_unique<Node>(Node{.value = peek(), .op = Type::IDENT}));
      advance();

      if (peek().type == Type::COMMA) advance();
    }

    advance();
  }
  else throw std::runtime_error("Runtime Error: Expected different token at line " + std::to_string(peek().position.y));
  
  std::vector<std::unique_ptr<Node>> body = parse_block(); 
  expect(Type::KW_END);

  return std::make_unique<Node>(Node{
    .value = funcName,
    .isLocal = isLocal,
    .op = kwFunc->op,
    .args = std::move(Args),
    .body = std::move(body),
  });
}

std::vector<std::unique_ptr<Node>> Parser::parse_block() {
  std::vector<std::unique_ptr<Node>> block;

  while (!endblock() && peek().type != Type::END_OF_FILE) {
    std::unique_ptr<Node> statement = parse_stat(); 
    if (statement) {
      block.push_back(std::move(statement)); 
    }
  }

  return block;
}

std::unique_ptr<Node> Parser::parse_stat() {
  switch (peek().type) {
    case Type::KW_LOCAL:
      return parse_local();
      break;
    case Type::KW_IF:
      return parse_if();
      break;
    case Type::KW_WHILE:
      return parse_while();
      break;
    case Type::KW_FOR:
      return parse_for();
      break;
    case Type::KW_FUNCTION:
      return parse_function(false);
    default:
      return parse_expr(0); 
  } 
}

int Parser::get_lbp() {
  switch (peek().type) {
    case Type::EQUAL:         return 10;
    case Type::COMMA:         return 10;
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
  if (peek().type == Type::IDENT || peek().type == Type::LIT_INT ||
      peek().type == Type::LIT_FLOAT || peek().type == Type::LIT_STRING)
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

  throw std::runtime_error("Runtime Error: Expected different token at line " + std::to_string(peek().position.y));
}

std::unique_ptr<Node> Parser::parse_expr(int min_lbp) {
  std::unique_ptr<Node> left = nud(); 
  
  while (true) {
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

    std::unique_ptr<Node> right; 
    if (op == Type::EQUAL) {
      right = parse_expr(lbp-1);
    }
    else right = parse_expr(lbp);

    left = std::make_unique<Node>(Node{
      .op = op,
      .left = std::move(left),
      .right = std::move(right),
    });
  }

  return left;
}
