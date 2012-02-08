.assembly extern mscorlib
{
  .ver 1:0:5000:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly 'Aho'
{
  .hash algorithm 0x00008004
  .ver  0:0:0:0
}
.module Aho.exe // GUID = {A311EB83-5376-49CD-A504-F0474F6116C8}


  .class private auto ansi beforefieldinit Hoge
  	extends [mscorlib]System.Object
  {
    .field  private  int32 a

    // method line 1
    .method public hidebysig  specialname  rtspecialname 
           instance default void .ctor (int32 aa)  cil managed 
    {
        // Method begins at RVA 0x20ec
	// Code size 14 (0xe)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::.ctor()
	IL_0006:  ldarg.0 
	IL_0007:  ldarg.1 
	IL_0008:  stfld int32 Hoge::a
	IL_000d:  ret 
    } // end of method Hoge::.ctor

  } // end of class Hoge

  .class public auto ansi beforefieldinit Aho
  	extends [mscorlib]System.Object
  {

    // method line 2
    .method public hidebysig  specialname  rtspecialname 
           instance default void .ctor ()  cil managed 
    {
        // Method begins at RVA 0x20fc
	// Code size 7 (0x7)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::.ctor()
	IL_0006:  ret 
    } // end of method Aho::.ctor

    // method line 3
    .method public static  hidebysig 
           default void Main (string[] args)  cil managed 
    {
        // Method begins at RVA 0x2104
	.entrypoint
	// Code size 9 (0x9)
	.maxstack 2
	.locals init (
		class Hoge	V_0)
	IL_0000:  ldc.i4.s 0x0a
	IL_0002:  newobj instance void class Hoge::.ctor(int32)
	IL_0007:  stloc.0 
	IL_0008:  ret 
    } // end of method Aho::Main

  } // end of class Aho

