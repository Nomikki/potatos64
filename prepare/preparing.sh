#!/bin/sh
#install packages we'll need
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo wget grub-common grub-pc-bin xorriso
#variables to help
export PREFIX="`pwd`/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
#which versions to install?
export BINUTILS=binutils-2.41
export GCCV=gcc-13.2.0
# download binutils and gcc sources
wget -nc https://ftp.gnu.org/gnu/binutils/$BINUTILS.tar.gz
wget -nc https://ftp.gnu.org/gnu/gcc/$GCCV/$GCCV.tar.gz
# extract source packages
echo Extracting packages...
echo Extracting binutils
tar xkf $BINUTILS.tar.gz
echo Extracting gcc
tar xkf $GCCV.tar.gz

#exit 0
# build binutils
mkdir build-binutils
cd build-binutils
../$BINUTILS/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

# build gcc
cd ..
which -- $TARGET-as ||echo $TARGET-as is not in the PATH
mkdir build-gcc
cd build-gcc
../$GCCV/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
# check if correct version
opt/cross/bin/$TARGET-gcc --version
export PATH="opt/cross/bin:$PATH"