require "net/smtp"

sm = Net::SMTPSession.new( "localhost" , 25 )
sm.start( "titan.co.jp" )
sm.sendmail( "Subject: asdfasdfasdf\r\n\r\nhoge\r\n" , "ahoge" , "ringo@titan.co.jp" )

sm.finish

