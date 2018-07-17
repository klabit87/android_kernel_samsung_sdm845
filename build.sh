#!/bin/bash
# Kernel Build Script

BUILD_WHERE=$(pwd)
BUILD_KERNEL_DIR=$BUILD_WHERE
BUILD_ROOT_DIR=$BUILD_KERNEL_DIR/..
BUILD_KERNEL_OUT_DIR=$BUILD_ROOT_DIR/kernel_out/KERNEL_OBJ
PRODUCT_OUT=$BUILD_ROOT_DIR/kernel_out

DEVICE=starlte_chn
KERNEL_TOOLCHAIN=/home/vaughnn/android/toolchain/aarch64-linux-android/bin/aarch64-linux-android-

case ${DEVICE} in
	"starlte_chn")
            KERNEL_DEFCONFIG=starqlte_chn_open_defconfig;;

        "star2lte_chn")
            KERNEL_DEFCONFIG=star2qlte_chn_open_defconfig;;

        "starlte_hk")
            KERNEL_DEFCONFIG=starqlte_chn_hk_defconfig;;

	"star2lte_hk")
	    KERNEL_DEFCONFIG=star2qlte_chn_hk_defconfig;;

	*)
            die "Invalid defconfig!" ;;
esac

BUILD_CROSS_COMPILE=$KERNEL_TOOLCHAIN
BUILD_JOB_NUMBER=`grep processor /proc/cpuinfo|wc -l`

KERNEL_IMG=$PRODUCT_OUT/Image.gz-dtb
DTIMG=$PRODUCT_OUT/dt.img

DTBTOOL=$KERNEL_DTBTOOL

FUNC_GENERATE_DEFCONFIG()
{
	echo ""
        echo "=============================================="
        echo "START : FUNC_GENERATE_DEFCONFIG"
        echo "=============================================="
        echo "build config="$KERNEL_DEFCONFIG ""
        echo ""

	make -C $BUILD_KERNEL_DIR O=$BUILD_KERNEL_OUT_DIR -j$BUILD_JOB_NUMBER ARCH=arm64 \
			CROSS_COMPILE=$BUILD_CROSS_COMPILE \
			$KERNEL_DEFCONFIG || exit -1

	cp $BUILD_KERNEL_OUT_DIR/.config $BUILD_KERNEL_DIR/arch/arm64/configs/$KERNEL_DEFCONFIG

	echo ""
	echo "================================="
	echo "END   : FUNC_GENERATE_DEFCONFIG"
	echo "================================="
	echo ""
}

FUNC_GENERATE_DTB()
{
	echo ""
        echo "=============================================="
        echo "START : FUNC_GENERATE_DTB"
        echo "=============================================="
        echo ""
	rm -rf $BUILD_KERNEL_OUT_DIR/arch/arm64/boot/dts

	make dtbs -C $BUILD_KERNEL_DIR O=$BUILD_KERNEL_OUT_DIR -j$BUILD_JOB_NUMBER ARCH=arm64 \
			CROSS_COMPILE=$BUILD_CROSS_COMPILE || exit -1
	echo ""
	echo "================================="
	echo "END   : FUNC_GENERATE_DTB"
	echo "================================="
	echo ""
}

FUNC_BUILD_KERNEL()
{
	echo ""
	echo "================================="
	echo "START   : FUNC_BUILD_KERNEL"
	echo "================================="
	echo ""
	rm $KERNEL_IMG $BUILD_KERNEL_OUT_DIR/arch/arm64/boot/Image
	rm -rf $BUILD_KERNEL_OUT_DIR/arch/arm64/boot/dts

if [ "$USE_CCACHE" ]
then
	make -C $BUILD_KERNEL_DIR O=$BUILD_KERNEL_OUT_DIR -j$BUILD_JOB_NUMBER ARCH=arm64 \
			CROSS_COMPILE=$BUILD_CROSS_COMPILE \
			CC="ccache "$BUILD_CROSS_COMPILE"gcc" CPP="ccache "$BUILD_CROSS_COMPILE"gcc -E" || exit -1
else
	make -C $BUILD_KERNEL_DIR O=$BUILD_KERNEL_OUT_DIR -j$BUILD_JOB_NUMBER ARCH=arm64 \
			CROSS_COMPILE=$BUILD_CROSS_COMPILE || exit -1
fi

	cp $BUILD_KERNEL_OUT_DIR/arch/arm64/boot/Image.gz-dtb $KERNEL_IMG
	echo "Made Kernel image: $KERNEL_IMG"
	echo "================================="
	echo "END   : FUNC_BUILD_KERNEL"
	echo "================================="
	echo ""
}

FUNC_GENERATE_DTIMG()
{
	echo ""
	echo "================================="
	echo "START   : FUNC_GENERATE_DTIMG"
	echo "================================="
	rm $DTIMG
	$DTBTOOL -o $DTIMG -s 2048 -p $BUILD_KERNEL_OUT_DIR/scripts/dtc/ $BUILD_KERNEL_OUT_DIR/arch/arm64/boot/dts/exynos
	if [ -f "$DTIMG" ]; then
		echo "Made DT image: $DTIMG"
	fi
	echo "================================="
	echo "END   : FUNC_GENERATE_DTIMG"
	echo "================================="
	echo ""
}

# MAIN FUNCTION
(
    START_TIME=`date +%s`

    FUNC_GENERATE_DEFCONFIG
    if [ "$2" = "--dt-only" ]
    then
        FUNC_GENERATE_DTB
    else
        FUNC_BUILD_KERNEL
    fi
    FUNC_GENERATE_DTIMG

    END_TIME=`date +%s`

    let "ELAPSED_TIME=$END_TIME-$START_TIME"
    echo "Total compile time is $ELAPSED_TIME seconds"
) 2>&1

if [ ! -f "$KERNEL_IMG" ]; then
  exit -1
fi
