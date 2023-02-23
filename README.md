# Jiangtun（江豚）

NX Macro Controller をゲームキューブ自動化に利用するための Arduino／Raspberry Pi Pico 用ファームウェア

## Keys

| NX Macro Controller | GameCube |
| :-----------------: | :------: |
|          Y          |    Y     |
|          B          |    B     |
|          A          |    A     |
|          X          |    X     |
|          L          |    L     |
|          R          |    R     |
|         ZL          |  (none)  |
|         ZR          |    Z     |
|          -          |  (none)  |
|          +          |  Start   |
|       L Click       |  (none)  |
|       R Click       |  (none)  |
|        Home         |  (none)  |
|       Capture       |  (none)  |

## Build

[Nintendo by NicoHood](https://github.com/NicoHood/Nintendo)ライブラリが必要です。

Raspberry Pi Pico 用は、`src`以下を[joybus-pio](https://github.com/mizuyoukanao/joybus-pio)のもので上書きするとビルドが通ります。

## Reference

- [Nintendo Switch を操作するプログラムの仕様を調べる](https://scrapbox.io/yatsuna827827-12010999/Nintendo_Switch%E3%82%92%E6%93%8D%E4%BD%9C%E3%81%99%E3%82%8B%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E4%BB%95%E6%A7%98%E3%82%92%E8%AA%BF%E3%81%B9%E3%82%8B)
- [CH55xSwitchSerialControl](https://github.com/608/CH55xSwitchSerialControl)
- [joybus-pio](https://github.com/mizuyoukanao/joybus-pio)
