# uberjar target:  Server:Server_deploy.jar
java_binary(
    name = "Server", # matches java file containing main routine
    srcs = glob(["Server/src/main/java/**/*.java"]),
    deps = ["//bindings/javalang:OpenOCFJava",
            "//bindings/javalang/demos/Common"],
    # OpenOCFJ expects to find svrs.cbor at root, so we need:
    classpath_resources = ["//bindings/javalang/demos/Server/src/resources:server_config"],
)

java_binary(
    name = "Client",
    srcs = glob(["Client/src/main/java/**/*.java"]),
    deps = ["//bindings/javalang:OpenOCFJava",
            "//bindings/javalang/demos/Common"],
    classpath_resources = ["//bindings/javalang/demos/Client/src/resources:client_config"],
    jvm_flags = ["-Djava.util.logging.config.file=logger.properties"]
)
