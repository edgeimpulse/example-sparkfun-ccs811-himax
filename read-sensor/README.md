docker build -t read-sensor -f Dockerfile.gnu .
mkdir -p build-gnu
docker run --rm -it -v $PWD:/app read-sensor /bin/bash -c "cd build-gnu && cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.gnu.cmake .."
docker run --rm -it -v $PWD:/app:delegated read-sensor /bin/bash -c "cd build-gnu && make -j && sh ../make-image.sh GNU"
