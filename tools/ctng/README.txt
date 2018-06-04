crosstool-NG is a toolchain suite.

To expose ctng, we need to define local repo in WORKSPACE:

new_local_repository(
  name = "ctng",
  path = "/Volumes/CrossToolNG",
  build_file = 'tools/ctng/BUILD'
)

Then for each toolchain we want to support, we need:

1.  a toolchain definition in CROSSTOOL
2.  a cc_toolchain rule in ctng/BUILD

Alternatively, we could create one package per toolchain,
e.g. tools/ctng/armv8-rpi3-linux-gnueabihf/BUILD. The problem with
this is we would then need one local repo per toolchain.

CROSSTOOL expects paths rather than labels, e.g. to gcc. If you try to
reference the ctng repo, e.g. path: "@ctng//...", Bazel will interpret
@ctng literally. So you have to use either an absolute path (which
works, but is bad for portability), or a relative path, which will be
interpreted relative to the directory in which CROSSTOOL lives.

One way to handle this would be to install crosstool-NG inside of your
project. That obviously would not help much if you wanted to use it
for multiple projects; that's why we treat it as an external
local_repository.

The alternative is to create shell scripts within the CROSSTOOL
directory and have them call the real tools. For example:

# armv8-rpi3-linux-gnueabihf-gcc.sh
#!/bin/bash --norc
PATH="external/ctng/armv8-rpi3-linux-gnueabihf/bin:${PATH}" \
  exec \
  armv8-rpi3-linux-gnueabihf-gcc
  "$@"

Unfortunately it will not do to refer to this directly from within CROSSTOOL, e.g.

    path: "armv8-rpi3-linux-gnueabihf-gcc.sh"

This will give: "execvp(tools/ctng/armv8-rpi3-linux-gnueabihf-gcc.sh, ...)": No such file or directory

Toolpath stuff:
  tool_path { name: "gcc" path: "...."}

The tricky bit is that the path must be absolute or relative, as
indicated above; not only that, but the path must be exported by a
build file, so that Bazel knows about it. The problem with the example
above, which refers directly to a file in the CROSSTOOL directory, is
that the file is not "registered" with Bazel.  Compare this with the
need to list all srcs as inputs to a cc_library target.

The usual way to do this is with filegroups.  The sources listed in a
filegroup are thereby exposed to Bazel, so other rules may refer to
them - the purpose of filegroup (like glob) is to make sure that all
files are explicitly know to Bazel. So what we need to do is put
"armv8-rpi3-linux-gnueabihf-gcc.sh" in a filegroup, and then refer to
the filegroup instead of the file.

In this particular case, once we have exposed
"armv8-rpi3-linux-gnueabihf-gcc.sh" in a filegroup, CROSSTOOL can
reference the file.

To make the filegroups known to CROSSTOOL, use them in the
cc_toolchain rules. A cc_toolchain rule needs the following filegroup
parameters: all, compiler, linker, objcopy, strip, dynamic_runtime,
static_runtime, and dwp.  So we want one filegroup per parameter. The
purpose (I think) is to make explicitly know files available to
CROSSTOOL.

Note that the BUILD file for the ctng repo must define filegroups for
the resources it wants to expose. These will be included in the
cc_toolchain filegroups.



ct-ng list-samples:

[G..]   aarch64-rpi3-linux-gnueabi
[G..]   aarch64-unknown-linux-gnueabi
[G..]   aarch64-unknown-linux-uclibcgnueabi
[G..]   alphaev56-unknown-linux-gnu
[G..]   alphaev67-unknown-linux-gnu
[G..]   arm-bare_newlib_cortex_m3_nommu-eabi
[G..]   arm-cortex_a15-linux-gnueabihf
[G..]   arm-cortex_a8-linux-gnueabi
[G.X]   arm-cortexa5-linux-uclibcgnueabihf
[G.X]   arm-cortexa9_neon-linux-gnueabihf
[G.X]   x86_64-w64-mingw32,arm-cortexa9_neon-linux-gnueabihf
[G..]   arm-multilib-linux-uclibcgnueabi
[G..]   arm-nano-eabi
[G..]   arm-unknown-eabi
[G..]   arm-unknown-linux-gnueabi
[G.X]   arm-unknown-linux-musleabi
[G..]   arm-unknown-linux-uclibcgnueabi
[G.X]   arm-unknown-linux-uclibcgnueabihf
[G..]   armeb-unknown-eabi
[G..]   armeb-unknown-linux-gnueabi
[G..]   armeb-unknown-linux-uclibcgnueabi
[G..]   armv6-nommu-linux-uclibcgnueabi
[G..]   armv6-rpi-linux-gnueabi
[G..]   armv7-rpi2-linux-gnueabihf
[G..]   armv8-rpi3-linux-gnueabihf
[G..]   avr
[G..]   i586-geode-linux-uclibc
[G..]   i686-centos6-linux-gnu
[G..]   i686-centos7-linux-gnu
[G..]   i686-nptl-linux-gnu
[G..]   i686-ubuntu12.04-linux-gnu
[G..]   i686-ubuntu14.04-linux-gnu
[G..]   i686-ubuntu16.04-linux-gnu
[G.X]   i686-w64-mingw32
[G..]   m68k-unknown-elf
[G..]   m68k-unknown-uclinux-uclibc
[G..]   powerpc-unknown-linux-uclibc,m68k-unknown-uclinux-uclibc
[G..]   mips-ar2315-linux-gnu
[G..]   mips-malta-linux-gnu
[G..]   mips-unknown-elf
[G..]   mips-unknown-linux-uclibc
[G..]   mips64el-multilib-linux-uclibc
[G..]   mipsel-multilib-linux-gnu
[G..]   mipsel-sde-elf
[G..]   mipsel-unknown-linux-gnu
[G.X]   i686-w64-mingw32,nios2-spico-elf
[G..]   powerpc-405-linux-gnu
[G..]   powerpc-860-linux-gnu
[G..]   powerpc-e300c3-linux-gnu
[G..]   powerpc-e500v2-linux-gnuspe
[G..]   x86_64-multilib-linux-uclibc,powerpc-unknown-elf
[G..]   powerpc-unknown-linux-gnu
[G..]   powerpc-unknown-linux-uclibc
[G..]   powerpc-unknown_nofpu-linux-gnu
[G..]   powerpc64-multilib-linux-gnu
[G..]   powerpc64-unknown-linux-gnu
[G..]   powerpc64le-unknown-linux-gnu
[G.X]   s390-ibm-linux-gnu
[G..]   s390x-ibm-linux-gnu
[G..]   sh4-multilib-linux-gnu
[G..]   sh4-multilib-linux-uclibc
[G..]   sh4-unknown-linux-gnu
[G..]   sparc-leon-linux-uclibc
[G..]   sparc-unknown-linux-gnu
[G..]   sparc64-multilib-linux-gnu
[G..]   x86_64-centos6-linux-gnu
[G..]   x86_64-centos7-linux-gnu
[G..]   x86_64-multilib-linux-gnu
[G.X]   x86_64-multilib-linux-musl
[G..]   x86_64-multilib-linux-uclibc
[G.X]   x86_64-w64-mingw32,x86_64-pc-linux-gnu
[G..]   x86_64-ubuntu12.04-linux-gnu
[G..]   x86_64-ubuntu14.04-linux-gnu
[G..]   x86_64-ubuntu16.04-linux-gnu
[G..]   x86_64-unknown-linux-gnu
[G..]   x86_64-unknown-linux-uclibc
[G.X]   x86_64-w64-mingw32
[G..]   xtensa-fsf-linux-uclibc
