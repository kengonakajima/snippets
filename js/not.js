var a ;

if( a == null ) {
    console.log( "a is null");
}

var b={};

if( b["aaa"] == null ) {
    console.log( "b['aaa'] is null");
 }
if( b["aaa"] == undefined ) {
    console.log( "b['aaa'] is undef");
 }
if( !b["aaa"] ){
    console.log( "b['aaa'] is not");
 }

var c=0;
if( !c ) {
    console.log( "c is not");
 }
c=false;
if( !c ) {
    console.log( "c is not");    
 }
