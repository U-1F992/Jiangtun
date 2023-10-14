# Jiangtun (江豚)

GC automation alternative firmware for Raspberry Pi Pico.

- [NX Macro Controller](https://blog.bzl-web.com/entry/2020/01/20/165719)
- [Poke-Controller Modified](https://github.com/Moi-poke/Poke-Controller-Modified)
- [ORCA GC Controller](https://github.com/yatsuna827/Orca-GC-Controller)(experimental)

Use GameCube: `GPIO5`, Servo: `GPIO6`. ([WHALE](https://github.com/mizuyoukanao/Bluewhale) compatible)

## Button mapping

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

## Development

This directory is a [PlatformIO](https://platformio.org/) project. Clone with `--recursive` as it contains submodules. Note [the path character length limit in Git for Windows](https://arduino-pico.readthedocs.io/en/latest/platformio.html#important-steps-for-windows-users-before-installing).

```
git config --system core.longpaths true
```

## Reference

- [GC自動化ver3アップデート](https://note.com/gamewagashi/n/n026a29d00a85)
