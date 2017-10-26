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
