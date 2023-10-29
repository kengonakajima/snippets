function flatten(o) {
  if (typeof o === "object" && o !== null) {
    const keys=Object.keys(o);
    const ary=[];
    for(const key of keys) {
      const v=o[key];
      if(typeof v==="string" ) ary.push(`${key}: '${v}'`);
      else if(typeof v==="object" && v!== null)  ary.push(`${key}: ${JSON.stringify(v)}`);
      else ary.push(`${key}: ${v}`);
    }
    return ary.join(", ");
  } else {
    return o;
  }
}


console.log(flatten("hoge"));
console.log(flatten(100));
console.log(flatten());
console.log(flatten({hoge:1, fuga:2}));
console.log(flatten({hoge:1, fuga: { a:1, b:2}}));

