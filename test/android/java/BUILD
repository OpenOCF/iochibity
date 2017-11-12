# run from adb:
# $ adb shell
# am start -n org.example.hello/org.example.hello.MainActivity

android_library(
    name = "OpenOCF",
    srcs = ["main/java/openocf/Service.java"],
)

android_binary(
    name = "openocf",
    srcs = glob([
        "main/java/org/example/hello/*.java",
    ]),
    manifest = "manifests/AndroidManifest.xml",
    resource_files = glob(["res/**"]),
    deps = [
        ":OpenOCFAndroid",
        # ":libopenocf_jni.so",
        "@androidsdk//com.android.support:support-v4-21.0.2",
        "@androidsdk//com.android.support:support-v13-21.0.2",
        "@androidsdk//com.android.support:cardview-v7-21.0.2",
        "@androidsdk//com.android.support:appcompat-v7-22.0.0",
    ],
)

# to select a target cpu pass --fat_apk_cpu cpu[,cpu]*
cc_library(
    name = "libopenocf_jni.so",
    copts = ["-Isrc/util"],
    deps = ["//src/ocf"],
    # linkshared = 1,
    srcs = ["main/jni/openocf_Service.c",
            "main/jni/openocf_Service.h"]
)

# to generate the jni headers, comment out the :jni dep in the :hello
# target, then build the following. Copy the resulting header into
# src/main/android/jni, then restore the :jni dep and comment out the
# following. A ghastly hack, but it works.

# unfortunately the following only works for one file at a time. we're
# probably better of building the lib and running a shell script to
# run javah

# genrule(
#   name = "gen_headers",
# # outs must list one file per app jni class contained in the srcs jar(s)
#     outs = [#"org_example_hello_HelloApi.h"],
#           "jni/org_example_hello_Goodbye.h"],
#   srcs = [":hello_deploy.jar"],
#   cmd = """
# mkdir classes_tmp
# unzip -d classes_tmp $(location :hello_deploy.jar) > /dev/null
# pwd
# # run javah once per class
# #javah -classpath classes_tmp -o $@ org.example.hello.HelloApi
#   # javah -classpath classes_tmp -o "`pwd`src/main/android/org_example_hello_HelloApi.h" org.example.hello.GoodbyeApi
# javah -classpath classes_tmp -o $@ org.example.hello.GoodbyeApi
# # javah -classpath classes_tmp -o org_example_hello_GoodbyeApi org.example.hello.GoodbyeApi
# rm -rf classes_tmp
# """)

# this won't work either. it works in a shell script but not in bazel:

# genrule(
#   name = "gen_headers",
#     outs = [#"org_example_hello_HelloApi.h"],
#           "jni/org_example_hello_Goodbye.h"],
#   srcs = glob(["src/main/android/java/**/*.java"]),
#   cmd = "javah -cp src/main/android/java -d src/main/android/jni org.example.hello.HelloApi"
# )
