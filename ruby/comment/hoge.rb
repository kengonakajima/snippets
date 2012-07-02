#
# uncomment engine
#


a = <<EOF
/* asdfas
dfa
 /* 
sdfa // asdkfasdlfajsdf
sdfasdf asd
fasdfasdf
 */

// asd fa sdf as df asd f asdf

-- lua comment 

CODE / CODE1    -- lua comment
CODE * CODE2    # shell comment
CODE [ CODE3 ]    --[[ lua multiline comment ]]
CODE - CODE4    --[[ lua multiline comment 
                  line 2
             --]]


CODE CODE5    // C comment
CODE CODE6    /* C comment 2 */

CODE CODE7   /* multi line C comment 
                line 2
                line 3 */

CODE CODE8  ;; emacs lisp 1
;; emacs lisp 2

EOF


def removeComment(s,ext)
  s = s.dup
  out=[]
  until s.empty?
    case s
    when /\A\#.*$/  # shell comment 1line
      out.push($&) if ext != "lua"
    when /\A\/\/.*$/   # js comment oneline
    when /\A;;.*$/   # elisp comment oneline
    when /\A\/\*.*?\*\//m # js comment multiline
    when /\A--\[\[.*?\]\]/m  # lua comment multiline
      out.push($&) if ext != "lua"
    when /\A--.*$/  # lua comment singleline
      out.push($&) if ext != "lua"
    when /\A.?/m
      out.push($&)
    else
      raise "invalid. s:#{s}"
    end
    s = $'
  end
  return out.join("")
end

print "LUA:", removeComment(a, "lua" )

print "OTHER:", removeComment(a, nil )
