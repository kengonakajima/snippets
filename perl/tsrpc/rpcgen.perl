#!/usr/local/bin/perl -w

package main;

#
#
#
# tsrpcgen コード
#
#
#
use strict "subs";
require "getopt.pl";						# コマンドライン解析用


my $cwd = `pwd`;


# 開発用の@INC拡張
push( @INC , "/home/ringo/perl/tsrpc" , "/home/ringo/tsrpc" );


#################################################################
# すごく一般的に使う変数
#################################################################

# 動作モード関連

my $use_pack=0;
my $use_encode=0;
my $use_auth=0;
my $verbose=0;

#
# 環境設定   何かデフォルトの値を入れとくこと(判定につかう)
# コードの値をおきかえる用につかうのだ。
my %environment = (
				"tcp_keep_alive_timeout" => "20" , 
				"tcp_connection_timeout" => "200" ,
				"version" => "0" , 
				"author" => "somebody"
);

# ハッシュを使ってどのタイプのものをつくるのか記憶
# client , server , both のどれかが入る
# これをforeachするのだ

my %create_suffix = (
				  "java" => "" ,
				  "perl" => "" ,
				  "unix"    => "" ,
				  "win"    => "" );

# プロトコルの内容
my @funcnames = (
#  "hoge"    こういう名前の関数があったとすると
);

my %funcspecs = (

#		  "transport_hoge" => "tcp" ,
#         "name_hoge" => "hoge" ,
#         "ret_type_hoge" =>"int32" ,
#         "ret_size_hoge" =>"0" ,
#         "arg_type_0_hoge" => "int32" ,
#         "arg_size_0_hoge" => "0"        このように、ひたすらこのハッシュにためこむ。
);



# 基本型たち
my @basictypes = (
			  "int32" ,"u_int32" , 
			  "long32","u_long32",
			  "short16" , "u_short16" , 
			  "char8" , "u_char8", 
			  "float32", "double64", 
			  "string"
			  );

# 同意語のリスト
my %aliastypes = (
				   "int32" => "int32" ,"u_int32" => "u_int32", 
				   "long32" => "long32" ,"u_long32" => "u_long32",
				   "short16"=>"short16"  , "u_short16" =>"u_short16", 
				   "char8"=>"char8" , "u_char8" => "u_char8", 
				   "float32"=>"float32" , "double64" => "double64", 
				   "string" => "string" , 

				   "int" => "int32" ,    "u_int" => "u_int32" , 
				   "long" => "long32" ,  "u_long" => "u_long32" ,
	  			   "short" => "short16" ,  "u_short" => "u_short16" ,
				   "char" => "char8" ,	 "u_char" => "u_char8",
				   "float" => "float32",  "double" => "double64" ,
				   "str" => "string"
				   );



#################################################################
# コマンドライン処理
#################################################################

if( @ARGV == 0 ){		printusage();   }


my $optnot = "###";

( $opt_a , $opt_b , $opt_c , $opt_e , $opt_p , $opt_s , $opt_v) = 
( $optnot ,$optnot ,$optnot ,$optnot ,$optnot ,$optnot ,$optnot);

my $infile = shift( @ARGV );    # getoptをうまく使うために。

Getopt( 'abceps' );


if( $opt_a ne $optnot ){ $use_auth=1;}
if( $opt_e ne $optnot ){ $use_encode=1; }
if( $opt_p ne $optnot ){ $use_pack=1; }
if( $opt_v ne $optnot ){ $verbose=1; }
if( $opt_b ne $optnot ){ $create_suffix{"$opt_b"} = "both"; }
if( $opt_c ne $optnot ){ $create_suffix{"$opt_c"} = "client";}
if( $opt_s ne $optnot ){ $create_suffix{"$opt_s"} = "server"; }

$verbose=1;      # デバッグ用




if( $infile eq "" ){  printusage(); }


#################################################################
# プロトコルファイルをパースする
#################################################################

eval{
	parseInFile( $infile );
};
if( $@){	
	die "couldn't parse $infile because of $error_count error(s).\n";     
} else {
	printv( "finish parsing\n" );
}

# ここで関数の表を作り終えた
$i = @funcnames;

printv( "total $i function(s) are found in $infile. \n" );
print join( '/', @funcnames) , "\n";

#################################################################
# それぞれ用に出力するのだ
#################################################################


@langs = ( "java", "perl", "unix", "win" );


foreach $lng ( @langs)
{


	if( $create_suffix{$lng} ne "" ){
		printv( "output $lng source code for $create_suffix{$lng}:\n");

		eval "output_$lng(\%proto , $create_suffix{$lng});";

	}

}


#################################################################
# Parser
# %protoをいじるのはここだけ
#
# 
#
# 行頭にくるキーワードは
# tcp udp
# tcp_keep_alive_timeout
# 
#################################################################
sub parseInFile
{
	printv( "start parsing\n" );

	my( $infile ) = shift;

	if( !open( INFILE , $infile ) ){die "cannot open file '" , $infile , "'\n"; }

	$error_count = 0;

	while( <INFILE> )
	{
		chomp;

#		printv( "$infile $. : $_\n");

# C/C++のスタイルのコメントを取る
		$_ =~  s/\/\*.*\*\/// ;
		$_ =~ s/\/\/.*$//;


# トークンのリストにする
		@_ = tokenize_c( $_ );
#		foreach $_ (@_)	{	print $_ ,"|";	}print "\n";

# 何もない行は次へ
		next if( @_ == 0 );		

# スタイルチェック ( ; が最後にあるか？ )
		if( $_[@_-1] ne ";" ){
			printerr( "one definition has to be in one line and terminated by ';'" , "';'" );
			$error_count++;
			next;
		}



# パース初期化は毎行する
		parseInit();

		@tl = @_;

		eval {
			
			$action = getActionType();
			
			if( $action eq "function" ){
				
				eval{
					getTransportName();
				};
				if( $@ ){
					printerr( "bad transport name" );
					die;
				}
				eval{
					getRettype();
				};
				if( $@){
					printerr( "bad return type" );
					die;
				}
				eval{
					getFuncDef();
				};
				if( $@ ){
					printerr( "incomplete function def" );
					die;
				}
				eval{
					getArgs();
				};
				if( $@ ){
					printerr( "invalid arguements"  );
					die;
				}

			} elsif ( $action eq "variable" ){
				eval{
					getVariableDef();
				};
				if( $@ ){
					printerr( "invalid expression"  );
					die;
				}
			}
		};

# エラーが発生したら何かする
		if( $@){ $error_count++; }

# 結果を吐く
		printParseResult();

# パース結果を処理する
		eval{
			realizeParseResult();
		};
		if( $@ ){$error_count++; }

	}

	close( INFILE );

	die if( $error_count > 0 );
}



sub getVariableDef
{
	my( $name )= shift( @tl );
	my( $equal_char ) = shift( @tl );
	my( $value ) = shift( @tl );
	my( $semic ) = shift( @tl );


	if( !( $name =~ /\w/) ){
		printerr( "bad variable name" , "$name" ) ;
		die;
	}

	if( $equal_char ne "=" ){
		printerr( "need '='" , "$equal_char" );
		die;
	}



	if( !( $value =~ /^\d+$/ ) ){
		printerr( "value being set has to be digit" , "$value" );
		die;
	}
	if( $semic ne ";" ){
		printerr( "need ';' at the end of variable definition" , ";" );
		die;
	}

	$variable_name = $name;
	$variable_value = $value;

	return 1;
}
sub getActionType
{

# 先頭のを見てまた戻す

	my( $token ) = shift(@tl);
	unshift(@tl , $token );

	return "function" if( $token =~ /tcp\b|udp\b/ );
	
	return "variable";

}

sub getArgs
{
	my( $basetype , $siz );

# コンマでおわってなくてかつ引数があったらコンマたす
	push( @tl , "," ) if( $tl[@tl-1] ne "," && $tl[@tl-1] ne "" );
	
	while(1){
# 引数がなくなったら正常終了

		eval{
			( $basetype , $siz ) = getType();
		};
		if( $@ ){
			printerr("cannot parse args" ,"" );
			die;
		}


		last if( @tl == 0 ); # 引数列を消費しつくした

		last if( $basetype eq "" );

		push( @arg_type , $basetype );
		push( @arg_size , $siz );
		
		shift( @tl );			# consume ','
		


	}

	return 1;
}

sub getFuncDef
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

sub getRettype
{
	($basetype , $siz ) = getType();

	die if( $basetype eq "" );
	
	$ret_type = $basetype;
	$ret_size = $siz;

	return 1;

}
sub getType($$$$)
{
	
	my($bt) = shift( @tl);
	my($left_br) = shift(@tl );
	my($nm) = shift( @tl );
	my($right_br) = shift( @tl );

	return ("","") if( $bt eq "" );

	if( !iscorrecttype( $bt) ){
		printerr("bad type : $bt"  );
		unshift( @tl , $right_br , $nm , $left_br , $bt );
		die;
	}

	if( $left_br eq "[" && $nm =~ /\d+/ && $right_br eq "]" ){
# 配列だった
		if( !( $nm =~ /^\d+$/ ) ){
			printerr( "size of array has to be digit : $nm" );
			die;
		}


	} else {

# 基本型だった
		unshift( @tl , $right_br );
		unshift( @tl ,$nm );
		unshift( @tl ,$left_br );

		$nm = 0;
	}

	return ( $aliastypes{$bt} , $nm );
	
}

#
#  tcp とか udpというトークンをたべてしまう
#
sub getTransportName
{
	$trname = shift(@tl);
	if( $trname =~ /tcp\b|udp\b/ ){
		$transport_name = $trname;
	} else {
		printerr( "bad transportation name: $trname" );
		die;
	}
}
sub printParseResult
{
	my($msg);
	if( $func_name ne "" ){
		$msg =  "FUNC transport:$transport_name name:$func_name rettype: $ret_type [$ret_size]\n";	
		
		for( $_ =0 ;$_ < @arg_type ; $_++ ){					
			$msg .= "arg$_ $arg_type[$_] [$arg_size[$_]]\n";
		}
	} elsif( $variable_name ne "" ){

		$msg .= "VARIABLE name:$variable_name value:$variable_value\n";
	} else {
		return;
	}

	printv( "$msg" );

}
sub parseInit
{
	@tl = ();					# トークンリスト

	$ret_type = "";
	$ret_size = "0";			# 0は基本型、1以上が配列

	@arg_type = ();				# 引数の型保存
	@arg_size = ();				# 引数の型のサイズ保存

	$func_name = "";			# 関数の名前保存

	$transport_name = "";		# トランスポートの名前保存

	$variable_name = "";		# 変数設定用の変数名保存
	$variable_value = "";		# 変数の値保存

}
#
# パースの結果を反映させる
#
sub realizeParseResult
{
# 変数設定だったら変数の値をセット
	if( $variable_name ne "" ){
		if( $environment{$variable_name} eq "" ){
			#そんな変数はないぞ
			printerr( "no such variable settable : $variable_name" );
			die;
		}
		$environment{$variable_name} = $variable_value;
	} elsif( $func_name ne "" ){

# 関数宣言だったら関数の設定をセット
		addFunc();
	}
	return 1;
}

#
# 関数を一個追加する関数
# 
sub addFunc
{

	push( @funcnames , $func_name );

# 関数ハッシュに追加
	$funcspecs{"transport_$func_name"} = $transport_name;
	$funcspecs{"name_$func_name"} = $func_name;
	$funcspecs{"ret_type_$func_name"} = $ret_type;
	$funcspecs{"ret_size_$func_name"} = $ret_size;

	for( $ind = 0 ; $ind < @arg_type ; $ind++ ){
		my $newfunc_name = $ind;
		$newfunc_name .= "_";
		$newfunc_name .= $func_name;
		print "arg_type_$newfunc_name\n";
		print "arg_size_$newfunc_name\n";
		$funcspecs{"arg_type_$newfunc_name"} = $arg_type[$ind];
		$funcspecs{"arg_size_$newfunc_name"} = $arg_size[$ind];
	}

}



#################################################################
# Output   eval によって呼び出される
#################################################################
sub output_win
{

}
sub output_java
{

}
sub output_unix
{

}

#ringo

#
# perlの出力をする。
#
sub output_perl
{

	outputPerlUtil();



}
# perlの有用関数を定義するモジュールを出力する
sub outputPerlUtil
{

# ファイルをひらいて
	open( PERLU , getFileNameInc( "tsrpcsrc.pm" ) );


# 変換しながら出力
	while( <PERLU> ){
		print;
	}

	close(PERLU );

}

	


#################################################################
# Utility
#################################################################
sub printusage
{
	die "Usage: $0 infile ((-c|-s|-b) (win|unix|java|perl))+ (-a|-e|-p|-v)*\n";  
}
sub printv
{
	if( $verbose ){	print shift;  }

}
sub printerr
{								
	$_ = shift;
	print "$infile:$.: $_ \n";
}
#
# Cのトークンに分ける
#
sub tokenize_c
{	
	my(@out );
	chomp;

	foreach $_ (split){
		@_ = split( /([-+*\/^=();\[\]])/ );       #separations are [+-*/^=()]
		foreach $_ (@_){
			push( @out , $_) if( $_ ne "" ); 
		}
	}
	return @out;
}

#
# TSRPCGEN にとって正しい型かどうか
#
sub iscorrecttype
{
	$_ = shift;
	$realname = $aliastypes{ $_ };
	foreach $t ( @basictypes){
		return 1 if( $realname eq $t );
	}
	return 0;
}

#
# @INCにあるファイルのファイル名を返す。
# getFileNameInc( $name )
sub getFileNameInc
{								

	my($fnm) = shift;
	foreach $_ ( @INC ){
		if( -e( $_ = "$_/$fnm" )){
			return $_;
		}
	}
}
