# define /usr/local packages here

# cdk = Curses Development Kit http://invisible-island.net/cdk/
cc_library(
   name = "cdk",
   # hdrs = ["include/cdk.h"] + glob(["include/cdk/*.h"]),
   hdrs = glob(["include/**/*.h"]),
   linkstatic = 1,
   linkopts = ["-lncurses"],
   # srcs = ["lib/libcdk.a"],
   # srcs = ["lib/libcdk.a"],
   srcs = ["lib/x86_64-linux-gnu/libcdk.so"],
   visibility = ["//visibility:public"],
)

cc_library(
   name = "ncurses",
   hdrs = glob(["include/**/*.h"]),
   linkstatic = 1,
   # linkopts = ["-lncurses"],
   srcs = ["lib/x86_64-linux-gnu/libncurses.so"],
   visibility = ["//visibility:public"],
)
