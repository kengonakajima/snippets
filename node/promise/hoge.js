function asyncF() {
    return new Promise(function(resolve,reject) {
        setTimeout( function() {
            resolve("async hello");
        },200);
    });
}

asyncF().then( function(value) {
    console.log(value);
}).catch(function(error) {
    console.log(error);
});
