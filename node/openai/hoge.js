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

const p0 = `
オンラインモールシステムであって、前記オンラインモールシステムは、ユーザーによる
メリットおよびメリット募集の登録機能と、前記メリットの承認機能と、前記メリット
の交換機能と、前記交換機能による収益を、ユーザーに分配する分配機能とを有し、前記
メリットは、請求項１に記載のステータスデータに関連可能であることを特徴とし、前記
登録機能は、メリットエディタを備え、前記メリットエディタは、前記登録機能によって
登録されるメリットおよびメリット募集の内容をユーザーが編集可能な項目を有し、前記
編集可能な項目は、ファイル、作者、分類、名前、画像、配分、ＭＭ、登録料、限定、交
換条件、価格、寄付先、寄付率、承認、コメントの項目と、スタート、ＵＲＬの項目と、
情報、広告料の項目と、販促料の項目と、主催、件名、対象、採用の項目を含むことを特
徴とし、前記承認機能は、承認条件を判定する条件判定モジュールを備え、前記交換機能
は、交換条件を判定する条件判定モジュールを備えることを特徴とする、一般的なネット
ワークおよびハードウェア構成上で実施可能である、新規なオンラインモールシステム。
`;


const prompt=p0+"\n 以上の文を100文字以下に要約して下さい。";
const concurrency=16;
let done=0;
function hoge() {
  wrap( async () => {
    
    const res = await openai.createCompletion({
      model: "text-davinci-003",
      //model: "text-curie-001",
      //prompt: "犬の名前を三つあげてください。",
      prompt,
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
        console.log("DONE!",done);
        done++;
        if(done==concurrency) {
          console.log("all done!");
          process.exit(0);
        }
      } else {
        const tks=text.split("\n");
        for(const tk of tks) {
          if(tk.length<2)continue;
          const json=tk.replace(/^data: /,"");
          try {
            const obj=JSON.parse(json);
            process.stdout.write(obj.choices[0].text);      
          } catch(e) {
            console.log("error:",e,"on json:",json);
            process.exit(1);
          }                  
        }
      }
    });
  })();  
}


for(let i=0;i<concurrency;i++) {
  console.log("start:",i);
  hoge();
}
