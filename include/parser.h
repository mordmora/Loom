#pragma once

#include "lex.h"
#include "arena.h"
namespace loom {

    class Parser {
        public:
        explicit Parser(Lexer& lexer, memory::STLAllocator<uint8_t> allocator) : lex(lexer), allocator(allocator) {}

        private:
        Lexer lex;
        memory::STLAllocator<uint8_t> allocator;
        Token current;
        Token prev;

    };

} 