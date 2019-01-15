load("//config:variables.bzl", "CSTD", "OS_COPTS")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# NOTE that *.so works on OS X.
cc_binary(
    name = "libopenocf_jni.so",
    #alwayslink = 1,
    linkshared=1,
    copts = ["-std=c11",
             "-Iinclude",
             "-Isrc/portability",
             # "-Iexternal/openocf/include", # for wrapped lib
             # "-Iexternal/openocf/src/portability",
             #"-Iexternal/openocf/src/util",
             # "-Iexternal/openocf/third_party/cjson",
             # "-Iconfig/darwin",
             # "-Iconfig/darwin/coap",
             "-Iexternal/libcoap/include",
             "-Iexternal/libcoap/include/coap",
             "-Iexternal/tinycbor/src",
             "-Iexternal/local_jdk/include",
    ] + select({"//config:darwin_with_jni": ["-Iexternal/local_jdk/include/darwin"],
                "//config:linux_with_jni": ["-Iexternal/local_jdk/include/linux"],
                "//conditions:default": ["BROKEN"]}),

    srcs = glob(["jni/*.c"]) # jni layer
    + glob(["jni/*.h"])
    + [":javah",
       "@local_jdk//:jni_header",
       #"@openocf//src/ocf",
    ] + select({"//config:darwin_with_jni": ["@local_jdk//:jni_md_header-darwin"],
                "//config:linux_with_jni": ["@local_jdk//:jni_md_header-linux"],
                "//conditions:default": ["BROKEN"]}),
    deps = [#"@openocf//src/ocf", # static - everything stuffed into the jni shared lib
        "//src/portability",
            "//include",
            "//:openocf",
            # "@openocf//third_party/cjson",
            # "@openocf//third_party/coap",
            # "@tin//third_party/tinycbor",
        #"@openocf//include"
    ],
    visibility = ["//visibility:public"]
)

java_library(
    name = "OpenOCFJava",
    srcs = glob(["main/**/*.java"],
                exclude = ["main/openocf/ConfigAndroid.java"]),
    # + ["src/main/java/openocf/ConfigJava.java",
    #    "src/main/java/openocf/OpenOCF.java"],
    # deps = [":OpenOCFJ"],
    # exports = ["openocfjni"],
    resources = [":libopenocf_jni.so"],
    # runtime_deps = ["//src/main/jni:libopenocf_jni.so"],
    visibility = ["//visibility:public"]
)

java_library(
    name = "openocf_japi",
    srcs = glob(["main/**/*.java"],
                # exclude anything depending on Android
                exclude = ["main/**/ConfigAndroid.java"]),
    #+ glob(["src/std/**/*.java"]),
    # resources = ["libopenocf_jni.so"],
    # resources = ["libopenocf_jni.dylib"],
    visibility = ["//visibility:public"]
)

java_library(
    name = "OpenOCFJ",
    srcs = glob(["main/**/*.java"],
                exclude = ["main/openocf/ConfigAndroid.java",
                           "main/openocf/ConfigJava.java"]),
    visibility = ["//visibility:public"]
)

genrule(
    name = "javah",
    visibility = ["//visibility:public"],
    # srcs = [":openocf_japi"],
    srcs = ["OpenOCFJ"],
    outs = [# "src/main/jni/openocf_android_OCFService.h",
        "openocf_app_CoResourceSP.h",
        # "openocf_app_DefaultCoRSP.h",
        # "openocf_app_ICoResourceSP.h",
        "openocf_app_ResourceSP.h",
        # "openocf_app_IResourceSP.h",
        "openocf_Message.h",
        "openocf_message_InboundRequest.h",
        "openocf_message_InboundResponse.h",
        "openocf_message_OutboundRequest.h",
        "openocf_utils_Endpoint.h",
        "openocf_OpenOCF.h",
        "openocf_OpenOCFClient.h",
        "openocf_OpenOCFServer.h"
    ],
    cmd = """
    mkdir classes_tmp
    unzip -d classes_tmp $(location :OpenOCFJ) > /dev/null
    for f in $(OUTS)
    do
        filename=$$(basename -- "$$f")
        extension="$${filename##*.}"
        filename="$${filename%.*}"
        filename=$${filename//_/.}
        echo $$filename
        javah -classpath classes_tmp -o $$f $$filename
    done
   rm -rf classes_tmp
""")


