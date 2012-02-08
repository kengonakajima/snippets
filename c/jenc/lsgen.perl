#!/usr/local/bin/perl
# 
#
#  C language RPC implemented by PERL language
#
#  
# ringo@titan.co.jp 1998 June
#
#
# sample HTML :
#
#<html>
#<body> 
#<ul>
#  <li>servertoclient aaafunc( string);  サーバーが送信してクライアントが受信
#  <LI>clienttoserver bbbfunc( int );    クライアントが送信してサーバーが受信
#</ul>
#</body>
#</html>
#
#
#
#


use strict subs;
use File::Basename;
require "ctime.pl";


$codeversion = "0.41 ( 1998 May)";


#
# To make version information clear in output files
#
$_ = ctime( time());chomp;
$runtime = $_;
$runuser = `whoami`;
$runhost = `hostname`;


#
# Nonblocking output
#
$| = 1;


@kinds =( 'clienttoserver' , 'servertoclient' );

$verbose = 0;
$error_count = 0;

#
# Contents of each files
#
$sv_c = "";
$sv_h = "";
$cl_c = "";
$cl_h = "";
$help_txt = "";

#
# Finally replace initialized string with following strings
#
$sv_c_sddefs = "";
$sv_c_ssdefs = "";   # LSRPCSADEFS
$sv_h_decls = "";
$cl_c_csdefs = "";
$cl_c_cddefs = "";   # LSRPCCLIDISPATCHDEFS
$cl_h_decls = "";
$cl_func_tobe_def = "";  # FOR HELP FILE
$sv_func_tobe_def = "";# FOR HELP FILE
$cl_func_def = "";# FOR HELP FILE
$sv_func_def = "";# FOR HELP FILE
#
# Max number of arguments of all functions defined in protocol text file
#
$args_max = 1;       

#
# Define types
#
@basictypes = (
			  "int" ,"u_int" , 
			  "long","u_long",
			  "short" , "u_short" , 
			  "char" , "u_char", 
			  "float", "double", 
			  "string" , "void"
			  );
%typetranslate = (
			  "int" => "int" ,
			  "u_int" => "unsigned int" ,
			  "long" => "long" ,
			  "u_long" => "unsigned long" ,
			  "short" => "short" , 
			  "u_short" => "unsigned short" , 
			  "char" => "char" ,
			  "u_char" => "unsigned char" ,
			  "float" => "float",
			  "double" => "double" , 
			  "string" => "char*" , 
			  "void" => "void"
			  );


$workbufsize = "LSRPCSTRUCTNAME.workbufsize/2 -1";


#
# Confirm args to this program
#

#
# get options and splice
#

$out_cpp = 'false';

@ARGV_NEW = ();

for( $i=0;$i< @ARGV;$i++)
{
	$element = $ARGV[$i];
	if( $element eq '-C' ){
		$out_cpp = 'true';
	} elsif( $element eq '-v' ){
		$verbose = 1;
	} else {
		push( @ARGV_NEW , $element );
	}
}



if( @ARGV_NEW == 0 ){
	die"Usage: $0 infile [outdir] [-C|v]\n-C: write cpp files also\n-v:verbose mode\n";
} elsif( @ARGV_NEW == 1 ){
	$infile = $ARGV_NEW[0];
	$output_dir = 'output';
} elsif( @ARGV_NEW == 2 ){
	$infile = $ARGV_NEW[0];
	$output_dir = $ARGV_NEW[1];
}


#
# Parse infile
#
$infile = $ARGV_NEW[0];
$infilebody = $infile;
$infilebody =~ s/\.html//;
$infilebody =~ s/\.htm//;
$infilebody =~ s/\.txt//;
$infilebody = basename( $infilebody , '' );



#
# Setup output files
#
$sv_c_file = "$output_dir/${infilebody}_serv.c";   # server side stub C code
$cl_c_file = "$output_dir/${infilebody}_cli.c";    # client side stub C code
$sv_h_file = "$output_dir/${infilebody}_serv.h";   # server side stub C header
$cl_h_file = "$output_dir/${infilebody}_cli.h";    # client side stub C header
$lsrpc_c_file = "$output_dir/${infilebody}_util.c";             # both side utility C code
$lsrpc_h_file = "$output_dir/${infilebody}_util.h";             # both side utility C header
$sv_cpp_file = "$output_dir/${infilebody}_serv.cpp";   # server side stub C++ code
$cl_cpp_file = "$output_dir/${infilebody}_cli.cpp";    # client side stub C++ code
$lsrpc_cpp_file = "$output_dir/${infilebody}_util.cpp";             # both side utility C++ code
$help_txt_file = "$output_dir/${infilebody}_help.txt";   # help file for coders

#
# Append change date information to output files
#

#open( TMPIN , ">>$infile" );
#print TMPIN  "<!-- $runtime -->\n";
#close( TMPIN );



#
# Open and start parsing
#

open(IN, $infile );


$in_list = 0;

$sv_c = outputCommonHeader( $sv_c_file );
$sv_h = outputCommonHeader( $sv_h_file );
$cl_c = outputCommonHeader( $cl_c_file );
$cl_h = outputCommonHeader( $cl_h_file );

outputHelpTXTInit();
outputServerCInit();
outputServerHInit();
outputClientCInit();
outputClientHInit();

cleanFile();



eval {
	while($line = <IN>){

		if( (($line =~ /<ul>/i) || ($line =~ /<ol>/i))){
			if( $in_list == 0  ){
				$in_list ++;
			} else {
				$in_list ++;
			}
		}
		if( (($line =~ /<\/ul>/i) || ($line =~ /<\/ol>/i))){	
			$in_list --;
			if( $in_list == 0 ){
				finishFuncComment();
				die "parsing finished in line $.\n";
			}
		}

		if( $in_list == 1 ){

			if( ($line =~ /<li>/i ) ){

# Found a line which must be a declaration of a function
				if( $line_parsed == 1 ){
					finishFuncComment();
				}
				$func_comment = "/*\n\n";

				@tk = tokenize_c($line );
				@tk = stylize( @tk );
				parseThis( @tk );     # Do everything in this function
				$line_parsed = 1;
		   } 
		}
		if( $in_list >= 1 ){
# Found a line in the list of functions, but this line is not for decl
		   if( $line_parsed == 1 ){
			   $func_comment .= "$. : ";
			   $func_comment .= $line;
		   }
	   } 
	}							
};  

if( $error_count == 0 ){

	initUtilFilesContents();

#
#  ファイルに書きこむ前に、置きかえをたくさんする必要があるのだ。
#

	$lsrpc_c_src =~ s/_LSRPCUTIL_/_\U${infilebody}UTIL\E_/g;    # マクロの置きかえ
    $lsrpc_c_src =~ s/LSRPCSTRUCTNAME/${infilebody}/g;			# 構造体の名前の置きかえ
    $lsrpc_c_src =~ s/LSRPCUTILINCLUDEFILE/${infilebody}_util.h/g; # include ファイル名の置きかえ
    $lsrpc_c_src =~ s/PROTOCOLNAME/${infilebody}_/g;   # 関数名等修飾
    writeFile( $lsrpc_c_file , $lsrpc_c_src ); # utility C file

    $lsrpc_h_src =~ s/_LSRPCUTIL_/_\U${infilebody}UTIL\E_/g;    # マクロの置きかえ
    $lsrpc_h_src =~ s/LSRPCSTRUCTNAME/${infilebody}/g;		    # 構造体の名前の置きかえ
    $lsrpc_h_src =~ s/PROTOCOLNAME/${infilebody}_/g;   # 関数名等修飾
	writeFile( $lsrpc_h_file , $lsrpc_h_src );  # utility H file
	
	$sv_c =~ s/LSRPCSERVERDISPATCHDEFS/$sv_c_sddefs/;        # clientcall関数定義の結果を置きかえ
	$sv_c =~ s/LSRPCSERVERSENDDEFS/$sv_c_ssdefs/;    # servercallservactive関数の結果を置きかえ
	$sv_c =~ s/_LSRPCSERV_/_\U${infilebody}SERV\E_/g;	# マクロの置きかえ
    $sv_c =~ s/LSRPCSTRUCTNAME/${infilebody}/g;         # 構造体名の置きかえ
    $sv_c =~ s/PROTOCOLNAME/${infilebody}_/g;   # 関数名等修飾
	writeFile( $sv_c_file , $sv_c );

	$sv_h =~ s/LSRPCDECLS/$sv_h_decls/;       # プロトタイプ宣言を置きかえ
	$sv_h =~ s/LSRPCARGSCOUNT/\( $args_max + 1 \) /g;    # 引数の最大値を置きかえ
    $sv_h =~ s/_LSRPCSERV_/_\U${infilebody}SERV\E_/g;					# マクロを置きかえ		
    $sv_h =~ s/LSRPCUTILINCLUDEFILE/${infilebody}_util.h/g;             # インクルードファイル名おきかえ
    $sv_h =~ s/PROTOCOLNAME/${infilebody}_/g;   # 関数名等修飾
	writeFile( $sv_h_file , $sv_h );

	$cl_c =~ s/LSRPCCLIENTSENDDEFS/$cl_c_csdefs/;               # 関数定義を置きかえ
	$cl_c =~ s/LSRPCCLIENTDISPATCHDEFS/$cl_c_cddefs/;  # Dispatch関数の中を置きかえ
    $cl_c =~ s/_LSRPCCLI_/_\U${infilebody}CLI\E_/g; # マクロを置きかえ	
    $cl_c =~ s/LSRPCSTRUCTNAME/${infilebody}/g;     # 構造体名を置きかえ
    $cl_c =~ s/PROTOCOLNAME/${infilebody}_/g;   # 関数名等修飾
	writeFile( $cl_c_file , $cl_c );

	$cl_h =~ s/LSRPCDECLS/$cl_h_decls/;                     # プロトタイプを置きかえ
	$cl_h =~ s/LSRPCARGSCOUNT/\( $args_max + 1 \)/g;                  # 引数の最大数を置きかえ
    $cl_h =~ s/_LSRPCCLI_/_\U${infilebody}CLI\E_/g;	        # マクロを置きかえ
    $cl_h =~ s/LSRPCUTILINCLUDEFILE/${infilebody}_util.h/g; # インクルードファイルを置きかえ
    $cl_h =~ s/PROTOCOLNAME/${infilebody}_/g;   # 関数名等修飾
	writeFile( $cl_h_file , $cl_h );

    $help_txt =~ s/PROTOCOLNAME/${infilebody}_/g;
    $help_txt =~ s/CLIENTFUNCTIONSTOBEDEFINED/$cl_func_tobe_def/;
    $help_txt =~ s/SERVERFUNCTIONSTOBEDEFINED/$sv_func_tobe_def/;
    $help_txt =~ s/CLIENTFUNCTIONSDEFINED/$cl_func_def/;
    $help_txt =~ s/SERVERFUNCTIONSDEFINED/$sv_func_def/;
    writeFile( $help_txt_file , $help_txt );

}


if( $out_cpp eq 'true' ){
								# CPPファイルを出力
	system( "cp $cl_c_file $cl_cpp_file" );
	system( "cp $sv_c_file $sv_cpp_file" );
	system( "cp $lsrpc_c_file $lsrpc_cpp_file" );
}
	

#
# Finish function comments
#
sub finishFuncComment()
{


	$func_comment .= "\n*/\n\n";
	$tobereplaced = "LSRPCCOMMENT_${func_name}";
	$sv_c_ssdefs =~ s/$tobereplaced/$func_comment/;
	$sv_c_sddefs =~ s/$tobereplaced/$func_comment/;
	$cl_c_csdefs =~ s/$tobereplaced/$func_comment/;
	$cl_c_cddefs =~ s/$tobereplaced/$func_comment/;
}



#
# Output File
# $ : filename 
# $ : contents
sub writeFile($$)
{
	my($filename) = shift;
	my($contents) = shift;
	
	$contents =~ s/\n[ \t]*\n/\n/g;

	open( OUT , ">>$filename" );
	print OUT $contents;
	close( OUT );
}

#
#
#
#
sub parseThis(@)
{

	initParse();
	@tl = @_;

	eval {

		$func_kind = getFunctionKind();
		if( ! $func_kind  ){
			my($l) = join( '/' , @kinds);
			printerr( "bad function kind. need $l first in a line.\n" );
			die ;
		}
		
		eval{getFuncDecl();};
		if($@){printerr("bad function decl");}
		
		eval{getArgs();};
		if($@){printerr("bad args");}



		if( $func_kind eq "servertoclient" ){
			addSenderC( "server" );
			addSenderH( "server" );

			addReceiverC( "client" );
			addReceiverH( "client" );
	
		} elsif( $func_kind eq "clienttoserver"){
			addSenderC( "client");
			addSenderH( "client");
			addReceiverC( "server" );
			addReceiverH( "server" );
		} else {
		}
	};

	if( $@){
		die "compilation failed : ( $error_count error(s)occurred)\n";
	}


}




#
# Create a string for mkstr_??? function
#
# $ : type of the variable : int , short , ...
# $ : element count of the variable
# $ : name of the variable
#

sub createMkstrString($$$)
{
	my($t) = shift;
	my($siz) = shift;
	my($name) = shift;
	
	if( $t eq "void" ){		return "\"\"";	}

	if( $siz > 0){
		my( $ta) = $t;
		$ta .= "_array";
		return "PROTOCOLNAMEmkstr_$ta( $siz , $name )";
	} else {
		return "PROTOCOLNAMEmkstr_$t( $name )";
	}
}

#
# Create a string for demkstr_??? function
#
# $ : type of the variable
# $ : name of the variabl( ex. arg2)
# $ : start point
# $ : size 
# $ : specify what number the argument has ( arg0 ~ )
#
sub createDemkstrString($$$$$)
{
	my($t ) = shift;
	my($name)= shift;
	my($st) = shift;
	my($siz) = shift;
	my($anum)= shift;

	if( $siz == 0 ){
 
		# string type is a special case ( need wrapper )
		if( $t eq "string" ){
			return "$name = PROTOCOLNAMEwrapStringAddr( PROTOCOLNAMEstringwrapper[$anum] , PROTOCOLNAMEdemkstr_$t( LSRPCSTRUCTNAME.token_list[$st] ))";
		} else {
			return "$name = PROTOCOLNAMEdemkstr_$t( LSRPCSTRUCTNAME.token_list[$st] )";
		}
	} else {
		my($i) = "$t";
		$i .= "_array";
		return "PROTOCOLNAMEdemkstr_$i( LSRPCSTRUCTNAME.token_list , $name , $st , $siz )";
	}
}


#
# 送信側の関数を定義する。引数は "server"または"client"
#
sub addSenderC($)
{
	$side = shift;

# サーバーサイドかクライントサイドかに関係なく送信側の関数は定義される
	$src = "";
	$src .= "LSRPCCOMMENT\n";
	$src .= "LSRPCPROTOTYPE\n";
	$src .= "{\n";
	$src .= "\tPROTOCOLNAMECreateHeader( LSRPCSTRUCTNAME.work , \"LSRPCFUNCNAME\" );\n";
	$src .= "LSRPCARGTRANSLATION\n";
	$src .= "\tPROTOCOLNAMESend( fd , LSRPCSTRUCTNAME.work );\n";
	$src .= "}\n";

	$src =~ s/LSRPCFUNCNAME/$func_name/;                # 関数名のところも共通
	$src =~ s/LSRPCCOMMENT/LSRPCCOMMENT_$func_name/;    # コメントは共通

	my( @prototypes ) = createPrototypeCS();
	$src =~ s/LSRPCPROTOTYPE/$prototypes[0]/;           # プロトタイプも同じ

# 引数の変換も同じ
	my($lsrpcargtranslation) = "";
    for($i = 0 ; $i < @arg_type ; $i++){
		$a = createMkstrString( $arg_type[$i] , $arg_size[$i] , $arg_name[$i] );
   	    $lsrpcargtranslation .= 
			"\tstrncat( LSRPCSTRUCTNAME.work , $a ,LSRPCSTRUCTNAME.workbufsize );\n";
    }		
    $src =~ s/LSRPCARGTRANSLATION/$lsrpcargtranslation/;

	if( $side eq "server" ){
		$sv_c_ssdefs .= $src;      # サーバーサイドだったら送信関数をサーバーにくわえる
	} elsif( $side eq "client" ){
		$cl_c_csdefs .= $src;      #クライアントサイドだったら送信関数をクライアントににくわえる
	} else {
		die "bad side";
	}
}

#
# 受信側の関数を定義する。引数は "server"または "client"
# Dispatchの中に入れる用である。
sub addReceiverC($)
{

	$side = shift;
	$src  = "";
	$src .= "LSRPCCOMMENT\n";
	$src .= "\tif( strcmp( funcname , \"LSRPCFUNCNAME\" ) == 0 ){\n";
	$src .= "LSRPCARGDECL\n";
	$src .= "LSRPCARGTRANSLATION\n";
	$src .= "\t\t${infilebody}_LSRPCFUNCNAME_recv( LSRPCARGLIST);\n";
	$src .= "\t}\n";


	$src =~ s/LSRPCFUNCNAME/$func_name/g; # 関数名は共通
	$src =~ s/LSRPCCOMMENT/LSRPCCOMMENT_${func_name}/;   # コメントは共通

# 引数列を宣言する
	my( $lsrpcargdecl ) = "";
	for($i = 0 ; $i < @arg_type ; $i++ ){
		if( $arg_type[$i] eq "void" ){ last;}
	
		$a = "\t\t";
		if( $arg_size[$i] > 0 ){
			$a .= "$typetranslate{$arg_type[$i]} $arg_name[$i] [$arg_size[$i]];\n";
		} else {
			$a .= "$typetranslate{$arg_type[$i]} $arg_name[$i];\n";
		}
		$lsrpcargdecl .= $a;
	}
	$src =~ s/LSRPCARGDECL/$lsrpcargdecl/;
	
# 文字列できた引数を元の型に変換する部分
	my($count) = 2;
	my($lsrpcargtranslation) = "";
	for($i = 0 ; $i < @arg_type ; $i++){
		last if( $arg_type[$i] eq "void" );

		$a = "\t\t";
		$a .= createDemkstrString( $arg_type[$i] , $arg_name[$i] ,
								  $count , $arg_size[$i] , $i + 1);
		$a .= ";\n";
		$count += $arg_size[$i];
		if( $arg_size[$i] == 0 ){ $count++;}    # 引数一個でもインクリメント
		$lsrpcargtranslation .= $a;

	}
	$src =~ s/LSRPCARGTRANSLATION/$lsrpcargtranslation/;
		
# 引数列
    if( $arg_type[0] eq "void" ){
		$src =~ s/LSRPCARGLIST/fd/;
	} else {
		$a = join( ',' , @arg_name );
		$src =~ s/LSRPCARGLIST/fd,$a/;
	}


	if( $side eq "server" ){
		$sv_c_sddefs .= $src;  # サーバーサイドに追加
	} elsif( $side eq "client" ){
		$cl_c_cddefs .= $src;  #クライアントサイドに追加
	} else {
		die "bad side";
	}
}

#
#  受信側のヘッダーに追加する
# $ : "server" or "client"
sub addSenderH($)
{
	$side = shift;

	($s,$r) = createPrototypeCS();
	$a = "; /* $infile line $. */\n";


	if( $side eq "server" ){
		$sv_h_decls .= "$s $a";
		$sv_func_def .= "$s;\n";
	}elsif( $side eq "client" ){
		$cl_h_decls .= "$s $a";
		$cl_func_def .= "$s;\n";
	} else {
		die "bad side" ;
	}

}

sub addReceiverH($)
{
	$side = shift;
	($s,$r) = createPrototypeCS();
	$a = "; /* $infile line $. */\n";
	
	if( $side eq "server" ){
		$sv_h_decls .= "$r $a";
		$sv_func_tobe_def .= "$r;\n";
	}elsif( $side eq "client" ){
		$cl_h_decls .= "$r $a";
		$cl_func_tobe_def .= "$r;\n";
	} else {
		die "bad side" ;
	}	
}


#
# make prototype from default variables for both side
#
sub createPrototypeCS()
{
	my( @ret_array ) = ();
	my( $pointer );
	my( $args);

	my( @args_array ) = ();
	my( $count ) = 0;

# $args_maxを求めておく
	if( @arg_type > $args_max ){ $args_max = @arg_type; }
		
# 引数がvoidだったらvoidとだけ出力するのだ

	if( $arg_type[0] eq "void" ){
	   	push( @ret_array, "void ${infilebody}_${func_name}_send( int fd )" );
		push( @ret_array, "void ${infilebody}_${func_name}_recv( int fd )" ); 
    	return @ret_array;
	}

	for($i = 0 ; $i < @arg_type ; $i++){
		my($atype) = $typetranslate{$arg_type[$i]};
		my($array);
		if( $arg_size[$i] > 0 ){
			$array = "[$arg_size[$i]]";
		} else {
			$array = "";
		}
		$newname = $arg_name[$i];
		$count++;
		if( $atype eq "void" ){
			push( @args_array , "void" );
		} else {
			push( @args_array ,  "$atype $newname$array" );
		}
	}
	


	$args = join( ',' , @args_array );
	push( @ret_array, "void ${infilebody}_${func_name}_send( int fd,$args )" );
	push( @ret_array, "void ${infilebody}_${func_name}_recv( int fd,$args )" ); 

	return @ret_array;
}



#
# init
#
sub outputHelpTXTInit()
{
	$help_txt .= "LSRPC AUTOMATIC HELP FILE for protocol [${infilebody}]\n\n\n";
	$help_txt .= "Client note:\n\n";
	$help_txt .= "step 1: Include ${infilebody}_cli.h in all source files in which you use RPC functions\n";
	$help_txt .= "step 2: Compile and add ${infilebody}_cli.c into your project\n";
	$help_txt .= "step 3: Compile and add ${infilebody}_util.c into your project\n";
	$help_txt .= "step 4: Write following functions somewhere:\n\n";
	$help_txt .= "CLIENTFUNCTIONSTOBEDEFINED\n\n\n";
	$help_txt .= "step 5: Use PROTOCOLNAMEInitClient( writefunc , worksize );\n";
	$help_txt .= "step 6: Call functions like following:\n\n\n";
	$help_txt .= "CLIENTFUNCTIONSDEFINED\n";
	$help_txt .= "step 7: If you want to cleanup lsrpc, use PROTOCOLNAMECleanupServer();\n";
	$help_txt .= "\n\n";

	$help_txt .= "Server note:\n\n";
	$help_txt .= "step 1: You include ${infilebody}_serv.h in all source files in which you use RPC functions\n";
	$help_txt .= "step 2: Compile and add ${infilebody}_serv.c into your project\n";
	$help_txt .= "step 3: Compile and add ${infilebody}_util.c into your project\n";
	$help_txt .= "step 4: Write following functions somewhere:\n\n";
	$help_txt .= "SERVERFUNCTIONSTOBEDEFINED\n\n\n";
	$help_txt .= "step 5: Use PROTOCOLNAMEInitServer( writefunc , worksize );\n";
	$help_txt .= "step 6: Call functions like following:\n\n\n";
	$help_txt .= "SERVERFUNCTIONSDEFINED\n";
	$help_txt .= "step 7: If you want to cleanup lsrpc, use PROTOCOLNAMECleanupServer();\n";
	$help_txt .= "\n\n\n-- end of generated help--\n";
}


sub outputServerCInit()
{
	
	$sv_c .= "#define _LSRPCSERV_C_\n";
	$sv_c .= "#include \"${infilebody}_serv.h\"\n";
	$sv_c .= "\n\n";
	$sv_c .= "void PROTOCOLNAMEServerDispatchMessage( int fd , char *line )\n";
	$sv_c .= "{\n";
	$sv_c .= "\tunsigned int msgid;\n";
	$sv_c .= "\tchar funcname[1024];\n";
	$sv_c .= "\tPROTOCOLNAMEsplitString( line );\n";
	$sv_c .= "\tPROTOCOLNAMEGetMessageInfo(&msgid , funcname , LSRPCSTRUCTNAME.token_list );\n";
	$sv_c .= "LSRPCSERVERDISPATCHDEFS\n\n";
	$sv_c .= "}\n\n";
	$sv_c .= "LSRPCSERVERSENDDEFS\n\n";
	$sv_c .= "int PROTOCOLNAMEInitServer(	int (*writefunc)(int,char*,int) ,	int worksiz )\n";
	$sv_c .= "{\n";
	$sv_c .= "\tint i;\n";
	$sv_c .= "\tif( (void*)writefunc == NULL){LSRPCSTRUCTNAME.write_func = PROTOCOLNAMEdefault_write_wrap;} else {LSRPCSTRUCTNAME.write_func = writefunc;}\n";
	$sv_c .= "\tPROTOCOLNAMEAllocateCommonWork(worksiz);\n";
	$sv_c .= "\tPROTOCOLNAMEstringwrapper = (char**)malloc(sizeof(char*) * MAXLSRPCARGS);\n";
	$sv_c .= "\tif(PROTOCOLNAMEstringwrapper ==NULL)return -1;\n";
	$sv_c .= "\tPROTOCOLNAMEbzero( (char*)PROTOCOLNAMEstringwrapper , sizeof(char*)*MAXLSRPCARGS);\n";
	$sv_c .= "\tfor(i=0;i<MAXLSRPCARGS;i++){\n";
    $sv_c .= "\t\tPROTOCOLNAMEstringwrapper[i] = (char*)malloc( worksiz );\n";
    $sv_c .= "\t\tif( PROTOCOLNAMEstringwrapper[i] == NULL){\n";
    $sv_c .= "\t\t\tfor(i=0;i<MAXLSRPCARGS;i++){free( PROTOCOLNAMEstringwrapper[i]);return -1;}\n";
	$sv_c .= "\t\t}\n";
	$sv_c .= "\t}\n";
	$sv_c .= "\tLSRPCSTRUCTNAME.ret_work = (char*)malloc(sizeof( worksiz ));\n";
	$sv_c .= "\tif( LSRPCSTRUCTNAME.ret_work == NULL ){ return -1; }\n";
	$sv_c .= "\treturn 0;\n";
	$sv_c .= "}\n\n";
	$sv_c .= "void PROTOCOLNAMECleanupServer( void )\n";
	$sv_c .= "{\n";
	$sv_c .= "\tint i;\n";
	$sv_c .= "\tfree( LSRPCSTRUCTNAME.work );\n";
    	$sv_c .= "\tfree( LSRPCSTRUCTNAME.arraywork);\n";
	$sv_c .= "\tfree( LSRPCSTRUCTNAME.escapework );\n";
	$sv_c .= "\tfree( LSRPCSTRUCTNAME.val_str);\n";
	$sv_c .= "\tfree( LSRPCSTRUCTNAME.token_list );\n";
	$sv_c .= "\tfor(i=0;i<MAXLSRPCARGS;i++){free( PROTOCOLNAMEstringwrapper[i]);}\n";
	$sv_c .= "\tfree( PROTOCOLNAMEstringwrapper );\n";
	$sv_c .= "\tfree( LSRPCSTRUCTNAME.ret_work );\n";
	$sv_c .= "}\n";
	$sv_c .= "/* end of the generated server code */";
}

sub outputServerHInit()
{
	$sv_h .= "#ifndef _LSRPCSERV_H_\n";
	$sv_h .= "#define _LSRPCSERV_H_\n";
	$sv_h .= "#include \"LSRPCUTILINCLUDEFILE\"\n";
	$sv_h .= "\n\n";
	$sv_h .=  "#ifdef MAXLSRPCARGS\n";
	$sv_h .=  "#if ( MAXLSRPCARGS <= LSRPCARGSCOUNT )\n";
	$sv_h .=  "#undef MAXLSRPCARGS\n";
	$sv_h .=  "#define MAXLSRPCARGS LSRPCARGSCOUNT\n";
	$sv_h .=  "#endif\n";
	$sv_h .=  "#else\n";
	$sv_h .=  "#define MAXLSRPCARGS LSRPCARGSCOUNT\n";
	$sv_h .=  "#endif\n";
	$sv_h .= "LSRPCDECLS\n";
	$sv_h .= "int PROTOCOLNAMEInitServer(int (*writefunc)(int,char*,int) ,	int worksiz );\n";
	$sv_h .= "void PROTOCOLNAMECleanupServer( void );\n";
	$sv_h .= "void PROTOCOLNAMEServerDispatchMessage( int fd, char *line );\n";
	$sv_h .= "#endif\n\n";
	$sv_h .= "/* end of the generated server header code */";
}


sub outputClientCInit()
{
	$cl_c .= "#define _LSRPCCLI_C_\n";
	$cl_c .= "#include \"${infilebody}_cli.h\"\n";
	$cl_c .= "\n\n";
	$cl_c .= "LSRPCCLIENTSENDDEFS\n\n";
	$cl_c .= "void PROTOCOLNAMEClientDispatchMessage(int fd ,char*line)\n";
	$cl_c .= "{\n";
	$cl_c .= "\tint msgid;\n";
	$cl_c .= "\tchar funcname[1024];\n";
	$cl_c .= "\tstrcpy( LSRPCSTRUCTNAME.work , line);\n";
	$cl_c .= "\tPROTOCOLNAMEsplitString( LSRPCSTRUCTNAME.work);\n";
	$cl_c .= "\tPROTOCOLNAMEGetMessageInfo( &msgid , funcname , LSRPCSTRUCTNAME.token_list);\n";
	$cl_c .= "LSRPCCLIENTDISPATCHDEFS\n";
	$cl_c .= "}\n\n";
	$cl_c .= "int PROTOCOLNAMEInitClient( int (*writefunc)(int,char*,int) ,int bufsiz ,int fd)\n";
	$cl_c .= "{\n";
	$cl_c .= "\tint i;\n";
	$cl_c .= "\tif( (void*)writefunc == NULL){LSRPCSTRUCTNAME.write_func = PROTOCOLNAMEdefault_write_wrap;} else {LSRPCSTRUCTNAME.write_func = writefunc;}\n";
	$cl_c .= "\tPROTOCOLNAMEAllocateCommonWork(bufsiz);\n";
	$cl_c .= "\tPROTOCOLNAMEstringwrapper = (char**)malloc(sizeof(char*) * MAXLSRPCARGS);\n";
	$cl_c .= "\tif(PROTOCOLNAMEstringwrapper ==NULL)return -1;\n";
	$cl_c .= "\tPROTOCOLNAMEbzero( (char*)PROTOCOLNAMEstringwrapper , sizeof(char*)*MAXLSRPCARGS);\n";
	$cl_c .= "\tfor(i=0;i<MAXLSRPCARGS;i++){\n";
    $cl_c .= "\t\tPROTOCOLNAMEstringwrapper[i] = (char*)malloc( bufsiz );\n";
    $cl_c .= "\t\tif( PROTOCOLNAMEstringwrapper[i] == NULL){\n";
    $cl_c .= "\t\t\tfor(i=0;i<MAXLSRPCARGS;i++){free( PROTOCOLNAMEstringwrapper[i]);return -1;}\n";
	$cl_c .= "\t\t}\n";
	$cl_c .= "\t}\n";
	$cl_c .= "\treturn 0;\n";
	$cl_c .= "}\n\n";
	$cl_c .= "void PROTOCOLNAMECleanupClient( void )\n";
	$cl_c .= "{\n";
	$cl_c .= "\tint i;\n";
	$cl_c .= "\tfree( LSRPCSTRUCTNAME.work );\n";
    	$cl_c .= "\tfree( LSRPCSTRUCTNAME.arraywork);\n";
	$cl_c .= "\tfree( LSRPCSTRUCTNAME.escapework );\n";
	$cl_c .= "\tfree( LSRPCSTRUCTNAME.val_str);\n";
	$cl_c .= "\tfree( LSRPCSTRUCTNAME.token_list );\n";
	$cl_c .= "\tfor(i=0;i<MAXLSRPCARGS;i++){free( PROTOCOLNAMEstringwrapper[i]);}\n";
	$cl_c .= "\tfree( PROTOCOLNAMEstringwrapper );\n";
	$cl_c .= "}\n";
	$cl_c .= "/* end of the generated client code */";
}

sub outputClientHInit()
{
	$cl_h .=  "#ifndef _LSRPCCLI_H_\n";
	$cl_h .=  "#define _LSRPCCLI_H_\n";
	$cl_h .=  "#include \"LSRPCUTILINCLUDEFILE\"\n";
	$cl_h .=  "\n\n";
	$cl_h .=  "#ifdef MAXLSRPCARGS\n";
	$cl_h .=  "#if ( MAXLSRPCARGS <= LSRPCARGSCOUNT )\n";
	$cl_h .=  "#undef MAXLSRPCARGS\n";
	$cl_h .=  "#define MAXLSRPCARGS LSRPCARGSCOUNT\n";
	$cl_h .=  "#endif\n";
	$cl_h .=  "#else\n";
	$cl_h .=  "#define MAXLSRPCARGS LSRPCARGSCOUNT\n";
	$cl_h .=  "#endif\n";
	$cl_h .=  "LSRPCDECLS\n";
	$cl_h .=  "int PROTOCOLNAMEInitClient( int(*)(int,char*,int)  , int bufsiz , int fd);\n";
	$cl_h .=  "void PROTOCOLNAMECleanupClient(void);\n";
	$cl_h .=  "void PROTOCOLNAMEClientDispatchMessage(int fd ,char*line);\n";
	$cl_h .=  "#endif\n\n";
	$cl_h .=  "/* end of the generated client header code */";
}

#
# 
#
sub outputCommonHeader($)
{
	my($nm)=shift;

	$_ = `pwd`;	chomp;
	my($pwd)=$_;

	my($msg)= "/* output by $0 $codeversion\n";
	$msg .=   " * made $runtime\n";
	$msg .=   " * user $runuser\n";
	$msg .=   " * host $runhost\n";
	$msg .=   " * file $pwd/$nm\n";
	$msg .=   " * util $lsrpc_c_file , $lsrpc_h_file\n";
	$msg .=   " * src  $pwd/$infile\n";
    $msg .=   " */\n";


	return $msg;
}


#
#
#
#

sub cleanFile()
{

	if( !( -e $output_dir) ){ system( "mkdir $output_dir" );}
	system( "rm -f $output_dir/${infilebody}_serv.h" );
	system( "rm -f $output_dir/${infilebody}_serv.c" );
	system( "rm -f $output_dir/${infilebody}_cli.h" );
	system( "rm -f $output_dir/${infilebody}_cli.c" );
	system( "rm -f $output_dir/${infilebody}_util.c" );
	system( "rm -f $output_dir/${infilebody}_util.h" );	
	system( "rm -f $output_dir/${infilebody}_help.txt" );
}

sub getArgs()
{
	my( $basetype , $siz ,$find_void);

# 変数の型とコンマの対応で考えることができるように、コンマを調整して入れる
	push( @tl , "," ) if( $tl[@tl-1] ne "," && $tl[@tl-1] ne "" );
	
	$find_void = 0;

	while(1){
# 引数がなくなったら終了
		eval{
			( $basetype , $siz ) = getType();
		};
		if( $@ ){
			printerr("cannot parse args" ,"" );
			die "cannot parse args";
		}

		last if( @tl == 0 ); # consumed args up!

		last if( $basetype eq "" );

		if( $basetype eq "void" ){ 
			if( $find_void == 1 ){
				printerr( "2 void's are not allowed");
			} else {
				$find_void = 1;
			}
		}

# 変数に名前をつけることができるようにする。次がコンマだったら終わり、次がそうじゃなかったら
# 名前であるとする
		my($nxt ) = shift(@tl );
		if( $nxt eq "," ){
			unshift( @tl , $nxt );
			my($a)= @arg_name;
			push( @arg_name , "arg$a" );
		} else {
			push( @arg_name , $nxt );
		}

		push( @arg_type , $basetype );
		push( @arg_size , $siz );


		shift( @tl );			# consume ','
	}

	if( @arg_type == 0 ){
		printerr( "no arguments!");
	}

		

	return 1;
}

#
# called when parsing a function
#
sub getFuncDecl()
{
	my($name ) = shift( @tl );
	my($left_br ) = shift( @tl );
	my($semic ) = pop( @tl );
	my($right_br ) = pop( @tl );

	if( !( $name =~ /\w/ ) ){
		printerr( "bad function name: $name" );
		die;
	}
	if( $left_br ne "(" ){
		printerr( "need '('" );
		die;
	}
	if( $right_br ne ")" ){
		printerr( "need ')'" );
		die;
	}		
	if( $semic ne ";" ){
		printerr( "need ';'" );
		die;
	}			

	$func_name = $name;
	return 1;
	

}


sub printFuncProfile()
{
	return if( ! $verbose );
	
	print( "kind:$func_kind funcname: $func_name rettype: $ret_type size:$ret_size\n" );

	my($i);
	for($i = 0; $i < @arg_type ; $i++){
		print "Arg $arg_type[$i] $arg_size[$i]\n"
	}
	
}

sub getType($$$$)
{
	my( $bt ) = shift(@tl );
	my( $left_br ) = shift( @tl );
	my( $nm) = shift( @tl );
	my( $right_br ) = shift( @tl );

	return ("","") if( $bt eq "" );

	if( !iscorrecttype( $bt )){
		printerr("bad type : $bt" );
		unshift(@tl , $right_br , $nm , $left_br , $bt );
		die;
	}

	if( $left_br eq "[" && $nm =~ /\d+/ && $right_br eq "]" ){
#It is an array
		if( !( $nm =~ /^\d+$/ ) ){
			printerr( "size of array has to be digits : $nm" );
			die;
		}
	} else {
#it is a single type
		unshift( @tl ,$right_br );
		unshift( @tl ,$nm );
		unshift( @tl ,$left_br );
		$nm = 0;
	}

	return ( $bt , $nm );
}

sub getFunctionKind()
{
	my($token ) =shift(@tl );



	foreach $a ( @kinds ){
		if( $a eq $token ){ return $a;}
	}

	unshift( @tl , $token );

	return "";

}
sub initParse()
{
	@tl = ();					# Token list

	$func_kind = "";			# Kind of a function( sendrecv, ...)

	$ret_type = "";
	$ret_size = "0";			# 0 is for single var, >=1 is for arrays

	@arg_type = ();				# for type of args of a function
	@arg_size = ();				# for size of args of a function
	@arg_name = ();             # for name of args of a function
	$func_name = "";			# name of the function
}

#
#
#
sub stylize(@)
{
	$find_kind = 0;

	my(@out);

	for(;;){
		$_ = shift;
		if( isAKind( $_ ) && $find_kind == 0){
			$find_kind = 1;
		}
		if( $find_kind == 1 ){
			push( @out , $_ );
		}
		last if( $_ eq ';' );
		last if( $_ eq '');
	}
	return @out;
}


sub isAKind($)
{
	$_ = shift;

	foreach $a (@kinds){
		if( $a eq $_ ){   return $_; }
	}
	return "";
}

#
# divide and split into C tokens
#
sub tokenize_c
{	
	$_ = shift;

	my(@out );
	chomp;

	foreach $_ (split){
		@sp = split( /([-+*\/^=();\[\]<>,])/ );       #separations are [+-*/^=()<>,\[\];]
		foreach $a (@sp){
			push( @out , $a) if( $a ne "" ); 
		}
	}

	return @out;
}

sub printv
{
	if( $verbose ){	print shift;  }
}
sub printerr
{								
	$_ = shift;
	print "$infile:$.: $_ \n";
	$error_count++;
}

#
# Confirm if the given type is correct or not
#
sub iscorrecttype($)
{
	my($argt) = shift;

	foreach $t ( @basictypes){
		return 1 if( $argt eq $t );
	}
	return 0;
}

#
# initialize rpcname_util.c and  rpcname_util.h
#
sub initUtilFilesContents()
{

$lsrpc_c_src = <<EOF
#define _LSRPCUTIL_C_


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#ifndef WIN32
#include <unistd.h>
#include <strings.h>
#endif
	
#include "LSRPCUTILINCLUDEFILE"

/*
  lsrpc routines
*/

int PROTOCOLNAMEAllocateCommonWork(int bufsiz)
{

	LSRPCSTRUCTNAME.workbufsize = bufsiz;

	LSRPCSTRUCTNAME.work = (char*)malloc( LSRPCSTRUCTNAME.workbufsize );
	if( LSRPCSTRUCTNAME.work == NULL ) return -1;
	PROTOCOLNAMEbzero( LSRPCSTRUCTNAME.work , LSRPCSTRUCTNAME.workbufsize );

	LSRPCSTRUCTNAME.arraywork = (char*)malloc( LSRPCSTRUCTNAME.workbufsize );
	if( LSRPCSTRUCTNAME.arraywork ==NULL){
		free( LSRPCSTRUCTNAME.work );
		return -3;
	}
	PROTOCOLNAMEbzero( LSRPCSTRUCTNAME.arraywork , LSRPCSTRUCTNAME.workbufsize );

	LSRPCSTRUCTNAME.escapework = (char*)malloc( LSRPCSTRUCTNAME.workbufsize );
	if( LSRPCSTRUCTNAME.escapework == NULL ){
		free(LSRPCSTRUCTNAME.work );free(LSRPCSTRUCTNAME.arraywork );
		return -4;
	}
	PROTOCOLNAMEbzero( LSRPCSTRUCTNAME.escapework , LSRPCSTRUCTNAME.workbufsize );

	LSRPCSTRUCTNAME.val_str = (char*)malloc( LSRPCSTRUCTNAME.workbufsize );
	if( LSRPCSTRUCTNAME.val_str == NULL ){
		free(LSRPCSTRUCTNAME.work );free(LSRPCSTRUCTNAME.escapework );free( LSRPCSTRUCTNAME.arraywork );
		return -5;
	}
	PROTOCOLNAMEbzero( LSRPCSTRUCTNAME.val_str , LSRPCSTRUCTNAME.workbufsize );

	LSRPCSTRUCTNAME.token_list = (char**)malloc( LSRPCSTRUCTNAME.workbufsize *sizeof( char** ) );
	if( LSRPCSTRUCTNAME.token_list == NULL){
		free(LSRPCSTRUCTNAME.work);free(LSRPCSTRUCTNAME.val_str);
		free(LSRPCSTRUCTNAME.escapework );free(LSRPCSTRUCTNAME.arraywork );
		return -6;
	}
	PROTOCOLNAMEbzero( (char*)LSRPCSTRUCTNAME.token_list , LSRPCSTRUCTNAME.workbufsize );

	return 0;
}

/**********
  Get message information from a network input

**********/
void PROTOCOLNAMEGetMessageInfo( int *id , char *funcname , char **tk )
{
	if( tk[0] == NULL || tk[1] == NULL ){
		*id = 0;
		strcpy( funcname , "" );
		return;
	}
	*id =  strtoul( tk[0] ,NULL,10);
	strcpy( funcname , tk[1] );

	return;
}

/********************************************
  string utilities

*********************************************/
char*PROTOCOLNAMEmkstr_int( int i )
{
#define MKSTR_INT(v) PROTOCOLNAMELtoa( (long)(v)) 
	strcpy( LSRPCSTRUCTNAME.val_str ,  (char*)MKSTR_INT(i) );
	strncat( LSRPCSTRUCTNAME.val_str ," " , LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;
}
char*PROTOCOLNAMEmkstr_u_int( unsigned int i )
{
#define MKSTR_U_INT(v) PROTOCOLNAMEUltoa( (unsigned long ) (v) )
	strcpy( LSRPCSTRUCTNAME.val_str , MKSTR_U_INT(i));
	strncat( LSRPCSTRUCTNAME.val_str ," " , LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;
}
char*PROTOCOLNAMEmkstr_long( long l )
{
#define MKSTR_LONG(v) PROTOCOLNAMELtoa( v ) 
	strcpy( LSRPCSTRUCTNAME.val_str , MKSTR_LONG(l) );
	strncat( LSRPCSTRUCTNAME.val_str ," " , LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;
}
char*PROTOCOLNAMEmkstr_u_long( unsigned long l )
{
#define MKSTR_U_LONG(v) PROTOCOLNAMEUltoa(v ) 
	strcpy( LSRPCSTRUCTNAME.val_str , MKSTR_U_LONG(l) );
	strncat( LSRPCSTRUCTNAME.val_str ," " , LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;	
}
char*PROTOCOLNAMEmkstr_short( short s )
{
#define MKSTR_SHORT16(v)    PROTOCOLNAMELtoa( (long)((int)(v) ))
	strcpy( LSRPCSTRUCTNAME.val_str , MKSTR_SHORT16(s) );
	strncat( LSRPCSTRUCTNAME.val_str ," " , LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;	
}
char*PROTOCOLNAMEmkstr_u_short( short s )
{
#define MKSTR_U_SHORT16(v)   PROTOCOLNAMELtoa( (long)(  ((long)(v)) & 0xffff ))
	strcpy( LSRPCSTRUCTNAME.val_str , MKSTR_U_SHORT16(s) );
	strncat( LSRPCSTRUCTNAME.val_str ," " , LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;	
}
char*PROTOCOLNAMEmkstr_char( char c )
{
#define MKSTR_CHAR8(v)   PROTOCOLNAMELtoa( ( long)((int)(v)))
	strcpy( LSRPCSTRUCTNAME.val_str , MKSTR_CHAR8(c));
	strncat( LSRPCSTRUCTNAME.val_str ," ", LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;	
}
char*PROTOCOLNAMEmkstr_u_char( char c)
{
#define MKSTR_U_CHAR8(v)   PROTOCOLNAMELtoa( (long)( ((long)(v)) & 0xff ))
	strcpy( LSRPCSTRUCTNAME.val_str , MKSTR_U_CHAR8(c));
	strncat( LSRPCSTRUCTNAME.val_str ," "  , LSRPCSTRUCTNAME.workbufsize );
	return LSRPCSTRUCTNAME.val_str;
}
char*PROTOCOLNAMEmkstr_string( char*a )
{
	char *ret = PROTOCOLNAMEescapeString( a );
	strcat( ret , " ");
	return ret;
}
char*PROTOCOLNAMEmkstr_float(float f )
{
	sprintf( LSRPCSTRUCTNAME.val_str , "%f " , f );
	return LSRPCSTRUCTNAME.val_str;
}
char*PROTOCOLNAMEmkstr_double( double d )
{
	sprintf( LSRPCSTRUCTNAME.val_str , "%f " , d );
	return LSRPCSTRUCTNAME.val_str;
}
char*PROTOCOLNAMEmkstr_int_array( int size , int *array )
{
#define MKSTR_ARRAYMACRO( func) \\
	{\\
		 int i;\\
         LSRPCSTRUCTNAME.arraywork[0] = '\\0';\\
		 for(i=0;i<size;i++){   \\
             strncat( LSRPCSTRUCTNAME.arraywork , func(array[i]) , LSRPCSTRUCTNAME.workbufsize );\\
		 }\\
         return LSRPCSTRUCTNAME.arraywork;\\
    }\\



	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_int );
}
char*PROTOCOLNAMEmkstr_u_int_array( int size , int *array )
{
	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_u_int );
}
char*PROTOCOLNAMEmkstr_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_short );
}
char*PROTOCOLNAMEmkstr_u_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_u_short );
}
char *PROTOCOLNAMEmkstr_char_array( int size , char *array )
{
	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_char );
}
char *PROTOCOLNAMEmkstr_u_char_array( int size , unsigned char *array )
{
	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_u_char );
}
char *PROTOCOLNAMEmkstr_float_array( int size , float *array )
{
	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_float );
}
char *PROTOCOLNAMEmkstr_double_array( int size , double *array )
{
	MKSTR_ARRAYMACRO( PROTOCOLNAMEmkstr_double );
}

/*********************
   translate string into base types
*********************/
int PROTOCOLNAMEdemkstr_int( char*a )
{
	if( a == (char*)NULL)return 0;
	return (int)strtol( a ,NULL , 10);
}
unsigned int PROTOCOLNAMEdemkstr_u_int( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned int ) strtoul( a ,NULL,10);
}
long PROTOCOLNAMEdemkstr_long( char*a )
{
	if( a == (char*)NULL)return 0;
	return (long)strtol( a , NULL , 10 );
}
unsigned long PROTOCOLNAMEdemkstr_u_long(char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned long ) strtoul( a , NULL , 10 ) ;
}
short PROTOCOLNAMEdemkstr_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (short) strtol( a , NULL , 10 );
}
unsigned short PROTOCOLNAMEdemkstr_u_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned short) strtoul( a , NULL , 10 );
}
char PROTOCOLNAMEdemkstr_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (char) strtol(  a  , NULL , 10 );
}
unsigned char PROTOCOLNAMEdemkstr_u_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned char ) strtoul( a,NULL , 10 );
}
float PROTOCOLNAMEdemkstr_float( char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (float) atof( a);
}
double PROTOCOLNAMEdemkstr_double(char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (double) strtod( a , NULL );
}
char* PROTOCOLNAMEdemkstr_string( char*a)
{
	if( a == (char*)NULL){
		strcpy( LSRPCSTRUCTNAME.escapework , "" );
		return LSRPCSTRUCTNAME.escapework;
	}
	return PROTOCOLNAMEdescapeString( a );
}
int *PROTOCOLNAMEdemkstr_int_array( char**tk ,int *buf  ,int start , int size )
{
#define DEMKSTR_ARRAYMACRO( func, defaultvalue )          \\
    {\\
        int i;\\
        for(i=start;i<(start+size);i++){\\
            if( tk[i] == NULL ){   \\
		        buf[i-start]=defaultvalue ;\\
            } else {\\
     	 	    buf[i-start] = func( tk[i] );\\
            }\\
	    }\\
        return buf;\\
    }

    DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_int , 0 );
}
int *PROTOCOLNAMEdemkstr_u_int_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_u_int , 0 );
}
unsigned int *PROTOCOLNAMEdemkstr_long_array(
 char **tk , unsigned int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_long  , 0);
}
unsigned long *PROTOCOLNAMEdemkstr_u_long_array(
 char **tk , unsigned long *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_u_long , 0);
}
short *PROTOCOLNAMEdemkstr_short_array( char **tk , short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_short , 0);
}
unsigned short* PROTOCOLNAMEdemkstr_u_short_array( 
 char **tk , unsigned short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_u_short , 0);
}
char *PROTOCOLNAMEdemkstr_char_array( char **tk , char *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_u_char , 0);
}
unsigned char *PROTOCOLNAMEdemkstr_u_char_array( 
char **tk , unsigned char*buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_u_char , 0);
}
float *PROTOCOLNAMEdemkstr_float_array( char **tk , float *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_float , (float)0.0);
}
double *PROTOCOLNAMEdemkstr_u_double_array( char **tk , double *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( PROTOCOLNAMEdemkstr_double , (double)0.0);
}
/*********************
	Escape a string
	Escape only  ' ' , '\\', '\n'
*********************/
char*  PROTOCOLNAMEescapeString( char*a )
{
	int i,c=0;

	LSRPCSTRUCTNAME.escapework[0] = '\\0';

	for(i=0;;i++){
		if( a[i] == '\\0' ){
			LSRPCSTRUCTNAME.escapework[c++] = '\\0';
			break;
		} else if( a[i] == '\\\\' ){
			LSRPCSTRUCTNAME.escapework[c++] = '\\\\';
			LSRPCSTRUCTNAME.escapework[c++] = '\\\\';
		} else if( a[i] == ' ' ){
			LSRPCSTRUCTNAME.escapework[c++] = '\\\\';
			LSRPCSTRUCTNAME.escapework[c++] = 'S';
		} else if( a[i] == '\\n' ){
			LSRPCSTRUCTNAME.escapework[c++] = '\\\\';
			LSRPCSTRUCTNAME.escapework[c++] = 'n';
		} else if( a[i] == '\\r' ){
			LSRPCSTRUCTNAME.escapework[c++] = '\\\\';
			LSRPCSTRUCTNAME.escapework[c++] = 'r';
		} else {
			LSRPCSTRUCTNAME.escapework[c++] = a[i];
		}
	}

	return LSRPCSTRUCTNAME.escapework;
}
char* PROTOCOLNAMEdescapeString( char*a )
{
	int i ,c =0;

	LSRPCSTRUCTNAME.escapework[0] = '\\0';

	for(i=0;;i++){
		if( a[i] == '\\0' ){
			LSRPCSTRUCTNAME.escapework[c++]='\\0';
			break;
		} else if( a[i] == '\\\\' ){
			if( a[i+1] == 'S' ){     /* space */
				LSRPCSTRUCTNAME.escapework[c++] = ' ';
			} else if( a[i+1] == 'n' ){
				LSRPCSTRUCTNAME.escapework[c++] = '\\n';
			} else if( a[i+1] == 'r' ){
				LSRPCSTRUCTNAME.escapework[c++] = '\\r';
			} else if( a[i+1] == '\\\\' ){
				LSRPCSTRUCTNAME.escapework[c++] = '\\\\';
			} else {
				LSRPCSTRUCTNAME.escapework[c++] = a[i];
			}
			i++;           
		} else {
			LSRPCSTRUCTNAME.escapework[c++] = a[i];
		}
	}
	return LSRPCSTRUCTNAME.escapework;
}
/*
   This function works only when char*src is escaped

   NOTICE: Effects and Modifies the contents of char*src!
   NOTICE : Ends the output token list with NULL pointer

Ex:
        v out[0]       v out[1]
  "     asdjfhasfdasdf asdf asf asdf "
 */
void PROTOCOLNAMEsplitString( char *src  )
{
	int i,c=0;

	for(i = 0 ; ; i++ ){
		if( src[i] == '\\0' ) break;
		if( i==0){
		    LSRPCSTRUCTNAME.token_list[c++]=&(src[i]);
		}
		if( src[i]== ' '){
		    LSRPCSTRUCTNAME.token_list[c++]=&(src[i+1]);
		}
	}
	while( *src ){
		if( ( *src  ) == ' ' || (*src ) == '\\n' || (*src ) == '\\r' ) *src = '\\0';
		src++;
 	}
	LSRPCSTRUCTNAME.token_list[c] = (char*)NULL;
}
void PROTOCOLNAMEModifyStyle( char *a )
{
	int l = strlen( a );

	if( l <= 0 )  return;

	if( a[l-1] != '\\n' ){

		if(a[l-1] == ' '){
			a[l-1] = '\\n'; 
		} else { 
			a[l] = '\\n'; a[l+1] = '\\0';
		}
	}
}
int PROTOCOLNAMEdefault_write_wrap( int fd , char *buf , int size )
{
#ifndef WIN32
	return write( fd , buf , size );
#else
	return 0;
#endif
}


void PROTOCOLNAMEconsumeLine(char *buf , int offset )
{
	int i;
	int shift=0;

	buf+= offset;

	for(i=0;;i++){
		if( buf[i] == '\\n' ){
			shift = i + 1;
			break;
		}
	}
    if( shift == 0 )return;
	for(i=shift;;i++){
		buf[i - shift] = buf[i];
		if( buf[i] == '\\0')break;
	}
}
void PROTOCOLNAMEcopyLine( char*src , char *out )
{
	int i;
	for(i=0;;i++){
		out[i] = src[i];
		if( src[i] == '\\n' ){
			out[i+1] = '\\0' ;
			return;
		}
		if( src[i] == '\\0' )return;
	}
	strcpy( out , "" );
}
unsigned int PROTOCOLNAMEGetNewMessageID(void)
{
	return LSRPCSTRUCTNAME.message_id++;

}
/****************
  Get last message which is made by this RPC routines
****************/
char *PROTOCOLNAMEClientGetVirtualMessage(void)
{
	return LSRPCSTRUCTNAME.work;
}
/*****************
  WRITE
   int flg : if 1, actually write. Otherwise no Network access
*****************/
void PROTOCOLNAMESend( int fd , char *msg )
{
    PROTOCOLNAMEModifyStyle( msg );
	LSRPCSTRUCTNAME.write_func( fd , msg , strlen( msg ));
}

/****************
  create a header which has function name and new Message ID
****************/
void PROTOCOLNAMECreateHeader( char *out ,char *fname )
{
	sprintf( out ,"%u %s " , PROTOCOLNAMEGetNewMessageID() , fname );
}
void PROTOCOLNAMECreateHeaderID( char *out,unsigned long msgid , char *fname )
{
	sprintf( out ,"%u %s " , (unsigned int)msgid , fname );
}
char *PROTOCOLNAMELtoa( long v )
{
	static char _ltoa_out[64];
	sprintf( _ltoa_out , "%d" , (int)v );
	return _ltoa_out;
}
char *PROTOCOLNAMEUltoa( unsigned long v )
{
	static char _ultoa_out[64];
	sprintf( _ultoa_out , "%u" , (unsigned int)v );
	return _ultoa_out;
}

/****************
   string address wrapper
 RINGO  
****************/
char *PROTOCOLNAMEwrapStringAddr( char *copy , char*src )
{
	strcpy( copy , src );
	return copy;
}

/***************
 bzero buffer ( some OSs like win32 don't have bzero )
***************/
void PROTOCOLNAMEbzero( char *b , int siz )
{
	int i;
	for(i=0;i<siz;i++) b[i]=0;

}
/***************
    Encoder routine made by jun 1998.11.15
***************/    
int jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)
{
	char sum;
	int i;
	if(srclen+1 > maxencodedlen){
		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++) encoded[i] = src[i];
	}
	if(srclen+1 <= maxencodedlen){
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
			if(((key%7) == (i%5))||((key%2) == (i%2)))src[i] = ~src[i];
		}
		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i) encoded[i] = src[i] + sum*((i*i)%3);
			if(abs((key%srclen)) == i) encoded[i] = sum;
			if(abs((key%srclen)) < i) encoded[i] = src[i-1] + sum*((i*i)%7);
		}
        return 0;
	} else {
        return -1;
    }
}
/**************
    Decoder routine made by jun 1998.11.15
**************/
void jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)
{
	char sum;
	int i;
	*decodedlen=srclen-1;
	sum = src[abs(key%(*decodedlen))];
	for(i=0;i<srclen;i++){
		if(abs((key%(*decodedlen))) > i) decoded[i] = src[i] - sum*((i*i)%3);
		if(abs((key%(*decodedlen))) < i) decoded[i-1] = src[i] - sum*((i*i)%7);
	}
	for(i=0;i<(*decodedlen);i++){
		if(((key%7) == (i%5))||((key%2) == (i%2))) decoded[i] = ~decoded[i];
	}
}


/* end of generated code */
EOF
;



$lsrpc_h_src=<<EOF
#ifndef _LSRPCUTIL_H_
#define _LSRPCUTIL_H_

#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <strings.h>
#include <unistd.h>
#endif



struct PROTOCOLNAME {
	int (*write_func)(int,char*,int) ;    /* write function */
	size_t workbufsize;                   /* size of work area */
    char  *work,*arraywork,*escapework,*val_str,*ret_work;      /* work areas which have same size  */
    char** token_list;                    /* token list */
    unsigned long message_id; /*= 1,  */       /* for generating message IDs */
};

#ifdef _LSRPCUTIL_C_

struct PROTOCOLNAME LSRPCSTRUCTNAME = {
	NULL,
	0,
	NULL,NULL,NULL,NULL,NULL,
	NULL,
	1,
};

char **PROTOCOLNAMEstringwrapper;
#else
extern char **PROTOCOLNAMEstringwrapper;
extern struct PROTOCOLNAME LSRPCSTRUCTNAME;

#endif

char* PROTOCOLNAMEescapeString( char*a );
char* PROTOCOLNAMEdescapeString( char*a );
void PROTOCOLNAMEsplitString( char *src  );
char*PROTOCOLNAMEmkstr_int( int i );
char*PROTOCOLNAMEmkstr_u_int( unsigned int i );
char*PROTOCOLNAMEmkstr_long( long l );
char*PROTOCOLNAMEmkstr_u_long( unsigned long l );
char*PROTOCOLNAMEmkstr_short( short s );
char*PROTOCOLNAMEmkstr_u_short( short s );
char*PROTOCOLNAMEmkstr_char( char c );
char*PROTOCOLNAMEmkstr_u_char( char c);
char*PROTOCOLNAMEmkstr_string( char*a );
char*PROTOCOLNAMEmkstr_float( float f );
char*PROTOCOLNAMEmkstr_double( double d );
char*PROTOCOLNAMEmkstr_int_array( int size , int *array );
char*PROTOCOLNAMEmkstr_u_int_array( int size , int *array );
char*PROTOCOLNAMEmkstr_short_array( int size , short *array );
char*PROTOCOLNAMEmkstr_u_short_array(int size , short *array );
char *PROTOCOLNAMEmkstr_char_array( int size , char *array );
char *PROTOCOLNAMEmkstr_u_char_array( int size , unsigned char *array );
char *PROTOCOLNAMEmkstr_float_array( int size , float *array );
char *PROTOCOLNAMEmkstr_double_array( int size , double *array );
int PROTOCOLNAMEdemkstr_int( char*a );
unsigned int PROTOCOLNAMEdemkstr_u_int( char*a );
long PROTOCOLNAMEdemkstr_long( char*a );
unsigned long PROTOCOLNAMEdemkstr_u_long(char*a );
short PROTOCOLNAMEdemkstr_short( char*a );
unsigned short PROTOCOLNAMEdemkstr_u_short( char*a );
char PROTOCOLNAMEdemkstr_char( char*a );
unsigned char PROTOCOLNAMEdemkstr_u_char( char*a );
float PROTOCOLNAMEdemkstr_float( char*a );
double PROTOCOLNAMEdemkstr_double(char*a );
char* PROTOCOLNAMEdemkstr_string( char*a);
int *PROTOCOLNAMEdemkstr_int_array( char**tk ,int *buf  ,int start , int size );
int *PROTOCOLNAMEdemkstr_u_int_array( char **tk , int *buf , int start , int size );
unsigned int *PROTOCOLNAMEdemkstr_long_array(
	char **tk , unsigned int *buf , int start , int size );
unsigned long *PROTOCOLNAMEdemkstr_u_long_array(
	char **tk , unsigned long *buf , int start , int size );
short *PROTOCOLNAMEdemkstr_short_array( char **tk , short *buf , int start , int size );
unsigned short*PROTOCOLNAMEdemkstr_u_short_array( 
	char **tk , unsigned short *buf , int start , int size );
char *PROTOCOLNAMEdemkstr_char_array( char **tk , char *buf , int start , int size );
unsigned char *PROTOCOLNAMEdemkstr_u_char_array( 
	char **tk , unsigned char*buf , int start , int size );
float *PROTOCOLNAMEdemkstr_float_array( char **tk , float *buf , int start , int size );
double *PROTOCOLNAMEdemkstr_u_double_array( char **tk , double *buf , int start , int size );

char *PROTOCOLNAMEwrapStringAddr( char *copy , char*src );

void PROTOCOLNAMEModifyStyle( char *a );
void PROTOCOLNAMEGetMessageInfo( int *id , char *funcname , char **tk );
int PROTOCOLNAMEClientRead(void);

void PROTOCOLNAMEconsumeLine(char *buf , int ofs );
void PROTOCOLNAMEcopyLine( char*src , char *out );
void PROTOCOLNAMESend( int fd , char *msg );
int PROTOCOLNAMEAllocateCommonWork(int bufsiz);
unsigned int PROTOCOLNAMEGetNewMessageID(void);
char *PROTOCOLNAMEClientGetVirtualMessage(void);
void PROTOCOLNAMECreateHeader(char*out, char *fname );
void PROTOCOLNAMECreateHeaderID( char *out,unsigned long msgid , char *fname );

int PROTOCOLNAMEdefault_write_wrap( int fd , char *buf , int size );

void PROTOCOLNAMEbzero( char *b , int siz );
char *PROTOCOLNAMELtoa( long v );
char *PROTOCOLNAMEUltoa( unsigned long v );

#endif


EOF
;





} # initUtilFilesContents()
