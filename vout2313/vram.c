//---------------------------------------------------------------------
// vram control
// by takuya matsubara
// http://nicotak.com
//---------------------------------------------------------------------
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "vram.h"
#include "sfont.h"	//4x6スモールフォント


char text_x=0;  // printf用カーソル位置
char text_y=0;
unsigned long vram[VRAMHEIGHT];



//---------------------------------------------------------------------
//ピクセル描画
void vram_pset(signed char x,signed char y,char color)
{
	unsigned long mask;

	if(x<0)return;
	if(y<0)return;
	if(x>=VRAMWIDTH)return;
	if(y>=VRAMHEIGHT)return;

	mask=0x80000000 >> x;

	if(color==1)
		vram[(int)y] |= mask;
	else if(color==0)
		vram[(int)y] &= ~mask;
	else if(color==2)
		vram[(int)y] ^= mask;

}

//--------------------------------------------------------------------------------
//ライン描画
void vram_line(signed char x1 ,signed char y1 ,signed char x2 ,signed char y2 ,char color)
{
	signed char xd;	// X2-X1座標の距離
	signed char yd;	// Y2-Y1座標の距離
	signed char xs=1;  // X方向の1pixel移動量
	signed char ys=1;  // Y方向の1pixel移動量
	signed char e;

	xd = x2 - x1;	 // X2-X1座標の距離
	if(xd < 0){
		xd = -xd;	 // X2-X1座標の絶対値
		xs = -1;	  // X方向の1pixel移動量
	}
	yd = y2 - y1;	 // Y2-Y1座標の距離
	if(yd < 0){
		yd = -yd;	 // Y2-Y1座標の絶対値
		ys = -1;	  // Y方向の1pixel移動量
	}
	vram_pset (x1, y1 ,color); //ドット描画
	e = 0;
	if( yd < xd ) {
		while( x1 != x2) {
			x1 += xs;
			e += (2 * yd);
			if(e >= xd) {
				y1 += ys;
				e -= (2 * xd);
			}
			vram_pset (x1, y1 ,color); //ドット描画
		}
	}else{
		while( y1 != y2) {
			y1 += ys;
			e += (2 * xd);
			if(e >= yd) {
				x1 += xs;
				e -= (2 * yd);
			}
			vram_pset (x1, y1 ,color); //ドット描画
		}
	}
}

//--------------------------------------------------------------------------------
// 画面スクロール
// ・VRAMの中身を強制的にシフトします。
// 引数y1: Y方向移動量。+だと上シフト(視点を下へ)、-だと下シフト(視点を上へ)
void vram_scroll(char y1)
{
	int i;
	if(y1!=0){
		if(y1>0){
			for(i=0; i<VRAMHEIGHT; i++){
				if((i+y1)>15)
					vram[i] = 0;
				else
					vram[i] = vram[i+y1];
			}
		}else{
//			for(i=VRAMHEIGHT-1; i>=0; i--){
//				if((i+y1)<0)
//					vram[i] = 0;
//				else
//					vram[i] = vram[i+y1];
//			}
		}
	}
}


//---------------------------------------------------------------------
// カーソル位置設定
void vram_locate(char tx,char ty)
{
	text_x = tx;
	text_y = ty;
}

//---------------------------------------------------------------------
// 1キャラクタをVRAM転送
// ・4x6スモールフォント用
// 引数ch：キャラクターコード（0x00-0xff）
// 戻り値：0を返します。
void vram_putch(char ch)
{
	char tx,ty;
	unsigned char i;
	unsigned char bitdata;
	PGM_P p;

	if((ch==10)||(text_x > (VRAMWIDTH-4))){
		text_x = 0;
		text_y += 6;
	}
	if(text_y > (VRAMHEIGHT-6)){
		vram_scroll(text_y-(VRAMHEIGHT-6));
		text_y = VRAMHEIGHT-6;
	}
	if((unsigned char)ch < 0x20)return;

	ty= text_y;
	tx = (VRAMWIDTH-4)-text_x;
	p = (PGM_P)smallfont;
	p += ((int)((unsigned char)ch - 0x20) * 3);

	for(i=0 ;i<6 ;i++) {
		bitdata = pgm_read_byte(p);
		if((i % 2)==0){
			bitdata >>= 4;
		}else{
			p++;
		}
		bitdata &= 0xf;
//		vram[ty] &= ~(0xf << tx);
		vram[(int)ty] |= ((unsigned long)bitdata << tx);
		ty++;
	}
	text_x += 4;    // カーソル移動
//	if(((unsigned char)ch>=0xB0)&&((unsigned char)ch<=0xDC))
//		text_x += 1;    // カーソル移動(カタカナ用)
}

void vram_puthex(unsigned char ch)
{
	ch &= 0x0F;
	if(ch < 10){
		ch += '0';
	}else{
		ch += ('A'-10);
	}
	vram_putch(ch);
}

