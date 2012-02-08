#!/bin/sh perl


#
#
# struct.perl
#
#
#


$varprefix = "_var";

initCSource();
initHSource();


print printEncoder( 'ai' , 'lpAA' , 'int' ) , "\n";
print printEncoder( 'ai' , 'lpAA' , 'int' , 10,20 ) , "\n";
print printEncoder( 'aa' , 'lpAA' , 'char*' ) , "\n";

finishCSource();
finishHSource();

############################################################

sub finishCSource()
{
	
}
sub finishHSource()
{
}

#
#  基本型と配列のサイズがわかったら、それ用の単体のエンコーダを出力する。
#  struct AA のポインタ lpAAの中にint のaaというメンバがあるとすると、
#  $_[0] : 構造体のメンバ名 'aa'
#  $_[1] : 構造体の変数名 'lpAA'
#  $_[2] : メンバの型名 'int'
sub printEncoder($$$@)
{
	$argc = @_;

	$name = $_[0];
	$parent = $_[1];
	$type = $_[2];

	$args_ofs = 3;

	if( $argc == $args_ofs ){
		if( ( $type eq 'int') || ( $type eq 'short' ) ||
		    ( $type eq 'unsigned int' ) || ( $type eq 'unsigned short' ) ||
		   ( $type eq 'char' ) || ($type eq 'unsigned char' ) ){
			$output = "PRINTFUNC( \"\%s.$name=\%d\\n\" , super , $parent->$name );\n";
		} elsif( ($type eq 'float') || ($type eq 'double' ) ){
			$output = "PRINTFUNC( \"\%s.$name=\%f\\n\" , super , $parent->$name );\n";
		} elsif( ($type eq 'char*') ){
			$output = "PRINTFUNC( \"\%s.$name=\%s\\n\" , super , escapeString($parent->$name) );\n";
		}
	} else {
		$output = "";
		$access = "$parent->$name";
		$access_in_print = "";
		$access_in_print_arg = "";
		for( $i = @_ - $args_ofs -1; $i >= 0 ; $i -- ){
			$output .= "for($varprefix$i=0;$varprefix$i<$_[$i+$args_ofs];$varprefix$i ++)";
			$access .= "[$varprefix$i]";
			$access_in_print .= "[%d]";
			$access_in_print_arg .= "$varprefix$i,";
		}
		$output .= "PRINTFUNC( \"\%s.$name$access_in_print=\%d\\n\" , super , $access_in_print_arg $access );\n";
	}



	return $output;

}


sub initCSource()
{

$c_source = <<EOF

#include <stdio.h>
#include <stdlib.h>

#include "printstruct.h"
#include "lsrpc.h"

#define BUFFERSIZE 16384

char tmpstr[BUFFERSIZE];
char escapework[BUFFERSIZE*2];


LOOPVARDECL

CONTENTS


EOF
;


}

sub initHSource()
{
$h_source =<<EOF

/*
	output by struct.perl
*/

#ifndef _PRINTSTRUCT_H_
#define _PRINTSTRUCT_H_

PROTOTYPES

#endif

EOF
;

}
