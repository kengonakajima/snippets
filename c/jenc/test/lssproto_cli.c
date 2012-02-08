/* output by lsgen.perl 0.41 ( 1998 May)
 * made Sun Nov 15 21:12:56 1998
 * user ringo
 * host jap
 * file /home/ringo/test/jenc/output/lssproto_cli.c
 * util output/lssproto_util.c , output/lssproto_util.h
 * src  /home/ringo/test/jenc//home/ringo/ls2/ls2/doc/lssproto.html
 */
#define _LSSPROTOCLI_C_
#include "lssproto_cli.h"

/*
71 :   <LI><a name="CS_W"><font color=red>clienttoserver W( string direction , int id);<br></font></a>
72 :       (Walk)歩く。向きをかえる。クライアントは適切なタイミングでこのコマンドを
73 :       送信(無意味な連打を抑制)する。このコマンドは何歩分かをまとめて
74 :       送信するが、その通りに歩かせる処理はサーバーがミリ秒単位で管理する。
75 :       基本的に0.4秒で1歩あるける。歩いた結果新しい部分が見え
76 :       たりする場合は、M(Map)関数をつかってサーバーがマップ情報を送信
77 :       してくる。それからちょっとでもステータスが変化するとS(Status)コマンドをつかって
78 : 	  新しい状態を送信してくる。さらにNPCなどが反応するとT(Text)コマンドで
79 : 	  テキストがくる。
80 : 	またさきよみを実現するのとレスポンスをよくするために、1歩ごとのコマンドに
81 : 	TCPのようにIDをふる。ゲームサーバーは1歩あるくごとにWコマンドでクライアント
82 : 	に対して送信しなければならない。そのときに使うIDはクライアントが送ってきたIDをそのまま
83 : 	コピーする。
84 : 
85 : 
86 : 	
87 :       <br><br>
88 :       <dl>
89 : 		<dt>string direction
90 : 		<dd>あるく方向 a,b,c,d,e,f,g,hが場所が変化する移動で、
91 : 		    aが真上(dx,dy)=(0,-1)で、みぎまわりに hが左上(-1,-1)
92 : 		    A,B,C,D,E,F,G,H がその場回転。向きはそれぞれ真上からみぎまわり。
93 : 		    右右上下とあるく場合は
94 : 		    "ccae"という文字列を送信し、その場で左に向くばあい は
95 : 		    "G"と送信することになる。かならず1歩1文字に対応している。
96 : 		    この文字列はエスケープしない。
97 : 		<dt>int id
98 : 		<dd>1歩目のID.すべての歩くコマンドにはIDが振られるが、
99 : 			まとめて送る性質上、最初のものについているIDだけ送信すれば
100 : 			十分で、その他のコマンド(aからh,AからH)はそれを1づつインクリメント
101 : 			したものになる。
102 : 
103 : 	  </dl>
104 : 	  <br>
105 :       <br>
106 : 
*/

void lssproto_W_send( int fd,char* direction,int id )
{
	lssproto_CreateHeader( lssproto.work , "W" );
	strncat( lssproto.work , lssproto_mkstr_string( direction ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( id ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
138 :   <LI><a name="AD"><font color=red>clienttoserver AD( int dir );<Br></font></a>
139 :       (AttackDir)装備しているアイテムでもって指定した方向に攻撃する。
140 : 	  <br><br>
141 :       <dl>
142 : 	<dt>int dir
143 : 	<dd>攻撃する方向
144 :       </dl>
145 : 
146 :       
*/

void lssproto_AD_send( int fd,int dir )
{
	lssproto_CreateHeader( lssproto.work , "AD" );
	strncat( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
147 :   <LI><a name="MGD"><font color=red>clienttoserver MGD( int dir , string  pattern );<br></font></a>
148 :       (MagicDir)
149 :       指定した魔法を指定した方向に唱える。
150 :       <br><br>
151 :       <dl>
152 : 	<dt>int dir
153 : 	<dd>魔法を使う方向
154 : 	<dt>string pattern
155 : 	<dd>魔法のパターン。
156 : 	    魔法のパターンは魔法のジェムの名前を英語でならべたものになる。
157 : 	    たとえば "blaze air air inteli"というようなもの。
158 : 	    この文字列はエスケープしない。
159 :       </dl>
160 : 
*/

void lssproto_MGD_send( int fd,int dir,char* pattern )
{
	lssproto_CreateHeader( lssproto.work , "MGD" );
	strncat( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_string( pattern ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
161 :   <LI><a name="SKD"><font color=red>clienttoserver SKD( int dir , int index );<br></font></a>
162 :       (SKillDir)
163 :       指定した方向にスキルをつかう。
164 :       <br><br>
165 :       <dl>
166 : 	<dt>int dir
167 : 	<dd>スキルを使う方向
168 : 	<dt>int index
169 : 	<dd>自分の持っているスキルの表へのindex.
170 :       </dl>
171 :       スキル表は以下の通りである。これはサーバのソースの一部分である。
172 :       <pre>
173 : 
174 :       ソースファイルではコメント
175 :       rpcgen Cのコメントそのままするから。コンパイル通らんくなる。
176 :       コメントスキル一覧enum
177 :       typedef enum
178 :       {
179 :           SKILL_FIRE,                 ほのおを吐く    
180 :           SKILL_MAGICIAN,             魔法使い    
181 :           SKILL_ALOTOFTHINGS,         ものもち    
182 :           SKILL_ALOTOFHITS,           命中率UP    
183 :           SKILL_AVOIDATTACK,          みかわしのじゅつ    
184 :           SKILL_DETERMINEITEM,        アイテム識別    
185 :           SKILL_DETERMINOTHERS,       千里眼    
186 :           SKILL_FIRECUT,              炎削減  
187 :           SKILL_THUNDERCUT,           稲妻削減  
188 :           SKILL_ICECUT,               氷削減  
189 :           SKILL_MERCHANT,             商人    
190 :           SKILL_PRIEST,               僧侶    
191 :           SKILL_THIEF,                盗賊    
192 :           SKILL_FIST,                 素手装備  
193 :           SKILL_SWORD,                剣装備  
194 :           SKILL_AXE,                  斧装備  
195 :           SKILL_SPEAR,                槍装備  
196 :           SKILL_BOW,                  弓装備  
197 :           SKILL_CANE,                 杖装備  
198 :           SKILL_LARGEVOICE,           大きな声    
199 :           SKILL_STINKINGSMELL,        臭い匂い  
200 :           SKILL_GOODSMELL,            良い匂い  
201 :           SKILL_UNDEAD,               アンデッド    
202 :           SKILL_NUM
203 :       }SKILL_ID;
204 :       </pre>
205 : 
206 :       
*/

void lssproto_SKD_send( int fd,int dir,int index )
{
	lssproto_CreateHeader( lssproto.work , "SKD" );
	strncat( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( index ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
207 :   <LI><a name="CM"><font color=red>clienttoserver CM( string data );<br></font></a>
208 :       (ChatMagic)
209 :       ことだま魔法を使う。ことだま魔法は、位置の指定をすることはない。<br><br>
210 :       <dl>
211 : 	<dt>string data
212 : 	<dd>ことだまの内容。ことだま魔法は、ことだまをカギカッコで括る
213 : 	    ことによって実現するので、その中だけを取りだしてサーバーにEUCで送
214 : 	    信する。この文字列はデリミタをふくまないので、エスケープする必要はない。
215 :       </dl>
216 :       ことだま魔法の結果はTxt関数を使ってテキストでかえってくる。
217 :       <br>
218 :       <br>
219 : 
220 : 
*/

void lssproto_CM_send( int fd,char* data )
{
	lssproto_CreateHeader( lssproto.work , "CM" );
	strncat( lssproto.work , lssproto_mkstr_string( data ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
221 :   <LI><a name="ID"><font color=red>clienttoserver ID( int dir, int itemindex );<br></font></a>
222 :       (useItemDir)
223 : 	  アイテムを、方向を指定して使う。
224 : 
225 :       <dl>
226 : 	<dt>int dir
227 : 	<dd>アイテムを使う方向
228 : 	<dt>int itemindex
229 : 	<dd>自分が持っているアイテム表へのindex
230 :       </dl>
231 :       アイテムを使った結果は、Txt関数を使って文字列で返ってくる。アイテムの変
232 :       更は、ItC関数で通知される。
233 :       <br>
234 :       <br>
235 : 
236 :       
237 : 
238 :       
*/

void lssproto_ID_send( int fd,int dir,int itemindex )
{
	lssproto_CreateHeader( lssproto.work , "ID" );
	strncat( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( itemindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
239 :   <LI><a name="ST"><font color=red>clienttoserver ST( int titleindex );<br></font></a>
240 :       (SelectTitle)
241 :       称号をえらぶ。titleindex が -1 のときは称号をはずすという事にな
242 :       る。<br><br>
243 :       <dl>
244 : 	<dt>int titleindex
245 : 	<dd>何番目の称号か。
246 :       </dl>
247 :       結果は、Txt関数をつかって、「～のしょうごうをえらんだ！」のよう
248 :       に通知される。
249 :       <br>
250 :       <br>
251 : 
252 :       
*/

void lssproto_ST_send( int fd,int titleindex )
{
	lssproto_CreateHeader( lssproto.work , "ST" );
	strncat( lssproto.work , lssproto_mkstr_int( titleindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
253 :   <LI><a name="DT"><font color=red>clienttoserver DT( int titleindex );<br></font></a>
254 :       (DeleteTitle)
255 :       称号を削除する。<br><br>
256 :       <dl>
257 : 	<dt>int titleindex
258 : 	<dd>削除する称号のインデクス。
259 :       </dl>
260 :       結果は、Txt関数をつかって、「～のしょうごうをさくじょした！」のように通
261 :       知される。
262 :       <br>
263 :       <br>
264 : 
265 : 
266 :       
267 : 
268 :       
*/

void lssproto_DT_send( int fd,int titleindex )
{
	lssproto_CreateHeader( lssproto.work , "DT" );
	strncat( lssproto.work , lssproto_mkstr_int( titleindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
269 :   <LI><a name="FT"><font color=red>clienttoserver FT( string data );<br></font></a>
270 :       (inputFreeTitle)
271 :       自分で入力する称号を入力した。<br><br>
272 :       <dl>
273 : 	<dt>string data
274 : 	<dd>自分で入力した称号。EUCでサーバーに送信される。この称号は、
275 : 	    マウスカーソルをあわせたときに画面下の1行infoに表示され
276 : 	    るものである。この文字列はデリミタをふくまないので、
277 : 	    エスケープする必要はない。
278 :       </dl>
279 :       この結果は、Txt関数を使って、文字列で通知される。これにともない、
280 :       自分のCHARINDEXに対して、Chデータがふたたび送信されてくることに
281 :       なる。
282 :       <br>
283 :       <br>
284 : 
285 :       
*/

void lssproto_FT_send( int fd,char* data )
{
	lssproto_CreateHeader( lssproto.work , "FT" );
	strncat( lssproto.work , lssproto_mkstr_string( data ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
286 :   <LI><a name="PI"><font color=red>clienttoserver PI( int dir );<br></font></a>
287 :       (PickupItem)
288 :       アイテムを拾う。拾うのは足元をふくむ隣接する9マスである。<br><br>
289 :       <dl>
290 : 	<dt>int dir
291 : 	<dd>拾う方向
292 :       </dl>
293 :       結果はTxt関数をつかって「アイテムをひろった！」のように
294 :       サーバーから送信されてくる。変化したアイテムの状況は、
295 :       ItC関数によって通知される。
296 :       <br>
297 :       <br>
298 : 
299 :       
*/

void lssproto_PI_send( int fd,int dir )
{
	lssproto_CreateHeader( lssproto.work , "PI" );
	strncat( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
300 :   <LI><a name="DI"><font color=red>clienttoserver DI( int itemindex);<br></font></a>
301 :       (DropItem)
302 :       アイテムを置く。置くのはドロップボタンに置くので、方向や位置の
303 :       指定はない。itemindexのアイテムを置く。いつでも自分の前に置く。<br><br>
304 : 
305 :       <dl>
306 : 	<dt>int itemindex
307 : 	<dd>置きたいアイテムのインデクス。
308 :       </dl>
309 :       結果はTxt関数をつかって「アイテムをおいた！」のように
310 :       サーバーから送信されてくる。変化したアイテムの状況は、
311 :       ItC関数によって通知される。
312 :       <br>
313 :       <br>
314 : 
315 :       
*/

void lssproto_DI_send( int fd,int itemindex )
{
	lssproto_CreateHeader( lssproto.work , "DI" );
	strncat( lssproto.work , lssproto_mkstr_int( itemindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
316 :   <LI><a name="DG"><font color=red>clienttoserver DG( int amount );<br></font></a>
317 :       (DropGold)
318 :       お金を置く。<br><br>
319 :       <dl>
320 : 	<dt>int amount
321 : 	<dd>置くお金の量。
322 :       </dl>
323 :       結果はTxt関数をつかって「お金をおいた！」のように
324 :       サーバーから送信されてくる。変化したアイテムの状況は、
325 :       ItC関数によって通知される。
326 :       <br>
327 :       <br>
328 : 
329 : 
*/

void lssproto_DG_send( int fd,int amount )
{
	lssproto_CreateHeader( lssproto.work , "DG" );
	strncat( lssproto.work , lssproto_mkstr_int( amount ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
330 :   <LI><a name="MI"><font color=red>clienttoserver MI( int fromindex , int toindex);<br></font></a>
331 :       (MoveItem)
332 :       アイテムを移動/装備/装備をはずす。装備になるかどうかは
333 :       移動の場所で決まるので、サーバーは適宜メッセージをかえる。<br><br>
334 :       <dl>
335 : 	<dt>int fromindex
336 : 	<dd>移動するアイテムのインデクス。
337 : 	<dt>int toindex
338 : 	<dd>目的地のアイテムインデクス。
339 :       </dl>
340 :       目的地とは以下の通り。
341 :       <ul>
342 : 	<li>0    攻撃アイテムを装備する所(手)
343 : 	<li>1    盾を装備する所( 手 ) 
344 : 	<li>2    兜を装備する所( 頭 )
345 : 	<li>3    鎧を装備する所( 体 )
346 : 	<li>4    靴を装備する所( 足 )
347 : 	<li>5    指輪を装備する所( 指 )
348 : 	<li>6    アクセサリを装備する所( 首 )
349 : 	<li>それ以上。   アイテム欄
350 :       </ul>
351 :       結果はTxt関数をつかって「よろいを装備した！」のように
352 :       サーバーから送信されてくる。変化したアイテムの状況は、
353 :       ItC関数によって通知される。
354 :       <br>
355 :       <br>
356 : 
*/

void lssproto_MI_send( int fd,int fromindex,int toindex )
{
	lssproto_CreateHeader( lssproto.work , "MI" );
	strncat( lssproto.work , lssproto_mkstr_int( fromindex ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( toindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
357 :   <LI><a name="SKUP"><font color=red>clienttoserver SKUP( int skillindex );<br></font></a>
358 :       (SKillUP)
359 :       スキルアップボタンをおした。<br><br>
360 :       <dl>
361 : 	<dt>int skillindex
362 : 	<dd>どのスキルをあげるか。
363 :       </dl>
364 :       結果は、Txt関数をつかって、「ほげほげのスキルが1あがった！」
365 :       のように通知する。
366 :       <br>
367 :       <br>
368 : 
369 :       
*/

void lssproto_SKUP_send( int fd,int skillindex )
{
	lssproto_CreateHeader( lssproto.work , "SKUP" );
	strncat( lssproto.work , lssproto_mkstr_int( skillindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
370 :   <LI><a name="MSG"><font color=red>clienttoserver MSG( int index , string  message , int color );<br></font></a>
371 :       (MeSsaGe)
372 :       アドレスブックの相手にメッセージを送信。indexは自分のアドレスブッ
373 :       クのインデクスである。<br><br>
374 :       <dl>
375 : 	<dt>int index
376 : 	<dd>アドレスブックにいる相手のインデクス。
377 : 	<dt>string message
378 : 	<dd>相手に送信するメッセージ。EUCでサーバーに送信する。
379 : 	    この文字列はデリミタをふくまないので、エスケープする必要はない。
380 : 	<dt>int color
381 : 	<dd>メッセージの色。
382 :       </dl>
383 :       <br>
384 :       <br>
385 : 
386 : 
*/

void lssproto_MSG_send( int fd,int index,char* message,int color )
{
	lssproto_CreateHeader( lssproto.work , "MSG" );
	strncat( lssproto.work , lssproto_mkstr_int( index ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_string( message ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( color ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
387 :   <LI><a name="CS_AB"><font color=red>clienttoserver AB( void );</font></a><br>
388 :       (AddressBook)
389 :       アドレスブックの内容をサーバーに問いあわせる。
390 :       引数はない。サーバーはこのコマンドをうけとるとアドレスブックの全エントリの
391 :       内容を返す。
392 : 
*/

void lssproto_AB_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "AB" );
	strncat( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
411 :   <LI><a name="DAB"><font color=red>clienttoserver DAB( int index );</font></a><br>
412 :       (DeleteAddressBookitem)アドレスブックの項目を削除する。
413 :       このコマンドによってアドレスブックの内容が変化するので、サーバーは
414 :       ABコマンドを送信して、クライアントに表示されているアドレスブックの内容を更新する
415 :       必要がある。
416 :       <dl>
417 : 	<dt>int index
418 : 	<dd>消したい項目のindex.最初の項目が0である。
419 :       </dl>
420 :       サーバーがこのコマンドのために特に返答をすることはない。
421 : 
*/

void lssproto_DAB_send( int fd,int index )
{
	lssproto_CreateHeader( lssproto.work , "DAB" );
	strncat( lssproto.work , lssproto_mkstr_int( index ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
422 :   <LI><a name="AAB"><font color=red>clienttoserver AAB( void );</font></a><br>
423 :       (AddAddressBookitem)
424 :       目の前にいるキャラクターをアドレスブックに加える。
425 :       成功するとアドレスブックの内容が変化するので、
426 :       サーバーはABコマンドを送信してクライアントの表示を更新する。
427 : 
*/

void lssproto_AAB_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "AAB" );
	strncat( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
441 :   <LI><a name="RAB"><font color=red>clienttoserver RAB( int index );<br></font></a>
442 :       (ReceiveAddressBookmessage)相手からのメッセージを受信する要求。
443 :       アドレスブックの相手が自分にたいしてメッセージを送信した場合、
444 :       キューにたまっていく。そのキューにひとつ以上メッセージがある場合は、
445 :       NABコマンドでクライアントに通知される。クライアントで、そのフラグが
446 :       たっている項目の受信ボタンをクリックすると、このRABが送信される。
447 :       サーバーはindexで指定される項目のキューにメッセージがたまっている場合、
448 :       その相手からのすべてのメッセージを送信する。
449 :       <dl>
450 : 	<dt>int index
451 : 	<dd>項目を指すインデックス。
452 :       </dl>
453 :       サーバーは、メッセージを、Tコマンドで送信する。
454 :       
*/

void lssproto_RAB_send( int fd,int index )
{
	lssproto_CreateHeader( lssproto.work , "RAB" );
	strncat( lssproto.work , lssproto_mkstr_int( index ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
455 :   <LI><a name="TK"><font color=red>clienttoserver TK( string message ,int color);<br></font></a>
456 :       (TalK)
457 :       チャット用メッセージを送信する。返答はない。<br><br>
458 :       <dl>
459 : 	<dt>string message
460 : 	<dd>送信する文字列。称号とかはふくめない。サーバーがふくめるため。
461 : 	    入力をEUCに変換して送信。
462 : 	    この文字列はデリミタをふくまないので、エスケープする必要はない。
463 : 	<dt>int color
464 : 	<dd>文字列の色。色コードは未定義。
465 :       </dl>
466 :       <br>
467 :       <br>
468 : 
469 :       
*/

void lssproto_TK_send( int fd,char* message,int color )
{
	lssproto_CreateHeader( lssproto.work , "TK" );
	strncat( lssproto.work , lssproto_mkstr_string( message ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( color ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
491 :   <LI><a name="CS_M"><font color=red>clienttoserver M(int fl , int x1 , int y1 , int x2, int y2 );<br></font></a>
492 : 	  マップを要求する。このコマンドの返答はMコマンドでかえってくる。
493 : 	  <dl>
494 : 		<dt>int fl
495 : 		<dd>フロア番号
496 : 		<dt>int x1
497 : 		<dd>左上X
498 : 		<dt>int y1
499 : 		<dd>左上Y
500 : 		<dt>int x2
501 : 		<dd>右下X
502 : 		<dt>int y2
503 : 		<dd>右下Y
504 : 	  </dl>
505 :       
*/

void lssproto_M_send( int fd,int fl,int x1,int y1,int x2,int y2 )
{
	lssproto_CreateHeader( lssproto.work , "M" );
	strncat( lssproto.work , lssproto_mkstr_int( fl ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( x1 ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( y1 ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( x2 ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( y2 ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
665 :   <LI><a name="CS_S"><font color=red>clienttoserver S( string category );<br></font></a>
666 :       (Status)
667 :       キャラのステータスを要求する。
668 :       <dl>
669 : 	<dt>string category
670 : 	<dd>どんな情報が欲しいのか。次のどれかを指定。
671 : 	    <ul>
672 : 	      <LI>P 全パラメータ
673 : 	      <LI>C 座標
674 : 	      <LI>I 全アイテム
675 : 	      <LI>S 全スキル
676 : 	      <LI>T 全称号
677 : 	      <LI>M 部分パラメータ
678 : 		  <LI>G 覚えているジェム
679 : 	    </ul>
680 :       </dl>
681 :       どういう情報がかえってくるのかは、サーバーからの送信を参照。
682 :       この文字列はエスケープされない。
683 :       
*/

void lssproto_S_send( int fd,char* category )
{
	lssproto_CreateHeader( lssproto.work , "S" );
	strncat( lssproto.work , lssproto_mkstr_string( category ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
887 :   <LI><a name="CS_CLIENTLOGIN"><font color=red>clienttoserver ClientLogin(string cdkey,string passwd);<br></font></a>
888 :       クライアントがサーバーにログインするときに使う。つまりアカウントのログイ
889 :       ンと考えればよい。ゲーム内容にタッチする種類の関数は、すべてこのログイン
890 :       をしないと実行されないようになっている。この関数はただ単にクライアントの
891 :       情報をゲームサーバにたくわえるためだけの物である。また、パスワー
892 :       ド変更したら、再度この関数を呼ぶ事。
893 :       
894 :       <br><br>
895 :       <dl>
896 : 	<dt>string cdkey
897 : 	<dd>CDKEY
898 : 	<dt>string passwd
899 : 	<dd>パスワード。この文字列はエスケープされない。
900 :       </dl>
901 :       
*/

void lssproto_ClientLogin_send( int fd,char* cdkey,char* passwd )
{
	lssproto_CreateHeader( lssproto.work , "ClientLogin" );
	strncat( lssproto.work , lssproto_mkstr_string( cdkey ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_string( passwd ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
913 :   <LI><a name="CS_CREATENEWCHAR"><font color=red>clienttoserver CreateNewChar(string charname,int imgno , string option);<br></font></a>
914 :       新しいキャラクターを作る。
915 :       <br><br>       
916 :       <dl>
917 : 	<dt>string charname
918 : 	<dd>キャラ名(空白と改行がない、プレーンな文字列)。
919 : 	    この文字列はデリミタを使わないので、エスケープする必要はない。
920 : 	<dt>int imgno
921 : 	<dd>キャラクターの画像番号。
922 : 	<dt>string option
923 : 	<dd>オプション文字列。文字列の内容は、どういう特性をもつキャラクターをつくるかを
924 : 	    指定する。
925 : 	    キャラ作成時に指定できる特性は、HP , MP , STR, TOUGH ,そしてスキルである。
926 : 	    その情報は以下のルールでならべる。まず、それぞれはたて棒'|'でくぎる。
927 : 	    <code>
928 : 	    1|2|3|4|5....N
929 : 	    </code>
930 : 	    この例でいくと、第1トークンが1、第2トークンが2、である。
931 : 	    次にそれぞれのトークンがもつ意味を説明する。
932 : 	    <table border>
933 : 	      <tr><td>token 1</td><td>最大HP</td></tr>
934 : 	      <tr><td>token 2</td><td>最大MP</td></tr>
935 : 	      <tr><td>token 3</td><td>最大STR</td></tr>
936 : 	      <tr><td>token 4</td><td>最大TOUGH</td></tr>
937 : 	      <tr><td>token 5</td><td>ひとつめのスキルのコード</td></tr>
938 : 	      <tr><td>token 6</td><td>ふたつめのスキルのコード</td></tr>
939 : 	      <tr><td>token 4+N</td><td>N個めのスキルのコード</td></tr>
940 : 	</table>
941 : 	さらに、ゲーム内容的に、キャラ作成時に指定できるパラメータの値には
942 : 	限界がある。これは仕様書を参照すること。この文字列はエスケープしない。
943 : 	    
944 :       </dl>
945 : 
946 :       
*/

void lssproto_CreateNewChar_send( int fd,char* charname,int imgno,char* option )
{
	lssproto_CreateHeader( lssproto.work , "CreateNewChar" );
	strncat( lssproto.work , lssproto_mkstr_string( charname ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_int( imgno ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_string( option ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
968 :   <LI><a name="CS_CHARDELETE"><font color=red>clienttoserver CharDelete(string charname);<br></font></a>
969 :       キャラを消す。
970 :       <br><br>
971 :       <dl>
972 : 	<dt>string charname
973 : 	<dd>キャラ名。デリミタをふくまないのでエスケープしない。
974 :       </dl>
975 :       <br>
976 :       <br>
*/

void lssproto_CharDelete_send( int fd,char* charname )
{
	lssproto_CreateHeader( lssproto.work , "CharDelete" );
	strncat( lssproto.work , lssproto_mkstr_string( charname ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
992 :   <LI><a name="CS_CHARLOGIN"><font color=red>clienttoserver CharLogin(string charname);<br></font></a>
993 :       キャラクターがサーバーにログインするときに使う。
994 :       <br><br>
995 :       <dl>
996 : 	<dt>string charname
997 : 	<dd>キャラ名。デリミタをふくまないので、エスケープしない。
998 :       </dl>
*/

void lssproto_CharLogin_send( int fd,char* charname )
{
	lssproto_CreateHeader( lssproto.work , "CharLogin" );
	strncat( lssproto.work , lssproto_mkstr_string( charname ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1012 :   <LI><a name="CS_CHANGEPASSWD"><font color=red>clienttoserver ChangePasswd(string oldpasswd,string newpasswd);<br></font></a>
1013 :       パスワードの変更をする。
1014 :       <br><br>
1015 :       <dl>
1016 : 	<dt>string oldpasswd
1017 : 	<dd>現在のパスワード。
1018 : 	<dt>string newpasswd
1019 : 	<dd>変更するパスワード。エスケープしない
1020 :       </dl>
*/

void lssproto_ChangePasswd_send( int fd,char* oldpasswd,char* newpasswd )
{
	lssproto_CreateHeader( lssproto.work , "ChangePasswd" );
	strncat( lssproto.work , lssproto_mkstr_string( oldpasswd ) ,lssproto.workbufsize );
	strncat( lssproto.work , lssproto_mkstr_string( newpasswd ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1033 :   <LI><a name="CS_CHARLIST"><font color=red>clienttoserver CharList( void );<br></font></a>
1034 :       キャラリストを得る。
1035 :       <br><br>
*/

void lssproto_CharList_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "CharList" );
	strncat( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1056 :   <LI><a name="CS_CHARLOGOUT"><font color=red>clienttoserver CharLogout(void);<br></font></a>
1057 :       キャラクターがサーバーからログアウトするときに使う。
1058 :       引数なし。
1059 :       
*/

void lssproto_CharLogout_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "CharLogout" );
	strncat( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1073 :   <LI><a name="CS_ECHO"><font color=red>clienttoserver Echo( string test);<br></font></a>
1074 :       サーバーが生きているかをテストしたり、テスト用に使う。
1075 :       <br><br>
1076 :       <dl>
1077 : 	<dt>string test
1078 : 	<dd>エコー用文字列。エスケープしない。
1079 :       </dl>
1080 :       
*/

void lssproto_Echo_send( int fd,char* test )
{
	lssproto_CreateHeader( lssproto.work , "Echo" );
	strncat( lssproto.work , lssproto_mkstr_string( test ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}

void lssproto_ClientDispatchMessage(int fd ,char*line)
{
	int msgid;
	char funcname[1024];
	strcpy( lssproto.work , line);
	lssproto_splitString( lssproto.work);
	lssproto_GetMessageInfo( &msgid , funcname , lssproto.token_list);


#if 0
    
/*
107 :   <LI><a name="SC_W"><font color=blue>servertoclient W(int id ,int x , int y);<br></font></a>
108 : 	1歩あるくごとにサーバーからクライアントにたいして送信される。
109 : 	歩けても歩けなくても1歩ごとに処理が終わった時点で送信しなければならない。
110 : 	このコマンドはどのIDの歩くコマンドの処理についてのものなのかを引数id
111 : 	で指定する。たとえば、4歩分まとめてあるくコマンドがクライアントからの
112 : 	Wコマンドできたら、4個のこのコマンドが0.4秒つまり1歩あるくごとにサーバーから
113 : 	クライアントに送信されることになる。またこのコマンドはいつでも
114 : 	最新の座標の情報をかえす。フロアがかわったときとかは、ステータス(S)の
115 : 	座標送信コマンド(C)を使う。
116 : 
117 : 
118 : 	<dl>
119 : 		<dt>int id
120 : 		<dd>どのコマンドにたいするものか
121 : 		<dt>int x
122 : 		<dd>最新のX座標
123 : 		<dt>int y
124 : 		<dd>最新のY座標
125 : 	</dL>	
126 : 	たとえば、クライアントから abcdというように歩くコマンドがID 4
127 : 	できたら、 <br>
128 : 	"W 4 1 1" <br>
129 : 	"W 5 1 0"<br>
130 : 	"W 6 1 0"<br>
131 : 	"W 7 1 1"<br>
132 : 	のような4つのこのコマンドがサーバーから送信されることになる。
133 : 	サーバーはこのIDを送るために、現在どこまでのIDを処理しているのかの
134 : 	変数をもつ必要があるだろう。またバッファリングも必要であろう。
135 : 	バッファーのサイズは、32歩分もあれば十分であろう。
136 : 
137 : 
*/

	if( strcmp( funcname , "W" ) == 0 ){
		int id;
		int x;
		int y;
		id = lssproto_demkstr_int( lssproto.token_list[2] );
		x = lssproto_demkstr_int( lssproto.token_list[3] );
		y = lssproto_demkstr_int( lssproto.token_list[4] );
		lssproto_W_recv( fd,id,x,y);
	}
/*
393 :   <LI><a name="SC_AB"><font color=blue>servertoclient AB( string data );</font></a><br>
394 :       サーバーがクライアントに送信するアドレスブックの全内容。
395 :       <dl>
396 : 	<dt>string data
397 : 	<dd>情報の内容。表示するために必要な情報は、相手のキャラの名前、
398 : 	    レベル、ライフの数、オンラインかどうか、メッセージがきているか
399 : 	    どうか のフラグ、である。それが人数分ならんでいる。
400 : 	    
401 : 	    <code>一人目|二人目|...</code>人数に制限はない。
402 : 	    
403 : 	    一人分のメッセージは以下の構成である。
404 : 	    <code>名前(文字列)|レベル数値|ライフ数値|受信フラグ値</code>
405 : 	    したがって、 N人目の名前にアクセスするには 4(N-1)+1番目のトークンを、
406 : 	    しらべればよいことになる。そのトークンが存在しないならば、
407 : 	    それ以降のエントリは存在しない。
408 : 	    名前文字列は<a href="#escaping">エスケープ</a>してからたて棒でつなぐ。
409 :       </dl>
410 : 
*/

	if( strcmp( funcname , "AB" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_AB_recv( fd,data);
	}
/*
428 :   <LI><a name="NAB"><font color=blue>servertoclient NAB( string data );</font></a><br>
429 :       (NotifyAddressBookmessage)相手からのメッセージがきているということを通知する。
430 :       <dl>
431 : 	<dt>string data
432 : 	<dd>メッセージがきているかどうかをあらわすフラグのリスト。
433 : 	    0ならばメッセージがきていない、1ならきている。
434 : 	    
435 : 	    たとえば、10項目あって2番と8番の相手にメッセージがきている場合は10桁の数値がくる。
436 : 	    <code>0010000010</code>30項目だったら30桁。
437 : 	    この文字列はエスケープしない。
438 :       </dl>
439 :       サーバーは受信状態に何か変化があったらこのコマンドを送信する。
440 : 
*/

	if( strcmp( funcname , "NAB" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_NAB_recv( fd,data);
	}
/*
470 :   <LI><a name="T"><font color=blue>servertoclient T( int charindex , string message , int color);<br></font></a>
471 :       (Text)
472 :       テキストを表示するためのサーバーからのコマンド。システムアナウン
473 :       スとかNPCのセリフもこれを使う。<br><br>
474 : 
475 :       <dl>
476 : 	<dt>int charindex
477 : 	<dd>キャラクターのサーバー内インデックス。このインデックスをもとに
478 : 	    吹きだしの表示をする。
479 : 	<dt>string message
480 : 	<dd>文字列。チャットは称号とかもふくめて送信されてくる。
481 : 	    文字コードはEUCで送信されてくる。
482 : 	    サーバーとネットワーク上に存在する文字コードはすべてEUCである。
483 : 	    この文字列はデリミタをふくまないので、エスケープする必要はない。
484 : 	<dt>int color
485 : 	<dd>色。色コードは未定義。
486 :       </dl>
487 :       <br>
488 :       <br>
489 : 
490 : 
*/

	if( strcmp( funcname , "T" ) == 0 ){
		int charindex;
		char* message;
		int color;
		charindex = lssproto_demkstr_int( lssproto.token_list[2] );
		message = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		color = lssproto_demkstr_int( lssproto.token_list[4] );
		lssproto_T_recv( fd,charindex,message,color);
	}
/*
506 :   <LI><a name="SC_M"><font color=blue>servertoclient M(int fl , int x1 , int y1 , int x2, int y2 ,string data );<br></font></a>
507 :       マップを送信する。どんな矩形でも送信できる。もちろん最大サイズ
508 :       はきまっていてよい。これはサーバーからのみ送信し、クライアント
509 :       が要求することはない。送信のタイミングは、キャラにとってあたら
510 :       しい部分が見えるようになった瞬間や、地形が変更された瞬間である。
511 :       <br><br>
512 :       <dl>
513 : 	<dt>int fl
514 : 	<dd>キャラのいるフロア番号
515 : 	<dt>int x1
516 : 	<dd>フロアマップの中の絶対位置。左上X
517 : 	<dt>int y1
518 : 	<dd>フロアマップの中の絶対位置。左上Y
519 : 	<dt>int x2
520 : 	<dd>フロアマップの中の絶対位置。右下X
521 : 	<dt>int y2
522 : 	<dd>フロアマップの中の絶対位置。右下Y
523 : 	<dt>string data
524 : 	<dd>フロアの見える文字列|タイル|オブジェクトになっている。
525 : 	    dataはマップタイル番号で"76,76,77,78,98,90,1,1,1,2"という
526 : 	    ように必要な要素数並んでいる。スペースでくぎるとエスケープ
527 : 	    の関係上、量がおおくなるので、コンマでくぎる。階段などに進
528 : 	    んだ場合、全画面分を送信して、一歩あるく場合は一歩分だけお
529 : 	    くる。こういう判断はサーバーでやるしかない。クライアントは
530 : 	    この関数でマップをうけとると、それをディスクに保存して、オー
531 : 	    トマップ用の情報をたくえる。この文字列はエスケープする必要はない。
532 :       </dl>
533 :       <br>
534 :       <br>
535 :       
*/

	if( strcmp( funcname , "M" ) == 0 ){
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;
		char* data;
		fl = lssproto_demkstr_int( lssproto.token_list[2] );
		x1 = lssproto_demkstr_int( lssproto.token_list[3] );
		y1 = lssproto_demkstr_int( lssproto.token_list[4] );
		x2 = lssproto_demkstr_int( lssproto.token_list[5] );
		y2 = lssproto_demkstr_int( lssproto.token_list[6] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[6] , lssproto_demkstr_string( lssproto.token_list[7] ));
		lssproto_M_recv( fd,fl,x1,y1,x2,y2,data);
	}
/*
536 :   <LI><a name="C"><font color=blue>servertoclient C( string data );<bR></font></a>
537 :       (Characters)
538 :       見える範囲にいるオブジェクトやキャラの情報をすべてサーバーからクライアン
539 :       トに送信する。キャラが出現したり移動したり、状態が変化したらい
540 :       つでも送信する。クライアントはこの関数を要求しない。状況が変化
541 :       したときはサーバーが自分で送信する。<br><br>
542 :       この情報が有効な範囲は、サーバーに設定されている、クライアントが
543 :       見えているだろう範囲である。だからクライアントのプログラムは、
544 :       見えている範囲のすべてのキャラクターとアイテムのうちこのパケット
545 :       に含まれていないものを削除することができる。デフォルトでは
546 :       自分を中心にして11マス四方のサイズに関してサーバーは調べる義務がある。
547 :       
548 :       <dl>
549 : 	<dt>string data
550 : 	<dd>	  フォーマットは 以下の3種類の項目をコンマでならべた
551 : 	    ものである.それぞれの項目の中身はさらにたて棒'|'でくぎられている。
552 : 
553 : 	    
554 : 	    <ul>
555 : 	      <li>たて棒でくぎられたトークンが7個の場合<br>
556 : 		  トークンの内容は
557 : 		  <code>CHARINDEX|BASEIMG|LEVEL|NAME|SELFTITLE|WALKABLE|HEIGHT</code>
558 : 		  それは他のプレイヤーやNPCや敵である。敵の場合はSELFTITLE
559 : 		  は何か特別な文字列を入れてクライアントの1行インフォで表示させることができる。
560 : 		  ものとする。サーバーが送信するときは見える範囲について全
561 : 		  部送信する。つまりクライアントは、このパケットを受けとったときに
562 : 		  このパケットに書かれていないキャラを持っていたら消してしまってよい
563 : 		  ということである。
564 : 		  また、マウスカーソルをあわせたときに
565 : 		  表示する情報はこの情報のみに基いている。だから、マウスカー
566 : 		  ソルをあわせたときに表示することが変更された場合は、
567 : 		  サーバーは、この関数を能動的に呼びださなければならない。
568 : 		  SELFTITLEについては、デリミタであるたて棒をふくまないように
569 : 		  エスケープされている。クライアントはアクションのコマンド(CA)がくるまでは
570 : 		  立ちで表示する。
571 : 		  CHARINDEXサーバー内の一意にキャラを特定できる番号、BASEIMGは表示のための
572 : 		  番号、LEVELはキャラのレベル(0なら表示しない。この値はNPCなどに使う。)
573 : 		  WALKABLEは1のときその上を通過することができ、0なら通過することができない。
574 : 		  HEIGHTは高さをもつものかそうでないのかの指定。
575 : 		  
576 : 		  キャラクターの名前と自由称号は、<a href="#escaping">エスケープ</a>
577 : 		  されなければならない。
578 : 		  '|'でトークンを取りだしてからエスケープを解除する。
579 : 		  エスケープすると'|'がほかの文字におきかわるので、最初は単純に'|'を
580 : 		  デリミタとしてよい。
581 : 		  送信する方も、
582 : 		  名前と自由称号をエスケープしてからたて棒でつないでから送信する。
583 : 		  
584 : 	      <Li>たて棒でくぎられたトークンが4個の場合<br>
585 : 		  トークンの内容は
586 : 		  <code>X|Y|BASEIMG|ITEM1LINEINFO</code>
587 : 		  で地面に落ちているアイテムについての情報である。
588 : 		  X,Yはアイテムのグローバル位置。
589 : 		  BASEIMGは画像の番号。ITEM1LINEINFOは1行INfoに表示するための
590 : 		  情報である。アイテムウインドウ内の表示用の情報は別の方法で用意する。
591 : 		  アイテムに関してはCAは来ない。ITEM1LINEINFOはエスケープされる。
592 : 		  このエスケープの方法は上の項目を参照。
593 : 
594 : 	    </ul>
595 :       </dl>
596 :       <br>
597 :       <br>
598 : 
599 : 
600 :       
*/

	if( strcmp( funcname , "C" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_C_recv( fd,data);
	}
/*
601 :   <LI><a name="CA"><font color=blue>servertoclient CA( string data );<br></font></a>
602 :       (CharacterAction)
603 :       見える範囲にいるキャラのアクション状態を更新する。
604 :       サーバーからクライアントに一方的に送信する。
605 :       各キャラの1アクションごとに送信。サーバーはアクションをできるだ
606 :       け圧縮すること。<br><br>
607 : 
608 :       <dl>
609 : 	<dt>string data
610 : 	<dd>CHARINDEX|X|Y|ACTION|PARAM1|PARAM2|PARAM3|PARAM4|....をコ
611 : 	    ンマでくぎったものにする。PARAMはアクションごとに個数も
612 : 	    使いかたもことなる。以下はアクション一覧。X,Yは位置でど
613 : 	    のアクションでも、あたらしい位置を指定する。
614 : 	    この文字列はエスケープされない。
615 : 	    <table border>
616 : 	      <tr><td>ACTION</td><td>PARAM1</td><td>PARAM2</td><td>PARAM3</td><td>PARAM4</td></tr>
617 : 	      <tr><td>Stand:0</td><td>方向0~7</td><td></td><td></td><td></td></tr>
618 : 	      <tr><td>Walk:1</td><td>方向0~7</td><td></td><td></td><td></td></tr>
619 : 	      <tr><td>Attack:2</td><td>方向0~7</td><td></td><td></td><td></td></tr>
620 : 	      <tr><td>ThrowItem:3</td><td>方向0~7</td><td></td><td></td><td></td></tr>
621 : 	      <tr><td>Damage:4</td><td>方向0~7</td><td>エフェクト番号</td><td>エフェクトに必要な数値</td><td></td></tr>
622 : 	      <tr><td>Dead:5</td><td>方向0~7</td><td>エフェクト番号</td><td>エフェクトに必要な数値</td><td></td></tr>
623 : 	      <tr><td>UseMagic:6</td><td>方向0~7</td><td>エファクト番号</td><td>エフェクトに必要な数値</td><td></td></tr>
624 : 	      <tr><td>UseItem:7</td><td>方向0~7</td><td>エフェクト番号</td><td>エフェクトに必要な数値</td><td></td></tr>		
625 : 	</table>
626 : 	ACTIONの値は整数で、内容は上の表の左端の項目である。
627 :   </dl>
628 :   <br>
629 :   <br>
630 : 
631 : 
632 :       
*/

	if( strcmp( funcname , "CA" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_CA_recv( fd,data);
	}
/*
633 :   <LI><a name="R"><font color=blue>servertoclient R( int result , string data );<br></font></a>
634 :       (Radar)
635 :       レーダーの内容をクライアントに送信する。クライアントはこの送信を
636 :       要求することはない。サーバーが適当なタイミングで送信する。
637 :       たとえば10歩あるくごととか、1分ごととか。<br><br>
638 : 
639 :       <dl>
640 : 	<dt>int result
641 : 	<dd>0 は正常終了。負はエラー
642 : 	<dt>string data
643 : 	<dd>x ,y, kind ,の順に値を'|'でくぎってならべたものを
644 : 	    さらに'|'でならべたもの。
645 : 	    x,yはキャラからの相対位置。kindは以下にしめす数値ではない記号。
646 : 	    レーダーにうつる物の種類を指定する。
647 : 	    送られてくるものの種類は、
648 : 	    それまでに唱えた魔法とか、スキルで変化し、それはサーバー
649 : 	    が考える。
650 : 	    <table border>
651 : 	      <tr><td>kindの値</td><td>内容</td></tr>
652 : 	      <tr><td>E</td><td>敵</td></tr>
653 : 	      <tr><td>P</td><td>プレイヤー</td></tr>
654 : 	      <tr><td>D</td><td>ドア</td></tr>
655 : 	      <tr><td>N</td><td>そのほかのNPC(看板とか)</td></tr>		  
656 : 	      <tr><td>L</td><td>見える階段</td></tr>
657 : 	      <tr><td>W</td><td>見えないワープポイント</td></tr>
658 : 	</table>
659 :   </dl>
660 :   dataの例：<code>"12|22|E|13|24|P|14|28|P"</code>
661 :   この文字列はエスケープされない。
662 :   <br>
663 :   <br>
664 : 
*/

	if( strcmp( funcname , "R" ) == 0 ){
		int result;
		char* data;
		result = lssproto_demkstr_int( lssproto.token_list[2] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_R_recv( fd,result,data);
	}
/*
684 :   <LI><a name="SC_S"><font color=blue>servertoclient S( string data );<br></font></a>
685 :       (Status)
686 :       キャラのステータスを送信する。
687 :       データは カテゴリ記号文字(一文字)内容 となっている。つまり最初の
688 :       1文字を見れば何のステータスか分る。内容は2文字目からである。
689 :       内容は以下のフォーマットにしたがう。たて棒記号'|'がデリミタである。
690 :       2個目以降のトークンが内容である。
691 :       <UL>
692 : 	<LI>P 全パラメータ<br><br>
693 : 	    hp maxhp mp maxmp str tough exp talklevel level soul attackpower
694 : 	    deffencepower class gold 付けている称号のindex 名前 自己称
695 : 	    号 status<br>
696 : 	    
697 : 	    デリミタは '|' である。また名前と自己称号は、
698 : 	    <a href="#escaping">エスケープ</a>したものを
699 : 	    たて棒でつなげたものが通信される。<br>
700 : 	    それぞれの値の型は以下。
701 : 	    <table border>
702 : 	      <tr><td>hp</td><td>int</td></tr>
703 : 	      <tr><td>maxhp</td><td>int</td></tr>
704 : 	      <tr><td>mp</td><td>int</td></tr>
705 : 	      <tr><td>maxmp</td><td>int</td></tr>
706 : 	      <tr><td>str</td><td>int</td></tr>
707 : 	      <tr><td>tough</td><td>int</td></tr>
708 : 	      <tr><td>exp</td><td>int</td></tr>
709 : 	      <tr><td>talklevel</td><td>int</td></tr>
710 : 	      <tr><td>level</td><td>int</td></tr>
711 : 	      <tr><td>soul</td><td>int</td></tr>	      	      
712 : 	      <tr><td>attackpower</td><td>int</td></tr>
713 :      	      <tr><td>deffencepower</td><td>int</td></tr>
714 : 	      <tr><td>class</td><td>int</td><td>値の意味 0:クラスA 1:B
715 : 		  2:C 3:D</td></tr>
716 : 		  <tr><td>gold</td><td>int</td></tr>
717 : 		  <tr><td>称号のindex</td><td>int</td></tr>
718 : 		  <tr><td>名前</td><td>文字列</td></tr>
719 : 		  <tr><td>自己称号</td><td>文字列</td></tr>
720 : 		  <tr><td>status</td><td>文字列</td></tr>
721 : 	    </table>
722 : 	    経験値がdoubleであったが、int に変更する。経験値の得られる
723 : 	    量は、自分と敵のレベルによって変化する事にする( Diablo の
724 : 	    ように )status , 自己称号以外はすべて整数値。状態は以下の
725 : 	    文字をならべたもの
726 : 	    <ul>
727 : 	      <LI>P 毒。
728 : 	      <LI>N 痺れ
729 : 	      <LI>Q 沈黙
730 : 	      <LI>S 石
731 : 	      <LI>D 暗闇
732 : 	      <LI>C 混乱
733 : 	    </ul>
734 : 	    具体例( char の配列そのまま書く )<br>
735 : 	    <code>
736 : 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|へんぱ|PC
737 : 	    </code>
738 : 	<li>C 座標<br><br>
739 : 	    floor x y<br>
740 : 	    を  | で区切って送る。<br>
741 : 	    具体例( char の配列そのまま書く )<br>
742 : 	    <code>
743 : 	    C1024|10|20
744 : 	    </code>
745 : 	    上記の例では、フロアID1024 , X 10,Y 20のところにキャラクターがいる
746 : 	    という内容になる。このマイキャラのフロアもふくめた座標をク
747 : 	    ライアントが知ることができるのはこのコマンドのみである。歩
748 : 	    きの結果はフロア内の位置しかわからない。よって、ゲーム開始
749 : 	    時、階段使用のとき、ワープのときなどは、このコマンドをつかっ
750 : 	    て正しい値を送信しなくてはならない。
751 : 	    
752 : 
753 : 	<LI>I 全アイテム<br><br>
754 : 	    (アイテム1)|(アイテム2)...(アイテムn)<br>
755 : 	    アイテムの内容は以下の方法でパックする。
756 : 	    ひとつのアイテムは、かならず以下の4つのトークンのセットで
757 : 	    送信される。ひとつひとつの内容は、<br>
758 : 	    名前|名前2|ステージ|メモ|アニメ番号<br>
759 : 	    <ul>
760 : 	      <li>アイテム名は識別レベルで自動的に変更される。
761 : 	      <li>名前2は識別レベルによって、アイテムの能力が入る予定。
762 : 		  クライアントのアイテム欄の2行目に入れる事。
763 : 	      <li>ステージは名前の色を替るのに使う。
764 : 	      <li>メモはメモ。
765 : 	      <li>アニメ番号は、画像番号。
766 : 	    </ul>
767 : 	    ここの中では、'|' と、'\'  だけバックスラッシュでエスケー
768 : 	    プされる。<br>
769 : 	    具体例( char の配列そのまま書く )<br>
770 : 	    <code>
771 : 	    Iあほ な 子|str+2|1|あほ\|いじ|10
772 : 	    </code>
773 : 	    ステージの値の意味は、整数値で
774 : 	    <table border>
775 : 	      <tr><td>1</td><td>ステージA</td></tr>
776 : 	      <tr><td>2</td><td>ステージB</td></tr>	      
777 : 	      <tr><td>3</td><td>ステージC</td></tr>
778 : 	      <tr><td>4</td><td>ステージD</td></tr>
779 : 	    </table>
780 : 	    とする。また、アイテム表の空の部分については、4個のトークンが
781 : 	    空で、つまりたて棒がならんだ状態で送信されてくる。
782 : 
783 : 	<LI>S 全スキル<br><br>
784 : 	    (スキル0)|(スキル1)|(スキル2) ... (スキルn)<br>
785 : 	    称号と同じで、キャラがもてる最大スキル数分を送信する。つま
786 : 	    りないものは ||| となるのである。
787 : 	    各エントリの内容は、<br>
788 : 	    スキルの種類コード|レベル<br>
789 : 	    である。<br>
790 : 	    具体例( char の配列そのまま書く )<br>
791 : 	    <code>
792 : 	    S10|2|20|2|||
793 : 	    </code>	    
794 : 
795 : 	<LI>T 称号<br><br>
796 : 	    (称号0)|(称号1)|(称号2) ... (称号n)<br><br>
797 : 	    称号のエントリが空の場合は '|' が連続するので、連続したも
798 : 	    のを省略してスキャンしてはならない。かならず最大個数分送信
799 : 	    する。<br>
800 : 	    ひとつ、ひとつの内容は、<br>
801 : 	    名前<br>
802 : 	    のみである。<br>
803 : 	    具体例( 送られてくる char の配列そのままである )<br>
804 : 	    <code>
805 : 	    Tあほ|よっぱらい
806 : 	    </code>
807 : 
808 : 	<LI>M 部分パラメータ<br><br>
809 : 	    キャラクターのステータスのうち、頻繁に変更されるもの(現在値)とそうでないもの(最大値など)
810 : 	    がある。どれかのステータスが変更されるたびに毎回全部のパラメータを送信するのは
811 : 	    ネットワークトラフィックを無駄に使うことになるので、特にHP , MP,EXPの値だけにしぼった
812 : 	    パラメータ送信コマンドを用意する。それがこのMコマンドである。
813 : 	    2文字目以降の第一トークンはHPの現在値 , 第二トークンはMP、第三トークンはEXPである。
814 : 	    以下に具体例をしめす。
815 : 	    <code>
816 : 	    M54|210|8944909
817 : 	    </code>
818 : 	    この例ではHPの現在値が54、MPの現在値が210、経験値が8944909になっているのだ。
819 : 	    この3種類は特に変更頻度が高いと思われるため独立したコマンド化したが、
820 : 	    もちろんこのMコマンドのかわりにPコマンドで全パラメータを送信してもよい。
821 : 	    ただし、最適にネットワークを使うためには、このコマンドを使うことが推奨される。
822 : 	    経験値の型はdoubleで整数である。
823 : 	    
824 : 	<LI>G ジェムリスト<br><br>
825 : 		Lifestorm2ではジェムを覚えてそれ以降そのジェムは使い放題になる
826 : 		が、このコマンドではどのジェムを覚えているのかを伝える。
827 : 		このコマンドが送信されるタイミングは、ゲームスタート時と、
828 : 		覚えているジェムに変化があったときだけである。<br>
829 : 		シンタックスはジェムの種類+1文字の計13文字固定。<br>
830 : 		<code>Gabcdefghijkl</code><br>
831 : 		2文字目から13文字目までは、それぞれ
832 : 		<table border>
833 : 		<tr><td>2文字目</td><td>LIFE</td></tr>
834 : 		<tr><td>3文字目</td><td>BLAZE</td></tr>
835 : 		<tr><td>4文字目</td><td>AIR</td></tr>
836 : 		<tr><td>5文字目</td><td>ELECT</td></tr>
837 : 		<tr><td>6文字目</td><td>LIGHT</td></tr>
838 : 		<tr><td>7文字目</td><td>ICE</td></tr>
839 : 		<tr><td>8文字目</td><td>INTELI</td></tr>
840 : 		<tr><td>9文字目</td><td>METAL</td></tr>
841 : 		<tr><td>10文字目</td><td>HOLY</td></tr>
842 : 		<tr><td>11文字目</td><td>EVIL</td></tr>
843 : 		<tr><td>12文字目</td><td>DEATH</td></tr>
844 : 		<tr><td>13文字目</td><td>SPACE</td></tr>
845 : 		</table>
846 : 		が対応していて、持っている場合はその文字は1、持っていない場合
847 : 		は0になる。つまり例は
848 : 		<code>G111010111000</code>
849 : 		のようになる。将来ジェムの種類が増える可能性があるが、そのとき
850 : 		は桁数を増やして対応する。
851 : 
852 :       </ul>
853 : 	  
854 : 
*/

	if( strcmp( funcname , "S" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_S_recv( fd,data);
	}
/*
855 :   <LI><a name="D"><font color=blue>servertoclient D( int category , int dx , int dy , string data );<br></font></a>
856 :       (Display)
857 :       画面に何か表示する指令。<br><br>
858 : 
859 :       <dl>
860 : 	<dt>int category
861 : 	<dd>何を表示するか。
862 : 	    <table border>
863 : 	      <tr><td>値</td><td>内容</td></tr>
864 : 	      <tr><td>1</td><td>自分以外に与えたダメージ。dataは文字
865 : 		  列になった値。</td></tr>
866 : 		  <tr><td>2</td><td>自分がうけたダメージ。dataは文字列に
867 : 		      なった値</td></tr>
868 : 		</table>
869 : 	<dt>int dx
870 : 	<dd>マイキャラからのグリッド相対位置X。一般的にはイベントが発生
871 : 	    した位置。クライアントはこの値から適宜適切な位置を計算して
872 : 	    描画する。
873 : 	<dt>int dy
874 : 	<dd>相対位置Y
875 : 	<dt>string data 
876 : 	<dd>表示する内容。内容はcategoryによって決まる。
877 : 	    この文字列は<a href="#escaping">エスケープ</a>されなければならない。
878 :       </dl>
879 :       <br>
880 :       <br>
881 : 
882 :       <hr>
883 : 
884 : 
885 :       <!-- ここから下はキャラ作成などゲーム外の関数   -->
886 : 	  
*/

	if( strcmp( funcname , "D" ) == 0 ){
		int category;
		int dx;
		int dy;
		char* data;
		category = lssproto_demkstr_int( lssproto.token_list[2] );
		dx = lssproto_demkstr_int( lssproto.token_list[3] );
		dy = lssproto_demkstr_int( lssproto.token_list[4] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[4] , lssproto_demkstr_string( lssproto.token_list[5] ));
		lssproto_D_recv( fd,category,dx,dy,data);
	}
/*
902 :   <LI><a name="SC_CLIENTLOGIN"><font color=blue>servertoclient ClientLogin(string result);<br></font></a>
903 :       ClientLoginの返答。
904 :       <br><br>
905 :       <dl>
906 : 	<dt>string result
907 : 	<dd>"ok" という文字列。この文字列はエスケープされない。
908 :       </dl>
909 :       <br>
910 :       <br>
911 : 
912 :       
*/

	if( strcmp( funcname , "ClientLogin" ) == 0 ){
		char* result;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_ClientLogin_recv( fd,result);
	}
/*
947 :   <LI><a name="SC_CREATENEWCHAR"><font color=blue>servertoclient CreateNewChar(string result,string data);<br></font></a>
948 :       CreateNewCharの返答。
949 :       <br><br>
950 :       <dl>
951 : 	<dt>string result
952 : 	<dd>"successful" か "failed" のいずれか。この文字列はエスケープしない。
953 : 	<dt>string data
954 : 	<dd>"failed" の時は理由を示す人間の見て分る文字
955 : 	    列である。アカウントサーバからの返答そのままである。
956 : 	    以下の文字列
957 : 	    <pre>
958 : 	    "failed bad parameter"
959 : 	    </pre>
960 : 	    の場合は、キャラ作成のときに規定のパラメータの範囲を越えているという
961 : 	    ことを意味する。これはゲームサーバーが出力するメッセージである。
962 : 	    この文字列はエスケープしない。
963 : 	    
964 :       </dl>
965 :       <br>
966 :       <br>
967 : 
*/

	if( strcmp( funcname , "CreateNewChar" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CreateNewChar_recv( fd,result,data);
	}
/*
977 :   <LI><a name="SC_CHARDELETE"><font color=blue>servertoclient CharDelete(string result,string data);<br></font></a>
978 :       CharDelete の返答。
979 :       <br><br>
980 :       <dl>
981 : 	<dt>string result
982 : 	<dd>"successful" か "failed" のいずれか。エスケープしない。
983 : 	<dt>string data
984 : 	<dd>"failed" の時は理由を示す人間の見て分る文字
985 : 	    列である。アカウントサーバからの返答そのままである。
986 : 	    デリミタをふくまないので、エスケープしない。
987 :       </dl>
988 :       <br>
989 :       <br>
990 : 
991 : 
*/

	if( strcmp( funcname , "CharDelete" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharDelete_recv( fd,result,data);
	}
/*
999 :   <LI><a name="SC_CHARLOGIN"><font color=blue>servertoclient CharLogin(string result,string data);<br></font></a>
1000 :       CharaLoginの返答。
1001 :       <br><br>
1002 :       <dl>
1003 : 	<dt>string result
1004 : 	<dd>"successful" か "failed" のいずれか。エスケープしない。
1005 : 	<dt>string data
1006 : 	<dd>"failed" の時は その理由の文字列。エスケープしない。
1007 :       </dl>
1008 :       <br>
1009 :       <br>
1010 : 
1011 : 
*/

	if( strcmp( funcname , "CharLogin" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharLogin_recv( fd,result,data);
	}
/*
1021 :   <LI><a name="SC_CHANGEPASSWD"><font color=blue>servertoclient ChangePasswd(string result,string data);<br></font></a>
1022 :       ChangePasswdの返答。
1023 :       <br><br>
1024 :       <dl>
1025 : 	<dt>string result
1026 : 	<dd>"successful" か "failed" のいずれか。エスケープしない
1027 : 	<dt>string data
1028 : 	<dd>"failed" の時はその理由の文字列。エスケープしない
1029 :       </dl>
1030 :       <br>
1031 :       <br>
1032 : 
*/

	if( strcmp( funcname , "ChangePasswd" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_ChangePasswd_recv( fd,result,data);
	}
/*
1036 :   <LI><a name="SC_CHARLIST"><font color=blue>servertoclient CharList(string result,string data);<br></font></a>
1037 :       CharListの返答。
1038 :       <br><br>
1039 :       <dl>
1040 : 	<dt>string result
1041 : 	<dd>"successful" か "failed" のいずれか。エスケープしない
1042 : 	<dt>string data
1043 : 	<dd>resultが"successful"の時は、アカウントサーバーに保存されているす
1044 : 	    べてのキャラの名前、オプションををスペースで区切った一個の
1045 : 	    文字列。result が "failed" の時は理由を示す人間の見て分る
1046 : 	    文字列である。
1047 : 	    オプションの中身は以下のとおりである。
1048 : 	    level|showstring|魂の数|画像番号|何回ログインしたか|クラス
1049 : 	    と "|" で区切られている。 それぞれの項目は、
1050 : 	    <a href="#escaping">エスケープ</a>されている。
1051 : 	    そのあとたて棒でつなげる。
1052 :       </dl>
1053 :       <br>
1054 :       <br>
1055 : 
*/

	if( strcmp( funcname , "CharList" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharList_recv( fd,result,data);
	}
/*
1060 :   <LI><a name="SC_CHARLOGOUT"><font color=blue>servertoclient CharLogout(string result , string data);<br></font></a>
1061 :       Logoutに対する返答。
1062 :       <br><br>
1063 :       <dl>
1064 : 	<dt>string result
1065 : 	<dd>"successful" か "failed" のいずれか。エスケープしない。
1066 : 	<dt>string data
1067 : 	<dd>"failed" の時にのみ意味があり、失敗の理由(状態)を示す人間
1068 : 	    の見て分る文字列である。エスケープしない。
1069 :       </dl>
1070 :       <br>
1071 :       <br>
1072 : 
*/

	if( strcmp( funcname , "CharLogout" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharLogout_recv( fd,result,data);
	}
/*
1081 :   <LI><a name="SC_ECHO"><font color=blue>servertoclient Echo( string test );<br></font></a>
1082 :       Echoに対する返答。
1083 :       <dl>
1084 : 	<dt>string test
1085 : 	<dd>さきほど入力された文字列。エスケープしない。
1086 :       </dl>
1087 :       <br>
1088 :       <br>
1089 : 
*/

#endif
    
	if( strcmp( funcname , "Echo" ) == 0 ){
		char* test;
		test = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_Echo_recv( fd,test);
	}
}
int lssproto_InitClient( int (*writefunc)(int,char*,int) ,int bufsiz ,int fd)
{
	int i;
	if( (void*)writefunc == NULL){lssproto.write_func = lssproto_default_write_wrap;} else {lssproto.write_func = writefunc;}
	lssproto_AllocateCommonWork(bufsiz);
	lssproto_stringwrapper = (char**)malloc(sizeof(char*) * MAXLSRPCARGS);
	if(lssproto_stringwrapper ==NULL)return -1;
	lssproto_bzero( (char*)lssproto_stringwrapper , sizeof(char*)*MAXLSRPCARGS);
    
	for(i=0;i<MAXLSRPCARGS;i++){
		lssproto_stringwrapper[i] = (char*)malloc( bufsiz );
		if( lssproto_stringwrapper[i] == NULL){
			for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);return -1;}
		}
	}
	return 0;
}
void lssproto_CleanupClient( void )
{
	int i;
	free( lssproto.work );
	free( lssproto.arraywork);
	free( lssproto.escapework );
	free( lssproto.val_str);
	free( lssproto.token_list );
	for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);}
	free( lssproto_stringwrapper );
}
/* end of the generated client code */
