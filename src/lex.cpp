#include "lex.h"
#include <iostream>


namespace umbra {

    struct SymbolTable {
        bool data[256] = {false};

        constexpr SymbolTable() {
            data['+'] = true; data['-'] = true; data['*'] = true; data['/'] = true;
            data['<'] = true; data['>'] = true; data['='] = true;
            data['('] = true; data[')'] = true; data['{'] = true; data['}'] = true;
            data['['] = true; data[']'] = true; data[','] = true; data[';'] = true;
            data['.'] = true; data[':'] = true; data['#'] = true;
            data['"'] = true;
        }
    };

    struct TokenMap {
        TokenType d[256];
        constexpr TokenMap() : d{} {
            for (int i = 0; i < 256; i++) d[i] = TokenType::None;
            d['+'] = TokenType::Plus;
            d['-'] = TokenType::Minus;
            d['*'] = TokenType::Asterisk;
            d['/'] = TokenType::Slash;
            d['='] = TokenType::Eq;
            d['<'] = TokenType::LessT;
            d['>'] = TokenType::GreatT;
            d['('] = TokenType::Lpar;
            d[')'] = TokenType::Rpar;
            d['{'] = TokenType::Lbrace;
            d['}'] = TokenType::Rbrace;
            d['['] = TokenType::Lbraq;
            d[']'] = TokenType::Rbraq;
            d[','] = TokenType::Comma;
            d[';'] = TokenType::Semicolon;
            d['.'] = TokenType::Dot;
            d[':'] = TokenType::Colon;
            d['#'] = TokenType::Hash;
        }
    };

    struct Keyword {
        const char* str;
        std::size_t len;
        TokenType type;
    };

    static constexpr Keyword keywords[] = {
        {"int",      3,  TokenType::Int},
        {"float",    5,  TokenType::Float},
        {"double",   6,  TokenType::Double},
        {"bool",     4,  TokenType::Bool},
        {"void",     4,  TokenType::Void},
        {"ptr",      3,  TokenType::Ptr},
        {"const",    5,  TokenType::Const},
        {"access",   6,  TokenType::Access},
        {"ref",      3,  TokenType::Ref},
        {"func",     4,  TokenType::Func},
        {"for",      3,  TokenType::For},
        {"true",     4,  TokenType::True},
        {"false",    5,  TokenType::False},
        {"if",       2,  TokenType::If},
        {"else",     4,  TokenType::Else},
        {"byte",     4,  TokenType::Byte},
        {"i8",       2,  TokenType::I8},
        {"i16",      3,  TokenType::I16},
        {"i32",      3,  TokenType::I32},
        {"i64",      3,  TokenType::I64},
        {"f8",       2,  TokenType::F8},
        {"f16",      3,  TokenType::F16},
        {"f32",      3,  TokenType::F32},
        {"f64",      3,  TokenType::F64},
        {"unsigned", 8,  TokenType::Unsigned},
        {"signed",   6,  TokenType::Signed},
        {"return",   6,  TokenType::Return},
        {"while",    5,  TokenType::While},
        {"break",    5,  TokenType::Break},
        {"continue", 8,  TokenType::Continue},
    };

    static constexpr std::size_t keywords_count = sizeof(keywords) / sizeof(keywords[0]);

    static TokenType lookup_keyword(const uint8_t* p, std::size_t len) {
        for (std::size_t i = 0; i < keywords_count; i++) {
            if (keywords[i].len == len && std::memcmp(keywords[i].str, p, len) == 0)
                return keywords[i].type;
        }
        return TokenType::Id;
    }

    static constexpr SymbolTable is_symbol;
    static constexpr TokenMap tk_map;

    #define RELEASE_COMPOUND_SYMBOL(T)   do\
    {                   \
        current++;      \
        pos++;          \
        col++;          \
        return Token(Str(start_p, 2), start_pos, 2, TokenType::T); \
    } while (false)     

    #define ADVANCE() do { current++; pos++; col++; } while (false)

    Token Lexer::getToken() {
        lexing:
        while (pos < buffer_len) {
            uint8_t w = *current;
            if (w == ' ' || w == '\t' || w == '\v' || w == '\f') {
                col++;
            } else if (w == '\r') {
                line++;
                col = 1;
                current++; pos++;
                if (pos < buffer_len && *current == '\n') {
                    current++; pos++;
                }
                continue;
            } else if (w == '\n') {
                line++;
                col = 1;
            } else {
                break;
            }
            current++;
            pos++;
        }

        if (pos >= buffer_len) {
            return Token(Str("EOF"), pos, 3, TokenType::Eof);
        }

        const uint8_t* start_p = current;
        uint32_t start_pos = pos;
        uint8_t c = *current;

        if (c == '"') {
            ADVANCE();
            while (pos < buffer_len) {
                c = *current;
                current++;
                pos++;
                if (c == '\n') {
                    line++;
                    col = 1;
                } else {
                    col++;
                }
                if (c == '\\' && pos < buffer_len) {
                    c = *current;
                    current++;
                    pos++;
                    if (c == '\n') { line++; col = 1; } else col++;
                    continue;
                }
                if (c == '"') break;
            }
            std::size_t len = pos - start_pos;
            return Token(Str(start_p, len), start_pos, len, TokenType::StrLit);
        }

        TokenType type = tk_map.d[c];
        if (type != TokenType::None) {
            ADVANCE();
            
            if(pos < buffer_len){
                uint8_t nc = *current;

                switch (c) {
                    case '-':
                        if (nc == '>') RELEASE_COMPOUND_SYMBOL(LArrow);
                        break;
                    case '=':
                        if (nc == '=') RELEASE_COMPOUND_SYMBOL(EqEq);
                        break;
                    case '>':
                        if (nc == '=') RELEASE_COMPOUND_SYMBOL(GreatOe);
                        if (nc == '>') RELEASE_COMPOUND_SYMBOL(RShift);
                        break;
                    case '<':
                        if (nc == '=') RELEASE_COMPOUND_SYMBOL(LessOe);
                        if (nc == '<') RELEASE_COMPOUND_SYMBOL(LShift);
                        break;
                    case '.':
                        if (nc == '.') RELEASE_COMPOUND_SYMBOL(DotDot);
                        break;
                    case '/':
                        if (nc == '/') {
                            while (pos < buffer_len && *current != '\n') {
                                current++;
                                pos++;
                                col++;
                            }
                            goto lexing;
                        }
                        if (nc == '*') {
                            ADVANCE();
                            uint32_t comment_line = line;
                            bool closed = false;
                            while (pos + 1 < buffer_len) {
                                if (*current == '\n') { line++; col = 1; }
                                if (*current == '*' && *(current + 1) == '/') {
                                    current += 2; pos += 2; col += 2;
                                    closed = true;
                                    break;
                                }
                                ADVANCE();
                            }
                            if (!closed) {
                                std::cerr << "error: comentario abierto en linea " << comment_line << " nunca fue cerrado\n";
                                return Token(Str(start_p, pos - start_pos), start_pos, pos - start_pos, TokenType::Inv);
                            }
                            goto lexing;
                        }
                        break;
                }
            }

            return Token(Str(start_p, 1), start_pos, 1, type);
        }

        if (c >= '0' && c <= '9') {
            while (pos < buffer_len) {
                c = *current;
                if ((c < '0' || c > '9') && c != '.') break;
                ADVANCE();
            }
            std::size_t len = pos - start_pos;
            return Token(Str(start_p, len), start_pos, len, TokenType::Num);
        }

        while (pos < buffer_len) {
            c = *current;
            if (c == ' ' || c == '\n' || c == '\t' || is_symbol.data[c]) {
                break; 
            }
            ADVANCE();
        }
        
        std::size_t len = pos - start_pos;
        TokenType kw = lookup_keyword(start_p, len);
        return Token(Str(start_p, len), start_pos, len, kw);
    }
}