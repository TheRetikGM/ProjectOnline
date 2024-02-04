project('SDLDemo', 'cpp',
  version : '0.1',
  default_options : ['cpp_std=c++17',
                     'default_library=static']
)

add_global_arguments('-DImTextureID=ImU64', language : 'cpp')

# Platform specific compile arguments.
compile_link_args = []
compile_cpp_args = []
if build_machine.system() == 'linux'
  compile_link_args = ['-Wall', '-Wextra']
  compile_cpp_args = ['-Wno-shadow', '-Wno-unused-parameter', '-Wno-unknown-pragmas']
  add_project_arguments('-D_BUILD_LINUX', language : ['c', 'cpp'])
elif build_machine.system() == 'windows'
  compile_link_args = '/W4 /WX'
  add_project_arguments(['-D_BUILD_WINDOWS'], language : ['c', 'cpp'])
endif

# Debug
if get_option('debug') == true
  add_project_arguments('-DREN_DEBUG', language : ['c', 'cpp'])
endif

# Configure file
conf_data = configuration_data()
conf_data.set('SOURCE_DIR', meson.project_source_root())
configure_file(input : 'config.h.in',
               output : 'config.h',
               configuration : conf_data)
conf_inc = include_directories('.')

# Add Ren library and sandbox.
subdir('src')

