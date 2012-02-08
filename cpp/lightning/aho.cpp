#include <iostream>
#include "lightning.h"
using namespace std;


static jit_insn codeBuffer[1024];

typedef int (*pifi)(int);



int main()
{
	
	pifi incr  = (pifi) ( jit_set_ip( codeBuffer).iptr );
	int in;
	
	jit_leaf(1);
	in = jit_arg_i();
	jit_getarg_i(JIT_R0,in);
	jit_addi_i(JIT_RET,JIT_R0,1);
	jit_addi_i(JIT_RET,JIT_R0,1);	
	jit_ret();
	
	jit_flush_code(codeBuffer, jit_get_ip().ptr );
	
	cout << "aho:" << incr(5) << endl;
	
	return 0;
}