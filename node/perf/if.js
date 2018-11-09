var n=10000000;

var tot=0;
var t0=new Date().getTime();
for(var i=0;i<n;i++) {
    if(i==1958) tot+=i;
    else if(i==19884) tot+=i;
    else if(i==198888) tot+=i;
    else if(i==5899991) tot+=i;
    else if(i==8888888) tot+=i;
    else if(i==3333333) tot+=i;
    else if(i==3332333) tot+=i;
    else if(i==1332333) tot+=i;
    else if(i==4332333) tot+=i;
    else if(i==7332333) tot+=i;
    else if(i==6332333) tot+=i;                    
}
var t1=new Date().getTime();
console.log("0:",t1-t0);

