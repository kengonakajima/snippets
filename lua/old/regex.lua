function split(str, delim)
    if string.find(str, delim) == nil then
        return { str }
    end

    local result = {}
    local pat = "(.-)" .. delim .. "()"
    local lastPos
    for part, pos in string.gfind(str, pat) do
       
        table.insert(result, part)
        lastPos = pos
    end
    table.insert(result, string.sub(str, lastPos))
    return result
end

for a,b in string.gfind( "aho.hoge", "%." ) do
   print(a,b)
end
