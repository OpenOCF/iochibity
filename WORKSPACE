# this will contain crosscompiled third party libs (ncurses, cdk)
new_local_repository(
  name = "cosysroot_rpi3b",
  path = "/Users/gar/cosysroots/rpi3b",
  build_file = "platforms/rpi3b/cosysroot.BUILD"
)

## toolchain repos
new_local_repository(
  name = "toolchain_ndk",
  path = "/Users/gar/android/android-ndk-r14b",
  build_file = 'platforms/ndk/toolchain.BUILD',
)

new_local_repository(
  name = "toolchain_rpi3b",
  path = "/Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf",
  build_file = 'platforms/rpi3b/toolchain.BUILD',
)

new_local_repository(
  name = "toolchain_wrlinux",
  path = "/Volumes/CrossToolNG/x86_64-unknown-linux-gnu",
  build_file = 'platforms/wrlinux/toolchain.BUILD',
)

# cross-compiled cosysroots
new_local_repository(
  name = "cosysroot_rpi3b",
  path = "/Users/gar/cosysroots/rpi3b",
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
    name = "usr_local",
    path = "/usr/local",
    build_file = "platforms/darwin/cosysroot.BUILD"
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

