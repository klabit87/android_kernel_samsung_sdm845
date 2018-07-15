#!/bin/bash

OUT_DIR=out

# you should change the "CROSS_COMPILE" to right toolchain path (you downloaded)
# ex)CROSS_COMPILE={android platform directory you downloaded}/android/prebuilt/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
COMMON_ARGS="-C $(pwd) O=$(pwd)/${OUT_DIR} ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- KCFLAGS=-mno-android"

export PATH=$(pwd)/../PLATFORM/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin:$PATH

[ -d ${OUT_DIR} ] && rm -rf ${OUT_DIR}
mkdir ${OUT_DIR}

make ${COMMON_ARGS} star2qlte_chn_hk_defconfig
make ${COMMON_ARGS}

cp ${OUT_DIR}/arch/arm64/boot/Image $(pwd)/arch/arm64/boot/Image

