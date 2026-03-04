#pragma once

#include "lex.h"
#include "arena.h"
namespace umbra {

    class Parser {
        public:
        explicit Parser(Lexer& lexer, memory::STLAllocator allocator) : lex(lexer), allocator(allocator) {}

        private:
        Lexer lex;
        memory::STLAllocator allocator;
        Token current;
        Token prev;

    };

} 