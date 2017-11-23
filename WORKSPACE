## toolchain repos
new_local_repository(
  name = "toolchain_ndk",
  path = "/Users/gar/android/android-ndk-r14b",
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
  # path = "/Users/gar/cosysroots/rpi3b",
  build_file = "platforms/rpi3b/cosysroot.BUILD"
)

new_local_repository(
  name = "cosysroot_wrlinux",
  path = "/Users/gar/cosysroots/wrlinux",
  build_file = "platforms/wrlinux/cosysroot.BUILD"
)

new_local_repository(
  name = "cosysroot_ndk",
  path = "/Users/gar/cosysroots/ndk",
  build_file = "platforms/ndk/cosysroot.BUILD"
)

## local repo, for access to stuff in /usr/local, e.g. cdk
new_local_repository(
    name = "usr_sys",
    path = "/usr",
    # build_file = "platforms/darwin/cosysroot.BUILD"
    build_file = "platforms/linux/cosysroot.BUILD"
)

new_local_repository(
    name = "usr_local",
    #path = "/usr/lib",
    path = "/usr/local", # macos
    # build_file = "platforms/darwin/cosysroot.BUILD"
    build_file = "platforms/linux/cosysroot.BUILD"
)

android_sdk_repository(
    name="androidsdk",
    path="/Users/gar/android/sdk",
    api_level=22,
    # build_tools_version="27.0.0"
)

android_ndk_repository(
    name="androidndk",
    path="/Users/gar/android/android-ndk-r14b",
    api_level=23,
)
