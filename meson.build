project('oppai', 'c', default_options: 'default_library=static')

sdl2_dep = dependency('sdl2')

sdl2_ttf_dep = dependency('sdl2_ttf')

executable('oppai', 'oppai.c',
           win_subsystem: 'windows',
           dependencies: [sdl2_dep, sdl2_ttf_dep])