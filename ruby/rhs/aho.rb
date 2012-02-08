
plain = "bar"

a = plain.crypt("ab")

print a , "\n"

print "bar".crypt(a) , "\n"

C = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

salt = "" << C[rand 64] << C[rand 64]

print salt , "\n"
