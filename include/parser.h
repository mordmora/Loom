#pragma once
#include "arena.h"
#include "ast.h"
#include "lex.h"
#include <cstdint>
#include <iostream>
#include <vector>

namespace loom
{

  class Parser
  {
  private:
    Lexer *lexer;

    using ASTVector = std::vector<ASTNode, memory::STLAllocator<ASTNode>>;
    using ExtraV = std::vector<uint32_t, memory::STLAllocator<uint32_t>>;
    ASTVector nodes;
    ExtraV extra;
    ExtraV errs;

    Token curr_tok;

    memory::PoolAllocator *arena;

    bool had_error = false;

    Token peek()
    {
      return curr_tok;
    }

    Token advance()
    {
      Token prev = curr_tok;
      curr_tok = lexer->getToken();
      return prev;
    }

    Token expect(TokenType t)
    {
      if (peek().type != t)
      {
        errs.push_back(curr_tok.start);
        had_error = true;
        return curr_tok;
      }
      return advance();
    }

    bool match(TokenType t)
    {
      if (peek().type == t)
      {
        advance();
        return true;
      }
      return false;
    }

    bool check(TokenType t)
    {
      return peek().type == t;
    }

    NodeType tokenToType(TokenType t);

  public:
    Parser(Lexer &lex, memory::PoolAllocator &alloc)
        : lexer(&lex), arena(&alloc), nodes(memory::STLAllocator<ASTNode>(alloc)),
          extra(memory::STLAllocator<uint32_t>(alloc))
    {
      nodes.reserve(1024);
      errs.reserve(1024);
      extra.reserve(1024);
      curr_tok = lexer->getToken();
    }

    NodeIDX parse();

    NodeIDX parseVarDecl();

    NodeIDX parseExpr();

    NodeIDX parseType();

    NodeIDX parseAttr();

    NodeIDX binaryExpr();
  };

} // namespace loom
