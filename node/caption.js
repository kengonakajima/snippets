function caption(obj,...keys) {
  let out=[];
  for(const key of keys) {
    out.push(key+":");
    out.push(obj[key]);
  }
  return out.join(" ");
}

var obj={a:1, b:2, c:3}
console.log(caption(obj,"a","b"));
