# cdk = Curses Development Kit http://invisible-island.net/cdk/
cc_library(
    name = "cdk",
    # hdrs = glob(["usr/include/**/*.h"]), # os x
    hdrs = glob(["usr/include/**/*.h"]),
    linkstatic = 1,
    # osx: srcs = glob(["usr/lib/arm-linux-gnueabihf/*.a"]), # this picks up the ncurses archives too
    # linux: srcs = glob(["usr/local/lib/*.a"]), # this picks up the ncurses archives too
    srcs = glob(["usr/lib/*.a"]), # this picks up the ncurses archives too
    visibility = ["//visibility:public"],
)

cc_library(
   name = "rpi3b",
   hdrs = glob(["usr/include/**/*.h"]),
   srcs = glob(["usr/lib/**/*.a",
   "usr/local/lib/*.a",
   "usr/lib/**/*.o",
   "usr/lib/**/*.so"]),
   visibility = ["//visibility:public"],
)
