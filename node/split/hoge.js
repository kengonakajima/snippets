a="asdf\nasdfasdfasdf\nasdfasdfasdfa\nasdfasdfasdf\neeeeee\n";

console.log(JSON.stringify(a.split("\n")));

ary=a.split("\n");
console.log(JSON.stringify(ary.slice(ary.length-3,ary.length)));

