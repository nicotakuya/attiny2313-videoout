#define VRAMWIDTH 32		// width
#define VRAMHEIGHT 16		// height
#define VRAMXMIN  0
#define VRAMXMAX  (VRAMWIDTH-1)
#define VRAMYMIN  0
#define VRAMYMAX  (VRAMHEIGHT-1)


void vram_pset(signed char x,signed char y,char color);
void vram_line(signed char x1 ,signed char y1 ,signed char x2 ,signed char y2 ,char color);
void vram_scroll(char y1);
void vram_locate(char x,char y);
void vram_putch(char ch);
void vram_puthex(unsigned char ch);
