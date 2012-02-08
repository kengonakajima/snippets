.assembly extern mscorlib
{
  .ver 1:0:5000:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly 'test'
{
  .hash algorithm 0x00008004
  .ver  0:0:0:0
}
.module test.exe // GUID = {9BA40CF7-9E9B-48B0-8E3A-BC1061A542D1}


  .class public auto ansi beforefieldinit Work
  	extends [mscorlib]System.Object
  {

    // method line 1
    .method public hidebysig  specialname  rtspecialname 
           instance default void .ctor ()  cil managed 
    {
        // Method begins at RVA 0x20ec
	// Code size 7 (0x7)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::.ctor()
	IL_0006:  ret 
    } // end of method Work::.ctor

    // method line 2
    .method public static  hidebysig 
           default void Do ()  cil managed 
    {
        // Method begins at RVA 0x20f4
	// Code size 40 (0x28)
	.maxstack 5
	.locals init (
		int32	V_0)
	IL_0000:  ldc.i4.0 
	IL_0001:  stloc.0 
	IL_0002:  br IL_001f

	IL_0007:  ldc.i4 1000
	IL_000c:  call void class [mscorlib]System.Threading.Thread::Sleep(int32)
	IL_0011:  ldstr "child\n"
	IL_0016:  call void class [mscorlib]System.Console::Write(string)
	IL_001b:  ldloc.0 
	IL_001c:  ldc.i4.1 
	IL_001d:  add 
	IL_001e:  stloc.0 
	IL_001f:  ldloc.0 
	IL_0020:  ldc.i4.s 0x0a
	IL_0022:  blt IL_0007

	IL_0027:  ret 
    } // end of method Work::Do

  } // end of class Work

  .class private auto ansi beforefieldinit MonoEmbed
  	extends [mscorlib]System.Object
  {

    // method line 3
    .method public hidebysig  specialname  rtspecialname 
           instance default void .ctor ()  cil managed 
    {
        // Method begins at RVA 0x2128
	// Code size 7 (0x7)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::.ctor()
	IL_0006:  ret 
    } // end of method MonoEmbed::.ctor

    // method line 4
    .method private static  hidebysig 
           default string gimme ()  cil managed internalcall 
    {
        // Method begins at RVA 0x0
    } // end of method MonoEmbed::gimme

    // method line 5
    .method private static  hidebysig 
           default void sayhello (string s)  cil managed internalcall 
    {
        // Method begins at RVA 0x0
    } // end of method MonoEmbed::sayhello

    // method line 6
    .method private static  hidebysig 
           default void Main ()  cil managed 
    {
        // Method begins at RVA 0x2130
	.entrypoint
	// Code size 47 (0x2f)
	.maxstack 6
	.locals init (
		string	V_0,
		class [mscorlib]System.Threading.Thread	V_1)
	IL_0000:  call string class MonoEmbed::gimme()
	IL_0005:  stloc.0 
	IL_0006:  ldloc.0 
	IL_0007:  call void class MonoEmbed::sayhello(string)
	IL_000c:  ldnull 
	IL_000d:  ldftn void class Work::Do()
	IL_0013:  newobj instance void class [mscorlib]System.Threading.ThreadStart::.ctor(object, native int)
	IL_0018:  newobj instance void class [mscorlib]System.Threading.Thread::.ctor(class [mscorlib]System.Threading.ThreadStart)
	IL_001d:  stloc.1 
	IL_001e:  ldloc.1 
	IL_001f:  callvirt instance void class [mscorlib]System.Threading.Thread::Start()
	IL_0024:  ldstr "parent\n"
	IL_0029:  call void class [mscorlib]System.Console::Write(string)
	IL_002e:  ret 
    } // end of method MonoEmbed::Main

  } // end of class MonoEmbed

