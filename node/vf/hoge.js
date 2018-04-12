//virtualfuncs

class Base {
    constructor() {
        this.b=1;
    }
    basefunc() {
        if(this.callback) this.callback(100);
    }
};

class Derived extends Base {
    constructor() {
        super();
        this.x=2;
    }
    callback(arg) {
        console.log("callback_derived:arg:",arg);
    }
}

d=new Derived();

d.basefunc();