-- utf8 simple divider



function utf8div(s)
  local t={}
  for c in s:gmatch("[\1-\127\192-\255][\128-\191]*") do
    table.insert(t,c)
  end
  return t
end

s = "oink barf 頑張っています 今日はＭＩＴ（マサチューセッツ工科大学）で模擬授業。世界最先端の研究を生み出すメディアラボの石井先生によるメディアラボの様々なプロジェクトの見学ツアー、そしてデジタル技術がもたらす社会インパクトについての講義。　　　ahoaho Q: 20代女性です。私はipodがないと不安なんです。音楽をいつもボリゥムを最大にしてイャホンで聞ける状態でなければ電車に乗ることができません。出掛けに忘れたりする事があってとても不便な思いをします。その場合はたとえ約束があっても取りに戻ります。そうしないと音楽をイャホンで聴いていない状態で外出を長時間続けると背中やこめかみ足の裏掌に汗をかいてきます。不安で顔を上げることができません。なるべく他人に見られないように脇をしめて肩幅を縮めて両肩のラインより後頭部を上に出さない心持ちで早足になっていきます。そうして早くこの状態から逃れるためにipodのところへ向かいます。この時が最も危険で心臓の鼓…"

out=utf8div(s)

for _,v in ipairs(out) do
  print("c:",v)
end

 

