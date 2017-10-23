## package CDK (Curses Dev Kit) for examples
new_local_repository(
    name = "sys_cdk",
    path = "/usr/local/",
    build_file_content =
"""
cc_library(
   name = "cdk-pkg",
   hdrs = glob(["include/cdk/*.h"]) + ["include/cdk.h"],
   linkopts = ["-lncurses"],
   srcs = ["lib/libcdk.a"],
   visibility = ["//visibility:public"],
)
""")