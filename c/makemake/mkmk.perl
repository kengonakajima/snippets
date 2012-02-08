#! /usr/bin/perl

# MKMK
#
#

$compiler = 'gcc';
$progname = shift(@ARGV);
if( $progname eq '' ){ $progname="a.out";}

#
# get directory name and file names
#
@headerdir = `find . -type d -print`;
chop( @headerdir );
@hfiles = `find . -name \'*.h\' -print`;
chop( @hfiles );
@cfiles = `find . -name \'*.c\' -print`;
chop( @cfiles );
@cppfiles = `find . -name \'*.cpp\' -print`;
chop( @cppfiles );
@allcfiles = ( @cfiles , @cppfiles );
@allfiles = ( @allcfiles, @hfiles );


foreach $tobenkfed ( @allfiles )
{
	doNKF($tobenkfed);
}


$incdir = '';
foreach $d ( @headerdir )
{
	$incdir .= "-I$d ";
}


$src = join( ' ', @allcfiles );


print $files;

$out = <<EOF
#
# created by mkmk.perl
#
	
CC=$compiler
SRC=$src
OBJ=\${SRC:.c=.o}
CFLAGS=-Wall -O -g $incdir
LIBS=
PROG=$progname
RM=/bin/rm
	
\${PROG} : \${OBJ}
	\${CC} \${CFLAGS} -o \${PROG} \${OBJ} \${LIBS}


clean :
	\${RM} -f *.o \${PROG}


EOF
;
	
open( OUT, ">Makefile" ) || die "fuck!\n";

print OUT $out;

close(OUT);


# do make finally

system( "make clean; make" );







# do nkf a file

sub doNKF( $ )
{
	my $fn = shift;

	my $a = `nkf -e $fn`;
	open( OUT , ">$fn" ) || die "cannot open $fn\n";

	print OUT $a;
	
	close(OUT);
	
}
