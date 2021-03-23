dic=[]
File.open("dic.txt").read.split("\n").each do|line|
  line.strip!
  pair=line.split(" ")
  dic.push(pair)
end

dic.each do |pair|
  print pair,"\n"
end


def need_convert?(t)
  return -10 if /[[:upper:]]/.match(t)
  boin=["a","i","u","e","o"]
  shiin=["b","c","d","f","g","h","j","k","l","m","n","p","q","r","s","t","v","w","x","y","z"]
  n_space=0
  n_letter=0
  n_boin=0
  shiin_cont=0
  n_word_max=0

  prev_n=false
  prev_shiin=false
  prev_ch=nil
  chars=t.split("")  
  return -1 if shiin.include?(chars[-1])
  chars.each do |ch|
    if ch==" " then n_space+=1 else n_letter+=1 end
    return -2 if ch==" " and prev_shiin==true 
    if shiin.include?(ch) then
      shiin_cont+=1 if prev_ch!="n"
      prev_shiin=true
    else
      shiin_cont=0
      prev_shiin=false
    end
    prev_ch=ch
    n_boin+=1 if boin.include?(ch)
    if ch==" " then
      n_word_max=0
    else
      n_word_max+=1
    end    
    return -3 if shiin_cont>=3
  end
  n_letter*=1.0
  n_space*=1.0
  n_boin*=1.0
  boin_rate = n_boin/n_letter
  space_rate = n_space/n_letter
  
#  print "    n_letter:#{n_letter} n_space=#{n_space} n_boin=#{n_boin} boin_rate=#{boin_rate} space_rate=#{space_rate}\n"
  
  return 1 if n_word_max>15
  return -4 if n_space>6
  return 2 if boin_rate>0.4
  return 3 if space_rate>0.15  
  return -5  
end

def convert(t,dic)
  need=need_convert?(t)
  return t if need<0

  100.times do |i|
    dic.each do |pair|
      r,k=pair
      nx=t.sub(r,k)
      if nx!=t then
#        print "TTT:#{nx}\n"
        t=nx
        break
      end
    end    
  end
  return t
end

texts = [
  "This version of ruby is included in macOS for compatibility with legacy software.",
  "In future versions of macOS the ruby runtime will not be available by",
  "default, and may require you to install an additional package.",
  "good morning",
  "ohayo",
  "hello",
  "konichiwa",
  "atodemouikkaikimasu",
  "sonna bokuhananimositenainoni-",
  "haro-wa-rudo",
  "akayorimidorinohougasuki",
  "sonnnakotoyorioudonntabetai",
  "hello world",
  "bye",
  "kekkou nihongo demo space wo ireru yonee",
  "soudemonai kana?",
  "wa-i",
  "wa---i",
  "noooooo",
  "madamada soto akarui",
  "kotti kite",
  "kill you",
  "Capital cause English",
  "remember there's a lot more letters than a through z!"
  
]
#print dic,"\n"

texts.each do |t|
  print convert(t,dic),"\n"
end

  
  
  
