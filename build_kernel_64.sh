#!/bin/bash
export CROSS_COMPILE=../PLATFORM/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
mkdir out
chmod -R 777 out
make -C $(pwd) O=out -j32 ARCH=arm64 CROSS_COMPILE=$(pwd)/../PLATFORM/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android- star2qlte_chn_hk_defconfig
make -C $(pwd) O=out -j32 ARCH=arm64 CROSS_COMPILE=$(pwd)/../PLATFORM/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
cp out/arch/arm64/boot/Image $(pwd)/arch/arm64/boot/Image
