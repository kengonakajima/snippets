#!/usr/local/bin/perl
#
# /lifestorm/serv/lserv
# /lifestorm/serv/general.cf
# のふたつのファイルを
# lsg1 , lsg2 , lsg3に
# FTP
# する。アップする前に
# /lifestorm/serv/lsm shutdown pika100
# を使ってサーバをシャットダウンする。
#
#  以上の処理を自動化するためのスクリプトである。
#
#
#  lsupdate.perl lsg1 lsg2 lsg3のように、引数をひとつでも指定すると、そのホストだけをアップする。
#

use Net::FTP;
use Ctime;


# 以下のフラグを1にすると安全なデバッグモードになる.0だと本チャン
$debug = 0;

# 以下は、アップするファイル。デバッグのためにいまはちがう場所にしてある
@srcfiles = ( "/lifestorm/serv/lserv" , "/lifestorm/serv/general.cf" );

if( $debug ){
	@destfiles = ( "/lifestorm/lserv", "/lifestorm/general.cf" );
}else{
	@destfiles = ( "/lifestorm/serv/lserv", "/lifestorm/serv/general.cf" );
}


# 以下は、対象となるホストと、ユーザと、パスワードと、メイルアドレスの組。セットで変更すること。
# 順番は、lsg1,lsg2,lsg3,lsdevである。
@uphosts = ("lsg1", "lsg2" ,"lsg3");
@upusers = ("root" ,"root" ,"root" );
@uppasswds = ( "se9se9se9" , "se9se9se9" , "se9se9se9" );
@upaccounts =( "root\@titan.co.jp" ,"root\@titan.co.jp" , "root\@titan.co.jp" );


# 以下は、サーバーを落とすために使う管理コマンド。 HOSTを、上のリスト中の lsg1とかとすりかえる。

$lsm_command = "/lifestorm/serv/lsm -S HOST shutdown pika100";

# 以下は、shutdown後の待ち時間
$sleep_sec = 3;

$argc = @ARGV;




for( $i = 0; $i < @uphosts ; $i++ )
{
	$host = @uphosts->[$i];
	$user = @upusers->[$i];
	$pass = @uppasswds->[$i];
	$acc = @upaccounts->[$i];

# 引数が指定されているときは、指定されたホストに対してのみ送信する
	if( $argc >= 1 ){
		$find = 0;
		foreach $arg ( @ARGV ){
			if( $arg eq $host ){
				$find = 1;
				last;
			}
		}
		if( $find == 0 ){		
			next;
		}

	}
	
	if( $debug == 0 ){
		print "\n\nNow shutdown server: ", $host , "\n";

		$comline = $lsm_command;
		$comline =~ s/HOST/$host/;
		print $comline , "\n";

#		system( $comline );
	}

	sleep( $sleep_sec );
	print "Now updating [$i] host:" , $host ," user:" , $user , " account:" , $acc , " begin.\n";

	$ftp = new Net::FTP( $host );
	if( !$ftp->login( $user , $pass , $acc ) ){
		print "cannot log in to the host: " , $host , "\n";
		next;
	}
	
# Do FTP
	for( $j = 0 ; $j < @srcfiles ; $j++ ){
		$src = @srcfiles->[$j];
		$dest = @destfiles->[$j];
		@srclist = split( '/' , $src );					# 
		@destlist = split( '/' , $dest );

		$bak = ctime( time());
		chomp( $bak) ;
		print "ftp: rename $dest to $dest.$bak\n" ;
		if( !$ftp->rename( $dest , "$dest.$bak" )){
#			print "cannot rename!\n";
# renameできなくてもいいのだ
		}

		print "ftp: src:" , $src , " dest:" , $dest , "\n";

		$destf = pop( @destlist );
		$destdir = join( '/' , @destlist );
		
		$srcf = pop( @srclist );
		$srcdir = join( '/' , @srclist );

		print "chdir $srcdir\n";
		chdir( $srcdir );
		
		print "ftp: cwd:" , $destdir ,"\n";
		if( !$ftp->cwd( $destdir )){
			print "cannot cwd!\n";
			next;
		}

		print "ftp: put:" , $destf , "\n";
		if( !$ftp->put( $destf )){
			print "cannot put!\n" ;
			next;
		}
								

	}

	$ftp->quit;


}


