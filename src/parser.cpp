#include "parser.h"

namespace loom {


    NodeIDX Parser::parseVarDecl(){
        Token id = expect(TokenType::Id);
        if(had_error){
            //TODO: sync
        }

        (void)expect(TokenType::Colon);
        if(had_error){
            //TODO: sync
        }

        ASTNode node;
        node.t = NodeType::VarDecl;
        node.tk_pos = id.start;
        node.data.var_decl.type_idx = parseType();
        node.data.var_decl.init_expr = parseExpr();
        node.data.var_decl.attr = parseAttr();
        nodes.push_back(node);
        return nodes.size() - 1;

    }

    NodeType Parser::tokenToType(TokenType t){
        switch (t)
        {
        case TokenType::Int :
        case TokenType::Float :
        case TokenType::Double :
        case TokenType::Bool :
        case TokenType::Void :
        return NodeType::TypeBase;
        default:
            return NodeType::Error;
            break;
        }
    }

    NodeIDX Parser::parseType(){

        if(check(TokenType::Asterisk)){
            //punteros
        }

        if(check(TokenType::Lbraq)){
            //arrays
        }

        Token type_name = expect(TokenType::Id);
        if(had_error){
            //TODO: sync
        }

        NodeType type = tokenToType(type_name.type);
        if(type == NodeType::Error){
            errs.push_back(type_name.start);
            had_error = true;
            //TODO: sync
        }

        ASTNode node;
        node.t = type;
        node.tk_pos = type_name.start;
        nodes.push_back(node);
        return nodes.size() - 1;
    }


    NodeIDX Parser::parseExpr(){
        return NullNode;
    }

    NodeIDX Parser::parseAttr(){
        return NullNode;
    }



}