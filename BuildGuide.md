# Build guide for Cura

## about lib not found
http://blog.csdn.net/sahusoft/article/details/7388617

## Probuf
Can't use `autogen`.

cd
git clone https://github.com/google/protobuf.git
autopoint 
aclocal -I m4
autoheader
libtoolize 
automake --add-missing
autoconf

./configure
make
sudo make install

sudo ldconfig

## libArcus

cd
git clone https://github.com/Ultimaker/libArcus.git
cd libArcus/
mkdir build && cd build
cmake ..
make
sudo make install

## CuraEngine
cd CuraEngine/
mkdir build && cd build
cmake ..
make
make install