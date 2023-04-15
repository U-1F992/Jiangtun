# Jiangtun（江豚）

NX Macro Controller をゲームキューブ自動化に利用するための Raspberry Pi Pico 用ファームウェア

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
|        Home         | (Reset)  |
|       Capture       |  (none)  |

## Build

### Arduino-pico

Git for Windowsのパス文字数制限に注意すること。\[[詳細](https://arduino-pico.readthedocs.io/en/latest/platformio.html)\]

```
git config --system core.longpaths true
```

### plog

現状`.pio/libdeps/pico/plog/samples`ディレクトリを削除しないとビルドが通らない。\[[Pull request](https://github.com/SergiusTheBest/plog/pull/244)\]

### GoogleTest

`platform = native`のビルドツールチェーンは自動ではインストールされない。 \[[参考](https://docs.platformio.org/en/latest/platforms/native.html)\] Windowsの場合はMSYS2にg++を用意してパスを通してねって書いてある。

## Reference

- [Nintendo Switch を操作するプログラムの仕様を調べる](https://scrapbox.io/yatsuna827827-12010999/Nintendo_Switch%E3%82%92%E6%93%8D%E4%BD%9C%E3%81%99%E3%82%8B%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E4%BB%95%E6%A7%98%E3%82%92%E8%AA%BF%E3%81%B9%E3%82%8B)
- [CH55xSwitchSerialControl](https://github.com/608/CH55xSwitchSerialControl)
- [joybus-pio](https://github.com/mizuyoukanao/joybus-pio)
