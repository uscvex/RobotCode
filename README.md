# Setup

## Windows

1. Install [Yagarto](http://sourceforge.net/projects/yagarto/files/YAGARTO%20for%20Windows/)
2. Install [MSYS2](https://msys2.github.io/)
3. Download [Convex](https://github.com/jpearman/convex/archive/master.zip) and extract to some
  directory. Also extract ChibiOS_2.6.2.zip to some directory
4. Download [Cortex Flash](https://github.com/jpearman/stm32flashCortex/archive/master.zip) and
  extract to some directory
5. Install [PuTTY](http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html)
6. Add the following system variables
  + CHIBIOS - path to chibios extracted in step 3
  + CONVEX - path to cortex directory in the convex folder extracted in step 3
  + MSYS - path to msys2 installed in step 2
7. Add putty installation directory to your windows PATH environment variable
7. Add cortexflash directory to your windows PATH environment variable

### Building code

1. Open MSYS shell and CD into one of the project directories (eg: SmallBot)
2. run `make`

### Flashing code

1. Open MSYS shell and CD into one of the project directories (eg: SmallBot)
2. run `cortexflash.exe -G -w bin/output.hex COM3`. Replace COM3 with serial port (COM3 usually works, try COM1-COM9 if not)

### Debugging

1. Open MSYS shell
2. run `plink.exe -serial COM3 -sercfg 115200`. Replace COM3 with serial port (COM3 usually works, try COM1-COM9 if not)

## Mac OSX
bit.ly/vexmac
