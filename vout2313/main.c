//VIDEO OUT 32x16dot
//for ATtiny2313 (ext. clock 20MHz)

//by takuya matsubara
// http://nicotak.com

// uart�Ŏ�M�����r�b�g�}�b�v��TV out

//�Ђ傢����̋Z�p���g���Ă܂�
// http://www7.big.jp/~kerokero/avr/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "vram.h"

//--------------------------------------
#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long


#define BAUDRATE 38400 // bps

struct PB{
	char :1;			//0
	char :1;			//1
	char VID_DOT:1;		//2 �r�f�I�E�h�b�g
	char VID_SYNC:1;	//3 �r�f�I�M��oc1a
	char BEEP:1;		//4 �T�E���h�o��oc1b
	char :1;			//5
	char :1;			//6
	char :1;			//7
};

#define pib (*(volatile struct PB*)&PINB)
#define pdb (*(volatile struct PB*)&DDRB)
#define pob (*(volatile struct PB*)&PORTB)

//--------------------------------------

#define F_CPU 20000000	// CPU�N���b�N���g��[Hz]
#define FREQ_H 15750	// horizontal frequency[Hz]


unsigned int linesync=0;
volatile BYTE tick=0;

#define OFFSETY 32		//�]��[line]
#define OFFSETX (unsigned char)(F_CPU/(1000000/11)) // ���s�̗]�� 11usec
#define DOTH 12			//1pixel�̃T�C�Y[line]

extern unsigned long vram[VRAMHEIGHT];


// 1sec/15750Hz =  63.49...usec
// (1sec/60) / (1sec/15750) = 262.5

//   1�`9:���������M��
//       1�`3�O�u�����p���X
//       4�`6���������p���X
//       7�`9��u�����p���X
//   10�`21:�e�X�g�M���Ȃ�
BYTE row=0;
BYTE hcnt=0;

void video_driver(void)
{
	linesync++;
	if(linesync <= OFFSETY){
		if(linesync == 1){
			OCR1A = F_CPU / (1000000/62);	//V sync 62usec
			row=0;
		}else if(linesync == 7){
			OCR1A = F_CPU / (1000000/4); // H sync 4usec
		}
		return;
	}

	if(row >= VRAMHEIGHT){
		if(linesync >= 262){
			linesync = 0;
			tick++;
		}
		return;
	}

	DWORD temp;
	temp = vram[row];

	while(TCNT1L < OFFSETX);	//// ���Ԓ��� ////

	char bitcnt = VRAMWIDTH;
	while(bitcnt--){
		pob.VID_DOT = ((temp & 0x80000000)!=0);
		asm("nop\n");
		asm("nop\n");
		asm("nop\n");
		asm("nop\n");
		asm("nop\n");
		asm("nop\n");
//		asm("nop\n");
//		asm("nop\n");
		temp <<=1;
	}
	pob.VID_DOT = 0;
	hcnt++;
	if(hcnt>=DOTH){
		row++;
		hcnt=0;
	}
	return;
}


//----------
void video_init(void)
{
	/// PWM�����ݒ� ///
	TCNT1 = 0;
	ICR1 = (F_CPU / FREQ_H) - 1;	/// TOP�l

	TCCR1A = (3<<COM1A0)|(2<<COM1B0)|(2<<WGM10);//fast pwm
	TCCR1B = (3<<WGM12)|(1<<CS10);

	//Waveform Generation Mode WGM1x = 0b1110
	//clock select= clk div1

	/// ���������
	TIMSK |= (1<<TOIE1);	// �^�C�}�[�P��ꊄ����

	/// �|�[�g�o��
	pdb.VID_DOT  = 1;
	pdb.VID_SYNC = 1;
	pdb.BEEP = 1;
}


//-------------------------�^�C�}���荞��
//SIGNAL(SIG_OVERFLOW1)
SIGNAL(TIMER1_OVF_vect)
{	//	15750Hz
	video_driver();
}

//--------------------------
void vram_dummy(void)
{
	vram_line(0,0,VRAMWIDTH-1,VRAMHEIGHT-1,1);
	vram_line(VRAMWIDTH-1,0,0,VRAMHEIGHT-1,1);
}


//--------------------------
void idle(void){
	sleep_mode();		//
}

//--------------------------
void sleep_loop(void)
{
	BYTE tickback;

	tickback = tick;

	set_sleep_mode(SLEEP_MODE_IDLE); //

	while(tickback == tick){
		idle();// 1/60sec
	}
	tickback=tick;
}

// --------------------UART�ݒ�
void sio_init(void)
{
#define MYUBRR ((F_CPU/8/BAUDRATE)-1) // UART������

	UBRRH = (MYUBRR >> 8); // �{�[���[�g���
	UBRRL = MYUBRR & 0xff; // �{�[���[�g����

	UCSRA = (1<<U2X);	//double speed
	UCSRB = (1<<RXEN)|(1<<TXEN);

	//set frame format data8, stop1, nonparity
	UCSRC=(1<<UCSZ1) | (1<<UCSZ0);

	DDRD  &= ~(1<<0);	//input rx
	PORTD |=  (1<<0);	//pull up rx
}

char seq = 0;

//----------
char sio_rxflag(void)
{
	return(UCSRA & (1<<RXC));
}

//----------
unsigned char sio_rx(void)
{
	int timeout=0;

	while((UCSRA & (1<<RXC))==0){
		sleep_mode();		//
		timeout++;
		if(timeout > 10000){
			seq = 64;
		}
	};

	return(UDR);
}


#define DIPSW_PORT PORTB
#define DIPSW_DDR DDRB
#define DIPSW_PIN PINB
#define DIPSW_MASK 0x3
#define DIPSW_VAL ((~DIPSW_PIN)&DIPSW_MASK)


//-------------------------------------------------------
int main(void)
{
	unsigned char ch;

	DIPSW_PORT |= DIPSW_MASK;
	DIPSW_DDR &= ~DIPSW_MASK;

	video_init();	// initialize H-sync intr.
	vram_dummy();
	sio_init();

	sei();
	while(1)
	{
		ch = sio_rx();

		if(DIPSW_VAL==0){
			if(seq >= 64){
				seq=0;
			}
			*((unsigned char *)vram +seq)=ch;
			seq++;
		}else if(DIPSW_VAL==1){
			vram_putch(ch);
		}else{
			vram_puthex(ch >> 4);
			vram_puthex(ch & 0xF);
		}
	}
	return 0;
}
