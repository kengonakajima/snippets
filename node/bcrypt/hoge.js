var bc = require("bcrypt");
console.log(new Date);
var salt = bc.genSaltSync(10);
console.log(new Date,salt);

var p = "pwpwpwpwpwpwpwpwppw";
var hash = bc.hashSync(p,salt);
console.log(new Date,hash);

console.log( bc.compareSync(p,hash) );
