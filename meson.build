project(
  'niji',
  'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++26,c++latest',
    'warning_level=3',
    'werror=true',
    'b_vscrt=md',
  ]
)

cpp = meson.get_compiler('cpp')

add_project_arguments('-DUNICODE', '-D_UNICODE', '-DWIN32_LEAN_AND_MEAN', language: 'cpp')

deps = [cpp.find_library('dwmapi', required: true)]
link_args = []

if cpp.get_argument_syntax() == 'gcc'
  link_args += [
    '-static-libgcc',
    '-static-libstdc++',
    '-Wl,-Bstatic',
    '-lwinpthread',
    '-Wl,-Bdynamic'
  ]
endif

executable(
  'niji',
  'main.cpp',
  dependencies: deps,
  link_args: link_args
)
