#pragma once
#include "lex.h"
#include "arena.h"
#include "ast.h" // Asumiendo que aquí definimos ASTNode y NodeIndex
#include <vector>
#include <iostream>

namespace loom {

    class Parser {
    private:
        Lexer lexer;
        Token current;
        Token previous;

        using ASTVector = std::vector<ASTNode, memory::STLAllocator<ASTNode>>;
        ASTVector nodes;

        memory::PoolAllocator* arena;

        // --- FUNCIONES AUXILIARES DE ESTADO ---
        void advance() {
            previous = current;
            current = lexer.getToken();
        }

        // Consume un token si es del tipo esperado, o falla silenciosamente (para control de flujo)
        bool match(TokenType type) {
            if (current.type == type) {
                advance();
                return true;
            }
            return false;
        }

        // Obliga a que el token actual sea de un tipo, si no, reporta error
        void consume(TokenType type, const char* err_msg) {
            if (current.type == type) {
                advance();
                return;
            }
            errorAtCurrent(err_msg);
        }

        void errorAtCurrent(const char* message) {
            std::cerr << "[Error de Sintaxis] " << message << "\n";
        }

        NodeIDX emitNode(const ASTNode& node) {
            NodeIDX idx = static_cast<NodeIDX>(nodes.size());
            nodes.push_back(node);
            return idx;
        }

    public:
        Parser(Lexer lex, memory::PoolAllocator& alloc) 
            : lexer(lex), 
              arena(&alloc),
              nodes(memory::STLAllocator<ASTNode>(alloc)) 
        {
            nodes.reserve(1024); 
            
            advance(); 
        }

        void parse();

    private:
        NodeIDX parseDeclaration();
        NodeIDX parseFunctionDecl();
        NodeIDX parseStatement();
    };

}