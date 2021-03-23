const ygg=require("yggdrasil")({
    host: 'https://authserver.mojang.com' //Optional custom host. No trailing slash.
});

//Authenticate a user
ygg.auth({
//  token: '', //Optional. Client token.
  agent: 'TestMy', //Agent name. Defaults to 'Minecraft'
  version: 1, //Agent version. Defaults to 1
  user: process.argv[2], //Username
    pass: process.argv[3] //Password
    ,
    requestUser: true
}, function(err, data){

    console.log("YGG: data:",data, "err:",err);
});

//Refresh an accessToken
//ygg.refresh(oldtoken, clienttoken, function(err, newtoken, response body){});

//Validate an accessToken
//ygg.validate(token, function(err){});

//Invalidate all accessTokens
//ygg.signout(username, password, function(err));
