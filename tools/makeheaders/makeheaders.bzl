# filegroup(
#   name = "mh_srcs",
#   srcs = glob(["src/**/*.c"]),
# )

MkHdrsList = provider(
    fields = {
        'files' : 'list of source files to be processed by makeheaders'
    }
)


# this gets called once per dep (BUILD target)
def _make_headers_aspect_impl(target, ctx):
    localfiles = []
    # print("FC aspect %s" % target.label)
    # Make sure the rule has a srcs attribute.
    if hasattr(ctx.rule.attr, 'srcs'):
        # Iterate through the sources counting files
        for src in ctx.rule.attr.srcs:
            srcfiles = [name for name in src.files if
                        name.path.startswith('src')]
            for f in srcfiles:
                #print(f.path)
                localfiles.append(f)
    for dep in ctx.rule.attr.deps:
       localfiles = localfiles + (dep[MkHdrsList].files)
    return [MkHdrsList(files = localfiles)]

make_headers_aspect = aspect(implementation = _make_headers_aspect_impl,
    attr_aspects = ['deps'],
    # attrs = {
    #     'extension' : attr.string(values = ['*', 'h', 'c']),
    # }
)

def _prep_headers_rule_impl(ctx):
    files = depset()
    for dep in ctx.attr.hdr_deps:
        files = files + (dep[MkHdrsList].files)

    # l = sorted(files.to_list())
    # mkhdrs = "\n".join(l)
    # # print(mkhdrs)

    # hdrs = [ctx.actions.declare_file((name.basename)[:-1] + "h", sibling=name)
    #             for name in files if name.path.endswith('.c')]

    # for h in hdrs:
    #   print(" h: %s" % h)

    # print(files)
    # print(hdrs)

    fnames = "\n".join([name.path for name in sorted(files)
                        if (name.path.endswith("c")
                            or name.basename.startswith("_"))])
    # print("\n" + fnames)
    dat = ctx.outputs.dat
    ctx.actions.write(output=dat,
                      content=fnames)

#     ctx.actions.run(
# #      inputs=[dat],  # files,
#       outputs=hdrs,
#       mnemonic = "makeheaders",
#       arguments=["-f", "mkhdrs.dat"],
#       progress_message="Making headers", #  into %s" % ctx.outputs.out.short_path,
#       executable=ctx.executable._mkhdrs_tool)

    # print("outfiles:\n%s" % outfiles)

    # ctx.attr.tool

# def _mh_rule_impl(ctx):
#     for s in ctx.attr.srcs:
#       print(" - %s" % s)

    # ctx.actions.run(
    #   inputs=[ctx.outputs.dat],
    #   outputs=hdrs,
    #   arguments=["-f", "mkhdrs.dat"],
    #   progress_message="Making headers", #  into %s" % ctx.outputs.out.short_path,
    #   executable=ctx.executable._mkhdrs_tool)

prep_headers = rule(
    implementation = _prep_headers_rule_impl,
    outputs={"dat": "mkhdrs.dat"},
    attrs = {
      # "srcs": attr.label_list(allow_files=False),
      'hdr_deps' : attr.label_list(aspects = [make_headers_aspect], allow_files=False),
      "_mkhdrs_tool": attr.label(executable=True,
                                 cfg="host",
                                 # allow_files=True,
                                 default=Label("//tools/makeheaders:makeheaders")),
      # "tool" : attr.label(default=Label("//tools/makeheaders"))
    },
)

def _make_headers_rule_impl(ctx):
  print("foo")

make_headers = rule(
    implementation = _make_headers_rule_impl,
    # outputs={"dat": "mkhdrs.dat"},
    attrs = {
      # "srcs": attr.label_list(allow_files=False),
      'deps' : attr.label_list(aspects = [make_headers_aspect], allow_files=False),
      "_mkhdrs_tool": attr.label(executable=True,
                                 cfg="host",
                                 # allow_files=True,
                                 default=Label("//tools/makeheaders:makeheaders")),
      # "tool" : attr.label(default=Label("//tools/makeheaders"))
    },
)
