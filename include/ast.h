#pragma once

#include "lex.h"

namespace umbra {

    using NodeIDX = uint32_t;
    constexpr NodeIDX NullNode = 0xFFFFFFFF;

enum class NodeType : uint8_t {
        Error = 0,

        VarDecl,
        StructDecl,
        FuncDecl,
        
        BlockStmt,
        IfStmt,
        ForStmt,
        WhileStmt,
        ReturnStmt,
        BreakStmt,
        ContinueStmt,
        ExprStmt,

        BinaryExpr,
        UnaryExpr,
        CallExpr,
        IndexExpr,
        MemberExpr,
        
        TypeBase,
        TypePtr,
        TypeArray,
        TypeGeneric,

        Attribute,
        Identifier,
        IntLiteral,
        FloatLiteral,
        StringLiteral,
        BoolLiteral
    };


    struct BinOp {
        NodeIDX left;
        NodeIDX right;
        TokenType op;
    };

    struct UnaryOp {
        NodeIDX operand;
        TokenType op; 
    };

    struct VarDecl {
        NodeIDX type_idx;
        NodeIDX init_expr;
        NodeIDX attr;
    };

}