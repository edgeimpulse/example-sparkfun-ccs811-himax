This directory contains the source code to read values from the Sparkfun environmental breakout board, and print values to the serial port.

To flash the firmware:

```
himax-flash-tool --firmware-path image_gen_linux/out.img
```

Data can then be forwarded to the Edge Impulse project using the data forwarder, ie:

```
edge-impulse-data-forwarder --frequency 2
```

You can also have an overview of the raw data by opening a serial connection (baudrate=115200), before using the Data Forwarder.


If you need to modify the firmware, check out the _main.cc_, and rebuild/compile with:

```
docker build -t read-sensor -f Dockerfile.gnu .
mkdir -p build-gnu
docker run --rm -it -v $PWD:/app read-sensor /bin/bash -c "cd build-gnu && cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.gnu.cmake .."
docker run --rm -it -v $PWD:/app:delegated read-sensor /bin/bash -c "cd build-gnu && make -j && sh ../make-image.sh GNU"
```
