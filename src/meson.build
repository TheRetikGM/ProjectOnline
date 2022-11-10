# Find dependencies
cmake = import('cmake')
ren_depends = [
  subproject('freetype2').get_variable('freetype_dep'),
  subproject('glm').get_variable('glm_dep'),
  subproject('sdl2').get_variable('sdl2_dep'),
  subproject('sdl2_image').get_variable('sdl2_image_dep'),
  subproject('sdl2_ttf').get_variable('sdl2_ttf_dep'),
  subproject('sdl2_mixer').get_variable('sdl2_mixer_dep'),
  subproject('imgui').get_variable('imgui_dep'),
  subproject('box2d').get_variable('box2d_dep'),
  subproject('entt').get_variable('entt_dep'),
  cmake.subproject('yaml-cpp').dependency('yaml-cpp')
]

# Set include directories
ren_inc = include_directories('include')

############## Ren library ##############
ren_src = []
subdir('Ren')
ren_lib = library('ren', ren_src, 
  link_args : compile_link_args,
  cpp_args : compile_cpp_args,
  include_directories : [ren_inc, conf_inc],
  dependencies : [ren_depends])

ren_dep = declare_dependency(
  include_directories : [ren_inc, conf_inc],
  link_with : ren_lib,
  dependencies : ren_depends)


############# Sandbox ##############
sandbox_src = []
subdir('sandbox')
executable('SDLDemo', sandbox_src, 
  link_args : compile_link_args,
  cpp_args : compile_cpp_args,
  include_directories : ['sandbox/include'],
  dependencies : [ren_dep])
