// フィボナッチ数 by JavaScript

var fib = function(n) {
    if (n < 2) {
        return n;
    } else {
        return fib(n-1) + fib(n-2);
    }
}

if (typeof process === "undefined" && typeof scriptArgs !== "undefined") {
    // for Spidermonkey
    print(fib(scriptArgs[0]));
} else if (typeof process !== "undefined") {
    // for node
    console.log(fib(process.argv[2]));
} else {
    // for d8
    print(fib(39));
}

