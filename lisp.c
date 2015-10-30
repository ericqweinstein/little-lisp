// A simple little LISP in C.
// Author: Eric Weinstein <eric.q.weinstein@gmail.com>

#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include "mpc.h"

// Check the op string to determine which
// arithmetic operation we should perform.
long eval_op(long x, char* op, long y) {
  // This trades on the fact that operators
  // are always single-character strings.
  // Dangerous? ALMOST CERTAINLY
  int o = op[0];

  switch(o)
  {
    case '+':
      return x + y;
    case '-':
      return x - y;
    case '*':
      return x * y;
    case '/':
      return x / y;
    default:
      return 0;
  }
}

// Recursively evaluate our AST.
long eval(mpc_ast_t* t) {
  // If tagged as a number, return it!
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  // The operator is always the second child.
  char* op = t->children[1]->contents;

  // Store the third child.
  long result = eval(t->children[2]);

  // Iterate over the remaining children and combine.
  int i = 3;

  while (strstr(t->children[i]->tag, "expr")) {
    result = eval_op(result, op, eval(t->children[i]));
    i++;
  }

  return result;
}

int main(int argc, char** argv)
{
  // Create some parsers.
  // See: https://github.com/orangeduck/mpc
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  // Define them!
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);

  puts("LISP version 0.0.0.0.1");
  puts("Press Ctrl+c to exit\n");

  while(1)
  {

    char* input = readline("LISP > ");
    add_history(input);

    // Attempt to parse user input.
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      // ...and on failure, print the error.
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  // Clean up after ourselves.
  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
