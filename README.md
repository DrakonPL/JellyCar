
![JellyCar logo](https://marcinp.xyz/jellycar/2020031417273500-DB1426D1DFD034027CECDE9C2DD914B8.jpg)

## JellyCar is great 2d game with soft body physics created by Walaber https://twitter.com/walaber.

This is homebrew version that I made with permission from original author for many platforms.
(PC, PSP, Wii,PlayStation 3, PlayStation Vita, Nintendo Switch)


Current implemetation uses my library: [Andromeda-Lib](https://github.com/DrakonPL/Andromeda-Lib)

## Build instructions.

### Important
For all platforms you must clone first 
[Andromeda-Lib](https://github.com/DrakonPL/Andromeda-Lib) , then this repository.

### PlayStation Vita

- Install newest VitaSDK using https://github.com/vitasdk/vdpm  
- Compile Androimeda-Lib
  - Go to Androimeda-Lib/Build/Vita 
  - Run "make -jn"  (where n is numer of cores of your cpu)
- Compile JellyCar
  - Go to JellyCar/Build/Vita
  - Run "make -jn"  (where n is numer of cores of your cpu)

### Nintendo Switch

- Install devkitpro with Nintendo Switch SDK
- Install additional libs
  - pacman -S switch-pkg-config switch-mesa switch-glad switch-glm switch-sdl2 switch-sdl2_mixer switch-freetype
- Compile JellyCar
  - Go to JellyCar/Build/Switch
  - Run "make -jn"  (where n is numer of cores of your cpu)
