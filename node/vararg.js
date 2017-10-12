function hoge() {
    var args=[].slice.call(arguments);
    console.log(args.join(","));
}

function fuga() {
    console.log(JSON.stringify(arguments));
}
    
hoge(1,2);

fuga(1,2,"a");
