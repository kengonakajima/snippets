//
var Prop = /** @class */ (function () {
    function Prop(hoge) {
        this.id = hoge;
        console.log("Propcons:", typeof (hoge), hoge);
    }
    return Prop;
}());
;
var p = new Prop(1);
