#pragma once

#include "lex.h"

namespace loom
{

    using NodeIDX = uint32_t;
    constexpr NodeIDX NullNode = 0xFFFFFFFF;

    enum class NodeType : uint8_t
    {
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

    struct BinOp
    {
        NodeIDX left;
        NodeIDX right;
        TokenType op;
    };

    struct UnaryOp
    {
        NodeIDX operand;
        TokenType op;
    };

    struct VarDecl
    {
        NodeIDX type_idx;
        NodeIDX init_expr;
        NodeIDX attr;
    };

    struct FuncDecl
    {
        NodeIDX ret_type;
        NodeIDX body_block;
        NodeIDX attr;
        uint32_t params_start;
        uint16_t params_count;
    };

    struct Call
    {
        NodeIDX callee;
        uint32_t params_start;
        uint16_t params_count;
    };

    struct IfDecl
    {
        NodeIDX cond;
        NodeIDX then;
        NodeIDX els; // can be nullIDX
    };

    struct ForDecl
    {
        NodeIDX init_decl;
        NodeIDX cond;
        NodeIDX post_exp;
        NodeIDX body_block;
    };

    struct Attribute
    {
        NodeIDX param_expr;
    };

    struct ASTNode
    {

        NodeType t;
        uint32_t tk_pos;

        union
        {
            uint64_t int_val;
            double float_val;
            BinOp bin_op;
            UnaryOp unary_op;
            VarDecl var_decl;
            FuncDecl func_decl;
            Call call;
            IfDecl if_stmt;
            ForDecl for_stmt;
            Attribute attribute;

            struct
            {
                uint32_t start;
                uint32_t count;
            } list;

            NodeIDX child;
        } data;
    };
    static_assert(sizeof(ASTNode) <= 32, "ASTNode se volvio demasiado grande");
}