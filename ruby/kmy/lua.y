class Lua

prechigh
 nonassoc MINUS NOT
 left AND
 left OR
preclow


rule

chunk :   { ep"chk-empty "; push( :chunk ) }
| statlist1 { ep"chk-sl "; sl= pop(:statlist); push(:chunk,sl,nil ) }
| statlist1 laststat { ep"chk-sl-l "; ls=poplaststat(); sl=pop(:statlist); push(:chunk,sl,ls) }
| laststat { ep"chk-l "; ls=poplaststat(); push(:chunk,nil,ls) }
;

statlist1: stat semi { ep"sl1-first "; s=pop(); push(:statlist,s) }
| statlist1 stat semi { ep"sl1-append "; s=pop(); sl=pop(:statlist); sl.push(s); push(*sl) }
;

stat : varlist1 '=' explist1 { ep"st-asign "; el=pop(:explist); vl=pop(:varlist); push( :asign,vl,el) } 
| functioncall  { ep"st-funcall "; push( *pop(:call)) }
| DO block END  { ep"st-do "; b=pop(:block); push(:do,b) }
| WHILE exp DO block END { ep"st-while "; b=pop(:block); e=pop(:exp); push( :while,e,b) }
| REPEAT block UNTIL exp { ep"st-repeat "; e=pop(:exp); b=pop(:block); push( :repeat,e,b) }
| FOR name '=' exp ',' exp DO block END { ep"st-for2 "; b=pop(:block); eend=pop(:exp); estart=pop(:exp); n=pop(:name); push(:for, n,estart,eend,nil,b) }
| FOR name '=' exp ',' exp ',' exp DO block END { ep"st-for3 "; b=pop(:block); estep=pop(:exp); eend=pop(:exp); estart=pop(:exp); n=pop(:name); push(:for,n,estart,eend,estep,b) }
| FOR namelist IN explist1 DO block END { ep"st-forin "; b=pop(:block); el=pop(:explist); nl=pop(:namelist); push(:forin,nl,el,b) }
| FUNCTION funcname funcbody { ep"stat-funcdef "; b=pop(:funcbody);n=pop(:funcname); push(:function,n,b) }
| LOCAL FUNCTION name funcbody { ep"stat-localfunc "; b=pop(:funcbody);n=pop(:name); push(:deflocal, n, b) }
| LOCAL namelist { ep"stat-local-def "; nl=pop(:namelist); push(:deflocal,nl,nil) }
| LOCAL namelist '=' explist1 { ep"stat-local-init "; el=pop(:explist); nl=pop(:namelist); push(:deflocal,nl,el) }
| ifstat { push(*pop(:if)) }
;

ifstat : IF exp THEN block END { ep"if-then-end ";  tru=pop(:block);e=pop(:exp); push(:if,e,tru,nil);  }
| IF exp THEN block ELSE block END { ep"if-then-else-end "; fal=pop(:block); tru=pop(:block); e=pop(:exp); push(:if,e,tru,fal); }
| IF exp THEN block elseifblock {
    ep"if-then-elseif ";
    fal=pop(:block);
    tru=pop(:block);
    e=pop(:exp);
    push(:if,e,tru,fal);
}
;
elseifblock : elseifpart END { ep"elseifblock ";  }
| elseifpart ELSE block END { ep"elseifblock-else "; b=pop(:block); prevbl=pop(:block); previf=findlastelse(prevbl); previf[3]=b; push(*prevbl) }
;
elseifpart : ELSEIF exp THEN block { ep"elseifpart-first "; tru=pop(:block); e=pop(:exp); push(:block,[:if,e,tru,nil]); }
| elseifpart ELSEIF exp THEN block { ep"elseifpart-append "; tru=pop(:block); e=pop(:exp); prevbl=pop(:block); previf=findlastelse(prevbl); previf[3]=[:block,[:if,e,tru,nil]]; push(*prevbl) }  
;


semi :  
| ';' 
;


laststat : RETURN semi { ep"laststat-ret-noarg "; push(:return,nil) }
| RETURN explist1 semi { ep"laststat-ret-exp "; el=pop(:explist);  ep("explsitsize:#{el.size} "); push(:return, el) } 
| BREAK semi { ep"laststat-break "; push(:break) }
;


funcname : name  { ep"funcname-name "; nm=pop(:name); push( :funcname, nm, nil) }
| name ':' NAME { ep"funcname-name-withcolon "; nm=pop(:name); push( :funcname, nm, val[2].to_sym) }
;


function : FUNCTION funcbody { ep"func "; b=pop(:funcbody); push(:function,b) }
;

funcbody : '(' parlist1 ')' block END { ep"fb-prms "; b=pop(:block); pl=pop(:parlist); push( :funcbody, pl,b ) }
| '(' ')' block END { ep"fb-noprms "; push( :funcbody, nil, pop(:block)) }
;

block : chunk { ep"block "; push(:block, pop(:chunk)) }
;

parlist1 :namelist { ep"pl1-nl "; nl=pop(:namelist); push(:parlist,nl) }
| namelist ',' DOTDOTDOT { ep "pl1-nl-vararg "; nl=pop(:namelist); nl.push([:vararg]); push(:parlist, nl) }
| DOTDOTDOT { ep"pl1-vararg "; push(:parlist, [:vararg]) }
;

namelist : name { ep"nl-first "; nm=pop(:name); push(:namelist, nm ) }
| namelist ',' name { ep "nl-append "; nm=pop(:name); nl=pop(:namelist); nl.push(nm); push(*nl) }
;

varlist1 : var { ep"vl-first "; v=pop(:var); push(:varlist,v) }
| varlist1 ',' var { ep"vl-append "; v=pop(:var); vl=pop(:varlist); vl.push(v); push(*vl) }
;

var : name  { ep"var-name "; nm=pop(:name); push( :var, nm ) }
| prefixexp '[' exp ']' { ep"var-pfexp[] "; e=pop(:exp); pe=pop(:prefixexp); push( :var, [:tblget,pe,e]) }
| prefixexp '.' NAME { ep"var-dotname "; pe=pop(:prefixexp); push(:var, [:tblget,pe,[:var,val[0].to_sym]]) }
;

explist1 : exp { ep"el-first "; e=pop(:exp); push(:explist, e) }
| explist1 ',' exp  { ep"el-append "; e=pop(:exp); el=pop(:explist); el.push(e); push(*el)}
;

exp : NIL { ep"exp-nil "; push(:exp,[:nil]) }
| FALSE { ep"exp-false "; push(:exp, [:false]) }
| TRUE { ep"exp-true "; push(:exp, [:true]) }
| number { ep"exp-num "; push( :exp, pop()) }
| STRING { ep"exp-string "; push( :exp, [:str, val[0] ] ) } 
| DOTDOTDOT { ep"exp-vararg "; push( :exp, [:vararg]) }
| function { ep"exp-func "; f=pop(:function); push(:exp,f) }
| prefixexp { ep"exp-pfexp "; push(:exp,pop(:prefixexp)) }
| tableconstructor { ep"exp-tcons "; tc=pop(:tcons); push(:exp,tc) }
| exp binop exp { ep("exp-binop "); e1=pop(:exp); op=pop(:op); e2=pop(:exp); push(:exp, [:binop, e2,op,e1] ) }
| unop exp { ep"exp-unop "; e=pop(:exp); op=pop(:op); push(:exp, [:unop, e, op] ) }
;

number : INTNUMBER { push(:lit, val[0].to_i) } 
| FLOATNUMBER { push( :lit, val[0].to_f ) }
| EXPNUMBER { push( :lit, val[0].to_f ) } 
| HEXNUMBER { push( :lit, val[0].to_i(16)) }
;


prefixexp : var { ep"pfexp-var "; v=pop(:var); push(:prefixexp,v)  } 
| functioncall { ep "pfexp-funcall "; c=pop(:call); push(:prefixexp,c) }
| '(' exp ')' { ep"pfexp-paren-exp "; e=pop(:exp); push(:prefixexp,e) }                                 
;

functioncall : prefixexp args { ep"fcall "; a=pop(:args); pe=pop(:prefixexp); push(:call, pe,nil,a) }
| prefixexp ':' NAME args { ep"fcall-named "; a=pop(:args); pe=pop(:prefixexp); push(:call,pe,[:name,val[2].to_sym],a) }
;

args : '(' explist1 ')' { ep"args "; push(:args, pop(:explist)) }
| '(' ')' { ep"args-empty "; push( :args ) }
| tableconstructor { ep"args-tcons "; tc=pop(:tcons); push(:args,tc) }
| STRING { ep"args-str "; push(:args,[:str, val[0] ] ) }
;

tableconstructor : '{' fieldlist '}' { ep"tcons "; fl=pop(:fieldlist); push(:tcons,fl) }
| '{' fieldlist fieldsep '}' { ep"tcons-lastsep "; fl=pop(:fieldlist); push(:tcons,fl) }
;

fieldlist : { ep"fl-empty "; push(:fieldlist) }
| field  { ep"fl-first "; f=pop(:field); push(:fieldlist, f) }
| fieldlist fieldsep field { ep"fl-append "; f=pop(:field); fl=pop(:fieldlist); fl.push(f); push(*fl) }
;

fieldsep : ',' { ep"fsep-, " }
| ';' { ep"fsep-; " }
;

field :  '[' exp ']' '=' exp  { ep"fld-expset ";  val=pop(:exp); ind=pop(:exp); push(:field, ind, val ) }
| NAME '=' exp { ep"fld-nameset "; e=pop(:exp); push(:field, [:name,val[0].to_sym], e ) }
| exp { ep"fld-exp "; e=pop(:exp); push(:field,nil,e) }
;

name : NAME { ep"name-first=#{val[0]} "; push( :name, val[0].to_sym) }
| name '.' NAME { ep"name-dotappend=#{val[2]} "; nm=pop(:name); nnm=val[2].to_sym; nm.push(nnm); push(*nm) }
;

binop : PLUS { push(:op, :plus) }
| MINUS { push(:op, :minus) }
| MUL { push(:op, :mul ) }
| DIV { push(:op, :div) }
| POWER { push(:op, :power) }
| MOD { push(:op, :mod) }
| APPEND { push(:op, :append) }
| LT { push(:op, :lt) }
| LTE { push(:op, :lte) }
| GT { push(:op, :gt) }
| GTE { push(:op, :gte) }
| EQUAL  { push(:op, :equal) }
| NEQ { push(:op, :neq) }
| AND { push(:op, :and) }
| OR { push(:op, :or) }
;

unop : MINUS { push(:op, :minus ) }
| NOT { push(:op, :not ) }
| LENGTH { push(:op, :length ) }
;



end



---- header = header.rb
---- inner = inner.rb
---- footer = footer.rb
