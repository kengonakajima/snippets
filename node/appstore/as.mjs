import  { api } from "node-app-store-connect-api";
import * as fs from 'fs' 

console.log("api:",api);
var b=fs.readFileSync("/tmp/hoge.json");
var o=JSON.parse(b);
var apiKey=o.apiKey;
var issuerId=o.issuerId;
var privateKey=fs.readFileSync("./privkey").toString();
console.log("ppp:",privateKey,issuerId,apiKey);
const { read, readAll, create, update, remove } = await api({issuerId, apiKey, privateKey});

// log all apps
const { data: apps } = await readAll('https://api.appstoreconnect.apple.com/v1/apps');1
console.log(apps);

