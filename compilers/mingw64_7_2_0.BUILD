package(default_visibility = ['//visibility:public'])

filegroup(
  name = 'gcc',
  srcs = [
    'bin/gcc',
  ],
)

filegroup(
  name = 'ar',
  srcs = [
    'bin/ar',
  ],
)

filegroup(
  name = 'ld',
  srcs = [
    'bin/ld',
  ],
)

filegroup(
  name = 'nm',
  srcs = [
    'bin/nm',
  ],
)

filegroup(
  name = 'objcopy',
  srcs = [
    'bin/objcopy',
  ],
)

filegroup(
  name = 'objdump',
  srcs = [
    'bin/objdump',
  ],
)

filegroup(
  name = 'strip',
  srcs = [
    'bin/strip',
  ],
)

filegroup(
  name = 'as',
  srcs = [
    'bin/as',
  ],
)

filegroup(
  name = 'compiler_pieces',
  srcs = glob([
    'x86_64-w64-mingw32/**',
    'libexec/**',
    'lib/gcc/x86_64-w64-mingw32/**',
    'include/**',
  ]),
)

filegroup(
  name = 'compiler_components',
  srcs = [
    ':gcc',
    ':ar',
    ':ld',
    ':nm',
    ':objcopy',
    ':objdump',
    ':strip',
    ':as',
  ],
)
