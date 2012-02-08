#
# 

set path=( /home1/ringo/java/bin /usr/dt/bin /usr/openwin/bin /bin /usr/bin /usr/contrib/bin /usr/local/bin /usr/local/netscape /opt/gnu/bin /opt/java/bin /usr/ccs/bin /usr/ucb /usr/sbin )

alias cd 'cd \!*; set pwd=`pwd`;set hostname=`hostname`;set prompt="${user}@${hostname}:${pwd} % "'

alias ls ls -CF
set user=`whoami`
alias home cd /home1/ringo
alias ap appletviewer
alias n mule -nw
alias la ls -a
alias l ls -l
alias jc javac
alias j java
alias sd setenv DISPLAY 


set term=vt100

cd


alias siz 'eval `resize`'
