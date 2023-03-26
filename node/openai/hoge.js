const fs=require("fs");
const { Configuration, OpenAIApi } = require("openai");

const wrap = fn => {
  return (req, res, next) => {
    return fn(req, res, next).catch(next);
  }
};





const creds=JSON.parse(fs.readFileSync("/Users/ringo/.openai_cred","utf8"));

const configuration = new Configuration({
  apiKey: creds.secretKey
});
const openai = new OpenAIApi(configuration);

wrap( async () => {
  const res = await openai.createCompletion({
    model: "text-davinci-003",
    //prompt: "犬の名前を三つあげてください。",
    prompt: "銀行員の受付担当者が、利用者の「こんばんは」という呼びかけに対して答えるサンプルを書いて下さい。ただし、サンプルの冒頭には [START]という記号を、おわりには [END]という記号をつける。ちなみに顧客の名前は山田さんです。山田さんはおしゃべりが好きなので、たくさん話しかけてください。200文字以上",
    max_tokens: 2000,
    temperature: 1,
    stream: true
  },{
    timeout: 5000    ,
    responseType: "stream"
  });
  res.data.on("data",(data)=> {
    const text=data.toString();
    if(text.includes("[DONE]")) {
      console.log("DONE!");
      process.exit(0);
    }
    const json=text.replace(/^data: /,"");
    try {
      const obj=JSON.parse(json);
      process.stdout.write(obj.choices[0].text);      
    } catch(e) {
      console.log("error:",e,"on json:",json);
      process.exit(1);
    }
  });
})();
