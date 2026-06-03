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

bool Parser::checkNext(Type type) {
  if (peekNext().type == type) {
    return true;
  }
  return false;
}

void Parser::expect(Type type) {
  if (peek().type == type) {
    advance();
    return;
  }
  throwError();
}

void Parser::throwError() {
  throw std::runtime_error("Runtime Error: Expected different token at line " + std::to_string(peek().position.y));
}

bool Parser::endblock() {
  for (Type type : endblockArray) {
    if (check(type)) return true;
  }
  return false;
}

std::unique_ptr<Node> Parser::parse_local() {
  advance();

  if (!check(Type::IDENT)) {
    if (check(Type::KW_FUNCTION)) {
     return parse_function(true);
    }
    else throwError();
  }

  Token name = advance();
  std::unique_ptr<Node> expr = nullptr;

  if (check(Type::L_BRACKET)) {
    advance();
    std::vector<std::unique_ptr<Node>> array;
    std::unique_ptr<Node> element;

    while (!check(Type::R_BRACKET)) { 
      element = parse_expr(0);
      array.push_back(std::move(element)); 

      if (!checkNext(Type::R_BRACKET)) advance();
    }

    return std::make_unique<Node>(Node{
      .value = name,
      .op = Type::KW_LOCAL,
      .arrayElements = std::move(array),
    });
  }

  if (check(Type::EQUAL)) {
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
  advance();

  std::unique_ptr<Node> condition = parse_expr(0); 
  std::vector<std::unique_ptr<Node>> body = parse_block();
  expect(Type::KW_END);

  return std::make_unique<Node>(Node{
    .op = Type::KW_WHILE,
    .condition = std::move(condition),
    .body = std::move(body), 
  });
}

std::unique_ptr<Node> Parser::parse_for() {
  advance();

  std::unique_ptr<Node> condition = parse_expr(0); 
  std::vector<std::unique_ptr<Node>> body = parse_block();
  expect(Type::KW_END);

  return std::make_unique<Node>( Node{
    .op = Type::KW_FOR,
    .condition = std::move(condition),
    .body = std::move(body), 
  });
}

std::unique_ptr<Node> Parser::parse_if() {
  advance();

  std::unique_ptr<Node> condition = parse_expr(0); 

  if (check(Type::KW_THEN)) {
    advance();
  } else throwError();

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
    .op = Type::KW_IF,
    .condition = std::move(condition),
    .body = std::move(body), 
    .elseifs = std::move(elseifs),
    .elseBody = std::move(elseBody),
  });
}

std::unique_ptr<Node> Parser::parse_elseif() {
  advance();

  std::unique_ptr<Node> condition = parse_expr(0);

  if (check(Type::KW_THEN)) {
    advance();
  } else throwError();
  std::vector<std::unique_ptr<Node>> body = parse_block(); 

  return std::make_unique<Node>(Node{
    .op = Type::KW_ELSEIF,
    .condition = std::move(condition),
    .body = std::move(body), 
  });
}

std::unique_ptr<Node> Parser::parse_function(bool isLocal) {
  advance();
  
  if (!check(Type::IDENT)) {
    throwError();
  }
  Token funcName = peek();
  advance();

  std::vector<std::unique_ptr<Node>> Args;
  if (check(Type::L_PAREN)) {
    advance();

    while (!check(Type::R_PAREN)) {
      if (!check(Type::IDENT)) {
        throwError();
      }

      Args.push_back(std::make_unique<Node>(Node{.value = peek(), .op = Type::IDENT}));
      advance();

      if (check(Type::COMMA)) advance();
    }

    advance();
  }
  else {
    if (check(Type::COLON_COLON)) {
      advance();
      Token className = peek();
      return parse_method(className, isLocal);
    }
    else throwError();
  } 
  
  std::vector<std::unique_ptr<Node>> body = parse_block(); 
  expect(Type::KW_END);

  return std::make_unique<Node>(Node{
    .value = funcName,
    .isLocal = isLocal,
    .op = Type::KW_FUNCTION,
    .Args = std::move(Args),
    .body = std::move(body),
  });
}

std::unique_ptr<Node> Parser::parse_method(Token className, bool isLocal) {
  advance();
  
  if (!check(Type::IDENT)) {
    throwError();
  }
  Token methodName = peek();
  advance();

  std::vector<std::unique_ptr<Node>> Args;
  if (check(Type::L_PAREN)) {
    advance();

    while (!check(Type::R_PAREN)) {
      if (!check(Type::IDENT)) {
        throwError();
      }

      Args.push_back(std::make_unique<Node>(Node{.value = peek(), .op = Type::IDENT}));
      advance();

      if (check(Type::COMMA)) advance();
    }

    advance();
  }
  else throwError();
  
  std::vector<std::unique_ptr<Node>> body = parse_block(); 
  expect(Type::KW_END);

  return std::make_unique<Node>(Node{
    .value = methodName,
    .className = className,
    .isLocal = isLocal,
    .op = Type::KW_FUNCTION,
    .Args = std::move(Args),
    .body = std::move(body),
  });
}

std::unique_ptr<Node> Parser::parse_ident() {
  Token value = peek();
  advance();

  std::unique_ptr<Node> args;
  std::vector<std::unique_ptr<Node>> argsVect;

  Type op;

  if (check(Type::L_PAREN)) {
    advance();
    while (!check(Type::R_PAREN)) {
      args = parse_expr(0);
      argsVect.push_back(std::move(args));

      if (!checkNext(Type::R_PAREN)) advance();
    }

    op = Type::CALLEDFUNCTION;
  }
  else if (check(Type::EQUAL)) {
    advance();

    args = parse_expr(0);
    argsVect.push_back(std::move(args));

    op = Type::IDENT;
  }
  else {
    throwError();
  }

  return std::make_unique<Node>(Node{
    .value = value,
    .op = op, 
    .Args = std::move(argsVect),
  });
}

std::vector<std::unique_ptr<Node>> Parser::parse_block() {
  std::vector<std::unique_ptr<Node>> block;

  while (!endblock() && !check(Type::END_OF_FILE)) {
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
    case Type::IDENT:
      return parse_ident();
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
  if (check(Type::IDENT) || check(Type::LIT_INT) ||
      check(Type::LIT_FLOAT) || check(Type::LIT_STRING)
      || check(Type::KW_TRUE) || check(Type::KW_FALSE))
  {
    Token value = peek();
    advance();

    return std::make_unique<Node>(Node{.value = value});
  }
  else if (check(Type::L_PAREN)) {
    advance();
    std::unique_ptr<Node> ParenExpr = parse_expr(0); 
    expect(Type::R_PAREN);
    
    return ParenExpr;
  }

  throwError();
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
