java_binary(
    name = "Client",
    srcs = glob(["src/main/java/**/*.java"]),
    deps = [ # "@openocf_java//:OpenOCFJ",
        "//:OpenOCFJava",
        "//demos/stdjava/common"],
    classpath_resources = [":client_config"],
    resources = ["src/main/resources/logger.properties"],
    jvm_flags = ["-Djava.util.logging.config.file=logger.properties"]
)

# prereq: edn-to-json and json2cbor are on the path (e.g. $HOME/bin)
genrule(
    name = "client_json",
    srcs = ["src/main/resources/client_owner_config.edn"],
    outs = ["client_config.json"],
    cmd = "edn-to-json $(location src/main/resources/client_owner_config.edn)  > \"$@\"",
)

genrule(
    name = "client_config",
    srcs = [":client_json"],
    outs = ["svrs.cbor"],
    cmd = "json2cbor $(location :client_json) > \"$@\"",
)
