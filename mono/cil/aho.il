.assembly extern mscorlib
{
  .ver 1:0:5000:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly 'human'
{
  .hash algorithm 0x00008004
  .ver  0:0:0:0
}
.module human.exe // GUID = {AEC8E53E-C578-4870-8E2F-062017F95594}


  .class public auto ansi beforefieldinit Human
  	extends [mscorlib]System.Object
  {

    // method line 1
    .method public hidebysig  specialname  rtspecialname 
           instance default void .ctor ()  cil managed 
    {
        // Method begins at RVA 0x20ec
	// Code size 17 (0x11)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::.ctor()
	IL_0006:  ldstr "a child without name ..."
	IL_000b:  call void class [mscorlib]System.Console::WriteLine(string)
	IL_0010:  ret 
    } // end of method Human::.ctor

    // method line 2
    .method public hidebysig  specialname  rtspecialname 
           instance default void .ctor (string name)  cil managed 
    {
        // Method begins at RVA 0x2100
	// Code size 23 (0x17)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::.ctor()
	IL_0006:  ldarg.1 
	IL_0007:  ldstr " is born ..."
	IL_000c:  call string string::Concat(string, string)
	IL_0011:  call void class [mscorlib]System.Console::WriteLine(string)
	IL_0016:  ret 
    } // end of method Human::.ctor

  } // end of class Human

  .class private auto ansi beforefieldinit Demo
  	extends [mscorlib]System.Object
  {

    // method line 3
    .method public hidebysig  specialname  rtspecialname 
           instance default void .ctor ()  cil managed 
    {
        // Method begins at RVA 0x2118
	// Code size 7 (0x7)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::.ctor()
	IL_0006:  ret 
    } // end of method Demo::.ctor

    // method line 4
    .method public static  hidebysig 
           default void Main ()  cil managed 
    {
        // Method begins at RVA 0x2120
	.entrypoint
	// Code size 18 (0x12)
	.maxstack 2
	.locals init (
		class Human	V_0,
		class Human	V_1)
	IL_0000:  newobj instance void class Human::.ctor()
	IL_0005:  stloc.0 
	IL_0006:  ldstr "Hugo"
	IL_000b:  newobj instance void class Human::.ctor(string)
	IL_0010:  stloc.1 
	IL_0011:  ret 
    } // end of method Demo::Main

  } // end of class Demo

