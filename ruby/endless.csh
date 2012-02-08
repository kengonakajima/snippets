#! /bin/csh

setenv LANG C

$gdbscript = <<EOF
handle SIGPIPE nostop noprint
run
where
list
quit
EOF


while 1
    echo rename log
    mv gdb.log gdb.log.`date '+%Y_%m_%d'`
    echo cp sv bin
	cp dddsv dddsv_endless
    echo start gdb
    gdb dddsv_endless  < autogdb.scr >>& gdb.log
#	gprof zonesv1 > log/prof.`date '+%Y_%m_%d'`
#    echo mailing
#    ruby gdbmailer.rb gdb.log
    
    sleep 10
    echo ENDLESS LOOPING
    
end
