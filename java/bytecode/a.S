Compiled from "Aho.java"
public class Aho extends java.lang.Object{
public Aho();
  Code:
   0:	aload_0
   1:	invokespecial	#1; //Method java/lang/Object."<init>":()V
   4:	return

public static void main(java.lang.String[]);
  Code:
   0:	new	#2; //class Hoge
   3:	dup
   4:	bipush	10
   6:	invokespecial	#3; //Method Hoge."<init>":(I)V
   9:	astore_1
   10:	return

}

Compiled from "Aho.java"
class Hoge extends java.lang.Object{
int a;

Hoge(int);
  Code:
   0:	aload_0
   1:	invokespecial	#1; //Method java/lang/Object."<init>":()V
   4:	aload_0
   5:	iload_1
   6:	putfield	#2; //Field a:I
   9:	return

}

