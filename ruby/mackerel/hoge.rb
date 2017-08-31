require "json"

apikeyfile = ARGV[0]
hostid = ARGV[1]

apikey=File.open(ARGV[0]).read.strip

json = `curl --silent -H 'X-Api-Key: #{apikey}' https://mackerel.io/api/v0/hosts/#{hostid}`

h = JSON.parse(json) # pretty_generate

root_used = h["host"]["meta"]["filesystem"]["/dev/root"]["percent_used"]
print root_used, "\n"

