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
|        Home         |  (none)  |
|       Capture       |  (none)  |

## ビルドするまでのいろいろ

### SDK

PlatformIOでもRasberry Pi Pico用ツールチェーンを提供しているが、Arduinoフレームワークとjoybus-pioが要求する`pico/stdio.h`を併用するために、Arduino-PicoをPlatformIOで使用したい。 \[[詳細](https://arduino-pico.readthedocs.io/en/latest/platformio.html)\] 名称が競合するので、公式のツールチェーンはアンインストールしておいた方がいいかも。

```ini
[env:pico]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = pico
framework = arduino
board_build.core = earlephilhower
```

ビルド時にgit cloneが走るが、Windowsの場合はGit for Windowsのパス文字数制限に注意。

```
git config --system core.longpaths true
```

### GoogleTest

`test/native/test_xxx`ディレクトリを作成して、`platformio.ini`を編集する。 \[[公式サンプル](https://github.com/platformio/platformio-examples/tree/develop/unit-testing/googletest/test)\]

```ini
[env:native]
platform = native
test_framework = googletest
lib_deps = etlcpp/Embedded Template Library@^20.35.12

[env:pico]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = pico
framework = arduino
board_build.core = earlephilhower
lib_deps = etlcpp/Embedded Template Library@^20.35.12
test_ignore = native/*
```

`platform = native`のビルドツールチェーンは自動ではインストールされない。 \[[参考](https://docs.platformio.org/en/latest/platforms/native.html)\] Windowsの場合はMSYS2にg++を用意してパスを通してねって書いてある。

## Reference

- [Nintendo Switch を操作するプログラムの仕様を調べる](https://scrapbox.io/yatsuna827827-12010999/Nintendo_Switch%E3%82%92%E6%93%8D%E4%BD%9C%E3%81%99%E3%82%8B%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E4%BB%95%E6%A7%98%E3%82%92%E8%AA%BF%E3%81%B9%E3%82%8B)
- [CH55xSwitchSerialControl](https://github.com/608/CH55xSwitchSerialControl)
- [joybus-pio](https://github.com/mizuyoukanao/joybus-pio)
