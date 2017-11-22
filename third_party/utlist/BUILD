licenses(["permissive"])

# utlist.h is not to be processed by makeheaders, since doing so leads
# to problems with libcoap, which contains its own copy of
# utlist.h. so we keep it separate from our source code. it must be
# included wherever used.
cc_library(
    name = "utlist",
    hdrs = ["utlist.h"],
    visibility = ["//visibility:public"]
)
