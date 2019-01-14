genrule(
    name = "client_json",
    srcs = ["client_config.edn"],
    outs = ["client_config.json"],
    cmd = "/usr/local/bin/edn-to-json $(location client_config.edn)  > \"$@\"",
)

# FIXME: make this dependent on json2cbor too
genrule(
    name = "client_config",
    visibility = ["//visibility:public"],
    srcs = [":client_json"],
    outs = ["svrs.cbor"],
    cmd = "/home/gar/bin/json2cbor $(location :client_json) > \"$@\"",
)
