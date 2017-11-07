# this will contain crosscompiled third party libs (ncurses, cdk)
new_local_repository(
  name = "sysroot_rpi3b",
  path = "/Users/gar/sysroots/rpi3b",
  build_file = "platforms/rpi3b/sysroot.BUILD"
)

new_local_repository(
  name = "toolchain_rpi3b",
  path = "/Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf",
  build_file = 'platforms/rpi3b/toolchain.BUILD',
)

## local repo, for access to stuff in /usr/local, e.g. cdk
new_local_repository(
    name = "usr_local",
    path = "/usr/local",
    build_file = "platforms/darwin/sysroot.BUILD"
)

android_sdk_repository(
    name="androidsdk",
    # path="<full path to your Android SDK>",
    api_level=23
)

android_ndk_repository(
    name="androidndk",
    # path="<path to your Android NDK>",
    api_level=23
)

# android
android_sdk_repository(
    name="androidsdk",
    path="/Users/gar/android/sdk",
    api_level=23,
)

android_ndk_repository(
    name="androidndk",
    path="/Users/gar/android/android-ndk-r14b",
    api_level=23,
)
