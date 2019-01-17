load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:java.bzl", "java_import_external")
#load("@bazel_tools//tools/jdk:default_java_toolchain.bzl", "current_java_runtime")
load("//config:variables.bzl", "CSTD", "OS_COPTS")


# cc_library(
#     name = "jni_headers",
#     srcs = [
#         "@local_jdk//:jni_header",
#         "@local_jdk//:jni_md_header-linux",
#     ],
#     includes = [
#         "external/local_jdk/include",
#         "external/local_jdk/include/linux",
#     ],
#     visibility = [
#         "//visibility:public",
#     ],
# )

# NOTE that *.so works on OS X.
cc_binary(name = "libopenocf_jni.so",
          linkshared=1,
          copts = ["-std=c11",
                   "-Iinclude",
                   "-Isrc/portability",
                   "-Ibindings/javalang", # app jni headers
                   "-Iexternal/bindings/javalang/pojo/jni", # app jni headers
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
             #"@bazel_tools//tools/jdk:jni_header"
          ] + select({"//config:darwin_with_jni": [#"@bazel_tools//tools/jdk:jni_md_header-darwin",
                                                   "@local_jdk//:jni_md_header-darwin"],
                      "//config:linux_with_jni": [#"@bazel_tools//tools/jdk:jni_md_header-linux",
                                                  "@local_jdk//:jni_md_header-linux"],
                      "//conditions:default": ["BROKEN"]}),
          deps = ["//src/portability",
                  "//include",
                  "//:openocf",
                  #"@openocf//src/ocf", # static - everything stuffed into the jni shared lib
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
    resources = [":libopenocf_jni.so"],
    visibility = ["//visibility:public"]
)

java_library(
    name = "OpenOCFAndroid",
    srcs = glob(["main/**/*.java"],
                exclude = ["main/openocf/ConfigJava.java"]),
    resources = [":libopenocf_jni.so"],
    visibility = ["//visibility:public"]
)

# Just the Java API, no jni. Provides sources (classfiles) for :javah target.
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
    srcs = ["OpenOCFJ"],
    outs = ["openocf_app_CoResourceSP.h",
            "openocf_app_ResourceSP.h",
            "openocf_Message.h",
            "openocf_message_InboundRequest.h",
            "openocf_message_InboundResponse.h",
            "openocf_message_OutboundRequest.h",
            "openocf_Endpoint.h",
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
        javah -classpath classes_tmp -o $$f $$filename
    done
   rm -rf classes_tmp
""")
#    echo $$filename


