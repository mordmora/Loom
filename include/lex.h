#ifndef LEXER_H
#define LEXER_H

#include <cstdint>
#include <vector>
#include "str.h"
#include <ostream>

namespace umbra {
    
    enum TokenType {
        None = -2,
        Inv = -1,
        Eof = 0,
        Id = 1,
        Num = 2,
    
        //operadores
        Plus,
        Minus,
        Slash,
        Asterisk,
        LessT,
        LessOe,
        GreatT,
        GreatOe,
        Eq,
        EqEq,
        RShift,
        LShift,
        LArrow,
        Hash,
        Rbrace,
        Lbrace,
        Lpar,
        Rpar,
        Lbraq,
        Rbraq,
        Dot,
        Comma,
        Semicolon,
        Colon,
        DotDot,

        //keywords
        Int,
        Float,
        Double,
        Bool,
        Void,
        Ptr,
        Const,
        Access,
        Ref,
        Func,
        For,
        True,
        False,
        If,
        Else,
        Byte, //unsigned 8-bit integer
        //sized types
        I8,
        I16,
        I32,
        I64,
        F8,
        F16,
        F32,
        F64,
        Unsigned,
        Signed,
        Return,
        While,
        Break,
        Continue,
        StrLit,
        Comment,
    };
    
    struct Token {
        Str       lexeme;
        int32_t   start;
        TokenType type;

        Token() = default;

        Token(Str lexeme, int32_t start, std::size_t /*ignored*/, TokenType t)
            : lexeme(lexeme), start(start), type(t) {}

        friend std::ostream& operator<<(std::ostream& os, const Token& tk) {
            os << "Lexeme: " << tk.lexeme.view()
               << "\nsize: "  << tk.lexeme.getLen()
               << "\nstart: " << tk.start << "\n";
            return os;
        }
    };
    
    class Lexer {
        public:
        Lexer(Str buffer) : buffer(buffer) {
            current = buffer.getPtr();
            buffer_len = buffer.getLen();
        }
        
        Token getToken();

        private:
        Str buffer;
        std::size_t buffer_len;
        uint32_t line = 1;
        uint32_t col = 1;
        const uint8_t* current;
        std::size_t pos = 0;

    };


}    
#endif
