#!/usr/local/bin/perl
#
# /lifestorm/serv/lserv
# /lifestorm/serv/general.cf
# $B$N$U$?$D$N%U%!%$%k$r(B
# lsg1 , lsg2 , lsg3$B$K(B
# FTP
# $B$9$k!#%"%C%W$9$kA0$K(B
# /lifestorm/serv/lsm shutdown pika100
# $B$r;H$C$F%5!<%P$r%7%c%C%H%@%&%s$9$k!#(B
#
#  $B0J>e$N=hM}$r<+F02=$9$k$?$a$N%9%/%j%W%H$G$"$k!#(B
#
#
#  lsupdate.perl lsg1 lsg2 lsg3$B$N$h$&$K!"0z?t$r$R$H$D$G$b;XDj$9$k$H!"$=$N%[%9%H$@$1$r%"%C%W$9$k!#(B
#

use Net::FTP;
use Ctime;


# $B0J2<$N%U%i%0$r(B1$B$K$9$k$H0BA4$J%G%P%C%0%b!<%I$K$J$k(B.0$B$@$HK\%A%c%s(B
$debug = 0;

# $B0J2<$O!"%"%C%W$9$k%U%!%$%k!#%G%P%C%0$N$?$a$K$$$^$O$A$,$&>l=j$K$7$F$"$k(B
@srcfiles = ( "/lifestorm/serv/lserv" , "/lifestorm/serv/general.cf" );

if( $debug ){
	@destfiles = ( "/lifestorm/lserv", "/lifestorm/general.cf" );
}else{
	@destfiles = ( "/lifestorm/serv/lserv", "/lifestorm/serv/general.cf" );
}


# $B0J2<$O!"BP>]$H$J$k%[%9%H$H!"%f!<%6$H!"%Q%9%o!<%I$H!"%a%$%k%"%I%l%9$NAH!#%;%C%H$GJQ99$9$k$3$H!#(B
# $B=gHV$O!"(Blsg1,lsg2,lsg3,lsdev$B$G$"$k!#(B
@uphosts = ("lsg1", "lsg2" ,"lsg3");
@upusers = ("root" ,"root" ,"root" );
@uppasswds = ( "se9se9se9" , "se9se9se9" , "se9se9se9" );
@upaccounts =( "root\@titan.co.jp" ,"root\@titan.co.jp" , "root\@titan.co.jp" );


# $B0J2<$O!"%5!<%P!<$rMn$H$9$?$a$K;H$&4IM}%3%^%s%I!#(B HOST$B$r!">e$N%j%9%HCf$N(B lsg1$B$H$+$H$9$j$+$($k!#(B

$lsm_command = "/lifestorm/serv/lsm -S HOST shutdown pika100";

# $B0J2<$O!"(Bshutdown$B8e$NBT$A;~4V(B
$sleep_sec = 3;

$argc = @ARGV;




for( $i = 0; $i < @uphosts ; $i++ )
{
	$host = @uphosts->[$i];
	$user = @upusers->[$i];
	$pass = @uppasswds->[$i];
	$acc = @upaccounts->[$i];

# $B0z?t$,;XDj$5$l$F$$$k$H$-$O!";XDj$5$l$?%[%9%H$KBP$7$F$N$_Aw?.$9$k(B
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
# rename$B$G$-$J$/$F$b$$$$$N$@(B
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


