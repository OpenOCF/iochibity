# define /usr/local packages here

# cdk = Curses Development Kit http://invisible-island.net/cdk/
cc_library(
    name = "cdk",
    hdrs = glob(["include/**/*.h"]),
    linkstatic = 1,
    #linkopts = ["-Lexternal/usr_local_linux/lib", "-lncurses"],
    linkopts = ["-lncurses"],
    # srcs = ["lib/x86_64-linux-gnu/libcdk.so"],
   srcs = ["lib/libcdk.a", # built locally, installed in /usr/local/lib
           # "lib/libncurses.a"
   ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "ncurses",
    hdrs = glob(["include/**/*.h"]),
    linkstatic = 1,
    # srcs = ["lib/x86_64-linux-gnu/libncurses.so"],
   srcs = ["lib/libncurses.so"], # /usr/local
    visibility = ["//visibility:public"],
)
