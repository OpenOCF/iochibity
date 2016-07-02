#! /bin/sh

# Ideally we will capture the exit code of each step and try them all before failing
# the build script.  For now, use set -e and fail the build at first failure.
set -e

# defaults
VERBOSE=false
SECURED=false
STAGE=dev
RA=false
RD=false
RM=false

function build_all()
{
	if [ $(uname -s) = "Linux" ]
        then
		build_linux_unsecured $1 $2
		build_linux_secured $1 $2
		build_linux_unsecured_with_ra $1 $2
		build_linux_secured_with_ra $1 $2
		build_linux_unsecured_with_rm $1 $2
		build_linux_unsecured_with_rd $1 $2
		build_linux_secured_with_rd $1 $2
		build_linux_unsecured_with_java $1 $2
		build_linux_secured_with_java $1 $2
		build_simulator $1 $2
	fi

	build_android $1 $2

	build_arduino $1 $2

	build_tizen $1 $2

	if [ $(uname -s) = "Darwin" ]
	then
		build_darwin_unsecured $1 $2
		build_darwin_secured $1 $2
		build_darwin_unsecured_with_java $1 $2
		build__secured_with_java $1 $2
		build_ios $1 $2
	fi
}

function build_linux()
{
	build_linux_unsecured $1 $2

	build_linux_secured $1 $2
}

function build_linux_unsecured()
{
	echo "*********** Build for linux ************"
	scons VERBOSE=$VERBOSE RELEASE=$1 $2
}

function build_linux_unsecured_with_rm()
{
	echo "*********** Build for linux with RoutingManager************"
	scons ROUTING=GW RELEASE=$1 $2
}

function build_linux_secured()
{
	echo "*********** Build for linux with Security *************"
	scons RELEASE=$1 SECURED=1 $2
}

function build_linux_unsecured_with_ra()
{

	echo "*********** Build for linux With Remote Access *************"
	scons RELEASE=$1 WITH_RA=1 WITH_RA_IBB=1 $2
}

function build_linux_secured_with_ra()
{
	echo "*********** Build for linux With Remote Access & Security ************"
	scons RELEASE=$1 WITH_RA=1 WITH_RA_IBB=1 SECURED=1 $2
}

function build_linux_unsecured_with_rd()
{
	echo "*********** Build for linux With Resource Directory *************"
	scons RELEASE=$1 WITH_RD=1 $2
}

function build_linux_secured_with_rd()
{
	echo "*********** Build for linux With Resource Directory & Security ************"
	scons RELEASE=$1 WITH_RD=1 SECURED=1 $2
}

function build_linux_unsecured_with_java()
{
	echo "*********** Build for linux With Resource Directory & Security ************"
	scons RELEASE=$1 BUILD_JAVA=ON TARGET_TRANSPORT=IP $2
}

function build_linux_secured_with_java()
{
	echo "*********** Build for linux With Resource Directory & Security ************"
	scons RELEASE=$1 SECURED=1 BUILD_JAVA=ON TARGET_TRANSPORT=IP $2
}

function build_android()
{
	# Note: for android, as oic-resource uses C++11 feature stoi and to_string,
	# it requires gcc-4.9, currently only android-ndk-r10(for linux)
	# and windows android-ndk-r10(64bit target version) support these features.

	build_android_x86 $1 $2
	build_android_x86_with_rm $1 $2
	build_android_armeabi $1 $2
	build_android_armeabi_with_rm $1 $2
}

function build_android_x86()
{
	echo "*********** Build for android x86 *************"
	scons TARGET_OS=android TARGET_ARCH=x86 RELEASE=$1 TARGET_TRANSPORT=IP $2
	scons TARGET_OS=android TARGET_ARCH=x86 RELEASE=$1 TARGET_TRANSPORT=BT $2
	scons TARGET_OS=android TARGET_ARCH=x86 RELEASE=$1 TARGET_TRANSPORT=BLE $2
}

function build_android_x86_with_rm()
{
	echo "*********** Build for android x86 with Routing Manager *************"
	scons TARGET_OS=android TARGET_ARCH=x86 ROUTING=GW RELEASE=$1 TARGET_TRANSPORT=IP $2
	scons TARGET_OS=android TARGET_ARCH=x86 ROUTING=GW RELEASE=$1 TARGET_TRANSPORT=BT $2
	scons TARGET_OS=android TARGET_ARCH=x86 ROUTING=GW RELEASE=$1 TARGET_TRANSPORT=BLE $2
}

function build_android_armeabi()
{
	echo "*********** Build for android armeabi *************"
	scons TARGET_OS=android TARGET_ARCH=armeabi RELEASE=$1 TARGET_TRANSPORT=IP $2
	scons TARGET_OS=android TARGET_ARCH=armeabi RELEASE=$1 TARGET_TRANSPORT=BT $2
	scons TARGET_OS=android TARGET_ARCH=armeabi RELEASE=$1 TARGET_TRANSPORT=BLE $2
}

function build_android_armeabi_with_rm()
{
	echo "*********** Build for android armeabi with Routing Manager*************"
	scons TARGET_OS=android TARGET_ARCH=armeabi ROUTING=GW RELEASE=$1 TARGET_TRANSPORT=IP $2
	scons TARGET_OS=android TARGET_ARCH=armeabi ROUTING=GW RELEASE=$1 TARGET_TRANSPORT=BT $2
	scons TARGET_OS=android TARGET_ARCH=armeabi ROUTING=GW RELEASE=$1 TARGET_TRANSPORT=BLE $2
}

function build_arduino()
{
	echo "*********** Build for arduino avr *************"
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=mega TARGET_ARCH=avr TARGET_TRANSPORT=IP SHIELD=ETH RELEASE=$1 $2
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=mega TARGET_ARCH=avr TARGET_TRANSPORT=IP SHIELD=WIFI RELEASE=$1 $2
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=mega TARGET_ARCH=avr TARGET_TRANSPORT=BLE SHIELD=RBL_NRF8001 RELEASE=$1 $2

	echo "*********** Build for arduino arm *************"
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=arduino_due_x TARGET_ARCH=arm TARGET_TRANSPORT=IP SHIELD=ETH RELEASE=$1 $2
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=arduino_due_x TARGET_ARCH=arm TARGET_TRANSPORT=IP SHIELD=WIFI RELEASE=$1 $2
	# BLE support for the Arduino Due is currently unavailable.
}

function build_tizen()
{
	echo "*********** Build for Tizen *************"
	./gbsbuild.sh

	echo "*********** Build for Tizen octbstack lib and sample *************"
	scons -f resource/csdk/stack/samples/tizen/build/SConscript TARGET_OS=tizen TARGET_TRANSPORT=IP LOGGING=true RELEASE=$1 $2

	echo "*********** Build for Tizen octbstack lib and sample with Security*************"
	scons -f resource/csdk/stack/samples/tizen/build/SConscript TARGET_OS=tizen TARGET_TRANSPORT=IP LOGGING=true SECURED=1 RELEASE=$1 $2

	echo "*********** Build for Tizen octbstack lib and sample with Routing Manager*************"
	scons -f resource/csdk/stack/samples/tizen/build/SConscript TARGET_OS=tizen TARGET_TRANSPORT=IP LOGGING=true ROUTING=GW RELEASE=$1 $2
}

function build_darwin_unsecured_with_ra()
{

	echo "*********** Build for darwin With Remote Access *************"
	scons RELEASE=$1 TARGET_OS=darwin WITH_RA=1 WITH_RA_IBB=1 $2
}

function build_darwin_unsecured()
{
	echo "******* Build for Darwin (RELEASE=$1, SECURED=0) *********"
	# echo "uname: `uname -a`"
	# echo
	# echo "clang:"
	# echo "`clang --version`"
	# echo "`clang -print-search-dirs`"
	# echo
	# echo "gcc:"
	# echo "`gcc --version`"
	# echo
	scons VERBOSE=$VERBOSE TARGET_OS=darwin SECURED=0 RELEASE=$1 $2
}

function build_darwin_secured()
{
	echo "******* Build for Darwin (RELEASE=$1, SECURED=1) *********"
	echo "uname: `uname -a`"
	echo
	echo "clang:"
	echo "`clang --version`"
	echo "`clang -print-search-dirs`"
	echo
	echo "gcc:"
	echo "`gcc --version`"
	echo
	echo "BEGIN SCONS BUILD..."
	scons VERBOSE=$VERBOSE TARGET_OS=darwin SECURED=1 RELEASE=$1 $2
}

function build_darwin_unsecured_with_java()
{
	echo "*********** Build for darwin With Resource Directory & Security ************"
	scons RELEASE=$1 TARGET_OS=darwin SECURED=0 BUILD_JAVA=ON TARGET_TRANSPORT=IP $2
}

function build_darwin_secured_with_java()
{
	echo "*********** Build for darwin With Resource Directory & Security ************"
	scons RELEASE=$1 TARGET_OS=darwin SECURED=1 BUILD_JAVA=ON TARGET_TRANSPORT=IP $2
}

function build_ios() # Apple iOS
{
	echo "*********** Build for IOS i386 *************"
	scons TARGET_OS=ios TARGET_ARCH=i386 SYS_VERSION=7.0 RELEASE=$1 $2

	echo "*********** Build for IOS x86_64 *************"
	scons TARGET_OS=ios TARGET_ARCH=x86_64 SYS_VERSION=7.0 RELEASE=$1 $2

	echo "*********** Build for IOS armv7 *************"
	scons TARGET_OS=ios TARGET_ARCH=armv7 SYS_VERSION=7.0 RELEASE=$1 $2

	echo "*********** Build for IOS armv7s *************"
	scons TARGET_OS=ios TARGET_ARCH=armv7s SYS_VERSION=7.0 RELEASE=$1 $2

	echo "*********** Build for IOS arm64 *************"
	scons TARGET_OS=ios TARGET_ARCH=arm64 SYS_VERSION=7.0 RELEASE=$1 $2
}

function build_simulator()
{
	echo "*********** Build for simulator plugin *************"
	scons SIMULATOR=1 RELEASE=$1 $2
}

function unit_tests()
{
	echo "*********** Unit test Start *************"
	scons VERBOSE=$VERBOSE resource RELEASE=false -c
	scons VERBOSE=$VERBOSE resource LOGGING=false RELEASE=false
	scons VERBOSE=$VERBOSE resource TEST=1 RELEASE=false
	echo "*********** Unit test Stop *************"
}

function  help()
{
	echo "Usage:"
        echo "  build:"
        echo "     `basename $0` <target_build>"
	echo "      Allowed values for <target_build>: all, linux_unsecured, linux_secured, linux_unsecured_with_ra, linux_secured_with_ra, linux_unsecured_with_rd, linux_secured_with_rd, android, arduino, tizen, simulator, darwin, darwin_unsecured, darwin_secured, darwin_unsecured_with_java, darwin_unsecured_with_java, ios"
	echo "      Note: \"linux\" will build \"linux_unsecured\", \"linux_secured\", \"linux_unsecured_with_ra\", \"linux_secured_with_ra\", \"linux_secured_with_rd\" & \"linux_unsecured_with_rd\"."
	echo "      Any selection will build both debug and release versions of all available targets in the scope you've"
	echo "      selected. To choose any specific command, please use the SCons commandline directly. Please refer"
	echo "      to [IOTIVITY_REPO]/Readme.scons.txt."
        echo "  clean:"
        echo "     `basename $0` -c"
}

# Suppress "Reading ..." message and enable parallel build
export SCONSFLAGS="-Q -j 4"

# defaults
JAVA=OFF			# j
PHASE=dev			# p:  dev | prod ( | test?)
REMOTE_ACCESS=false		# a
RESOURCE_DIRECTORY=false	# d
ROUTING_MANAGER=false		# m
SECURED=0			# s
TARGET_OS='darwin' # `uname`    # o:
TARGET_TRANSPORT=IP		# t:
verbose=false			# v

while getopts adjmp:o:rst:v OPT
do
    case $OPT in
	a) REMOTE_ACCESS=true
	   ;;
	d) RESOURCE_DIRECTORY=true
	   ;;
	j) BUILD_JAVA='ON'
	   ;;
	m) ROUTING_MANAGER=true
	   ;;
	o) TOS=$OPTARG
	   echo "TOS: $TOS"
	   TARGET_OS=`echo "$TOS" | tr '[A-Z]' '[a-z]'`
	   ;;
	p) PHASE=$OPTARG
	   if PHASE == dev; then
	       RELEASE=0
	   else
	       RELEASE=0
	   fi
	   ;;
	r) ROUTING_MANAGER=true
	   ;;
	s) SECURED=1
	   ;;
	t) TARGET_TRANSPORT=$OPTARG
	   ;;
	v) VERBOSE=true
	   ;;
    esac
done

echo "JAVA: " $JAVA
echo "RELEASE?: " $RELEASE
echo "REMOTE_ACCESS: " $REMOTE_ACCESS
echo "RESOURCE_DIRECTORY: " $RESOURCE_DIRECTORY
echo "ROUTING_MANAGER: " $ROUTING_MANAGER
echo "SECURED: " $SECURED
echo "TARGET_OS: " $TARGET_OS
echo "TARGET_TRANSPORT: " $TARGET_TRANSPORT
echo "VERBOSE: " $VERBOSE

set -x

scons RELEASE=$RELEASE \
      TARGET_OS=$TARGET_OS \
      SECURED=$SECURED \
      BUILD_JAVA=$JAVA \
      TARGET_TRANSPORT=$TARGET_TRANSPORT