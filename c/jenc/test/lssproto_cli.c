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
72 :       (Walk)�����B������������B�N���C�A���g�͓K�؂ȃ^�C�~���O�ł��̃R�}���h��
73 :       ���M(���Ӗ��ȘA�ł�}��)����B���̃R�}���h�͉����������܂Ƃ߂�
74 :       ���M���邪�A���̒ʂ�ɕ������鏈���̓T�[�o�[���~���b�P�ʂŊǗ�����B
75 :       ��{�I��0.4�b��1�����邯��B���������ʐV��������������
76 :       ���肷��ꍇ�́AM(Map)�֐��������ăT�[�o�[���}�b�v���𑗐M
77 :       ���Ă���B���ꂩ�炿����Ƃł��X�e�[�^�X���ω������S(Status)�R�}���h��������
78 : 	  �V������Ԃ𑗐M���Ă���B�����NPC�Ȃǂ����������T(Text)�R�}���h��
79 : 	  �e�L�X�g������B
80 : 	�܂�������݂���������̂ƃ��X�|���X���悭���邽�߂ɁA1�����Ƃ̃R�}���h��
81 : 	TCP�̂悤��ID���ӂ�B�Q�[���T�[�o�[��1�����邭���Ƃ�W�R�}���h�ŃN���C�A���g
82 : 	�ɑ΂��đ��M���Ȃ���΂Ȃ�Ȃ��B���̂Ƃ��Ɏg��ID�̓N���C�A���g�������Ă���ID�����̂܂�
83 : 	�R�s�[����B
84 : 
85 : 
86 : 	
87 :       <br><br>
88 :       <dl>
89 : 		<dt>string direction
90 : 		<dd>���邭���� a,b,c,d,e,f,g,h���ꏊ���ω�����ړ��ŁA
91 : 		    a���^��(dx,dy)=(0,-1)�ŁA�݂��܂��� h������(-1,-1)
92 : 		    A,B,C,D,E,F,G,H �����̏��]�B�����͂��ꂼ��^�ォ��݂��܂��B
93 : 		    �E�E�㉺�Ƃ��邭�ꍇ��
94 : 		    "ccae"�Ƃ���������𑗐M���A���̏�ō��Ɍ����΂��� ��
95 : 		    "G"�Ƒ��M���邱�ƂɂȂ�B���Ȃ炸1��1�����ɑΉ����Ă���B
96 : 		    ���̕�����̓G�X�P�[�v���Ȃ��B
97 : 		<dt>int id
98 : 		<dd>1���ڂ�ID.���ׂĂ̕����R�}���h�ɂ�ID���U���邪�A
99 : 			�܂Ƃ߂đ��鐫����A�ŏ��̂��̂ɂ��Ă���ID�������M�����
100 : 			�\���ŁA���̑��̃R�}���h(a����h,A����H)�͂����1�ÂC���N�������g
101 : 			�������̂ɂȂ�B
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
139 :       (AttackDir)�������Ă���A�C�e���ł����Ďw�肵�������ɍU������B
140 : 	  <br><br>
141 :       <dl>
142 : 	<dt>int dir
143 : 	<dd>�U���������
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
149 :       �w�肵�����@���w�肵�������ɏ�����B
150 :       <br><br>
151 :       <dl>
152 : 	<dt>int dir
153 : 	<dd>���@���g������
154 : 	<dt>string pattern
155 : 	<dd>���@�̃p�^�[���B
156 : 	    ���@�̃p�^�[���͖��@�̃W�F���̖��O���p��łȂ�ׂ����̂ɂȂ�B
157 : 	    ���Ƃ��� "blaze air air inteli"�Ƃ����悤�Ȃ��́B
158 : 	    ���̕�����̓G�X�P�[�v���Ȃ��B
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
163 :       �w�肵�������ɃX�L���������B
164 :       <br><br>
165 :       <dl>
166 : 	<dt>int dir
167 : 	<dd>�X�L�����g������
168 : 	<dt>int index
169 : 	<dd>�����̎����Ă���X�L���̕\�ւ�index.
170 :       </dl>
171 :       �X�L���\�͈ȉ��̒ʂ�ł���B����̓T�[�o�̃\�[�X�̈ꕔ���ł���B
172 :       <pre>
173 : 
174 :       �\�[�X�t�@�C���ł̓R�����g
175 :       rpcgen C�̃R�����g���̂܂܂��邩��B�R���p�C���ʂ�񂭂Ȃ�B
176 :       �R�����g�X�L���ꗗenum
177 :       typedef enum
178 :       {
179 :           SKILL_FIRE,                 �ق̂���f��    
180 :           SKILL_MAGICIAN,             ���@�g��    
181 :           SKILL_ALOTOFTHINGS,         ���̂���    
182 :           SKILL_ALOTOFHITS,           ������UP    
183 :           SKILL_AVOIDATTACK,          �݂��킵�̂����    
184 :           SKILL_DETERMINEITEM,        �A�C�e������    
185 :           SKILL_DETERMINOTHERS,       �痢��    
186 :           SKILL_FIRECUT,              ���팸  
187 :           SKILL_THUNDERCUT,           ��ȍ팸  
188 :           SKILL_ICECUT,               �X�팸  
189 :           SKILL_MERCHANT,             ���l    
190 :           SKILL_PRIEST,               �m��    
191 :           SKILL_THIEF,                ����    
192 :           SKILL_FIST,                 �f�葕��  
193 :           SKILL_SWORD,                ������  
194 :           SKILL_AXE,                  ������  
195 :           SKILL_SPEAR,                ������  
196 :           SKILL_BOW,                  �|����  
197 :           SKILL_CANE,                 �񑕔�  
198 :           SKILL_LARGEVOICE,           �傫�Ȑ�    
199 :           SKILL_STINKINGSMELL,        �L������  
200 :           SKILL_GOODSMELL,            �ǂ�����  
201 :           SKILL_UNDEAD,               �A���f�b�h    
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
209 :       ���Ƃ��ܖ��@���g���B���Ƃ��ܖ��@�́A�ʒu�̎w������邱�Ƃ͂Ȃ��B<br><br>
210 :       <dl>
211 : 	<dt>string data
212 : 	<dd>���Ƃ��܂̓��e�B���Ƃ��ܖ��@�́A���Ƃ��܂��J�M�J�b�R�Ŋ���
213 : 	    ���Ƃɂ���Ď�������̂ŁA���̒���������肾���ăT�[�o�[��EUC�ő�
214 : 	    �M����B���̕�����̓f���~�^���ӂ��܂Ȃ��̂ŁA�G�X�P�[�v����K�v�͂Ȃ��B
215 :       </dl>
216 :       ���Ƃ��ܖ��@�̌��ʂ�Txt�֐����g���ăe�L�X�g�ł������Ă���B
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
223 : 	  �A�C�e�����A�������w�肵�Ďg���B
224 : 
225 :       <dl>
226 : 	<dt>int dir
227 : 	<dd>�A�C�e�����g������
228 : 	<dt>int itemindex
229 : 	<dd>�����������Ă���A�C�e���\�ւ�index
230 :       </dl>
231 :       �A�C�e�����g�������ʂ́ATxt�֐����g���ĕ�����ŕԂ��Ă���B�A�C�e���̕�
232 :       �X�́AItC�֐��Œʒm�����B
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
241 :       �̍�������ԁBtitleindex �� -1 �̂Ƃ��͏̍����͂����Ƃ������ɂ�
242 :       ��B<br><br>
243 :       <dl>
244 : 	<dt>int titleindex
245 : 	<dd>���Ԗڂ̏̍����B
246 :       </dl>
247 :       ���ʂ́ATxt�֐��������āA�u�`�̂��傤����������񂾁I�v�̂悤
248 :       �ɒʒm�����B
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
255 :       �̍����폜����B<br><br>
256 :       <dl>
257 : 	<dt>int titleindex
258 : 	<dd>�폜����̍��̃C���f�N�X�B
259 :       </dl>
260 :       ���ʂ́ATxt�֐��������āA�u�`�̂��傤�������������債���I�v�̂悤�ɒ�
261 :       �m�����B
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
271 :       �����œ��͂���̍�����͂����B<br><br>
272 :       <dl>
273 : 	<dt>string data
274 : 	<dd>�����œ��͂����̍��BEUC�ŃT�[�o�[�ɑ��M�����B���̏̍��́A
275 : 	    �}�E�X�J�[�\�������킹���Ƃ��ɉ�ʉ���1�sinfo�ɕ\������
276 : 	    ����̂ł���B���̕�����̓f���~�^���ӂ��܂Ȃ��̂ŁA
277 : 	    �G�X�P�[�v����K�v�͂Ȃ��B
278 :       </dl>
279 :       ���̌��ʂ́ATxt�֐����g���āA������Œʒm�����B����ɂƂ��Ȃ��A
280 :       ������CHARINDEX�ɑ΂��āACh�f�[�^���ӂ����ё��M����Ă��邱�Ƃ�
281 :       �Ȃ�B
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
288 :       �A�C�e�����E���B�E���̂͑������ӂ��ޗאڂ���9�}�X�ł���B<br><br>
289 :       <dl>
290 : 	<dt>int dir
291 : 	<dd>�E������
292 :       </dl>
293 :       ���ʂ�Txt�֐��������āu�A�C�e�����Ђ�����I�v�̂悤��
294 :       �T�[�o�[���瑗�M����Ă���B�ω������A�C�e���̏󋵂́A
295 :       ItC�֐��ɂ���Ēʒm�����B
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
302 :       �A�C�e����u���B�u���̂̓h���b�v�{�^���ɒu���̂ŁA������ʒu��
303 :       �w��͂Ȃ��Bitemindex�̃A�C�e����u���B���ł������̑O�ɒu���B<br><br>
304 : 
305 :       <dl>
306 : 	<dt>int itemindex
307 : 	<dd>�u�������A�C�e���̃C���f�N�X�B
308 :       </dl>
309 :       ���ʂ�Txt�֐��������āu�A�C�e�����������I�v�̂悤��
310 :       �T�[�o�[���瑗�M����Ă���B�ω������A�C�e���̏󋵂́A
311 :       ItC�֐��ɂ���Ēʒm�����B
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
318 :       ������u���B<br><br>
319 :       <dl>
320 : 	<dt>int amount
321 : 	<dd>�u�������̗ʁB
322 :       </dl>
323 :       ���ʂ�Txt�֐��������āu�������������I�v�̂悤��
324 :       �T�[�o�[���瑗�M����Ă���B�ω������A�C�e���̏󋵂́A
325 :       ItC�֐��ɂ���Ēʒm�����B
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
332 :       �A�C�e�����ړ�/����/�������͂����B�����ɂȂ邩�ǂ�����
333 :       �ړ��̏ꏊ�Ō��܂�̂ŁA�T�[�o�[�͓K�X���b�Z�[�W��������B<br><br>
334 :       <dl>
335 : 	<dt>int fromindex
336 : 	<dd>�ړ�����A�C�e���̃C���f�N�X�B
337 : 	<dt>int toindex
338 : 	<dd>�ړI�n�̃A�C�e���C���f�N�X�B
339 :       </dl>
340 :       �ړI�n�Ƃ͈ȉ��̒ʂ�B
341 :       <ul>
342 : 	<li>0    �U���A�C�e���𑕔����鏊(��)
343 : 	<li>1    ���𑕔����鏊( �� ) 
344 : 	<li>2    ���𑕔����鏊( �� )
345 : 	<li>3    �Z�𑕔����鏊( �� )
346 : 	<li>4    �C�𑕔����鏊( �� )
347 : 	<li>5    �w�ւ𑕔����鏊( �w )
348 : 	<li>6    �A�N�Z�T���𑕔����鏊( �� )
349 : 	<li>����ȏ�B   �A�C�e����
350 :       </ul>
351 :       ���ʂ�Txt�֐��������āu��낢�𑕔������I�v�̂悤��
352 :       �T�[�o�[���瑗�M����Ă���B�ω������A�C�e���̏󋵂́A
353 :       ItC�֐��ɂ���Ēʒm�����B
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
359 :       �X�L���A�b�v�{�^�����������B<br><br>
360 :       <dl>
361 : 	<dt>int skillindex
362 : 	<dd>�ǂ̃X�L���������邩�B
363 :       </dl>
364 :       ���ʂ́ATxt�֐��������āA�u�ق��ق��̃X�L����1���������I�v
365 :       �̂悤�ɒʒm����B
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
372 :       �A�h���X�u�b�N�̑���Ƀ��b�Z�[�W�𑗐M�Bindex�͎����̃A�h���X�u�b
373 :       �N�̃C���f�N�X�ł���B<br><br>
374 :       <dl>
375 : 	<dt>int index
376 : 	<dd>�A�h���X�u�b�N�ɂ��鑊��̃C���f�N�X�B
377 : 	<dt>string message
378 : 	<dd>����ɑ��M���郁�b�Z�[�W�BEUC�ŃT�[�o�[�ɑ��M����B
379 : 	    ���̕�����̓f���~�^���ӂ��܂Ȃ��̂ŁA�G�X�P�[�v����K�v�͂Ȃ��B
380 : 	<dt>int color
381 : 	<dd>���b�Z�[�W�̐F�B
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
389 :       �A�h���X�u�b�N�̓��e���T�[�o�[�ɖ₢���킹��B
390 :       �����͂Ȃ��B�T�[�o�[�͂��̃R�}���h�������Ƃ�ƃA�h���X�u�b�N�̑S�G���g����
391 :       ���e��Ԃ��B
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
412 :       (DeleteAddressBookitem)�A�h���X�u�b�N�̍��ڂ��폜����B
413 :       ���̃R�}���h�ɂ���ăA�h���X�u�b�N�̓��e���ω�����̂ŁA�T�[�o�[��
414 :       AB�R�}���h�𑗐M���āA�N���C�A���g�ɕ\������Ă���A�h���X�u�b�N�̓��e���X�V����
415 :       �K�v������B
416 :       <dl>
417 : 	<dt>int index
418 : 	<dd>�����������ڂ�index.�ŏ��̍��ڂ�0�ł���B
419 :       </dl>
420 :       �T�[�o�[�����̃R�}���h�̂��߂ɓ��ɕԓ������邱�Ƃ͂Ȃ��B
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
424 :       �ڂ̑O�ɂ���L�����N�^�[���A�h���X�u�b�N�ɉ�����B
425 :       ��������ƃA�h���X�u�b�N�̓��e���ω�����̂ŁA
426 :       �T�[�o�[��AB�R�}���h�𑗐M���ăN���C�A���g�̕\�����X�V����B
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
442 :       (ReceiveAddressBookmessage)���肩��̃��b�Z�[�W����M����v���B
443 :       �A�h���X�u�b�N�̑��肪�����ɂ������ă��b�Z�[�W�𑗐M�����ꍇ�A
444 :       �L���[�ɂ��܂��Ă����B���̃L���[�ɂЂƂȏチ�b�Z�[�W������ꍇ�́A
445 :       NAB�R�}���h�ŃN���C�A���g�ɒʒm�����B�N���C�A���g�ŁA���̃t���O��
446 :       �����Ă��鍀�ڂ̎�M�{�^�����N���b�N����ƁA����RAB�����M�����B
447 :       �T�[�o�[��index�Ŏw�肳��鍀�ڂ̃L���[�Ƀ��b�Z�[�W�����܂��Ă���ꍇ�A
448 :       ���̑��肩��̂��ׂẴ��b�Z�[�W�𑗐M����B
449 :       <dl>
450 : 	<dt>int index
451 : 	<dd>���ڂ��w���C���f�b�N�X�B
452 :       </dl>
453 :       �T�[�o�[�́A���b�Z�[�W���AT�R�}���h�ő��M����B
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
457 :       �`���b�g�p���b�Z�[�W�𑗐M����B�ԓ��͂Ȃ��B<br><br>
458 :       <dl>
459 : 	<dt>string message
460 : 	<dd>���M���镶����B�̍��Ƃ��͂ӂ��߂Ȃ��B�T�[�o�[���ӂ��߂邽�߁B
461 : 	    ���͂�EUC�ɕϊ����đ��M�B
462 : 	    ���̕�����̓f���~�^���ӂ��܂Ȃ��̂ŁA�G�X�P�[�v����K�v�͂Ȃ��B
463 : 	<dt>int color
464 : 	<dd>������̐F�B�F�R�[�h�͖���`�B
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
492 : 	  �}�b�v��v������B���̃R�}���h�̕ԓ���M�R�}���h�ł������Ă���B
493 : 	  <dl>
494 : 		<dt>int fl
495 : 		<dd>�t���A�ԍ�
496 : 		<dt>int x1
497 : 		<dd>����X
498 : 		<dt>int y1
499 : 		<dd>����Y
500 : 		<dt>int x2
501 : 		<dd>�E��X
502 : 		<dt>int y2
503 : 		<dd>�E��Y
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
667 :       �L�����̃X�e�[�^�X��v������B
668 :       <dl>
669 : 	<dt>string category
670 : 	<dd>�ǂ�ȏ�񂪗~�����̂��B���̂ǂꂩ���w��B
671 : 	    <ul>
672 : 	      <LI>P �S�p�����[�^
673 : 	      <LI>C ���W
674 : 	      <LI>I �S�A�C�e��
675 : 	      <LI>S �S�X�L��
676 : 	      <LI>T �S�̍�
677 : 	      <LI>M �����p�����[�^
678 : 		  <LI>G �o���Ă���W�F��
679 : 	    </ul>
680 :       </dl>
681 :       �ǂ�������񂪂������Ă���̂��́A�T�[�o�[����̑��M���Q�ƁB
682 :       ���̕�����̓G�X�P�[�v����Ȃ��B
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
888 :       �N���C�A���g���T�[�o�[�Ƀ��O�C������Ƃ��Ɏg���B�܂�A�J�E���g�̃��O�C
889 :       ���ƍl����΂悢�B�Q�[�����e�Ƀ^�b�`�����ނ̊֐��́A���ׂĂ��̃��O�C��
890 :       �����Ȃ��Ǝ��s����Ȃ��悤�ɂȂ��Ă���B���̊֐��͂����P�ɃN���C�A���g��
891 :       �����Q�[���T�[�o�ɂ����킦�邽�߂����̕��ł���B�܂��A�p�X���[
892 :       �h�ύX������A�ēx���̊֐����ĂԎ��B
893 :       
894 :       <br><br>
895 :       <dl>
896 : 	<dt>string cdkey
897 : 	<dd>CDKEY
898 : 	<dt>string passwd
899 : 	<dd>�p�X���[�h�B���̕�����̓G�X�P�[�v����Ȃ��B
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
914 :       �V�����L�����N�^�[�����B
915 :       <br><br>       
916 :       <dl>
917 : 	<dt>string charname
918 : 	<dd>�L������(�󔒂Ɖ��s���Ȃ��A�v���[���ȕ�����)�B
919 : 	    ���̕�����̓f���~�^���g��Ȃ��̂ŁA�G�X�P�[�v����K�v�͂Ȃ��B
920 : 	<dt>int imgno
921 : 	<dd>�L�����N�^�[�̉摜�ԍ��B
922 : 	<dt>string option
923 : 	<dd>�I�v�V����������B������̓��e�́A�ǂ��������������L�����N�^�[�����邩��
924 : 	    �w�肷��B
925 : 	    �L�����쐬���Ɏw��ł�������́AHP , MP , STR, TOUGH ,�����ăX�L���ł���B
926 : 	    ���̏��͈ȉ��̃��[���łȂ�ׂ�B�܂��A���ꂼ��͂��Ė_'|'�ł�����B
927 : 	    <code>
928 : 	    1|2|3|4|5....N
929 : 	    </code>
930 : 	    ���̗�ł����ƁA��1�g�[�N����1�A��2�g�[�N����2�A�ł���B
931 : 	    ���ɂ��ꂼ��̃g�[�N�������Ӗ����������B
932 : 	    <table border>
933 : 	      <tr><td>token 1</td><td>�ő�HP</td></tr>
934 : 	      <tr><td>token 2</td><td>�ő�MP</td></tr>
935 : 	      <tr><td>token 3</td><td>�ő�STR</td></tr>
936 : 	      <tr><td>token 4</td><td>�ő�TOUGH</td></tr>
937 : 	      <tr><td>token 5</td><td>�ЂƂ߂̃X�L���̃R�[�h</td></tr>
938 : 	      <tr><td>token 6</td><td>�ӂ��߂̃X�L���̃R�[�h</td></tr>
939 : 	      <tr><td>token 4+N</td><td>N�߂̃X�L���̃R�[�h</td></tr>
940 : 	</table>
941 : 	����ɁA�Q�[�����e�I�ɁA�L�����쐬���Ɏw��ł���p�����[�^�̒l�ɂ�
942 : 	���E������B����͎d�l�����Q�Ƃ��邱�ƁB���̕�����̓G�X�P�[�v���Ȃ��B
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
969 :       �L�����������B
970 :       <br><br>
971 :       <dl>
972 : 	<dt>string charname
973 : 	<dd>�L�������B�f���~�^���ӂ��܂Ȃ��̂ŃG�X�P�[�v���Ȃ��B
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
993 :       �L�����N�^�[���T�[�o�[�Ƀ��O�C������Ƃ��Ɏg���B
994 :       <br><br>
995 :       <dl>
996 : 	<dt>string charname
997 : 	<dd>�L�������B�f���~�^���ӂ��܂Ȃ��̂ŁA�G�X�P�[�v���Ȃ��B
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
1013 :       �p�X���[�h�̕ύX������B
1014 :       <br><br>
1015 :       <dl>
1016 : 	<dt>string oldpasswd
1017 : 	<dd>���݂̃p�X���[�h�B
1018 : 	<dt>string newpasswd
1019 : 	<dd>�ύX����p�X���[�h�B�G�X�P�[�v���Ȃ�
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
1034 :       �L�������X�g�𓾂�B
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
1057 :       �L�����N�^�[���T�[�o�[���烍�O�A�E�g����Ƃ��Ɏg���B
1058 :       �����Ȃ��B
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
1074 :       �T�[�o�[�������Ă��邩���e�X�g������A�e�X�g�p�Ɏg���B
1075 :       <br><br>
1076 :       <dl>
1077 : 	<dt>string test
1078 : 	<dd>�G�R�[�p������B�G�X�P�[�v���Ȃ��B
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
108 : 	1�����邭���ƂɃT�[�o�[����N���C�A���g�ɂ������đ��M�����B
109 : 	�����Ă������Ȃ��Ă�1�����Ƃɏ������I��������_�ő��M���Ȃ���΂Ȃ�Ȃ��B
110 : 	���̃R�}���h�͂ǂ�ID�̕����R�}���h�̏����ɂ��Ă̂��̂Ȃ̂�������id
111 : 	�Ŏw�肷��B���Ƃ��΁A4�����܂Ƃ߂Ă��邭�R�}���h���N���C�A���g�����
112 : 	W�R�}���h�ł�����A4�̂��̃R�}���h��0.4�b�܂�1�����邭���ƂɃT�[�o�[����
113 : 	�N���C�A���g�ɑ��M����邱�ƂɂȂ�B�܂����̃R�}���h�͂��ł�
114 : 	�ŐV�̍��W�̏����������B�t���A����������Ƃ��Ƃ��́A�X�e�[�^�X(S)��
115 : 	���W���M�R�}���h(C)���g���B
116 : 
117 : 
118 : 	<dl>
119 : 		<dt>int id
120 : 		<dd>�ǂ̃R�}���h�ɂ���������̂�
121 : 		<dt>int x
122 : 		<dd>�ŐV��X���W
123 : 		<dt>int y
124 : 		<dd>�ŐV��Y���W
125 : 	</dL>	
126 : 	���Ƃ��΁A�N���C�A���g���� abcd�Ƃ����悤�ɕ����R�}���h��ID 4
127 : 	�ł�����A <br>
128 : 	"W 4 1 1" <br>
129 : 	"W 5 1 0"<br>
130 : 	"W 6 1 0"<br>
131 : 	"W 7 1 1"<br>
132 : 	�̂悤��4�̂��̃R�}���h���T�[�o�[���瑗�M����邱�ƂɂȂ�B
133 : 	�T�[�o�[�͂���ID�𑗂邽�߂ɁA���݂ǂ��܂ł�ID���������Ă���̂���
134 : 	�ϐ������K�v�����邾�낤�B�܂��o�b�t�@�����O���K�v�ł��낤�B
135 : 	�o�b�t�@�[�̃T�C�Y�́A32����������Ώ\���ł��낤�B
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
394 :       �T�[�o�[���N���C�A���g�ɑ��M����A�h���X�u�b�N�̑S���e�B
395 :       <dl>
396 : 	<dt>string data
397 : 	<dd>���̓��e�B�\�����邽�߂ɕK�v�ȏ��́A����̃L�����̖��O�A
398 : 	    ���x���A���C�t�̐��A�I�����C�����ǂ����A���b�Z�[�W�����Ă��邩
399 : 	    �ǂ��� �̃t���O�A�ł���B���ꂪ�l�����Ȃ��ł���B
400 : 	    
401 : 	    <code>��l��|��l��|...</code>�l���ɐ����͂Ȃ��B
402 : 	    
403 : 	    ��l���̃��b�Z�[�W�͈ȉ��̍\���ł���B
404 : 	    <code>���O(������)|���x�����l|���C�t���l|��M�t���O�l</code>
405 : 	    ���������āA N�l�ڂ̖��O�ɃA�N�Z�X����ɂ� 4(N-1)+1�Ԗڂ̃g�[�N�����A
406 : 	    ����ׂ�΂悢���ƂɂȂ�B���̃g�[�N�������݂��Ȃ��Ȃ�΁A
407 : 	    ����ȍ~�̃G���g���͑��݂��Ȃ��B
408 : 	    ���O�������<a href="#escaping">�G�X�P�[�v</a>���Ă��炽�Ė_�łȂ��B
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
429 :       (NotifyAddressBookmessage)���肩��̃��b�Z�[�W�����Ă���Ƃ������Ƃ�ʒm����B
430 :       <dl>
431 : 	<dt>string data
432 : 	<dd>���b�Z�[�W�����Ă��邩�ǂ���������킷�t���O�̃��X�g�B
433 : 	    0�Ȃ�΃��b�Z�[�W�����Ă��Ȃ��A1�Ȃ炫�Ă���B
434 : 	    
435 : 	    ���Ƃ��΁A10���ڂ�����2�Ԃ�8�Ԃ̑���Ƀ��b�Z�[�W�����Ă���ꍇ��10���̐��l������B
436 : 	    <code>0010000010</code>30���ڂ�������30���B
437 : 	    ���̕�����̓G�X�P�[�v���Ȃ��B
438 :       </dl>
439 :       �T�[�o�[�͎�M��Ԃɉ����ω����������炱�̃R�}���h�𑗐M����B
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
472 :       �e�L�X�g��\�����邽�߂̃T�[�o�[����̃R�}���h�B�V�X�e���A�i�E��
473 :       �X�Ƃ�NPC�̃Z���t��������g���B<br><br>
474 : 
475 :       <dl>
476 : 	<dt>int charindex
477 : 	<dd>�L�����N�^�[�̃T�[�o�[���C���f�b�N�X�B���̃C���f�b�N�X�����Ƃ�
478 : 	    ���������̕\��������B
479 : 	<dt>string message
480 : 	<dd>������B�`���b�g�͏̍��Ƃ����ӂ��߂đ��M����Ă���B
481 : 	    �����R�[�h��EUC�ő��M����Ă���B
482 : 	    �T�[�o�[�ƃl�b�g���[�N��ɑ��݂��镶���R�[�h�͂��ׂ�EUC�ł���B
483 : 	    ���̕�����̓f���~�^���ӂ��܂Ȃ��̂ŁA�G�X�P�[�v����K�v�͂Ȃ��B
484 : 	<dt>int color
485 : 	<dd>�F�B�F�R�[�h�͖���`�B
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
507 :       �}�b�v�𑗐M����B�ǂ�ȋ�`�ł����M�ł���B�������ő�T�C�Y
508 :       �͂��܂��Ă��Ă悢�B����̓T�[�o�[����̂ݑ��M���A�N���C�A���g
509 :       ���v�����邱�Ƃ͂Ȃ��B���M�̃^�C�~���O�́A�L�����ɂƂ��Ă�����
510 :       ����������������悤�ɂȂ����u�Ԃ�A�n�`���ύX���ꂽ�u�Ԃł���B
511 :       <br><br>
512 :       <dl>
513 : 	<dt>int fl
514 : 	<dd>�L�����̂���t���A�ԍ�
515 : 	<dt>int x1
516 : 	<dd>�t���A�}�b�v�̒��̐�Έʒu�B����X
517 : 	<dt>int y1
518 : 	<dd>�t���A�}�b�v�̒��̐�Έʒu�B����Y
519 : 	<dt>int x2
520 : 	<dd>�t���A�}�b�v�̒��̐�Έʒu�B�E��X
521 : 	<dt>int y2
522 : 	<dd>�t���A�}�b�v�̒��̐�Έʒu�B�E��Y
523 : 	<dt>string data
524 : 	<dd>�t���A�̌����镶����|�^�C��|�I�u�W�F�N�g�ɂȂ��Ă���B
525 : 	    data�̓}�b�v�^�C���ԍ���"76,76,77,78,98,90,1,1,1,2"�Ƃ���
526 : 	    �悤�ɕK�v�ȗv�f������ł���B�X�y�[�X�ł�����ƃG�X�P�[�v
527 : 	    �̊֌W��A�ʂ��������Ȃ�̂ŁA�R���}�ł�����B�K�i�Ȃǂɐi
528 : 	    �񂾏ꍇ�A�S��ʕ��𑗐M���āA������邭�ꍇ�͈����������
529 : 	    ����B�����������f�̓T�[�o�[�ł�邵���Ȃ��B�N���C�A���g��
530 : 	    ���̊֐��Ń}�b�v�������Ƃ�ƁA������f�B�X�N�ɕۑ����āA�I�[
531 : 	    �g�}�b�v�p�̏�����������B���̕�����̓G�X�P�[�v����K�v�͂Ȃ��B
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
538 :       ������͈͂ɂ���I�u�W�F�N�g��L�����̏������ׂăT�[�o�[����N���C�A��
539 :       �g�ɑ��M����B�L�������o��������ړ�������A��Ԃ��ω������炢
540 :       �ł����M����B�N���C�A���g�͂��̊֐���v�����Ȃ��B�󋵂��ω�
541 :       �����Ƃ��̓T�[�o�[�������ő��M����B<br><br>
542 :       ���̏�񂪗L���Ȕ͈͂́A�T�[�o�[�ɐݒ肳��Ă���A�N���C�A���g��
543 :       �����Ă��邾�낤�͈͂ł���B������N���C�A���g�̃v���O�����́A
544 :       �����Ă���͈͂̂��ׂẴL�����N�^�[�ƃA�C�e���̂������̃p�P�b�g
545 :       �Ɋ܂܂�Ă��Ȃ����̂��폜���邱�Ƃ��ł���B�f�t�H���g�ł�
546 :       �����𒆐S�ɂ���11�}�X�l���̃T�C�Y�Ɋւ��ăT�[�o�[�͒��ׂ�`��������B
547 :       
548 :       <dl>
549 : 	<dt>string data
550 : 	<dd>	  �t�H�[�}�b�g�� �ȉ���3��ނ̍��ڂ��R���}�łȂ�ׂ�
551 : 	    ���̂ł���.���ꂼ��̍��ڂ̒��g�͂���ɂ��Ė_'|'�ł������Ă���B
552 : 
553 : 	    
554 : 	    <ul>
555 : 	      <li>���Ė_�ł�����ꂽ�g�[�N����7�̏ꍇ<br>
556 : 		  �g�[�N���̓��e��
557 : 		  <code>CHARINDEX|BASEIMG|LEVEL|NAME|SELFTITLE|WALKABLE|HEIGHT</code>
558 : 		  ����͑��̃v���C���[��NPC��G�ł���B�G�̏ꍇ��SELFTITLE
559 : 		  �͉������ʂȕ���������ăN���C�A���g��1�s�C���t�H�ŕ\�������邱�Ƃ��ł���B
560 : 		  ���̂Ƃ���B�T�[�o�[�����M����Ƃ��͌�����͈͂ɂ��đS
561 : 		  �����M����B�܂�N���C�A���g�́A���̃p�P�b�g���󂯂Ƃ����Ƃ���
562 : 		  ���̃p�P�b�g�ɏ�����Ă��Ȃ��L�����������Ă���������Ă��܂��Ă悢
563 : 		  �Ƃ������Ƃł���B
564 : 		  �܂��A�}�E�X�J�[�\�������킹���Ƃ���
565 : 		  �\��������͂��̏��݂̂Ɋ�Ă���B������A�}�E�X�J�[
566 : 		  �\�������킹���Ƃ��ɕ\�����邱�Ƃ��ύX���ꂽ�ꍇ�́A
567 : 		  �T�[�o�[�́A���̊֐���\���I�ɌĂт����Ȃ���΂Ȃ�Ȃ��B
568 : 		  SELFTITLE�ɂ��ẮA�f���~�^�ł��邽�Ė_���ӂ��܂Ȃ��悤��
569 : 		  �G�X�P�[�v����Ă���B�N���C�A���g�̓A�N�V�����̃R�}���h(CA)������܂ł�
570 : 		  �����ŕ\������B
571 : 		  CHARINDEX�T�[�o�[���̈�ӂɃL���������ł���ԍ��ABASEIMG�͕\���̂��߂�
572 : 		  �ԍ��ALEVEL�̓L�����̃��x��(0�Ȃ�\�����Ȃ��B���̒l��NPC�ȂǂɎg���B)
573 : 		  WALKABLE��1�̂Ƃ����̏��ʉ߂��邱�Ƃ��ł��A0�Ȃ�ʉ߂��邱�Ƃ��ł��Ȃ��B
574 : 		  HEIGHT�͍����������̂������łȂ��̂��̎w��B
575 : 		  
576 : 		  �L�����N�^�[�̖��O�Ǝ��R�̍��́A<a href="#escaping">�G�X�P�[�v</a>
577 : 		  ����Ȃ���΂Ȃ�Ȃ��B
578 : 		  '|'�Ńg�[�N������肾���Ă���G�X�P�[�v����������B
579 : 		  �G�X�P�[�v�����'|'���ق��̕����ɂ��������̂ŁA�ŏ��͒P����'|'��
580 : 		  �f���~�^�Ƃ��Ă悢�B
581 : 		  ���M��������A
582 : 		  ���O�Ǝ��R�̍����G�X�P�[�v���Ă��炽�Ė_�łȂ��ł��瑗�M����B
583 : 		  
584 : 	      <Li>���Ė_�ł�����ꂽ�g�[�N����4�̏ꍇ<br>
585 : 		  �g�[�N���̓��e��
586 : 		  <code>X|Y|BASEIMG|ITEM1LINEINFO</code>
587 : 		  �Œn�ʂɗ����Ă���A�C�e���ɂ��Ă̏��ł���B
588 : 		  X,Y�̓A�C�e���̃O���[�o���ʒu�B
589 : 		  BASEIMG�͉摜�̔ԍ��BITEM1LINEINFO��1�sINfo�ɕ\�����邽�߂�
590 : 		  ���ł���B�A�C�e���E�C���h�E���̕\���p�̏��͕ʂ̕��@�ŗp�ӂ���B
591 : 		  �A�C�e���Ɋւ��Ă�CA�͗��Ȃ��BITEM1LINEINFO�̓G�X�P�[�v�����B
592 : 		  ���̃G�X�P�[�v�̕��@�͏�̍��ڂ��Q�ƁB
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
603 :       ������͈͂ɂ���L�����̃A�N�V������Ԃ��X�V����B
604 :       �T�[�o�[����N���C�A���g�Ɉ���I�ɑ��M����B
605 :       �e�L������1�A�N�V�������Ƃɑ��M�B�T�[�o�[�̓A�N�V�������ł��邾
606 :       �����k���邱�ƁB<br><br>
607 : 
608 :       <dl>
609 : 	<dt>string data
610 : 	<dd>CHARINDEX|X|Y|ACTION|PARAM1|PARAM2|PARAM3|PARAM4|....���R
611 : 	    ���}�ł����������̂ɂ���BPARAM�̓A�N�V�������ƂɌ���
612 : 	    �g�����������ƂȂ�B�ȉ��̓A�N�V�����ꗗ�BX,Y�͈ʒu�ł�
613 : 	    �̃A�N�V�����ł��A�����炵���ʒu���w�肷��B
614 : 	    ���̕�����̓G�X�P�[�v����Ȃ��B
615 : 	    <table border>
616 : 	      <tr><td>ACTION</td><td>PARAM1</td><td>PARAM2</td><td>PARAM3</td><td>PARAM4</td></tr>
617 : 	      <tr><td>Stand:0</td><td>����0~7</td><td></td><td></td><td></td></tr>
618 : 	      <tr><td>Walk:1</td><td>����0~7</td><td></td><td></td><td></td></tr>
619 : 	      <tr><td>Attack:2</td><td>����0~7</td><td></td><td></td><td></td></tr>
620 : 	      <tr><td>ThrowItem:3</td><td>����0~7</td><td></td><td></td><td></td></tr>
621 : 	      <tr><td>Damage:4</td><td>����0~7</td><td>�G�t�F�N�g�ԍ�</td><td>�G�t�F�N�g�ɕK�v�Ȑ��l</td><td></td></tr>
622 : 	      <tr><td>Dead:5</td><td>����0~7</td><td>�G�t�F�N�g�ԍ�</td><td>�G�t�F�N�g�ɕK�v�Ȑ��l</td><td></td></tr>
623 : 	      <tr><td>UseMagic:6</td><td>����0~7</td><td>�G�t�@�N�g�ԍ�</td><td>�G�t�F�N�g�ɕK�v�Ȑ��l</td><td></td></tr>
624 : 	      <tr><td>UseItem:7</td><td>����0~7</td><td>�G�t�F�N�g�ԍ�</td><td>�G�t�F�N�g�ɕK�v�Ȑ��l</td><td></td></tr>		
625 : 	</table>
626 : 	ACTION�̒l�͐����ŁA���e�͏�̕\�̍��[�̍��ڂł���B
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
635 :       ���[�_�[�̓��e���N���C�A���g�ɑ��M����B�N���C�A���g�͂��̑��M��
636 :       �v�����邱�Ƃ͂Ȃ��B�T�[�o�[���K���ȃ^�C�~���O�ő��M����B
637 :       ���Ƃ���10�����邭���ƂƂ��A1�����ƂƂ��B<br><br>
638 : 
639 :       <dl>
640 : 	<dt>int result
641 : 	<dd>0 �͐���I���B���̓G���[
642 : 	<dt>string data
643 : 	<dd>x ,y, kind ,�̏��ɒl��'|'�ł������ĂȂ�ׂ����̂�
644 : 	    �����'|'�łȂ�ׂ����́B
645 : 	    x,y�̓L��������̑��Έʒu�Bkind�͈ȉ��ɂ��߂����l�ł͂Ȃ��L���B
646 : 	    ���[�_�[�ɂ��镨�̎�ނ��w�肷��B
647 : 	    �����Ă�����̂̎�ނ́A
648 : 	    ����܂łɏ��������@�Ƃ��A�X�L���ŕω����A����̓T�[�o�[
649 : 	    ���l����B
650 : 	    <table border>
651 : 	      <tr><td>kind�̒l</td><td>���e</td></tr>
652 : 	      <tr><td>E</td><td>�G</td></tr>
653 : 	      <tr><td>P</td><td>�v���C���[</td></tr>
654 : 	      <tr><td>D</td><td>�h�A</td></tr>
655 : 	      <tr><td>N</td><td>���̂ق���NPC(�ŔƂ�)</td></tr>		  
656 : 	      <tr><td>L</td><td>������K�i</td></tr>
657 : 	      <tr><td>W</td><td>�����Ȃ����[�v�|�C���g</td></tr>
658 : 	</table>
659 :   </dl>
660 :   data�̗�F<code>"12|22|E|13|24|P|14|28|P"</code>
661 :   ���̕�����̓G�X�P�[�v����Ȃ��B
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
686 :       �L�����̃X�e�[�^�X�𑗐M����B
687 :       �f�[�^�� �J�e�S���L������(�ꕶ��)���e �ƂȂ��Ă���B�܂�ŏ���
688 :       1����������Ή��̃X�e�[�^�X������B���e��2�����ڂ���ł���B
689 :       ���e�͈ȉ��̃t�H�[�}�b�g�ɂ��������B���Ė_�L��'|'���f���~�^�ł���B
690 :       2�ڈȍ~�̃g�[�N�������e�ł���B
691 :       <UL>
692 : 	<LI>P �S�p�����[�^<br><br>
693 : 	    hp maxhp mp maxmp str tough exp talklevel level soul attackpower
694 : 	    deffencepower class gold �t���Ă���̍���index ���O ���ȏ�
695 : 	    �� status<br>
696 : 	    
697 : 	    �f���~�^�� '|' �ł���B�܂����O�Ǝ��ȏ̍��́A
698 : 	    <a href="#escaping">�G�X�P�[�v</a>�������̂�
699 : 	    ���Ė_�łȂ������̂��ʐM�����B<br>
700 : 	    ���ꂼ��̒l�̌^�͈ȉ��B
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
714 : 	      <tr><td>class</td><td>int</td><td>�l�̈Ӗ� 0:�N���XA 1:B
715 : 		  2:C 3:D</td></tr>
716 : 		  <tr><td>gold</td><td>int</td></tr>
717 : 		  <tr><td>�̍���index</td><td>int</td></tr>
718 : 		  <tr><td>���O</td><td>������</td></tr>
719 : 		  <tr><td>���ȏ̍�</td><td>������</td></tr>
720 : 		  <tr><td>status</td><td>������</td></tr>
721 : 	    </table>
722 : 	    �o���l��double�ł��������Aint �ɕύX����B�o���l�̓�����
723 : 	    �ʂ́A�����ƓG�̃��x���ɂ���ĕω����鎖�ɂ���( Diablo ��
724 : 	    �悤�� )status , ���ȏ̍��ȊO�͂��ׂĐ����l�B��Ԃ͈ȉ���
725 : 	    �������Ȃ�ׂ�����
726 : 	    <ul>
727 : 	      <LI>P �ŁB
728 : 	      <LI>N Ⴢ�
729 : 	      <LI>Q ����
730 : 	      <LI>S ��
731 : 	      <LI>D �È�
732 : 	      <LI>C ����
733 : 	    </ul>
734 : 	    ��̗�( char �̔z�񂻂̂܂܏��� )<br>
735 : 	    <code>
736 : 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|�ւ��|PC
737 : 	    </code>
738 : 	<li>C ���W<br><br>
739 : 	    floor x y<br>
740 : 	    ��  | �ŋ�؂��đ���B<br>
741 : 	    ��̗�( char �̔z�񂻂̂܂܏��� )<br>
742 : 	    <code>
743 : 	    C1024|10|20
744 : 	    </code>
745 : 	    ��L�̗�ł́A�t���AID1024 , X 10,Y 20�̂Ƃ���ɃL�����N�^�[������
746 : 	    �Ƃ������e�ɂȂ�B���̃}�C�L�����̃t���A���ӂ��߂����W���N
747 : 	    ���C�A���g���m�邱�Ƃ��ł���̂͂��̃R�}���h�݂̂ł���B��
748 : 	    ���̌��ʂ̓t���A���̈ʒu�����킩��Ȃ��B����āA�Q�[���J�n
749 : 	    ���A�K�i�g�p�̂Ƃ��A���[�v�̂Ƃ��Ȃǂ́A���̃R�}���h������
750 : 	    �Đ������l�𑗐M���Ȃ��Ă͂Ȃ�Ȃ��B
751 : 	    
752 : 
753 : 	<LI>I �S�A�C�e��<br><br>
754 : 	    (�A�C�e��1)|(�A�C�e��2)...(�A�C�e��n)<br>
755 : 	    �A�C�e���̓��e�͈ȉ��̕��@�Ńp�b�N����B
756 : 	    �ЂƂ̃A�C�e���́A���Ȃ炸�ȉ���4�̃g�[�N���̃Z�b�g��
757 : 	    ���M�����B�ЂƂЂƂ̓��e�́A<br>
758 : 	    ���O|���O2|�X�e�[�W|����|�A�j���ԍ�<br>
759 : 	    <ul>
760 : 	      <li>�A�C�e�����͎��ʃ��x���Ŏ����I�ɕύX�����B
761 : 	      <li>���O2�͎��ʃ��x���ɂ���āA�A�C�e���̔\�͂�����\��B
762 : 		  �N���C�A���g�̃A�C�e������2�s�ڂɓ���鎖�B
763 : 	      <li>�X�e�[�W�͖��O�̐F��ւ�̂Ɏg���B
764 : 	      <li>�����̓����B
765 : 	      <li>�A�j���ԍ��́A�摜�ԍ��B
766 : 	    </ul>
767 : 	    �����̒��ł́A'|' �ƁA'\'  �����o�b�N�X���b�V���ŃG�X�P�[
768 : 	    �v�����B<br>
769 : 	    ��̗�( char �̔z�񂻂̂܂܏��� )<br>
770 : 	    <code>
771 : 	    I���� �� �q|str+2|1|����\|����|10
772 : 	    </code>
773 : 	    �X�e�[�W�̒l�̈Ӗ��́A�����l��
774 : 	    <table border>
775 : 	      <tr><td>1</td><td>�X�e�[�WA</td></tr>
776 : 	      <tr><td>2</td><td>�X�e�[�WB</td></tr>	      
777 : 	      <tr><td>3</td><td>�X�e�[�WC</td></tr>
778 : 	      <tr><td>4</td><td>�X�e�[�WD</td></tr>
779 : 	    </table>
780 : 	    �Ƃ���B�܂��A�A�C�e���\�̋�̕����ɂ��ẮA4�̃g�[�N����
781 : 	    ��ŁA�܂肽�Ė_���Ȃ�񂾏�Ԃő��M����Ă���B
782 : 
783 : 	<LI>S �S�X�L��<br><br>
784 : 	    (�X�L��0)|(�X�L��1)|(�X�L��2) ... (�X�L��n)<br>
785 : 	    �̍��Ɠ����ŁA�L���������Ă�ő�X�L�������𑗐M����B��
786 : 	    ��Ȃ����̂� ||| �ƂȂ�̂ł���B
787 : 	    �e�G���g���̓��e�́A<br>
788 : 	    �X�L���̎�ރR�[�h|���x��<br>
789 : 	    �ł���B<br>
790 : 	    ��̗�( char �̔z�񂻂̂܂܏��� )<br>
791 : 	    <code>
792 : 	    S10|2|20|2|||
793 : 	    </code>	    
794 : 
795 : 	<LI>T �̍�<br><br>
796 : 	    (�̍�0)|(�̍�1)|(�̍�2) ... (�̍�n)<br><br>
797 : 	    �̍��̃G���g������̏ꍇ�� '|' ���A������̂ŁA�A��������
798 : 	    �̂��ȗ����ăX�L�������Ă͂Ȃ�Ȃ��B���Ȃ炸�ő�������M
799 : 	    ����B<br>
800 : 	    �ЂƂA�ЂƂ̓��e�́A<br>
801 : 	    ���O<br>
802 : 	    �݂̂ł���B<br>
803 : 	    ��̗�( �����Ă��� char �̔z�񂻂̂܂܂ł��� )<br>
804 : 	    <code>
805 : 	    T����|����ς炢
806 : 	    </code>
807 : 
808 : 	<LI>M �����p�����[�^<br><br>
809 : 	    �L�����N�^�[�̃X�e�[�^�X�̂����A�p�ɂɕύX��������(���ݒl)�Ƃ����łȂ�����(�ő�l�Ȃ�)
810 : 	    ������B�ǂꂩ�̃X�e�[�^�X���ύX����邽�тɖ���S���̃p�����[�^�𑗐M����̂�
811 : 	    �l�b�g���[�N�g���t�B�b�N�𖳑ʂɎg�����ƂɂȂ�̂ŁA����HP , MP,EXP�̒l�����ɂ��ڂ���
812 : 	    �p�����[�^���M�R�}���h��p�ӂ���B���ꂪ����M�R�}���h�ł���B
813 : 	    2�����ڈȍ~�̑��g�[�N����HP�̌��ݒl , ���g�[�N����MP�A��O�g�[�N����EXP�ł���B
814 : 	    �ȉ��ɋ�̗�����߂��B
815 : 	    <code>
816 : 	    M54|210|8944909
817 : 	    </code>
818 : 	    ���̗�ł�HP�̌��ݒl��54�AMP�̌��ݒl��210�A�o���l��8944909�ɂȂ��Ă���̂��B
819 : 	    ����3��ނ͓��ɕύX�p�x�������Ǝv���邽�ߓƗ������R�}���h���������A
820 : 	    ������񂱂�M�R�}���h�̂�����P�R�}���h�őS�p�����[�^�𑗐M���Ă��悢�B
821 : 	    �������A�œK�Ƀl�b�g���[�N���g�����߂ɂ́A���̃R�}���h���g�����Ƃ����������B
822 : 	    �o���l�̌^��double�Ő����ł���B
823 : 	    
824 : 	<LI>G �W�F�����X�g<br><br>
825 : 		Lifestorm2�ł̓W�F�����o���Ă���ȍ~���̃W�F���͎g������ɂȂ�
826 : 		���A���̃R�}���h�ł͂ǂ̃W�F�����o���Ă���̂���`����B
827 : 		���̃R�}���h�����M�����^�C�~���O�́A�Q�[���X�^�[�g���ƁA
828 : 		�o���Ă���W�F���ɕω����������Ƃ������ł���B<br>
829 : 		�V���^�b�N�X�̓W�F���̎��+1�����̌v13�����Œ�B<br>
830 : 		<code>Gabcdefghijkl</code><br>
831 : 		2�����ڂ���13�����ڂ܂ł́A���ꂼ��
832 : 		<table border>
833 : 		<tr><td>2������</td><td>LIFE</td></tr>
834 : 		<tr><td>3������</td><td>BLAZE</td></tr>
835 : 		<tr><td>4������</td><td>AIR</td></tr>
836 : 		<tr><td>5������</td><td>ELECT</td></tr>
837 : 		<tr><td>6������</td><td>LIGHT</td></tr>
838 : 		<tr><td>7������</td><td>ICE</td></tr>
839 : 		<tr><td>8������</td><td>INTELI</td></tr>
840 : 		<tr><td>9������</td><td>METAL</td></tr>
841 : 		<tr><td>10������</td><td>HOLY</td></tr>
842 : 		<tr><td>11������</td><td>EVIL</td></tr>
843 : 		<tr><td>12������</td><td>DEATH</td></tr>
844 : 		<tr><td>13������</td><td>SPACE</td></tr>
845 : 		</table>
846 : 		���Ή����Ă��āA�����Ă���ꍇ�͂��̕�����1�A�����Ă��Ȃ��ꍇ
847 : 		��0�ɂȂ�B�܂���
848 : 		<code>G111010111000</code>
849 : 		�̂悤�ɂȂ�B�����W�F���̎�ނ�������\�������邪�A���̂Ƃ�
850 : 		�͌����𑝂₵�đΉ�����B
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
857 :       ��ʂɉ����\������w�߁B<br><br>
858 : 
859 :       <dl>
860 : 	<dt>int category
861 : 	<dd>����\�����邩�B
862 : 	    <table border>
863 : 	      <tr><td>�l</td><td>���e</td></tr>
864 : 	      <tr><td>1</td><td>�����ȊO�ɗ^�����_���[�W�Bdata�͕���
865 : 		  ��ɂȂ����l�B</td></tr>
866 : 		  <tr><td>2</td><td>�������������_���[�W�Bdata�͕������
867 : 		      �Ȃ����l</td></tr>
868 : 		</table>
869 : 	<dt>int dx
870 : 	<dd>�}�C�L��������̃O���b�h���ΈʒuX�B��ʓI�ɂ̓C�x���g������
871 : 	    �����ʒu�B�N���C�A���g�͂��̒l����K�X�K�؂Ȉʒu���v�Z����
872 : 	    �`�悷��B
873 : 	<dt>int dy
874 : 	<dd>���ΈʒuY
875 : 	<dt>string data 
876 : 	<dd>�\��������e�B���e��category�ɂ���Č��܂�B
877 : 	    ���̕������<a href="#escaping">�G�X�P�[�v</a>����Ȃ���΂Ȃ�Ȃ��B
878 :       </dl>
879 :       <br>
880 :       <br>
881 : 
882 :       <hr>
883 : 
884 : 
885 :       <!-- �������牺�̓L�����쐬�ȂǃQ�[���O�̊֐�   -->
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
903 :       ClientLogin�̕ԓ��B
904 :       <br><br>
905 :       <dl>
906 : 	<dt>string result
907 : 	<dd>"ok" �Ƃ���������B���̕�����̓G�X�P�[�v����Ȃ��B
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
948 :       CreateNewChar�̕ԓ��B
949 :       <br><br>
950 :       <dl>
951 : 	<dt>string result
952 : 	<dd>"successful" �� "failed" �̂����ꂩ�B���̕�����̓G�X�P�[�v���Ȃ��B
953 : 	<dt>string data
954 : 	<dd>"failed" �̎��͗��R�������l�Ԃ̌��ĕ��镶��
955 : 	    ��ł���B�A�J�E���g�T�[�o����̕ԓ����̂܂܂ł���B
956 : 	    �ȉ��̕�����
957 : 	    <pre>
958 : 	    "failed bad parameter"
959 : 	    </pre>
960 : 	    �̏ꍇ�́A�L�����쐬�̂Ƃ��ɋK��̃p�����[�^�͈̔͂��z���Ă���Ƃ���
961 : 	    ���Ƃ��Ӗ�����B����̓Q�[���T�[�o�[���o�͂��郁�b�Z�[�W�ł���B
962 : 	    ���̕�����̓G�X�P�[�v���Ȃ��B
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
978 :       CharDelete �̕ԓ��B
979 :       <br><br>
980 :       <dl>
981 : 	<dt>string result
982 : 	<dd>"successful" �� "failed" �̂����ꂩ�B�G�X�P�[�v���Ȃ��B
983 : 	<dt>string data
984 : 	<dd>"failed" �̎��͗��R�������l�Ԃ̌��ĕ��镶��
985 : 	    ��ł���B�A�J�E���g�T�[�o����̕ԓ����̂܂܂ł���B
986 : 	    �f���~�^���ӂ��܂Ȃ��̂ŁA�G�X�P�[�v���Ȃ��B
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
1000 :       CharaLogin�̕ԓ��B
1001 :       <br><br>
1002 :       <dl>
1003 : 	<dt>string result
1004 : 	<dd>"successful" �� "failed" �̂����ꂩ�B�G�X�P�[�v���Ȃ��B
1005 : 	<dt>string data
1006 : 	<dd>"failed" �̎��� ���̗��R�̕�����B�G�X�P�[�v���Ȃ��B
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
1022 :       ChangePasswd�̕ԓ��B
1023 :       <br><br>
1024 :       <dl>
1025 : 	<dt>string result
1026 : 	<dd>"successful" �� "failed" �̂����ꂩ�B�G�X�P�[�v���Ȃ�
1027 : 	<dt>string data
1028 : 	<dd>"failed" �̎��͂��̗��R�̕�����B�G�X�P�[�v���Ȃ�
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
1037 :       CharList�̕ԓ��B
1038 :       <br><br>
1039 :       <dl>
1040 : 	<dt>string result
1041 : 	<dd>"successful" �� "failed" �̂����ꂩ�B�G�X�P�[�v���Ȃ�
1042 : 	<dt>string data
1043 : 	<dd>result��"successful"�̎��́A�A�J�E���g�T�[�o�[�ɕۑ�����Ă��邷
1044 : 	    �ׂẴL�����̖��O�A�I�v�V���������X�y�[�X�ŋ�؂������
1045 : 	    ������Bresult �� "failed" �̎��͗��R�������l�Ԃ̌��ĕ���
1046 : 	    ������ł���B
1047 : 	    �I�v�V�����̒��g�͈ȉ��̂Ƃ���ł���B
1048 : 	    level|showstring|���̐�|�摜�ԍ�|���񃍃O�C��������|�N���X
1049 : 	    �� "|" �ŋ�؂��Ă���B ���ꂼ��̍��ڂ́A
1050 : 	    <a href="#escaping">�G�X�P�[�v</a>����Ă���B
1051 : 	    ���̂��Ƃ��Ė_�łȂ���B
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
1061 :       Logout�ɑ΂���ԓ��B
1062 :       <br><br>
1063 :       <dl>
1064 : 	<dt>string result
1065 : 	<dd>"successful" �� "failed" �̂����ꂩ�B�G�X�P�[�v���Ȃ��B
1066 : 	<dt>string data
1067 : 	<dd>"failed" �̎��ɂ݈̂Ӗ�������A���s�̗��R(���)�������l��
1068 : 	    �̌��ĕ��镶����ł���B�G�X�P�[�v���Ȃ��B
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
1082 :       Echo�ɑ΂���ԓ��B
1083 :       <dl>
1084 : 	<dt>string test
1085 : 	<dd>�����قǓ��͂��ꂽ������B�G�X�P�[�v���Ȃ��B
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
