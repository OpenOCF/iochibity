## package CDK (Curses Dev Kit) for examples
new_local_repository(
    name = "sys_cdk",
    path = "/usr/local",
    build_file_content =
"""
cc_library(
   name = "cdk-pkg",
   hdrs = ["include/cdk.h"] + glob(["include/cdk/*.h"]),
   linkstatic = 1,
   linkopts = ["-lncurses",
   "-L/lib64"], # linux
   srcs = ["lib/libcdk.a"],
   visibility = ["//visibility:public"],
)
""")

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

# new_local_repository(
#   name = "android_ndk_repo",
#   path = $ANDROID_NDK_HOME,
#   build_file = 'compilers/android-ndk.BUILD'
# )

# new_local_repository(
#   name = "android_ndk_repo",
#   path = "/Users/gar/android/android-ndk-r15c",
#   build_file = 'tools/android-ndk.BUILD',
# )

new_local_repository(
  name = "rpi3b_repo",
  path = "/Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf",
  build_file = 'platforms/rpi3b/repo.BUILD',
)
