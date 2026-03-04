#include "arena.h"
#include "lex.h"
#include "bench.h"
#include "io.h"
#include <iostream>
#include <cstdint>
#include <vector>

int main(int argc, char** argv){

    if(argc < 2){
        std::cerr << "Faltan argumentos " << std::endl;
        return 1;
    }

    loom::SourceManager s_manager;
    uint16_t f = s_manager.loadFile(argv[1]);
    loom::SourceFileView v(s_manager.getView(f));
    Str s(static_cast<const uint8_t*>(v.buff), v.size);
    loom::Lexer l(s);

    memory::PoolAllocator _allocator(4096);
    memory::STLAllocator<loom::Token> token_alloc(_allocator);

    std::vector<loom::Token, memory::STLAllocator<loom::Token>> tokens(token_alloc);
    tokens.reserve(256);

    while (true) {
        loom::Token tk = l.getToken();
        if (tk.type == loom::TokenType::Eof || tk.type == loom::TokenType::Inv)
            break;
        tokens.push_back(tk);
    }

    std::cout << tokens.size() << " tokens lexados desde arena\n";
    for (const auto& tk : tokens)
        std::cout << tk;

    _allocator.memoryDump();

    return 0;
}
