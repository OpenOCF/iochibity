#
# Copyright 2015 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "toolchain",
    srcs = [
        # ":cc-compiler-armeabi-v7a",
        ":empty",
        ":everything",
        # "//android-ndk/platforms/android-23/arch-arm:everything",
    ],
)

filegroup(
    name = "empty",
    srcs = [],
)

cc_library(
    name = "malloc",
    srcs = [],
)


filegroup(
    name = "compile",
    srcs = glob(
        [
            # "sysroot/**/*.h",
            # "sysroot/arm-linux-androideabi",
            # "toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/**/*.h",
            "platforms/android-23/arch-arm/usr/include/*.h",
        ],
    ),
)

filegroup(
    name = "link",
    srcs = glob(
        [
            "platforms/android-23/arch-arm/usr/lib/**/*.a",
            "platforms/android-23/arch-arm/usr/lib/**/*.o",
            "platforms/android-23/arch-arm/usr/lib/**/*.so",
        ],
    )
)

filegroup(
    name = "everything",
    srcs = [
        ":compile",
        ":link",
        "gcc-arm-android-4.9-toolchain",
    ],
)

filegroup(
    name = "objcopy",
    srcs = [
        "toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-objcopy"
    ],
)

filegroup(
    name = "strip",
    srcs = [
        "toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-strip"
    ],
)


filegroup(
    name = "gcc-arm-android-4.9-toolchain",
    srcs = glob([
        "toolchains/arm-linux-androideabi-4.9/**",
    ]),
    output_licenses = ["unencumbered"],
)

filegroup(
    name = "android-armeabi-v7a-files",
    srcs = [
        ":gcc-arm-android-4.9-toolchain",
        ":everything"
    ],
)
