java_library(
    name = "Common",
    srcs = ["Common/org/openocf/test/PojoLogger.java"],
    deps = ["//bindings/javalang/pojo:OpenOCFJ",
            "@slf4j//jar"],
    #runtime_deps = ["@slf4j_jul//jar"],
    visibility = ["//bindings/javalang/pojo/demos:__pkg__"],
)

# uberjar target:  Server:Server_deploy.jar
java_binary(
    name = "Server", # matches java file containing main routine
    srcs = glob(["Server/src/main/java/**/*.java"]),
    deps = [":Common",
            "//bindings/javalang/pojo:OpenOCFJava",
            "@slf4j//jar"],
    runtime_deps = ["@slf4j_log4j//jar",
                    "@log4j//jar"],
    # OpenOCFJ expects to find svrs.cbor at root, so we need:
    classpath_resources = ["//bindings/javalang/pojo/demos/Server/src/resources:server_config",
                           "//bindings/javalang/pojo/demos/Server/src/resources:log4j.properties"],

)

java_binary(
    name = "Client",
    srcs = glob(["Client/src/main/java/**/*.java"]),
    deps = [":Common",
            "//bindings/javalang/pojo:OpenOCFJava",
            "@slf4j//jar"],
    runtime_deps = ["@slf4j_log4j//jar",
                    "@log4j//jar"],
    classpath_resources = ["//bindings/javalang/pojo/demos/Client/src/resources:client_config",
                           "//bindings/javalang/pojo/demos/Client/src/resources:log4j.properties"],

    #runtime_deps = ["@slf4j_simple//jar"],
    #runtime_deps = ["@slf4j_jul//jar"],
    # NB: JUL logging config file is relative to current dir, not classpath!
    # jvm_flags = ["-Djava.util.logging.config.file=./bindings/javalang/pojo/demos/CLient/src/resources/logger.properties"]
)
