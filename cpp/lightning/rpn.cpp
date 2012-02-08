#include <iostream>
#include "lightning.h"

using namespace std;

typedef int (*pifi)(int);       /* Pointer to Int Function of Int */

   
   pifi compile_rpn(char *expr)
   {
     pifi fn;
     int in;
     fn = (pifi) (jit_get_ip().iptr);
     jit_leaf(1);
     in = jit_arg_i();
     jit_getarg_i(JIT_R0, in);
   
     while (*expr) {
       char buf[32];
       int n;
       if (sscanf(expr, "%[0-9]%n", buf, &n)) {
         expr += n - 1;
         jit_pushr_i(JIT_R0);
         jit_movi_i(JIT_R0, atoi(buf));
       } else if (*expr == '+') {
         jit_popr_i(JIT_R1);
         jit_addr_i(JIT_R0, JIT_R1, JIT_R0);
       } else if (*expr == '-') {
         jit_popr_i(JIT_R1);
         jit_subr_i(JIT_R0, JIT_R1, JIT_R0);
       } else if (*expr == '*') {
         jit_popr_i(JIT_R1);
         jit_mulr_i(JIT_R0, JIT_R1, JIT_R0);
       } else if (*expr == '/') {
         jit_popr_i(JIT_R1);
         jit_divr_i(JIT_R0, JIT_R1, JIT_R0);
       } else {
         fprintf(stderr, "cannot compile: %s\n", expr);
         abort();
       }
       ++expr;
     }
     jit_movr_i(JIT_RET, JIT_R0);
     jit_ret();
     return fn;
   }

static jit_insn codeBuffer[1024];

     int main()
     {
       pifi c2f, f2c;
       int i;

       jit_set_ip(codeBuffer);
       c2f = compile_rpn("9*5/32+");
       f2c = compile_rpn("32-5*9/");
       jit_flush_code(codeBuffer, jit_get_ip().ptr);

       printf("\nC:");
       for (i = 0; i <= 100; i += 10) printf("%3d ", i);
       printf("\nF:");
       for (i = 0; i <= 100; i += 10) printf("%3d ", c2f(i));
       printf("\n");

       printf("\nF:");
       for (i = 32; i <= 212; i += 10) printf("%3d ", i);
       printf("\nC:");
       for (i = 32; i <= 212; i += 10) printf("%3d ", f2c(i));
       printf("\n");
       return 0;
     }

