import  { api } from "node-app-store-connect-api";
import * as fs from 'fs' 

console.log("api:",api);
var b=fs.readFileSync("/tmp/hoge.json");
var o=JSON.parse(b);
var apiKey=o.apiKey;
var issuerId=o.issuerId;
var privateKey=fs.readFileSync("./privkey").toString();
console.log("ppp:",privateKey,issuerId,apiKey);
const { read, readAll, create, update, remove, fetch } = await api({issuerId, apiKey, privateKey});

// log all apps
//const { data: apps } = await readAll('https://api.appstoreconnect.apple.com/v1/apps');
//console.log(apps);



// POST https://api.storekit-sandbox.itunes.apple.com/inApps/v1/notifications/test
const { appStoreVersion } = await fetch('https://api.storekit-sandbox.itunes.apple.com/inApps/v1/notifications/test', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ data: {
  }})
})
      .then(r=> {
        console.log("r:",r);
        return r.json();
      })
      .then(data=>{
        console.log("data:",data);
      });



