#include <iostream>
#include "lightning.h"
     
     static jit_insn codeBuffer[1024];
     
     typedef int (*pifi)(int);       /* Pointer to Int Function of Int */
     
     int main()
     {
       pifi      nfibs = (pifi) (jit_set_ip(codeBuffer).iptr);
       int       in;                 /* offset of the argument */
       jit_insn  *ref;               /* to patch the forward reference */
     
             jit_prolog   (1);
       in =  jit_arg_ui   ();
             jit_getarg_ui(JIT_V0, in);              /* V0 = n */
       ref = jit_blti_ui  (jit_forward(), JIT_V0, 2);
             jit_subi_ui  (JIT_V1, JIT_V0, 1);       /* V1 = n-1 */
             jit_subi_ui  (JIT_V2, JIT_V0, 2);       /* V2 = n-2 */
             jit_prepare(1);
               jit_pusharg_ui(JIT_V1);
             jit_finish(nfibs);
             jit_retval(JIT_V1);                     /* V1 = nfibs(n-1) */
             jit_prepare(1);
               jit_pusharg_ui(JIT_V2);
             jit_finish(nfibs);
             jit_retval(JIT_V2);                     /* V2 = nfibs(n-2) */
             jit_addi_ui(JIT_V1,  JIT_V1,  1);
             jit_addr_ui(JIT_RET, JIT_V1, JIT_V2);   /* RET = V1 + V2 + 1 */
             jit_ret();
     
       jit_patch(ref);                               /* patch jump */
             jit_movi_i(JIT_RET, 1);                 /* RET = 1 */
             jit_ret();
     
       /* call the generated code, passing 32 as an argument */
       jit_flush_code(codeBuffer, jit_get_ip().ptr);
       printf("nfibs(%d) = %d\n", 10, nfibs(10));
       printf("nfibs(%d) = %d\n", 32, nfibs(32));
       return 0;
     }