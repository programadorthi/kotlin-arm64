#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#include "parser.c"

void sys_call(FILE *out) {
  fprintf(out, "    svc 0\n");
  fprintf(out, "\n");
}

void exit_program(FILE *out, int code) {
  fprintf(out, "_exit:\n");
  fprintf(out, "    mov X0, #%d\n", code);
  // TODO: X16 is MacOS M* register only
  // #1 is syscall number for exit
  fprintf(out, "    mov X16, #1\n");
  sys_call(out);
}

void println(FILE *out, char *label) {
  // #1 file descriptor 1 (stdout)
  fprintf(out, "    mov X0, #1\n");
  // adr is MacOS M* instruction only
  fprintf(out, "    adr X1, %s\n", label);
  fprintf(out, "    mov X2, #%s_length\n", label);
  // TODO: X16 is MacOS M* register only
  // #4 syscall number for write
  fprintf(out, "    mov X16, #4\n");
  sys_call(out);
}

void label(FILE *out, char *l, char *ascii_value) {
  fprintf(out, "%s: .ascii \"%s\\n\"\n", l, ascii_value);
  fprintf(out, "%s_length = . - %s\n", l, l);
}

void ast_to_assembly(AST *ptr, FILE *out) {
  if (!ptr) return;

  AST ast = *ptr;
  switch (ast.tag) {
    case AST_ARGS: {
      struct AST_ARGS data = ast.data.AST_ARGS;
      ast_to_assembly(data.arg1, out);
      break;
    }
    case AST_CALL: {
      struct AST_CALL data = ast.data.AST_CALL;
      ast_to_assembly(data.id, out);
      ast_to_assembly(data.args, out);
      break;
    }
    case AST_FUN: {
      struct AST_FUN data = ast.data.AST_FUN;
      ast_to_assembly(data.id, out);
      ast_to_assembly(data.body, out);
      break;
    }
    case AST_ID: {
      struct AST_ID data = ast.data.AST_ID;
      if (strcmp(data.name, "main") == 0) {
        fprintf(out, ".global _%s\n", data.name);
        fprintf(out, ".align 2\n");
        fprintf(out, "\n");
        fprintf(out, "_%s:\n", data.name);
      }
      break;
    }
    case AST_STR_LITERAL: {
      struct AST_STR_LITERAL data = ast.data.AST_STR_LITERAL;
      println(out, "lab1");
      exit_program(out, 0);
      label(out, "lab1", data.value);
      break;
    }
    default: {
      fprintf(stderr, "AST not supported yet: %d", ast.tag);
      break;
    }
  }
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

  FILE *out = fopen("helloworld.s", "w+");
  if (out != NULL) {
    ast_to_assembly(ast, out);
    fclose(out);
  }
  ast_free(ast);

  fclose(file);

  return 0;
}

#endif //_ASSEMBLY_H_
