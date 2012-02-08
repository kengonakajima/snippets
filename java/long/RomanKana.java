
// かな変換をするクラス。英文字列を入れると、intの配列を返してくれるような
// メソッドがひとつあったらよい。


import java.awt.*;
import java.applet.Applet;
import java.io.*;

class RomanKana
{

	/* ユニコードの表、ひらがなとかたかなを256文字にまとめた。 */

	char chars[] = {
		' ',' ',' ',' ', ' ',' ',' ',' ', ' ',' ',' ',' ', ' ',' ',' ',' ',
		' ',' ',' ',' ', ' ',' ',' ',' ', ' ',' ',' ',' ', ' ',' ',' ',' ',
		' ','!','\"','#', '$','%','&','\'', '(',')','*','+', ',','-','.','/',
	
		'0','1','2','3', '4','5','6','7', '8','9',':',';', '<','=','>','?',
		'@','A','B','C', 'D','E','F','G', 'H','I','J','K', 'L','M','N','O',

		'P','Q','R','S', 'T','U','V','W', 'X','Y','Z','[', '\\',']','^','_',
		'`','a','b','c', 'd','e','f','g', 'h','i','j','k', 'l','m','n','o',
		'p','q','r','s', 't','u','v','w', 'x','y','z','{', '|','}','~',' ',

		'\u3042','\u3044','\u3046','\u3048','\u304a', /* a */
		'\u304b','\u304d','\u304f','\u3051','\u3053', /* ka */
		'\u3055','\u3057','\u3059','\u305b','\u305d', /* sa */
		'\u305f','\u3061','\u3064','\u3066','\u3068', /* ta */
		'\u306a','\u306b','\u306c','\u306d','\u306e', /* na */ 
		'\u306f','\u3072','\u3075','\u3078','\u307b', /* ha */
		'\u307e','\u307f','\u3080','\u3081','\u3082', /* ma */
		'\u3084','\u3086','\u3088',  /* ya */
		'\u3089','\u308a','\u308b','\u308c','\u308d', /* ra */
		'\u308f','\u3092','\u3093', /* wa */
		'\u3041','\u3043','\u3045','\u3047','\u3049', /* small a */
		'\u3063','\u3083','\u3085','\u3087',  /* tt ya yu yo */

		'\u30a2','\u30a4','\u30a6','\u30a8','\u30aa', /* a */
		'\u30ab','\u30ad','\u30af','\u30b1','\u30b3', /* ka */
		'\u30b5','\u30b7','\u30b9','\u30bb','\u30bd', /* sa */
		'\u30bf','\u30c1','\u30c4','\u30c6','\u30c8', /* ta */
		'\u30ca','\u30cb','\u30cc','\u30cd','\u30ce', /* na */
		'\u30cf','\u30d2','\u30d5','\u30d8','\u30db', /* ha */
		'\u30de','\u30df','\u30e0','\u30e1','\u30e2', /* ma */
		'\u30e4','\u30e6','\u30e8', /* ya */
		'\u30e9','\u30ea','\u30eb','\u30ec','\u30ed', /* ra */
		'\u30ef','\u30f2','\u30f3', /* wa */
		'\u30a1','\u30a3','\u30a5', '\u30a7','\u30a9', /* small a */
		'\u30c3','\u30e3', '\u30e5','\u30e7', /* tt ya yu yo */

		'\u309b','\u309c',


		'\u3001','\u3002',
		'\u30fc','\u300c','\u300d',
		'*','*','*','*', '*','*','*','*', '*','*','*'


	};



	int counter=0;
	int [] buf = new int[10];

	public final int eisuu_mode = 0;
	public final int hiragana_mode = 128;
	public final int katakana_mode = 183;
	public int mode = hiragana_mode;
	int moji = 0;
	int y_flg = 0;
	
	int result[] = new int[256];
	int result_pos = 0;

	RomanKana()
	{
		init();

	}

	void init()
	{
		//まず初期化する。
		counter=0;
		for(int i = 0 ; i < 10 ; i++ ) buf[i] = 0;
		for(int i = 0 ; i < 256 ; i++) result[i] =0;
		mode = hiragana_mode;
		moji = 0;
		y_flg = 0;
		result[0] = 0;
		result_pos = 0;
	}
	public String convert( String input )
	{

		byte b[] = new byte[ input.length() + 1 ];
		input.getBytes( 0 , input.length() , b , 0 );
		int itmp[]  = new int[ input.length() + 1];
		for(int i = 0 ; i < input.length() ; i++ ){
			itmp[i] = ( int ) b[i];

		}
		return convert( itmp );
		
	}

	public String convert( int[] input )
	{
		int lastc=0;
		int len;

		init();

		for(int i = 0 ; ; i++ ){
			if(  input[i] == 0 ){
				if( i != 0 ) lastc = input[i-1];
				len = i;
				break;
			}
			putIntoBuf(  input[i] );
		}
		/* ここまでで、int型の配列になるから、それをStringに変換 */
		/* 最後が子音だったら、その文字も最後につける */
		char ca[]  = new char[257];
		int i;

		for(i = 0 ; ;i++){
			if( result[i] == 0 )break;
			ca[i] = chars[result[i] ];
			ca[i+1] = 0;
		}
		/*
		if( lastc == 'k' || lastc == 's' || lastc == 't' || lastc == 'n' ||
		    lastc == 'h' || lastc == 'm' || lastc == 'y' || lastc == 'r' ||
		    lastc == 'w' || lastc == 'g' || lastc == 'b' || lastc == 'p' ||
		    lastc == 'd' || lastc == 'x' || lastc == 'l'){
			ca[i] = chars[ lastc ];
			ca[i+1] = 0;
		}
		*/
		for(int j=0;j < counter;j ++ ){
			ca[i+j] = (char)input[len - counter + j];
			ca[i+j+1] = 0;
		}

		String s = new String( ca );
		
		return s;

	}
	
	// モードを変える。
	void changeMode()
	{
		if( mode == eisuu_mode ){
			mode = hiragana_mode;
		} else if( mode == hiragana_mode ){
			mode = katakana_mode;
		} else {
			mode = eisuu_mode;
		}
		counter=0;
	}
	
	// 一文字をバッファに入れる。
	void putIntoBuf( int c )
	{


		buf[counter] = c;
		int bo;

		switch( c ){

		case '/':
			if( mode == eisuu_mode){
				mode = hiragana_mode;
			} else if( mode == hiragana_mode ){
				mode = katakana_mode;
			} else {
				mode = eisuu_mode;
			}
			counter = 0;
			break;
		case ' ':
			addResult( 32 );
			counter=0;
			break;
		case 10:
			addResult( 10 );

			break;
		default:
			switch( mode ){
			case eisuu_mode:
				addResult( c );

				break;
			default:
				switch( c )
				{
				case 'a':
					bo=0;break;
				case 'i':
					bo=1;break;
				case 'u':
					bo=2;break;
				case 'e':
					bo=3;break;
				case 'o':
					bo=4;break;
				default:
					bo=-1;
					moji = 0;
					switch( c ){
					case ']':
						moji++;
					case '[':
						moji++;
					case '-':
						moji++;/* 「ー」 */
					case '.':
						moji++;/* 「。」 */
					case ',':
						moji++;/* 「、」 */
					case ':':
						moji++;/* 「゜」 */
					case ';':
						moji += 238;/* 「゛」 */
						addResult( moji );

						counter = 0;
						break;
					
					default:
						if(c<'a' || c>'z'){
							addResult( c );
							counter = 0;
						}
						break;
					}
				}
				if( bo != -1 ){
					switch( counter){
					case 3:
						if( buf[3] == 'u' ){
							addResult( mode + 51 );
						}
						counter = 0;
						break;
					case 0:
						counter=0;
						addResult( mode+bo );

						break;
					case 2:
						if( buf[0] == 'x' && buf[1] == 't' && buf[2] == 'u' ){
							addResult( mode + 51 );
							counter = 0;
							break;
						}
						y_flg = 1;
						if( buf[1] == 'h' ) y_flg = 2;
						if( buf[1] == 'y' && buf[2] == 'i') y_flg = 0;
						if( buf[1] == 'h' && buf[2] == 'e') y_flg = 0;
					case 1:
						//子音の後の母音。
						int s=0;
						int dakuten=0;
						int komoji=0;
						switch( buf[0] ){
						case 'g':
							dakuten=1;
						case 'k':
							s=5;break;
						case 'z':
							dakuten=1;
						case 's':
							s=10;break;
						case 'd':
							dakuten=1;
						case 't':
							s=15;break;
						case 'n':
							s=20;break;
						case 'p':
							dakuten=1;
						case 'b':
							dakuten++;
						case 'h':
							s=25;break;
						case 'm':
							s=30;break;
						case 'y':
							s=35;
							if( bo == 1 || bo == 3 ){
								s=0;
							} else if( bo == 2 ){
								s=34;
							} else if( bo == 4 ){
								s=33;
							}
							break;
						case 'r':
							s=38;break;
						case 'w':
							s=43;
							if( bo == 4 ){
								s=40;
							} else if( bo == 1 || bo == 2 || bo == 3 ){
								s=0;
							}
							if( y_flg == 1)s=0;
							break;
						case 'f':
							s=25;komoji=1;
							if(bo == 2) komoji=0;
							break;
						case 'j':
							s=10;y_flg=1;dakuten=1;
							if(bo == 1) y_flg=0;
							break;
						case 'q':
							s=5;komoji=1;
							if(bo == 2) komoji=0;
							break;
						case 'v':
							s=0;dakuten=1;komoji=1;
							if(bo == 2) komoji=0;
							break;
						case 'x':
							s=46;
							y_flg = 0;
							if( buf[1] == 'y' ){
								s = 52;
								if( bo == 1 || bo == 3 ){
									s=46;
								} else if( bo == 2 ){
									s=51;
								} else if( bo == 4 ){
									s=50;
								}
							}								
						}
						counter=0;
						switch( y_flg ){
						case 1:
							y_flg = bo + 1;
							bo = 1;
							break;
						case 2:
							y_flg = bo + 1;
							bo = 3;
							break;
						}
						if(komoji > 0){
							komoji = bo + 1;
							bo = 2;
						}
						addResult( mode + s + bo );

						if(dakuten > 0){
							addResult( 237 + dakuten );

						}
						if( komoji > 0){
							addResult( mode + komoji + 45 );

						}
						bo = 0;
						switch( y_flg ){
						case 5:bo = 1;
						case 3:bo += 1;
						case 1:bo += 3;
						case 4:bo += 2;
						case 2:bo += 47;
							addResult( mode + bo );

							y_flg = 0;
							break;
						}
					}
				} else{
					switch( counter ){
					case 0:
						if( buf[0] >= 'a' && buf[0] <= 'z' ) counter++;
						break;
					case 1:
						if(buf[0] == 'n' && buf[1] != 'y' && buf[1] != 'n'){
							addResult( mode + 45);
							counter = 1;
							buf[0] = buf[1];
						} else if(buf[0] == buf[1]){
							switch( buf[1] ){
							case 'n':
								addResult( mode + 45 );

								counter = 0;
								break;
							case 'g':
							case 'k':
							case 'z':
							case 's':
							case 'd':
							case 't':
							case 'p':
							case 'b':
							case 'h':
							case 'm':
							case 'y':
							case 'r':
							case 'w':
							case 'f':
							case 'j':
							case 'q':
							case 'v':
							case 'c':
								addResult( mode + 51 );

								break;
							}
						} else {
							if( buf[0] == 't' && buf[1] == 's' ){
								buf[1] = 't';
								counter = 1;
							}
							if( buf[0] == 'c' && buf[1] == 'h' ){
								buf[0] = 't';
								buf[1] = 'y';
							}
							if( buf[0] == 's' && buf[1] == 'h' ){
								buf[1] = 'y';
							}
							switch( buf[1] ){
							case 'y':
							case 'h':
								counter++;break;
							default:
								buf[0]=buf[1];break;
							}
						}
						break;
					case 2:
						if( buf[0] == 'x' && buf[1] == 't' && buf[2] == 's')
							break;
						counter = 1;
						buf[0] = buf[2];
						break;
					case 3:
						counter = 1;
						buf[0] = buf[3];
						break;
					}
				}
				
			}
		}	       
	}
	
	void addResult( int c )
	{
		result[result_pos++] = c;
		
	}


}



