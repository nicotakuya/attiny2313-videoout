# ATtiny2313 video out
## overview
UARTから受信したデータをビデオ信号として出力します。

・モノクロの32ｘ16ピクセルで表示します。

・ボーレートが38400bps固定になっています。

・(SW1)CHRモードがONだと、キャラクタ表示。

・(SW2)HEXモードがONだと、16進数2桁表示。

・両方OFFだと、ビットマップモード。ビットマップモードでは64バイトをUARTから受信すると、1画面ぶん表示します。

・microchip Studio 7.0で作りました。

## Parts

U1 : AVRマイコン。ATtiny2313

R1 : 470 ohm　抵抗

R2 : 220 ohm　抵抗

X1 : 20MHz 発振子

CN1 : 3pin pinheader

CN2 : RCAコネクタ

CN3 : 3x2pin pinheader。ISPコネクタ

SW1/2  : switch

## Fuse bit 参考値
EXTENDED:0xFF

HIGH:0xDB

LOW:0xDF

## Photo
![Image](https://github.com/user-attachments/assets/5e0478ad-24b2-4295-9fb8-f34d813f3c2e)

## 免責
動作無保証です。自己責任で取り扱ってください。
