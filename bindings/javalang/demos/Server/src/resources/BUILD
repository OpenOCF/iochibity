genrule(
    name = "server_json",
    srcs = ["server_config.edn"],
    outs = ["server_config.json"],
    cmd = "edn-to-json $(location server_config.edn)  > \"$@\"",
)

# FIXME: make this dependent on json2cbor too
genrule(
    name = "server_config",
    visibility = ["//visibility:public"],
    srcs = [":server_json"],
    outs = ["svrs.cbor"],
    cmd = "json2cbor $(location :server_json) > \"$@\"",
)
