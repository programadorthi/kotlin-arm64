#ifndef _LEXER_
#define _LEXER_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_NAME 100

typedef enum {
  Eof = 0,
  Identifier,
  LBra,
  LPar,
  Literal,
  RBra,
  RPar,
} TokenType;

char name[MAX_NAME];
char text[MAX_NAME];

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
        return Identifier;
      } else {
        fprintf(stderr, "Invalid token: %c", c);
        exit(1);
      }
  }
}

int main(void) {
  FILE *file = fopen("01-helloworld.kt", "r");
  if (file == NULL) {
    fprintf(stderr, "Kotlin file not found");
    exit(1);
  }

  TokenType t = getToken(file);
  while (t != Eof) {
    printf("Token: %d -> %s -> %s\n", t, name, text);
    t = getToken(file);
  }

  fclose(file);

  return 0;
}

#endif // _LEXER_
