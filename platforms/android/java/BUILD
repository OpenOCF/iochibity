android_binary(
    name = "hello",
    srcs = glob([
        "java/org/example/hello/*.java",
    ]),
    manifest = "AndroidManifest.xml",
    resource_files = glob(["res/**"]),
    deps = [
        "//src/ocf",
        # ":openocf_jni",
        "@androidsdk//com.android.support:appcompat-v7-25.0.0",
    ],
)

# cc_library(
#     name = "openocf_jni",
#     srcs = ["jni/hello_api.c",
#             "jni/org_example_hello_HelloApi.h",
#             "jni/goodbye_api.c",
#             "jni/org_example_hello_GoodbyeApi.h"]
# )

