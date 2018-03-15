bind(name = "android/crosstool", actual = "@android_ndk//:toolchain-libcpp")

local_repository(
	name = "openocf",
	path = "/home/gar/openocf",
)

android_sdk_repository(
    name="androidsdk",
    path="/home/gar/android/sdk",
    api_level=26,	   # Oreo, 8.0.0, required for java.nio.file.*
    # default uses latest build tools installed
    # build_tools_version="27.0.3"
)

android_ndk_repository(
    name="androidndk",
    path="/home/gar/android/android-ndk-r14b",
    #path="/home/gar/android/sdk/ndk-bundle/",
    api_level=26,
)

## toolchain repos
new_local_repository(
  name = "toolchain_mingw64",
  path = "c:/tools/msys64/mingw64",
  build_file = 'platforms/windows/toolchain.BUILD',
)

new_local_repository(
  name = "toolchain_ndk",
  path="/home/gar/android/sdk/ndk-bundle/",
  build_file = 'platforms/ndk/toolchain.BUILD',
)

new_local_repository(
  name = "toolchain_rpi3b",
  path = "/home/gar/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf", # linux
  # path = "/Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf", # os x
  build_file = 'platforms/rpi3b/toolchain.BUILD',
)

new_local_repository(
  name = "toolchain_wrlinux",
  path = "/Volumes/CrossToolNG/x86_64-unknown-linux-gnu",
  build_file = 'platforms/wrlinux/toolchain.BUILD',
)

# cross-compiled cosysroots
# these will contain crosscompiled third party libs (ncurses, cdk)
new_local_repository(
  name = "cosysroot_rpi3b",
  path = "/home/gar/cosysroots/rpi3b",
  # path = "/home/gar/cosysroots/rpi3b",
  build_file = "platforms/rpi3b/cosysroot.BUILD"
)

new_local_repository(
  name = "cosysroot_wrlinux",
  path = "/home/gar/cosysroots/wrlinux",
  build_file = "platforms/wrlinux/cosysroot.BUILD"
)

new_local_repository(
  name = "cosysroot_ndk",
  path = "/home/gar/cosysroots/ndk",
  build_file = "platforms/ndk/cosysroot.BUILD"
)

## local repo, for access to stuff in /usr, e.g. cdk
new_local_repository(
    name = "usr_sys",
    path = "/usr",
    # build_file = "platforms/darwin/cosysroot.BUILD"
    build_file = "platforms/linux/cosysroot.BUILD"
)

new_local_repository(
    name = "usr_local_linux",
    path = "/usr/local",
    build_file = "platforms/linux/sysroot.BUILD"
)

new_local_repository(
    name = "usr_local_macos",
    path = "/usr/local",
    build_file = "platforms/darwin/cosysroot.BUILD"
)

android_sdk_repository(
    name="androidsdk",
    path="/home/gar/android/sdk",
    api_level=22,
    # build_tools_version="27.0.0"
)

android_ndk_repository(
    name="androidndk",
    path="/home/gar/android/sdk/ndk-bundle/",
    api_level=23,
)
