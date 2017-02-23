require "bcrypt"

pw1 = BCrypt::Password.create("hogehoge" )

print "pw1:", pw1, "\n"
print "salt:", pw1.salt(), "\n"
#print "ver:", pw1.version(), "\n"
#print "cost:", pw1.cost(), "\n"

pw2 = BCrypt::Password.create("hogehoge" )
print "pw2:", pw2, "\n"
print "pw2 salt:", pw2.salt(), "\n"

print "checkpw1:", pw1 == "hogehoge", "\n"
print "checkpw2:", pw2 == "hogehoge", "\n"

loaded = BCrypt::Password.new(pw1)

print "loaded pw:", loaded, "\n"
print "loaded salt:", loaded.salt(), "\n"
print "loaded cost:", loaded.cost(), "\n"

