require "rubygems"
require "rest-client"


# curl -i 

out = RestClient.get "https://api.github.com/users/kengonakajima/repos"

print out