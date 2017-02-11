
T.prototype.hoge=1;
function T() {
    this.__proto__.hoge++;
}

new T();
console.log(T.prototype.hoge);
new T();
console.log(T.prototype.hoge);
new T();
console.log(T.prototype.hoge);