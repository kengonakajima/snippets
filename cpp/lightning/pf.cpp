#include <iostream>
#include "lightning.h"
using namespace std;


static jit_insn codeBuffer[1024];

typedef void (*pvfi)(int);



int main()
{
	
	pvfi myf  = (pvfi) ( jit_set_ip( codeBuffer).vptr );
	int in;
	char *start, *end;
	
	
	jit_leaf(1);
	start = jit_get_ip().ptr;
	jit_prolog(1);
	in = jit_arg_i();
	jit_movi_p( JIT_R0, "asdfasd %d\n");
	jit_getarg_i(JIT_R1,in);
	jit_prepare(2);
		jit_pusharg_i(JIT_R1);
		jit_pusharg_i(JIT_R0);
	jit_finish(printf);
	jit_ret();
	end = jit_get_ip().ptr;
	
	jit_flush_code(start,end);
	
	myf(end - start );
	
	return 0;
}