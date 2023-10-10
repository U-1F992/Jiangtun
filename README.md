# Jiangtun（江豚）

[NX Macro Controller](https://blog.bzl-web.com/entry/2020/01/20/165719)/[Poke-Controller Modified](https://github.com/Moi-poke/Poke-Controller-Modified)をゲームキューブ自動化に利用するためのRaspberry Pi Pico用ファームウェア

ゲームキューブ：`GPIO5`、サーボモーター：`GPIO6`を使用します（WHALE互換）。

| NXMC2/PokeCon | GameCube |
| :-----------: | :------: |
|       Y       |    Y     |
|       B       |    B     |
|       A       |    A     |
|       X       |    X     |
|       L       |    L     |
|       R       |    R     |
|      ZL       |  (none)  |
|      ZR       |    Z     |
|       -       |  (none)  |
|       +       |  Start   |
|    L Click    |  (none)  |
|    R Click    |  (none)  |
|     Home      |  Reset   |
|    Capture    |  (none)  |

## Build

サブモジュールを含むので`--recursive`を付けてクローンしてください。

Git for Windowsのパス文字数制限に注意。\[[詳細](https://arduino-pico.readthedocs.io/en/latest/platformio.html)\]

```
git config --system core.longpaths true
```

## Reference

- [GC自動化ver3アップデート](https://note.com/gamewagashi/n/n026a29d00a85)
