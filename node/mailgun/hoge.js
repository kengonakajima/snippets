var api_key=process.argv[2];

var domain="mail.testtube.com";

var mailgun = require("mailgun-js")({apiKey: api_key, domain:domain });

var data = {
    from: "Kengo Naaaaakajima <kengo.nakajima@gmail.com>",
    to: "kengo.nakajima@gmail.com",
    subject: "helllo",
    text: "some hogehogehogeho hogehogehoge."
};


mailgun.messages().send(data,function(err,body) {
    console.log("body got:",body);
});

