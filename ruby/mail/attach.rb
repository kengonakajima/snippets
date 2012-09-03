#! /usr/local/bin/ruby

$sv = "localhost"
$from = "editor@so-ti.com"
$to = "kengo.nakajima@gmail.com"


require "net/smtp"
require "kconv"
require 'digest/sha1'


pngdata = File.open( "../../moai/sol.png","r").read
pdfdata = File.open( "./0x10c.pdf","r").read


begin
  STDERR.print "start smtp...\n"
  smtp = Net::SMTP.start( $sv , 25 )

  boundary = Digest::SHA1.hexdigest(Time.now.to_f.to_s)
  date = `date`.chop

  text  = "Subject: test mail with attachment\n"
  text += "From: #{$from}\n"
  text += "Content-type: multipart/mixed; boundary=" + boundary + "\n"
  text += "Sender: #{$from}\n"
  text += "Date: #{date}\n"
  text += "To: #{$to}\n"
  text += "MIME-Version: 1.0\n"
  text += "\n"
  text += "--" + boundary + "\n"
  text += "Content-Type: text/plain; charset=iso-2022-jp;\n"
  text += "Content-Transfer-Encoding: 7bit\n"
  text += "\n"
  text += Time.now.to_s.tojis + "\n"
  text += "おわったよー。".tojis + "\n"
  text += "\n"
  text += "--" + boundary + "\n"
  text += "Content-Type: application/octet-stream; name=\"sol.png\"\n"
  text += "Content-Disposition: attachment; filename=\"sol.png\"\n"
  text += "Content-Transfer-Encoding: base64\n"
  text += "\n"
  text += [pngdata].pack("m") # base64 encode
  text += "--" + boundary + "\n"
  text += "Content-Type: application/octet-stream; name=\"0x10c.pdf\"\n"
  text += "Content-Disposition: attachment; filename=\"0x10c.pdf\"\n"
  text += "Content-Transfer-Encoding: base64\n"
  text += "\n"
  text += [pdfdata].pack("m")
  text += "--" + boundary + "--\n"


  STDERR.print " send_mail:"
  smtp.send_mail( text, $from, $to )
  smtp.finish
  STDERR.print " finished."
rescue	

  STDERR.print "SEND ERROR : #{$!}\n"
end

