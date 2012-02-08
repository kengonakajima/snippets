
trap("SIGINT") do
    STDERR.print "hogehoge\n"
end

trap("SIGTERM") do
    STDERR.print "ahohogehoge\n"
end


while TRUE
    sleep 10
end
