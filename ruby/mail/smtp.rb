
require "net/smtp"
require "kconv"

def send(from,to,msg)
  date = `date`.chop

  text  = "Subject: #{msg}\n"
  text += "From: #{from}\n"
  text += "Content-type: text/plain; charset=iso-2022-jp\n"
  text += "Sender: #{from}\n"
  text += "Date: #{date}\n"
  text += "To: #{to}\n"
  text += "\n\n"
  text += "#{msg}\n"
  text += "-----end of message---------------------\n"

  begin
    STDERR.print "start smtp...\n"
    smtp = Net::SMTP.start( "localhost" , 25 )

    STDERR.print "send_mail:"
    smtp.send_mail( text, from, to )
    smtp.finish
    STDERR.print "finished.\n"
    return true
  rescue
    STDERR.print "SEND ERROR : #{$!}\n"
    STDERR.print "mail text:\n"
    STDERR.print text
    return false
  end
end

def main(argv)
  if argv.size != 3 then
    STDERR.print "arg: FROM TO MSG\n"
    return 
  end
  send( argv[0], argv[1], argv[2] )
end


main(ARGV)


