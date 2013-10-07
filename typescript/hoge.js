var Greeting;
(function (Greeting) {
    var Hello = (function () {
        function Hello(text) {
            this.text = text;
        }
        Hello.prototype.say = function () {
        };
        return Hello;
    })();
    Greeting.Hello = Hello;
})(Greeting || (Greeting = {}));
