var validator = require("email-validator");
console.log( validator.validate("test@gmail.com") );
console.log( validator.validate("te..st@gmail.com") ); // false
console.log( validator.validate("tes<t@gmail.com") ); // false
console.log( validator.validate("<test@gmail.com>") ); // false
console.log( validator.validate("ほげ@gmail.com") ); // false
