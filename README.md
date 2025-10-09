# ATtiny2313 video out
## overview
UARTから受信したデータをビデオ信号として出力します。

・モノクロの32ｘ16ピクセルで表示します。

・ボーレートが38400bps固定になっています。

・(SW1)CHRモードがONだと、キャラクタ表示。

・(SW2)HEXモードがONだと、16進数2桁表示。

・両方OFFだと、ビットマップモード。

・ビットマップモードでは64バイトをUARTから受信すると、1画面ぶん表示します。

・Atmel Studio 6.2で作りました。

## Parts

U1 : AVRマイコン。ATtiny2313

R1 : 470 ohm　抵抗

R2 : 220 ohm　抵抗

CN1 : 3pin pinheader

CN2 : RCAコネクタ

CN3 : 3x2pin pinheader。ISPコネクタ

SW1/2  : switch
