-- alphabet to roman
require("lumino")

  
function checkConvertRoman(s)
  local convstr = "a あ i い u う e え o お ka か ki き ku く ke け ko こ"
  local t = split(convstr," ")
  local convtbl = {}
  for i=1,#t,2 do
    insert( convtbl, { t[i] , t[i+1] } )
  end
  convtbl = reverse(convtbl)

  local ss = utf8div(s)
  for i=3,0,-1 do
    local lastchars = slice(ss,#ss-i,#ss)
    local laststr = join(lastchars)
    if isalpha(laststr) then
      for _,cv in ipairs(convtbl) do
        local ro,ka = cv[1], cv[2]
        if ro == laststr then
          return s:sub(1, #s-#laststr ) .. ka
        end
      end
    end
  end
  return s
end

print("conv1:", checkConvertRoman( "あいうeo" ) )
print("conv2:", checkConvertRoman( "あいうka" ) )
print("conv3:", checkConvertRoman( "あいうk" ) )
print("conv4:", checkConvertRoman( "あいうkia" ) )
