#! node
if(process.argv.length<=2) process.exit(1);  

const { Configuration, OpenAIApi } = require("openai");
const configuration = new Configuration({apiKey: process.env.OPENAI_API_KEY});
const openai = new OpenAIApi(configuration);
const wrap = fn => {
  return (req, res, next) => {
    return fn(req, res, next).catch(next);
  }
};
wrap( async ()=>{
  try {
    const completion = await openai.createCompletion({
      model: "text-davinci-003",      
      prompt: process.argv.slice(2).join(" "),
      max_tokens: 300
    });
    console.log(completion.data.choices[0].text);
  } catch (error) {
    if (error.response) {
      console.log(error.response.status);
      console.log(error.response.data);
    } else {
      console.log(error.message);
    }
  }
})();
