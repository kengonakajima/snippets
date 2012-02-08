Std = function() { }
Std.__name__ = ["Std"];
Std["is"] = function(v,t) {
	return js.Boot.__instanceof(v,t);
}
Std.string = function(s) {
	return js.Boot.__string_rec(s,"");
}
Std["int"] = function(x) {
	return Math.floor(x);
}
Std.bool = function(x) {
	return (x !== 0 && x != null && x !== false);
}
Std.parseInt = function(x) {
	{
		var v = parseInt(x);
		if(Math.isNaN(v)) return null;
		return v;
	}
}
Std.parseFloat = function(x) {
	return parseFloat(x);
}
Std.chr = function(x) {
	return String.fromCharCode(x);
}
Std.ord = function(x) {
	if(x == "") return null;
	else return x.charCodeAt(0);
}
Std.random = function(x) {
	return Math.floor(Math.random() * x);
}
Std.resource = function(name) {
	return js.Boot.__res[name];
}
Std.prototype.__class__ = Std;
IntIter = function(min,max) { if( min === $_ ) return; {
	this.min = min;
	this.max = max;
}}
IntIter.__name__ = ["IntIter"];
IntIter.prototype.hasNext = function() {
	return this.min < this.max;
}
IntIter.prototype.max = null;
IntIter.prototype.min = null;
IntIter.prototype.next = function() {
	return this.min++;
}
IntIter.prototype.__class__ = IntIter;
haxe = {}
haxe.Log = function() { }
haxe.Log.__name__ = ["haxe","Log"];
haxe.Log.trace = function(v,infos) {
	js.Boot.__trace(v,infos);
}
haxe.Log.clear = function() {
	js.Boot.__clear_trace();
}
haxe.Log.prototype.__class__ = haxe.Log;
Test = function() { }
Test.__name__ = ["Test"];
Test.main = function() {
	haxe.Log.trace("Hello World !",{ methodName : "main", className : "Test", lineNumber : 3, fileName : "Test.hx"});
}
Test.prototype.__class__ = Test;
js = {}
js.Lib = function() { }
js.Lib.__name__ = ["js","Lib"];
js.Lib.isIE = null;
js.Lib.isOpera = null;
js.Lib.alert = function(v) {
	alert(js.Boot.__string_rec(v,""));
}
js.Lib.eval = function(code) {
	return eval(code);
}
js.Lib.setErrorHandler = function(f) {
	js.Lib.onerror = f;
}
js.Lib.prototype.__class__ = js.Lib;
js.Boot = function() { }
js.Boot.__name__ = ["js","Boot"];
js.Boot.__unhtml = function(s) {
	return s.split("&").join("&amp;").split("<").join("&lt;").split(">").join("&gt;");
}
js.Boot.__trace = function(v,i) {
	{
		var msg = (i != null?i.fileName + ":" + i.lineNumber + ": ":"");
		msg += js.Boot.__unhtml(js.Boot.__string_rec(v,"")) + "<br/>";
		var d = document.getElementById("haxe:trace");
		if(d == null) alert("No haxe:trace element defined\n" + msg);
		else d.innerHTML += msg;
	}
}
js.Boot.__clear_trace = function() {
	{
		var d = document.getElementById("haxe:trace");
		if(d != null) d.innerHTML = "";
		else null;
	}
}
js.Boot.__closure = function(o,f) {
	{
		var m = o[f];
		if(m == null) return null;
		var f1 = function() {
			return m.apply(o,arguments);
		}
		f1.scope = o;
		f1.method = m;
		return f1;
	}
}
js.Boot.__string_rec = function(o,s) {
	{
		if(o == null) return "null";
		if(s.length >= 5) return "<...>";
		var t = typeof(o);
		if(t == "function" && (o.__name__ != null || o.__ename__ != null)) t = "object";
		switch(t) {
		case "object":{
			if(o instanceof Array) {
				if(o.__enum__ != null) {
					if(o.length == 1) return o[0];
					var str = o[0] + "(";
					s += "\t";
					{
						var _g1 = 1, _g = o.length;
						while(_g1 < _g) {
							var i = _g1;
							++_g1;
							{
								if(i != 1) str += "," + js.Boot.__string_rec(o[i],s);
								else str += js.Boot.__string_rec(o[i],s);
							}
						}
					}
					return str + ")";
				}
				var l = o.length;
				var i;
				var str = "[";
				s += "\t";
				{
					var _g1 = 0, _g = l;
					while(_g1 < _g) {
						var i1 = _g1;
						++_g1;
						str += ((i1 > 0?",":"")) + js.Boot.__string_rec(o[i1],s);
					}
				}
				str += "]";
				return str;
			}
			var tostr;
			try {
				tostr = o.toString;
			}
			catch( $e0 ) {
				{
					var e = $e0;
					{
						return "???";
					}
				}
			}
			if(tostr != null && tostr != Object.toString) {
				var s2 = o.toString();
				if(s2 != "[object Object]") return s2;
			}
			var k;
			var str = "{\n";
			s += "\t";
			var hasp = (o.hasOwnProperty != null);
			for( var k in o ) { ;
			if(hasp && !o.hasOwnProperty(k)) continue;
			if(k == "prototype" || k == "__class__" || k == "__super__" || k == "__interfaces__") continue;
			if(str.length != 2) str += ", \n";
			str += s + k + " : " + js.Boot.__string_rec(o[k],s);
			}
			s = s.substring(1);
			str += "\n" + s + "}";
			return str;
		}break;
		case "function":{
			return "<function>";
		}break;
		case "string":{
			return o;
		}break;
		default:{
			return String(o);
		}break;
		}
	}
}
js.Boot.__interfLoop = function(cc,cl) {
	if(cc == null) return false;
	if(cc == cl) return true;
	var intf = cc.__interfaces__;
	if(intf != null) {
		var _g1 = 0, _g = intf.length;
		while(_g1 < _g) {
			var i = _g1;
			++_g1;
			{
				var i1 = intf[i];
				if(i1 == cl || js.Boot.__interfLoop(i1,cl)) return true;
			}
		}
	}
	return js.Boot.__interfLoop(cc.__super__,cl);
}
js.Boot.__instanceof = function(o,cl) {
	{
		try {
			if(o instanceof cl) {
				if(cl == Array) return (o.__enum__ == null);
				return true;
			}
			if(js.Boot.__interfLoop(o.__class__,cl)) return true;
		}
		catch( $e1 ) {
			{
				var e = $e1;
				{
					if(cl == null) return false;
				}
			}
		}
		switch(cl) {
		case Int:{
			return (Math.ceil(o) === o) && isFinite(o);
		}break;
		case Float:{
			return typeof(o) == "number";
		}break;
		case Bool:{
			return (o === true || o === false);
		}break;
		case String:{
			return typeof(o) == "string";
		}break;
		case Dynamic:{
			return true;
		}break;
		default:{
			if(o != null && o.__enum__ == cl) return true;
			return false;
		}break;
		}
	}
}
js.Boot.__init = function() {
	{
		js.Lib.isIE = (document.all != null && window.opera == null);
		js.Lib.isOpera = (window.opera != null);
		Array.prototype.copy = Array.prototype.slice;
		Array.prototype.insert = function(i,x) {
			this.splice(i,0,x);
		}
		Array.prototype.remove = function(obj) {
			var i = 0;
			var l = this.length;
			while(i < l) {
				if(this[i] == obj) {
					this.splice(i,1);
					return true;
				}
				i++;
			}
			return false;
		}
		Array.prototype.iterator = function() {
			return { next : function() {
				return this.arr[this.cur++];
			}, hasNext : function() {
				return this.cur < this.arr.length;
			}, arr : this, cur : 0}
		}
		String.prototype.__class__ = String;
		String.__name__ = ["String"];
		Array.prototype.__class__ = Array;
		Array.__name__ = ["Array"];
		var cca = String.prototype.charCodeAt;
		String.prototype.charCodeAt = function(i) {
			var x = cca.call(this,i);
			if(isNaN(x)) return null;
			return x;
		}
		var oldsub = String.prototype.substr;
		String.prototype.substr = function(pos,len) {
			if(pos != null && pos != 0 && len != null && len < 0) return "";
			if(len == null) len = this.length;
			if(pos < 0) {
				pos = this.length + pos;
				if(pos < 0) pos = 0;
			}
			else if(len < 0) {
				len = this.length + len - pos;
			}
			return oldsub.apply(this,[pos,len]);
		}
		Int = new Object();
		Dynamic = new Object();
		Float = Number;
		Bool = new Object();
		Bool["true"] = true;
		Bool["false"] = false;
		$closure = js.Boot.__closure;
	}
}
js.Boot.prototype.__class__ = js.Boot;
$Main = function() { }
$Main.__name__ = ["@Main"];
$Main.prototype.__class__ = $Main;
$_ = {}
js.Boot.__res = {}
js.Boot.__init();
{
	Math.NaN = Number["NaN"];
	Math.NEGATIVE_INFINITY = Number["NEGATIVE_INFINITY"];
	Math.POSITIVE_INFINITY = Number["POSITIVE_INFINITY"];
	Math.isFinite = function(i) {
		return isFinite(i);
	}
	Math.isNaN = function(i) {
		return isNaN(i);
	}
}
{
	
			onerror = function(msg,url,line) {
				var f = js.Lib.onerror;
				if( f == null )
					return false;
				return f(msg,[url+":"+line]);
			}
		;
}
js.Lib.document = document;
js.Lib.window = window;
js.Lib.onerror = null;
$Main.init = Test.main();
