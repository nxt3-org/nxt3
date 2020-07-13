TARGET = armv5-linux-musleabi

OUTPUT = /home/kuba/musl-ev3

BINUTILS_VER = 2.33.1
GCC_VER   = 8.4.0
MUSL_VER  = 1.2.0
GMP_VER   = 6.1.2
MPC_VER   = 1.1.0
MPFR_VER  = 4.0.2
ISL_VER   = 0.21
LINUX_VER = headers-4.19.88

COMMON_CONFIG += CFLAGS="-g0 -Os" CXXFLAGS="-g0 -Os" LDFLAGS="-s"
COMMON_CONFIG += --disable-nls
COMMON_CONFIG += --with-debug-prefix-map=$(CURDIR)=

GCC_CONFIG += --disable-libquadmath --disable-decimal-float --disable-fixed-point
GCC_CONFIG += --disable-libgomp --disable-libitm
GCC_CONFIG += --with-cpu=arm926ej-s --enable-lto
GCC_CONFIG += --enable-languages=c,c++
GCC_CONFIG += --disable-shared
MUSL_CONFIG += --disable-shared
TOOLCHAIN_CONFIG += --with-pkgversion="C4EV3.Toolchain musl-gcc 8.4.0" --with-bugurl="https://github.com/c4ev3/C4EV3.Toolchain/issues/new"
