# uberjar target:  Server:Server_deploy.jar
java_binary(
    name = "Server", # matches java file containing main routine
    srcs = glob(["Server/src/main/java/**/*.java"]),
    deps = ["//bindings/javalang:OpenOCFJava",
            "//bindings/javalang/demos/java/common"],
    # OpenOCFJ expects to find svrs.cbor at root, so we need:
    classpath_resources = ["//bindings/javalang/demos/Server/src/resources:server_config"],
)

java_binary(
    name = "Client",
    srcs = glob(["Client/src/main/java/**/*.java"]),
    deps = ["//bindings/javalang:OpenOCFJava",
            "//bindings/javalang/demos/java/common"],
    # srcs = glob(["Client/src/main/java/**/*.java"]),
    # deps = ["//bindings/javalang:OpenOCFJava",
    #         "//bindings/javalang/demos/java/common"],
    # deps = [ # "@openocf_java//:OpenOCFJ",
    #     "//:OpenOCFJava",
    #     "//demos/stdjava/common"],
    classpath_resources = ["//bindings/javalang/demos/Client/src/resources:client_config"],
    #classpath_resources = [":client_config"],
    #resources = ["//bindings/javalang/demos/Client/src/main/resources/logger.properties"],
    jvm_flags = ["-Djava.util.logging.config.file=logger.properties"]
)

# # prereq: edn-to-json and json2cbor are on the path (e.g. $HOME/bin)
# genrule(
#     name = "client_json",
#     srcs = ["src/main/resources/client_owner_config.edn"],
#     outs = ["client_config.json"],
#     cmd = "edn-to-json $(location src/main/resources/client_owner_config.edn)  > \"$@\"",
# )

# genrule(
#     name = "client_config",
#     srcs = [":client_json"],
#     outs = ["svrs.cbor"],
#     cmd = "json2cbor $(location :client_json) > \"$@\"",
# )
