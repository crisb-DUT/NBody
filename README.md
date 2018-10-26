# NBody-Linux

## Environment
- Ubuntu 16.04
- libsdl2-dev = 2.0.4
- libsdl2-ttf-dev = 2.0.14

Install scripts:
```
$ sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev
```

## How to use
```bash
$ mkdir build && cd build
$ cmake ..
$ make -j8 && make install
$ cd bin
$ ./NBody_Linux
```