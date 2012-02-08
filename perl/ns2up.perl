#!/usr/local/bin/perl
#
# murasakiにあるファイルを ns2にもっていくためのツールである。cronで動かすのだ。
#
# srcのトップディレクトリに、もっていったファイルの記録をアペンドするぞ
#
#
#

use Net::FTP;
use Ctime;



$filecount = 0;

$| = 1;							# non blocking

$desttop = shift( @ARGV );

if( @ARGV < 1 ){
	die("usage: ns2up.perl /home1/someone/fromlan /home/someone/toout /home/someone/hoge ...");
}

if( !($desttop =~ /^\// )){
	die( "destination name has to start with '/' : '$desttop'\n" );

} else {
	print "destination top is ns2 : '$desttop'\n" ;
	
# setup FTP module

	$ftp = new Net::FTP( "ns2" );	# ns2にログイン
	$ftp->login( "root" , "hotaruho" , "root\@titan.co.jp" );

	if( !$ftp->mkdir( $desttop ) ){

		print "mkdir $desttop error or already exsists\n";
	}else {
		print "mkdir $desttop successful\n";
	}

	if( !$ftp->cwd( $desttop )){
		print "cwd $desttop error\n";
	} else {
		print "cwd $desttop successful\n";
	}
	print "FTP setup ok.\n";
}





foreach $dn ( @ARGV )
{ 
	if( !(-d $dn) ){ 
		print " '$dn' is not a directory.\n";
	} else {
		if( !( $dn =~ /^\// ) ){
			print "'$dn' is not specified from /.\n" ;
		} else {
			$appendlog = ">>$dn/transfer.log";
			open( LOG , $appendlog );
			print LOG "transferred " , ctime(time());
			close( LOG );

#			doIt( $dn);
			tarDoIt( $dn );
		}
	}

}

print "copied " , $filecount , " files\n";

$ftp->quit;

die;

################END
# tar.gzバージョン
sub tarDoIt
{
	my $dirname = shift;


	
	my @l = split( '/' , $dirname );
	my $itsname = pop(@l);

	print "dirname:$dirname itsname:$itsname desttop:$desttop destline:$destline srcline:$srcline\n";

# tarする
	my $cmd = "/usr/local/bin/tar zcf $dirname/$itsname.tar.gz $dirname" ;
	print $cmd , "\n";
	system( $cmd );

	if( !$ftp->put( "$dirname/$itsname.tar.gz" ,"$desttop/$itsname.tar.gz" ) ){
		print "put $destline error\n";
	} else {
		print "put $srcline $destline successful\n";
	}
	$filecount++;
}

# 生ファイルバージョン
sub doIt
{
	my $dir = shift;

	my $aho = time();
	my $tmpfile = "/tmp/ns2up$aho";

	system( "find $dir -print  \> $tmpfile" ); 

	open( IN , $tmpfile );

	while( $srcline = <IN> ){
		chomp( $srcline );
		$destline = "$desttop/$srcline";
		$destline =~ s/\/\//\//;

		if( -d( $srcline ) ){ 
			# ソースがディレクトリだったからdestでもmkdir
			if( !$ftp->mkdir( $destline , 1) ){
				print "mkdir $destline error\n";
			} elsif( !$ftp->cwd( $destline ) ){
				print "cwd $destline error\n";
			} else {
				print "mkdir/cwd $destline sccessful\n";
			}
		} else {
			# ソースがファイルだったからbi put
			if( !$ftp->put( $srcline , $destline ) ){
				print "put $destline error\n";
			} else {
				print "put $srcline $destline successful\n";
			}

			$filecount++;
		}
	}
	close(IN);

	system( "rm $tmpfile" );

}





