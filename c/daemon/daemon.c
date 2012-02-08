#include <stdio.h>
#include <signal.h>
#include <sys/param.h>
#include <errno.h>
extern int errno;

#include <sys/file.h>
#include <sys/ioctl.h>

void daemon_start(int ignsigcld );
void err_sys( char * );
int sig_child();

int main( )
{
	int fd;
	daemon_start( 0 );

	fd = open("/dev/console" , O_WRONLY );
	while(1){
		write( fd, "boke" , 4 );
		sleep(1);
	}
	
	
}

void err_sys( char *msg )
{
	;
}

void daemon_start(int ignsigcld )
{
	int childpid, fd;

	signal( SIGTTOU , SIG_IGN );
	signal( SIGTTIN , SIG_IGN );
	signal( SIGTSTP , SIG_IGN );

	if( ( childpid = fork() ) < 0 )
		err_sys(" can't fork first child.");
	else if ( childpid > 0 )
		exit(0);

	if( setpgrp( 0 , getpid() ) == -1 )
		err_sys( "can't change process group." );

	/* $B%3%s%H%m!<%k%?!<%_%J%k$r@Z$j$O$J$9!#(B */
	if( ( fd = open("/dev/tty" , O_RDWR )) >= 0 ){
		ioctl( fd,  TIOCNOTTY , ( char * )NULL );
		close( fd );
	}

out:
	for( fd = 0 ; fd < NOFILE  ; fd ++ )
		close( fd );
	
	errno = 0;	/* EBADF$B$K$J$C$F$k$@$m$&$+$i!#(B */

	chdir("/" );

	umask(0 );
/*
	if( ignsigcld )
		signal( SIGCLD , sig_child );
	*/     
}

