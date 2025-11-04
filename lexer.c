#ifndef _LEXER_
#define _LEXER_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_NAME 100

typedef enum {
  Eof = 0,
  Identifier,
  Key_Fun,
  LBra,
  LPar,
  Literal,
  RBra,
  RPar,
} TokenType;

char name[MAX_NAME];
char text[MAX_NAME];

static bool is_fun(void) {
  return strcmp(name, "fun") == 0;
}

int nextToken(FILE *file) {
  return fgetc(file);
}

TokenType getToken(FILE *file) {
  name[0] = '\0';
  text[0] = '\0';

  int c = nextToken(file);

  while (isspace(c)) {
    c = nextToken(file);
  }

  switch (c) {
    case EOF:
      return Eof;
    case '(':
      return LPar;
    case ')':
      return RPar;
    case '{':
      return LBra;
    case '}':
      return RBra;
    case '"': {
      int i = 0;
      for ( ; (c = nextToken(file)) != '"'; i++) {
        if (c == EOF) {
          return Eof;
        }
        text[i] = c;
      }
      text[i] = '\0';
      return Literal;
    }
    default:
      if (isalpha(c) || c == '_') {
        int i = 0;
        do {
          name[i++] = c;
          c = nextToken(file);
          if (c == EOF) {
            return Eof;
          }
        } while (isalnum(c) || c == '_');
        name[i] = '\0';
        ungetc(c, file);
        if (is_fun()) {
          return Key_Fun;
        }
        return Identifier;
      } else {
        fprintf(stderr, "Invalid token: %c", c);
        exit(1);
      }
  }
}

#endif // _LEXER_
