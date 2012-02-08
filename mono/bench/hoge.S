Compiled from "Bench.java"
public class Bench extends java.lang.Object{
public Bench();
  Code:
   0:	aload_0
   1:	invokespecial	#1; //Method java/lang/Object."<init>":()V
   4:	return

public static void main(java.lang.String[]);
  Code:
   0:	aload_0
   1:	iconst_0
   2:	aaload
   3:	invokestatic	#2; //Method java/lang/Integer.parseInt:(Ljava/lang/String;)I
   6:	istore_1
   7:	iload_1
   8:	anewarray	#3; //class V
   11:	astore_2
   12:	iconst_0
   13:	istore_3
   14:	iload_3
   15:	iload_1
   16:	if_icmpge	37
   19:	aload_2
   20:	iload_3
   21:	new	#3; //class V
   24:	dup
   25:	iload_3
   26:	iconst_1
   27:	invokespecial	#4; //Method V."<init>":(IZ)V
   30:	aastore
   31:	iinc	3, 1
   34:	goto	14
   37:	iconst_2
   38:	istore_3
   39:	iload_3
   40:	iload_1
   41:	if_icmpge	94
   44:	iconst_2
   45:	istore	4
   47:	iload	4
   49:	iload_1
   50:	if_icmpge	88
   53:	iload_3
   54:	iload	4
   56:	if_icmpne	62
   59:	goto	82
   62:	aload_2
   63:	iload	4
   65:	aaload
   66:	getfield	#5; //Field V.value:I
   69:	iload_3
   70:	irem
   71:	ifne	82
   74:	aload_2
   75:	iload	4
   77:	aaload
   78:	iconst_0
   79:	putfield	#6; //Field V.is_a_prime:Z
   82:	iinc	4, 1
   85:	goto	47
   88:	iinc	3, 1
   91:	goto	39
   94:	iconst_0
   95:	istore	4
   97:	iconst_2
   98:	istore_3
   99:	iload_3
   100:	iload_1
   101:	if_icmpge	152
   104:	aload_2
   105:	iload_3
   106:	aaload
   107:	getfield	#6; //Field V.is_a_prime:Z
   110:	ifeq	146
   113:	iinc	4, 1
   116:	getstatic	#7; //Field java/lang/System.out:Ljava/io/PrintStream;
   119:	new	#8; //class java/lang/StringBuilder
   122:	dup
   123:	invokespecial	#9; //Method java/lang/StringBuilder."<init>":()V
   126:	aload_2
   127:	iload_3
   128:	aaload
   129:	getfield	#5; //Field V.value:I
   132:	invokevirtual	#10; //Method java/lang/StringBuilder.append:(I)Ljava/lang/StringBuilder;
   135:	ldc	#11; //String  
   137:	invokevirtual	#12; //Method java/lang/StringBuilder.append:(Ljava/lang/String;)Ljava/lang/StringBuilder;
   140:	invokevirtual	#13; //Method java/lang/StringBuilder.toString:()Ljava/lang/String;
   143:	invokevirtual	#14; //Method java/io/PrintStream.print:(Ljava/lang/String;)V
   146:	iinc	3, 1
   149:	goto	99
   152:	getstatic	#7; //Field java/lang/System.out:Ljava/io/PrintStream;
   155:	new	#8; //class java/lang/StringBuilder
   158:	dup
   159:	invokespecial	#9; //Method java/lang/StringBuilder."<init>":()V
   162:	ldc	#15; //String done. cnt=
   164:	invokevirtual	#12; //Method java/lang/StringBuilder.append:(Ljava/lang/String;)Ljava/lang/StringBuilder;
   167:	iload	4
   169:	invokestatic	#16; //Method java/lang/Integer.toString:(I)Ljava/lang/String;
   172:	invokevirtual	#12; //Method java/lang/StringBuilder.append:(Ljava/lang/String;)Ljava/lang/StringBuilder;
   175:	invokevirtual	#13; //Method java/lang/StringBuilder.toString:()Ljava/lang/String;
   178:	invokevirtual	#17; //Method java/io/PrintStream.println:(Ljava/lang/String;)V
   181:	return

}

