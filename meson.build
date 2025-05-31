project('nightcore-cmd', 'c', 
        version: '0.0.1')


gst_dep = dependency('gstreamer-1.0', fallback: ['gstreamer', 'gst_dep'])
glib_dep = dependency('glib-2.0', fallback: ['glib', 'libglib_dep'])


subdir('libs')

exec_src = ['./source/main.c']
inc_dir = include_directories('./include')


executable('nightcorek', exec_src, 
                include_directories: [inc_dir],
                dependencies: [gst_dep, glib_dep, nightcore_dep])