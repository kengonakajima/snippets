#!/usr/local/bin/perl -w

package main;

#
#
#
# tsrpcgen �R�[�h
#
#
#
use strict "subs";
require "getopt.pl";						# �R�}���h���C����͗p


my $cwd = `pwd`;


# �J���p��@INC�g��
push( @INC , "/home/ringo/perl/tsrpc" , "/home/ringo/tsrpc" );


#################################################################
# ��������ʓI�Ɏg���ϐ�
#################################################################

# ���샂�[�h�֘A

my $use_pack=0;
my $use_encode=0;
my $use_auth=0;
my $verbose=0;

#
# ���ݒ�   �����f�t�H���g�̒l�����Ƃ�����(����ɂ���)
# �R�[�h�̒l������������p�ɂ����̂��B
my %environment = (
				"tcp_keep_alive_timeout" => "20" , 
				"tcp_connection_timeout" => "200" ,
				"version" => "0" , 
				"author" => "somebody"
);

# �n�b�V�����g���Ăǂ̃^�C�v�̂��̂�����̂��L��
# client , server , both �̂ǂꂩ������
# �����foreach����̂�

my %create_suffix = (
				  "java" => "" ,
				  "perl" => "" ,
				  "unix"    => "" ,
				  "win"    => "" );

# �v���g�R���̓��e
my @funcnames = (
#  "hoge"    �����������O�̊֐����������Ƃ����
);

my %funcspecs = (

#		  "transport_hoge" => "tcp" ,
#         "name_hoge" => "hoge" ,
#         "ret_type_hoge" =>"int32" ,
#         "ret_size_hoge" =>"0" ,
#         "arg_type_0_hoge" => "int32" ,
#         "arg_size_0_hoge" => "0"        ���̂悤�ɁA�Ђ����炱�̃n�b�V���ɂ��߂��ށB
);



# ��{�^����
my @basictypes = (
			  "int32" ,"u_int32" , 
			  "long32","u_long32",
			  "short16" , "u_short16" , 
			  "char8" , "u_char8", 
			  "float32", "double64", 
			  "string"
			  );

# ���ӌ�̃��X�g
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
# �R�}���h���C������
#################################################################

if( @ARGV == 0 ){		printusage();   }


my $optnot = "###";

( $opt_a , $opt_b , $opt_c , $opt_e , $opt_p , $opt_s , $opt_v) = 
( $optnot ,$optnot ,$optnot ,$optnot ,$optnot ,$optnot ,$optnot);

my $infile = shift( @ARGV );    # getopt�����܂��g�����߂ɁB

Getopt( 'abceps' );


if( $opt_a ne $optnot ){ $use_auth=1;}
if( $opt_e ne $optnot ){ $use_encode=1; }
if( $opt_p ne $optnot ){ $use_pack=1; }
if( $opt_v ne $optnot ){ $verbose=1; }
if( $opt_b ne $optnot ){ $create_suffix{"$opt_b"} = "both"; }
if( $opt_c ne $optnot ){ $create_suffix{"$opt_c"} = "client";}
if( $opt_s ne $optnot ){ $create_suffix{"$opt_s"} = "server"; }

$verbose=1;      # �f�o�b�O�p




if( $infile eq "" ){  printusage(); }


#################################################################
# �v���g�R���t�@�C�����p�[�X����
#################################################################

eval{
	parseInFile( $infile );
};
if( $@){	
	die "couldn't parse $infile because of $error_count error(s).\n";     
} else {
	printv( "finish parsing\n" );
}

# �����Ŋ֐��̕\�����I����
$i = @funcnames;

printv( "total $i function(s) are found in $infile. \n" );
print join( '/', @funcnames) , "\n";

#################################################################
# ���ꂼ��p�ɏo�͂���̂�
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
# %proto��������̂͂�������
#
# 
#
# �s���ɂ���L�[���[�h��
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

# C/C++�̃X�^�C���̃R�����g�����
		$_ =~  s/\/\*.*\*\/// ;
		$_ =~ s/\/\/.*$//;


# �g�[�N���̃��X�g�ɂ���
		@_ = tokenize_c( $_ );
#		foreach $_ (@_)	{	print $_ ,"|";	}print "\n";

# �����Ȃ��s�͎���
		next if( @_ == 0 );		

# �X�^�C���`�F�b�N ( ; ���Ō�ɂ��邩�H )
		if( $_[@_-1] ne ";" ){
			printerr( "one definition has to be in one line and terminated by ';'" , "';'" );
			$error_count++;
			next;
		}



# �p�[�X�������͖��s����
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

# �G���[�����������牽������
		if( $@){ $error_count++; }

# ���ʂ�f��
		printParseResult();

# �p�[�X���ʂ���������
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

# �擪�̂����Ă܂��߂�

	my( $token ) = shift(@tl);
	unshift(@tl , $token );

	return "function" if( $token =~ /tcp\b|udp\b/ );
	
	return "variable";

}

sub getArgs
{
	my( $basetype , $siz );

# �R���}�ł�����ĂȂ��Ă���������������R���}����
	push( @tl , "," ) if( $tl[@tl-1] ne "," && $tl[@tl-1] ne "" );
	
	while(1){
# �������Ȃ��Ȃ����琳��I��

		eval{
			( $basetype , $siz ) = getType();
		};
		if( $@ ){
			printerr("cannot parse args" ,"" );
			die;
		}


		last if( @tl == 0 ); # ����������������

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
# �z�񂾂���
		if( !( $nm =~ /^\d+$/ ) ){
			printerr( "size of array has to be digit : $nm" );
			die;
		}


	} else {

# ��{�^������
		unshift( @tl , $right_br );
		unshift( @tl ,$nm );
		unshift( @tl ,$left_br );

		$nm = 0;
	}

	return ( $aliastypes{$bt} , $nm );
	
}

#
#  tcp �Ƃ� udp�Ƃ����g�[�N�������ׂĂ��܂�
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
	@tl = ();					# �g�[�N�����X�g

	$ret_type = "";
	$ret_size = "0";			# 0�͊�{�^�A1�ȏオ�z��

	@arg_type = ();				# �����̌^�ۑ�
	@arg_size = ();				# �����̌^�̃T�C�Y�ۑ�

	$func_name = "";			# �֐��̖��O�ۑ�

	$transport_name = "";		# �g�����X�|�[�g�̖��O�ۑ�

	$variable_name = "";		# �ϐ��ݒ�p�̕ϐ����ۑ�
	$variable_value = "";		# �ϐ��̒l�ۑ�

}
#
# �p�[�X�̌��ʂ𔽉f������
#
sub realizeParseResult
{
# �ϐ��ݒ肾������ϐ��̒l���Z�b�g
	if( $variable_name ne "" ){
		if( $environment{$variable_name} eq "" ){
			#����ȕϐ��͂Ȃ���
			printerr( "no such variable settable : $variable_name" );
			die;
		}
		$environment{$variable_name} = $variable_value;
	} elsif( $func_name ne "" ){

# �֐��錾��������֐��̐ݒ���Z�b�g
		addFunc();
	}
	return 1;
}

#
# �֐�����ǉ�����֐�
# 
sub addFunc
{

	push( @funcnames , $func_name );

# �֐��n�b�V���ɒǉ�
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
# Output   eval �ɂ���ČĂяo�����
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
# perl�̏o�͂�����B
#
sub output_perl
{

	outputPerlUtil();



}
# perl�̗L�p�֐����`���郂�W���[�����o�͂���
sub outputPerlUtil
{

# �t�@�C�����Ђ炢��
	open( PERLU , getFileNameInc( "tsrpcsrc.pm" ) );


# �ϊ����Ȃ���o��
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
# C�̃g�[�N���ɕ�����
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
# TSRPCGEN �ɂƂ��Đ������^���ǂ���
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
# @INC�ɂ���t�@�C���̃t�@�C������Ԃ��B
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
