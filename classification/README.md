This directory contains the source code to run classification on the Himax board using the sensors from the Sparkfun environmental breakout.

* First, export the C++ library from your Edge Impulse project and extract the 3 directories in this _classification_ folder. **Do not replace the CMakeLists.txt file**.

* Build the Docker container and compile the firmware:

```
docker build -t classification -f Dockerfile.gnu .
mkdir -p build-gnu
docker run --rm -it -v $PWD:/app classification /bin/bash -c "cd build-gnu && cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.gnu.cmake .."
docker run --rm -it -v $PWD:/app:delegated classification /bin/bash -c "cd build-gnu && make -j && sh ../make-image.sh GNU"
```

* Flash the firmware:

```
himax-flash-tool --firmware-path image_gen_linux/out.img
```

* Open a serial terminal with baudrate=115200 to show real time classification:

```
Edge Impulse inferencing on Himax WE-I board and Sparkfun environmental breakout
Features (0 ms.): 0.26429 0.20879 0.00306 0.26429 0.20879 0.00306 0.26429 0.20879 0.00305 0.26429 0.20879 0.00305 0.26429 0.20879 0.00306 0.26429 0.20879 0.00306 0.26429 0.20879 0.00306 0.26429 0.20879 0.00306 0.26639 0.21269 0.00306 0.26639 0.21269 0.00306 
Running neural network...
Predictions (time: 1 ms.):
ambient:        0.00808
chocolate:      0.28124
tea:    0.71066
run_classifier returned: 0
```