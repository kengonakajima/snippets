#if 0

#include <stdio.h>
     #include "lightning.h"
     
     static jit_insn codeBuffer[1024];
     
     typedef void (*pvfi)(int);      /* Pointer to Void Function of Int */
     
     int main()
     {
       pvfi          myFunction;             /* ptr to generated code */
       int           in;                     /* to get the argument */
       char          *start, *end;           /* a couple of labels */

     
       myFunction = (pvfi) (jit_set_ip(codeBuffer).vptr);
       start = jit_get_ip().ptr;
       jit_prolog(1);
       in = jit_arg_i();
       jit_movi_p(JIT_R0, "generated %d bytes\n");
       jit_getarg_i(JIT_R1, in);
       jit_prepare(2);
         jit_pusharg_i(JIT_R1);              /* push in reverse order */
         jit_pusharg_p(JIT_R0);
       jit_finish(printf);
       jit_ret();
       end = jit_get_ip().ptr;
     
       /* call the generated code, passing its size as argument */
       jit_flush_code(start, end);
       myFunction(end - start);
     }
#else

#include <stdio.h>
#include "lightning.h"

static jit_insn codeBuffer[1024];

typedef void (*pvfi)(int);

int main()
{
	pvfi myfunc;
	char *start, *end;	
	int in;
	
	myfunc = (pvfi) ( jit_set_ip( codeBuffer).vptr );

	start = jit_get_ip().ptr;
	jit_prolog(1);
	in = jit_arg_i();
	jit_movi_p( JIT_R0, "generated %d bytes\n");
	jit_getarg_i(JIT_R1,in);
	jit_prepare(2);
		jit_pusharg_i(JIT_R1);
		jit_pusharg_p(JIT_R0);
	jit_finish(printf);
	jit_ret();
	end = jit_get_ip().ptr;
	
	jit_flush_code(start,end);
	
	myfunc(end - start );
	return 0;
}

#endif
