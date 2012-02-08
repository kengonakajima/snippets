#! /usr/local/bin/ruby

$sv = "mail.ce-lab.net"
$from = "gdbmailer@ce-lab.net"
$to = "ringo@ce-lab.net"


require "net/smtp"
require "kconv"


if( ARGV.size != 1 ) then
	STDERR.print "need 1 arg(filename)\n"
	exit
end

fn = ARGV[0]

src = `tail -1000 #{fn}`



STDERR.print "date...\n"

date = `date`.chop

begin
	STDERR.print "start smtp...\n"
	smtp = Net::SMTP.start( $sv , 25 )

	text  = "Subject: server is down. Sending GDB log\n"
	text += "From: #{$from}\n"
	text += "Content-type: text/plain; charset=iso-2022-jp\n"
	text += "Sender: #{$from}\n"
	text += "Date: #{date}\n"
	text += "To: #{$to}\n"
	text += "\n\n"
	text += "@@@@@@ SERVER CRASH @@@@@\n\n"
	text += `uname -a`
	text += Kconv.tojis(src)
	text += `date`
	text += `df`
	text += `uptime`
	text += "-----end of message---------------------\n"

	STDERR.print " send_mail:"
	smtp.send_mail( text, $from, $to )
	smtp.finish
	STDERR.print " finished."
rescue	

	STDERR.print "SEND ERROR : #{$!}\n"
end

