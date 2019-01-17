load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl",
     "http_archive",
     "http_file",
     "http_jar",
)

# bind(name = "android/crosstool", actual = "@android_ndk//:toolchain-libcpp")

git_repository(
    name = "build_bazel_rules_apple",
    remote = "https://github.com/bazelbuild/rules_apple.git",
    tag = "0.10.0",
)

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()

# apple_support_dependencies()

# for ios_unit_test and ios_ui_test:
# http_file(
#     name = "xctestrunner",
#     executable = 1,
#     urls = ["https://github.com/google/xctestrunner/releases/download/0.2.5/ios_test_runner.par"],
# )

# git_repository(
#     name = "bazel_skylib",
#     remote = "https://github.com/bazelbuild/bazel-skylib.git",
#     tag = "0.3.1",
# )

android_sdk_repository(
    name="androidsdk",
    path="/home/gar/sdk/android",
    api_level=27,	   # 27, 26: Oreo; 25, 24: Nougat
    # default uses latest build tools installed
    # build_tools_version="27.0.3"
)

android_ndk_repository(
    name="androidndk",
    #path="/home/gar/android/android-ndk-r14b",
    #path="/home/gar/sdk/android/ndk-bundle/",
    api_level=26,
)

local_repository(
	name = "openocf",
	path = "/home/gar/openocf",
)

## toolchain repos
new_local_repository(
  name = "toolchain_mingw64",
  path = "c:/tools/msys64/mingw64",
  build_file = 'platforms/windows/toolchain.BUILD',
)

# new_local_repository(
#   name = "toolchain_ndk",
#   path="/home/gar/sdk/android/ndk-bundle/",
#   build_file = 'platforms/ndk/toolchain.BUILD',
# )

new_local_repository(
  name = "toolchain_rpi3b",
  # linux:
  # path = "/home/gar/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf",
  # osx:
  path = "/Volumes/CrossToolNG/armv8-rpi3-linux-gnueabihf",
  build_file = 'platforms/rpi3b/toolchain.BUILD',
)

new_local_repository(
  name = "ctng",
  path = "/Volumes/CrossToolNG",
  build_file = 'tools/ctng/ctng.BUILD'
)

new_local_repository(
  name = "toolchain_wrlinux",
  path = "/Volumes/CrossToolNG/x86_64-unknown-linux-gnu",
  build_file = 'platforms/wrlinux/toolchain.BUILD',
)

# cross-compiled cosysroots
# these will contain crosscompiled third party libs (ncurses, cdk)
new_local_repository(
  name = "cosysroot_linux_rpi3b",
  path = "/home/gar/cosysroots/rpi3b",
  build_file = "platforms/rpi3b/cosysroot.BUILD"
)

new_local_repository(
  name = "cosysroot_osx_rpi3b",
  path = "/Users/gar/cosysroots/rpi3b",
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

################################################################
####  External repos
# Third-party package for Boost, from https://github.com/nelhage/rules_boost
# This will download and compile Boost when you reference labels like "@boost//:iostreams"
# git_repository(
#     name = "com_github_nelhage_rules_boost",
#     commit = "239ce40e42ab0e3fe7ce84c2e9303ff8a277c41a",
#     remote = "https://github.com/nelhage/rules_boost",
# )
# load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
# boost_deps()

# libcoap
http_archive(
    name = "libcoap",
    build_file = "@//:src/coap/libcoap/BUILD",
    urls = ["https://github.com/dthaler/libcoap/archive/IoTivity-1.4.zip"],
    sha256="d68062be5b8c82309295847b7d1924a78d466bdf32f691470af86184bfa8810d",
    strip_prefix = "libcoap-IoTivity-1.4"
)
    # urls = ["https://github.com/obgm/libcoap/archive/v4.1.2.zip"],
    # sha256 = "4caf73446a4994a0571afaf5e60a62a749bda92a29e9407174614733e984c238",
    # strip_prefix = "libcoap-4.1.2",

# new_local_repository(
# 	name = "coap_hdrs",
# 	path = "/Users/gar/iotivity/gerrit/config/darwin",
# 	build_file = "config/darwin/BUILD",
# 	#build_file_content = "filegroup(name='foo', srcs=['darwin/coap.h','darwin/coap_config.h'])"
# )

http_archive(
    name = "mbedtls",
    urls = ["https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.4.2.zip"],
    sha256 = "dacb9f5dd438c456b9ef6627637f46e16fd41e86d828217ec9f8547d3d22a338",
    strip_prefix = "mbedtls-mbedtls-2.4.2",
    build_file = "@//:src/sec/mbedtls/BUILD",
)
    # urls = ["https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.7.zip"],
    # sha256 = "ab1ea18e7fa721bb4ab08294ab34008c4a934841b037a3cfa5f7ce65648228a2",
    # strip_prefix = "mbedtls-mbedtls-2.7",

# tinycbor
http_archive(
    name = "tinycbor",
    build_file = "@//:src/cbor/tinycbor/BUILD",
    urls = ["https://github.com/intel/tinycbor/archive/v0.5.1.zip"],
    sha256 = "48e664e10acec590795614ecec1a71be7263a04053acb9ee81f7085fb9116369",
    strip_prefix = "tinycbor-0.5.1",
)
    # urls = ["https://github.com/intel/tinycbor/archive/v0.4.1.zip"],
    # sha256 = "c5ace3389cfdd6a121824a26f9c14486df298ba0088209d67aef5bc192827d6e",
    # strip_prefix = "tinycbor-0.4.1",

# https://troydhanson.github.io/uthash/
http_archive(
    name = "uthash",
    urls = ["https://github.com/OpenOCF/uthash/archive/v2.0.2.zip"],
    # urls = ["https://github.com/troydhanson/uthash/archive/v2.0.2.zip"],
    sha256 = "1c56f7e75ee42e162fb3f72cede7738118c3e6fb88514d40db6afc952dcd7cae",
    strip_prefix = "uthash-2.0.2",
    build_file = "@//:src/util/uthash/BUILD"
)

# gtest
http_archive(
    name = "gtest",
    urls = ["https://github.com/google/googletest/archive/release-1.7.0.zip"],
    sha256 = "b58cb7547a28b2c718d1e38aee18a3659c9e3ff52440297e965f5edffe34b6d0",
    strip_prefix = "googletest-release-1.7.0",
    build_file = "@//:config/gtest.BUILD",
)

# sqlite3
http_archive(
    name = "sqlite3",
    urls = ["https://www.sqlite.org/2015/sqlite-amalgamation-3081101.zip"],
    sha256 = "a3b0c07d1398d60ae9d21c2cc7f9be6b1bc5b0168cd94c321ede9a0fce2b3cd7",
    strip_prefix = "sqlite-amalgamation-3081101",
    build_file = "@//:config/sqlite3.BUILD",
)

# cJSON
http_archive(
    name = "cjson",
    urls = ["https://github.com/DaveGamble/cJSON/archive/v1.7.7.zip"],
    sha256 = "9494f0a9b2005f471c846fbdbb40a9de074ad5aee56e3e73518055cd639fa77e",
    strip_prefix = "cJSON-1.7.7",
    build_file = "@//:config/cjson.BUILD",
)

## For Java demo pgms:

maven_jar(
    name = "slf4j",
    artifact = "org.slf4j:slf4j-api:1.7.25",
    sha1 = "da76ca59f6a57ee3102f8f9bd9cee742973efa8a"
# sha1_src = "962153db4a9ea71b79d047dfd1b2a0d80d8f4739"
)

maven_jar(
    name = "slf4j_jul",
    artifact = "org.slf4j:slf4j-jdk14:1.7.25",
    sha1 = "bccda40ebc8067491b32a88f49615a747d20082d",
# sha1_src = "f62afc0e42c562f3afae648b95e755675102d3f9"
)

maven_jar(
    name = "slf4j_log4j",
    artifact = "org.slf4j:slf4j-log4j12:1.7.25",
#    sha1 = "110cefe2df103412849d72ef7a67e4e91e4266b4"
)

maven_jar(
    name = "log4j",
    artifact = "log4j:log4j:1.2.17",
    sha1 = "5af35056b4d257e4b64b9e8069c0746e8b08629f",
)

maven_jar(
    name = "slf4j_simple",
    artifact = "org.slf4j:slf4j-simple:1.7.25",
    sha1 = "8dacf9514f0c707cbbcdd6fd699e8940d42fb54e",
# sha1_src = "af3cd3ad1ea4b08b27b54f12529b4bf343bc5ca1"
)

