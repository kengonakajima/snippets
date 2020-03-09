#  urls = %r{(?:https?|ftp|mailto)://\S+}i
#  text = text.gsub urls, '<a href="\0">\0</a>'

def surround(text, kw, prefix, suffix)
  e = %r[#{kw}]
  return text.gsub( e, prefix + '\0' + suffix)
end

#print ("ほげふがぴよほげふが".gsub e, 'A\0B' )

print surround("ほげふがぴよほげふが", "ほげ", "A", "B")

