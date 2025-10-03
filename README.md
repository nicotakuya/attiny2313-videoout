# ATtiny2313 video out
## overview
UARTから受信したデータをモノクロのビデオ信号として出力します。
・32ｘ24ピクセルで表示します。

・(SW1)CHRモードがONだと、キャラクタ表示。

・(SW2)HEXモードがONだと、16進数2桁表示。

・両方OFFだと、ビットマップモード。

・ボーレートが38400bps固定になっています。

・ビットマップモードでは64バイトをUARTから受信すると、1画面ぶん表示します。

・Atmel Studio 6.2で作りました。

## Parts

U1 : ATtiny2313

R1 : 470 ohm

R2 : 220 ohm

CN1 : 3pin pinheader

CN2 : RCAコネクタ

CN3 : 3x2pin pinheader

SW1/2  : switch
