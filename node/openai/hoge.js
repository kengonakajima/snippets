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
    prompt: "犬の名前を三つあげてください。",
    //prompt: "単純なローパスフィルタのコードの重要な部分だけを、C#で書いて下さい",
    max_tokens: 300,
    temperature: 0,
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
