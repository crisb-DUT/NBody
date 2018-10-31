# NBody-Linux
------------------------------------
## Environment
- Ubuntu >= 16.04
- libsdl2-dev >= 2.0.4
- libsdl2-ttf-dev = 2.0.14
- cmake >= 3.8

Install scripts:
```
$ sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev
```

## How to use
```bash
$ mkdir Release && cd Release
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j8 && make install
$ cd bin
$ ./NBody_Linux 1
```
---------------------------------
## Main features
1. Optimized cuda code for NBody. The number of bodies could be 10K under 60 fps in GTX 980Ti.
2. Creating trails effection during rasterization with cuda accelarated.
3. 2 initialization methods are created.
4. Rasterization is implemmented using cuda kernel.
5. Detecting boundary during time step compute.
6. Mouse click interation.

## Demo
[![IMAGE ALT TEXT](http://img.youtube.com/vi/ZpVGct7cHPE/0.jpg)](https://www.youtube.com/embed/ZpVGct7cHPE "Play")