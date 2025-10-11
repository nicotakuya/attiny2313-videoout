# SEND BITMAP DATA example
# for Raspberry Pi Pico(MicroPython) + OLED SSD1306
from machine import Pin ,I2C ,UART
import time
import random

#----VRAM
VRAMW = 128
VRAMH = 64
VRAMSIZE = int(VRAMW*VRAMH/8)
vram = bytearray(VRAMSIZE)

#---- vram all clear
def vram_clear():
    global vram
    for i in range(VRAMSIZE):
        vram[i] = 0

#---- vram get
def vram_pget( x, y):
    if (x>=VRAMW)or(y>=VRAMH):return(0)
    adr = x+(VRAMW*int(y/8))
    mask = (1<<(y % 8))
    if vram[adr] & mask:
        return(1)
    else:
        return(0)

#---- vram put 
def vram_pset(x,y,color):
    global vram
    if (x>=VRAMW)or(y>=VRAMH):return
    adr = x+(VRAMW*int(y/8))
    mask = (1<<(y % 8))
    if color==1:
        vram[adr] |= mask
    elif color==0:
        vram[adr] &= (mask ^ 0xff)
    else:
        vram[adr] ^= mask

# OLED(SSD1306)
SET_CONTRAST_CONTROL  = 0x81
SET_CHARGE_PUMP       = 0x8D
SET_ADDRESSING_MODE   = 0x20
SET_DISPLAY_STARTLINE = 0x40
SET_SEGMENT_REMAP     = 0xA1
SET_ENTIRE_DISPLAY    = 0xA4 
SET_DISPLAY_NORMAL    = 0xA6
SET_MULTIPLEX_RATIO   = 0xA8
SET_DISPLAY_ON        = 0xAF
SET_COM_OUTPUT_SCAN   = 0xC8
SET_DISPLAY_OFFSET    = 0xD3
SET_OSCILLATOR_FREQ   = 0xD5
SET_COM_PINS_HARDWARE = 0xDA
SET_COLUMN_ADDRESS    = 0x21
SET_PAGE_ADDRESS      = 0x22

# SSD1306:command
def oled_command( data):
    global i2cport,i2caddr
    send = bytearray(2)
    send[0] = 0x80 # control(single + command)
    send[1] = data             
    i2cport.writeto(i2caddr, send)

#SSD1306:command2
def oled_command2( data1, data2):
    global i2cport,i2caddr
    send = bytearray(3)
    send[0] = 0x00 # control(Continuation + command)
    send[1] = data1             
    send[2] = data2             
    i2cport.writeto(i2caddr, send)

# SSD1306: initialize
def oled_init():
    global i2cport,i2caddr
    
    i2cport = I2C(1, scl=Pin(19), sda=Pin(18), freq=400000)
    i2caddr = 0x78 >> 1  # SSD1306 slave address
    time.sleep(0.05)
    oled_command2(SET_MULTIPLEX_RATIO , 0x3F)  # multiplex ratio
    oled_command2(SET_DISPLAY_OFFSET,0)
    oled_command(SET_DISPLAY_STARTLINE)  # starting address of display RAM
    oled_command(SET_COM_OUTPUT_SCAN)
    oled_command(SET_SEGMENT_REMAP)  # column address and the segment driver
    oled_command2(SET_COM_PINS_HARDWARE, 0x12)
    oled_command2(SET_CONTRAST_CONTROL , 0x80)
    oled_command(SET_ENTIRE_DISPLAY) # entire display “ON” stage
    oled_command(SET_DISPLAY_NORMAL)
    oled_command2(SET_OSCILLATOR_FREQ  , 0x80)  
    oled_command2(SET_ADDRESSING_MODE  ,0) 
    oled_command2(SET_CHARGE_PUMP , 0x14)  # Enable charge pump
    oled_command(SET_DISPLAY_ON)
    time.sleep(1)

# SSD1306:screen update
def oled_redraw():
    send = bytearray(7)
    send[0] = 0x00 # control(Continuation + command)
    send[1] = SET_COLUMN_ADDRESS
    send[2] = 0       # start column
    send[3] = VRAMW-1 # end column
    send[4] = SET_PAGE_ADDRESS
    send[5] = 0           # start page
    send[6] = int(VRAMH/8)-1 # end page
    i2cport.writeto(i2caddr,send)

    send = bytearray(9)
    addr = 0
    while addr < VRAMSIZE:  
        send[0] = 0x40 #control(Continuation + data)
        for i in range(8):  
            send[i+1] = vram[addr]
            addr += 1
        i2cport.writeto(i2caddr,send)

#---UARTからビットマップデータ送信(64Bytes)
def send_vram():
    for tmpy in range(0,64,4):
        for tmpx in range(0,128,4*8):
            senddata = 0
            for xofs in range(0,32,4):
                senddata >>= 1
                if vram_pget(tmpx+xofs,tmpy) != 0:
                    senddata |= 0x80
                
            uart.write(senddata.to_bytes(1,"big"))
            uart.flush()   # データ送信まで待機

#--- 
def draw_ball(x,y):
    x &= 0xfc
    y &= 0xfc
    for xd in range(4):
        for yd in range(4):
             vram_pset(x+xd,y+yd,1)

#---
def draw_racket(x,y):
    x &= 0xfc
    y &= 0xfc
    y -= 4
    for yd in range(4*3):
        for xd in range(4):
             vram_pset(x+xd,y+yd,1)

#--- main
GPIOTX = 0   # GPIO pin:UART0 TX
GPIORX = 1   # GPIO pin:UART0 RX
uart = UART(0, baudrate=38400, tx=Pin(GPIOTX), rx=Pin(GPIORX))

vram_clear()
oled_init()
time.sleep(3)

PIXELSIZE = 4

ball_x = 16*PIXELSIZE
ball_y = 8*PIXELSIZE
ball_x1 = 4
ball_y1 = 3
racket1_x = 2*PIXELSIZE
racket1_y = 8*PIXELSIZE
racket2_x = (31-2)*PIXELSIZE
racket2_y = 8*PIXELSIZE

while True:
    ball_x += ball_x1
    ball_y += ball_y1
    if ball_x<=0 or ball_x>=31*PIXELSIZE:ball_x1 = -ball_x1
    if ball_y<=0 or ball_y>=15*PIXELSIZE:ball_y1 = -ball_y1

    move = random.randrange(4)
    if racket1_y>1*PIXELSIZE  and move==0: racket1_y -= PIXELSIZE
    if racket1_y<14*PIXELSIZE and move==1: racket1_y += PIXELSIZE
    if racket2_y>1*PIXELSIZE  and move==2: racket2_y -= PIXELSIZE
    if racket2_y<14*PIXELSIZE and move==3: racket2_y += PIXELSIZE

    if abs(ball_x - racket1_x)<PIXELSIZE and abs(ball_y - racket1_y)<2*PIXELSIZE:
        ball_x1 = -ball_x1
        
    if abs(ball_x - racket2_x)<PIXELSIZE and abs(ball_y - racket2_y)<2*PIXELSIZE:
        ball_x1 = -ball_x1

    vram_clear()
    draw_ball(ball_x,ball_y)
    draw_racket(racket1_x,racket1_y);
    draw_racket(racket2_x,racket2_y);
    oled_redraw()
    send_vram()
    time.sleep(0.01)

