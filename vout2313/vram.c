//---------------------------------------------------------------------
// vram control
// by takuya matsubara
// http://nicotak.com
//---------------------------------------------------------------------
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "vram.h"
#include "sfont.h"	//4x6�X���[���t�H���g


char text_x=0;  // printf�p�J�[�\���ʒu
char text_y=0;
unsigned long vram[VRAMHEIGHT];



//---------------------------------------------------------------------
//�s�N�Z���`��
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
//���C���`��
void vram_line(signed char x1 ,signed char y1 ,signed char x2 ,signed char y2 ,char color)
{
	signed char xd;	// X2-X1���W�̋���
	signed char yd;	// Y2-Y1���W�̋���
	signed char xs=1;  // X������1pixel�ړ���
	signed char ys=1;  // Y������1pixel�ړ���
	signed char e;

	xd = x2 - x1;	 // X2-X1���W�̋���
	if(xd < 0){
		xd = -xd;	 // X2-X1���W�̐�Βl
		xs = -1;	  // X������1pixel�ړ���
	}
	yd = y2 - y1;	 // Y2-Y1���W�̋���
	if(yd < 0){
		yd = -yd;	 // Y2-Y1���W�̐�Βl
		ys = -1;	  // Y������1pixel�ړ���
	}
	vram_pset (x1, y1 ,color); //�h�b�g�`��
	e = 0;
	if( yd < xd ) {
		while( x1 != x2) {
			x1 += xs;
			e += (2 * yd);
			if(e >= xd) {
				y1 += ys;
				e -= (2 * xd);
			}
			vram_pset (x1, y1 ,color); //�h�b�g�`��
		}
	}else{
		while( y1 != y2) {
			y1 += ys;
			e += (2 * xd);
			if(e >= yd) {
				x1 += xs;
				e -= (2 * yd);
			}
			vram_pset (x1, y1 ,color); //�h�b�g�`��
		}
	}
}

//--------------------------------------------------------------------------------
// ��ʃX�N���[��
// �EVRAM�̒��g�������I�ɃV�t�g���܂��B
// ����y1: Y�����ړ��ʁB+���Ə�V�t�g(���_������)�A-���Ɖ��V�t�g(���_�����)
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
// �J�[�\���ʒu�ݒ�
void vram_locate(char tx,char ty)
{
	text_x = tx;
	text_y = ty;
}

//---------------------------------------------------------------------
// 1�L�����N�^��VRAM�]��
// �E4x6�X���[���t�H���g�p
// ����ch�F�L�����N�^�[�R�[�h�i0x00-0xff�j
// �߂�l�F0��Ԃ��܂��B
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
	text_x += 4;    // �J�[�\���ړ�
//	if(((unsigned char)ch>=0xB0)&&((unsigned char)ch<=0xDC))
//		text_x += 1;    // �J�[�\���ړ�(�J�^�J�i�p)
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

