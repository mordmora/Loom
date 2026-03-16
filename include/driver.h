#ifndef DRIVER_H
#define DRIVER_H

#include "arena.h"
#include "io.h"
#include "lex.h"
#include "bench.h"
#include <vector>
#include <iostream>

namespace loom {

    class LoomDriver {
    public:
        LoomDriver(uint32_t arena_block_size = 64 * 1024) 
            : allocator(arena_block_size), token_alloc(allocator) {}

        int run(const char* file_path) {
            BENCH_BEGIN("Total Compilation");

            uint16_t file_id;
            {
                BENCH_BEGIN("File Load");
                file_id = source_manager.loadFile(file_path);
                BENCH_END();
            }

            if (file_id == 0xFFFF) {
                return 1;
            }

            SourceFileView view = source_manager.getView(file_id);
            Str source_str(static_cast<const uint8_t*>(view.buff), view.size);
            Lexer lexer(source_str);

            std::vector<Token, memory::STLAllocator<Token>> tokens(token_alloc);
            tokens.reserve(1024);

            {
                BENCH_BEGIN("Lexing");
                while (true) {
                    Token tk = lexer.getToken();
                    if (tk.type == TokenType::Eof || tk.type == TokenType::Inv)
                        break;
                    tokens.push_back(tk);
                }
                BENCH_END();
            }

            std::cout << tokens.size() << " tokens lexados desde arena\n";


            
            BENCH_END();
            allocator.memoryDump();

            return 0;
        }

    private:
        SourceManager source_manager;
        memory::PoolAllocator allocator;
        memory::STLAllocator<Token> token_alloc;
    };

}

#endif
