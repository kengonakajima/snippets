int main()
{
   int i= execl( "/bin/ls","/", 0);
   extern int errno;
   
   printf( "%d, %s\n",i, strerror(errno));
        
}
