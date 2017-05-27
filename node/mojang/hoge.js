var MojangAPI = require('mojang-api');
var date = new Date();
date.setMonth(0); // 0 = January
MojangAPI.uuidAt('jeb_', date, function(err, res) {
    if (err) {
        console.log(err);
    } else {
        console.log("On " + date + ", jeb_'s UUID was " + res.id);
        MojangAPI.profile(res.id, function(err, res) {
            if (err)
                console.log(err);
            else {
                console.log(res);
            }
        });
    }
});
