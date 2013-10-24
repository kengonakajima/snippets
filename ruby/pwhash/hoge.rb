require "securerandom"
require "openssl"
require "base64"

def do_hash(pw,salt)
  return Base64.encode64(OpenSSL::PKCS5::pbkdf2_hmac_sha1( pw, salt, 1000,  24 ))
end

def makeHash(pw)
  salt = SecureRandom.base64( 24 )
  h = do_hash(pw,salt)
  return salt + ":" + h
end

def validateHash(pw,pair)
  s,savedh = pair.split(":")
  h = do_hash(pw,s)
  return ( h == savedh )
end


tosave = makeHash( "foo-bar-baz" )

print "tosave:", tosave, "\n"

should_ok = validateHash( "foo-bar-baz", tosave )
should_ng = validateHash( "fooo-bar-baz", tosave )

print "shouldok:", should_ok, " shouldng:", should_ng, "\n"