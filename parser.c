#ifndef _PARSER_
#define _PARSER_

#include <string.h>
#include "lexer.c"

#define AST_NEW(tag, ...) \
  ast_new((AST){tag, {.tag=(struct tag){__VA_ARGS__}}})

// TODO: token concatenation ## macro operator

typedef struct AST AST; // Forward reference

struct AST {
  enum {
    AST_ARGS,
    AST_CALL,
    AST_FUN,
    AST_ID,
    AST_STR_LITERAL,
  } tag;

  union {
    // TODO: Is there support to args list instead?
    struct AST_ARGS {
      AST *arg1;
      AST *arg2;
      AST *arg3;
    } AST_ARGS;

    struct AST_CALL {
      AST *id;
      AST *args;
    } AST_CALL;

    struct AST_FUN {
      AST *id;
      AST *body;
    } AST_FUN;

    struct AST_ID {
      char *name;
    } AST_ID;

    struct AST_STR_LITERAL {
      char *value;
    } AST_STR_LITERAL;
  } data;
};

AST *ast_new(AST ast) {
  AST *ptr = malloc(sizeof(AST));
  if (ptr) {
    *ptr = ast;
  }
  return ptr;
}

void ast_free(AST *ptr) {
  if (!ptr) return;

  AST ast = *ptr;
  switch (ast.tag) {
    case AST_ARGS: {
      struct AST_ARGS data = ast.data.AST_ARGS;
      ast_free(data.arg1);
      ast_free(data.arg2);
      ast_free(data.arg3);
      break;
    }
    case AST_CALL: {
      struct AST_CALL data = ast.data.AST_CALL;
      ast_free(data.id);
      ast_free(data.args);
      break;
    }
    case AST_FUN: {
      struct AST_FUN data = ast.data.AST_FUN;
      ast_free(data.id);
      ast_free(data.body);
      break;
    }
    case AST_ID: {
      struct AST_ID data = ast.data.AST_ID;
      free(data.name);
      break;
    }
    case AST_STR_LITERAL: {
      struct AST_STR_LITERAL data = ast.data.AST_STR_LITERAL;
      free(data.value);
      break;
    }
    default: {
      break;
    }
  }
  free(ptr);
}

void ast_print(AST *ptr) {
  if (!ptr) return;

  AST ast = *ptr;
  switch (ast.tag) {
    case AST_ARGS: {
      struct AST_ARGS data = ast.data.AST_ARGS;
      printf("ARGS: \n");
      ast_print(data.arg1);
      ast_print(data.arg2);
      ast_print(data.arg3);
      break;
    }
    case AST_CALL: {
      struct AST_CALL data = ast.data.AST_CALL;
      printf("CALL: \n");
      ast_print(data.id);
      ast_print(data.args);
      break;
    }
    case AST_FUN: {
      struct AST_FUN data = ast.data.AST_FUN;
      printf("FUN: \n");
      ast_print(data.id);
      ast_print(data.body);
      break;
    }
    case AST_ID: {
      struct AST_ID data = ast.data.AST_ID;
      printf("ID: %s\n", data.name);
      break;
    }
    case AST_STR_LITERAL: {
      struct AST_STR_LITERAL data = ast.data.AST_STR_LITERAL;
      printf("STR: %s\n", data.value);
      break;
    }
    default: {
      fprintf(stderr, "AST not supported yet: %d", ast.tag);
      break;
    }
  }
}

char *ast_txt_cpy(char *txt) {
  char *ptr = malloc(strlen(txt) + 1);
  if (ptr) {
    strcpy(ptr, txt);
  }
  return ptr;
}

AST *ast_expression(FILE *file, TokenType t);

// stringexpr ::= '"' string '"'
AST *ast_str_literal(void) {
  AST *ast = AST_NEW(AST_STR_LITERAL, ast_txt_cpy(text));
  return ast;
}

// parexpr ::= '(' <expr> ')'
AST *ast_par_expression(FILE *file) {
  TokenType t = getToken(file); // skip '('
  if (t == RPar) {
    // empty params
    return NULL;
  }
  
  AST *ast = ast_expression(file, t);
  t = getToken(file);
  if (t != RPar) {
    ast_free(ast);
    fprintf(stderr, "Expected ')' but found: %d\n", t);
    return NULL;
  }

  return ast;
}

// identifierexpr 
//   ::= identifier
//   ::= identifier <parexpr>
AST *ast_identifier(FILE *file) {
  AST *id = AST_NEW(AST_ID, ast_txt_cpy(name));
  TokenType t = getToken(file); // skip identifier
  if (t != LPar) {
    return id;
  }

  AST *args = ast_par_expression(file);
  AST *ast = AST_NEW(
      AST_CALL,
      id,
      args
  );
  return ast;
}

// braceexpr ::= '{' <expr> '}'
AST *ast_bra_expression(FILE *file) {
  TokenType t = getToken(file); // skip '{'
  if (t == RBra) {
    // empty body
    return NULL;
  }
  
  AST *ast = ast_expression(file, t);
  t = getToken(file);
  if (t != RBra) {
    ast_free(ast);
    fprintf(stderr, "Expected '}' but found: %d\n", t);
    return NULL;
  }

  return ast;
}

// expr 
//   ::= <braceexpr>
//   ::= <identifierexpr>
//   ::= <stringexpr>
AST *ast_expression(FILE *file, TokenType t) {
  switch (t) {
    case Identifier:
      return ast_identifier(file);
    case LBra:
      return ast_bra_expression(file);
    case LPar:
      return ast_par_expression(file);
    case Literal:
      return ast_str_literal();
    default: {
      fprintf(stderr, "Expression not supported yet: %d\n", t);
      return NULL;
    }
  }
}

// funexpr ::= 'fun' <identifierexpr> <expr>
AST *ast_function(FILE *file) {
  TokenType t = getToken(file); // skip 'fun'
  if (t != Identifier) {
    fprintf(stderr, "Expected a function identifier: %d\n", t);
    return NULL;
  }

  AST *id = ast_identifier(file);
  if (id == NULL) {
    fprintf(stderr, "Function identifier not found\n");
    return id;
  }

  t = getToken(file);
  if (t != LBra) {
    ast_free(id);
    fprintf(stderr, "Function has not body\n");
    return NULL;
  }

  AST *body = ast_bra_expression(file);
  if (body == NULL) {
    ast_free(id);
    fprintf(stderr, "Function body not found\n");
    return body;
  }
  
  AST *ast = AST_NEW(
      AST_FUN,
      id,
      body
  );
  return ast;
}

int main(void) {
  FILE *file = fopen("01-helloworld.kt", "r");
  if (file == NULL) {
    fprintf(stderr, "Kotlin file not found");
    exit(1);
  }

  AST *ast = NULL;
  TokenType t = getToken(file);
  if (t == Key_Fun) {
    ast = ast_function(file);
  }
  ast_print(ast);
  ast_free(ast);

  fclose(file);

  return 0;
}

#endif // _PARSER_
