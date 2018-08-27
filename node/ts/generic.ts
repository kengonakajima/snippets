//

class Prop<T> {
    public id: T;
    constructor(hoge:T) {
        this.id=hoge;
        console.log("Propcons:", typeof(hoge),hoge);
    }
};


var p=new Prop(1);
