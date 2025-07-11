utils_sources = [
    './src/utils.c'
]

utils_incdir = include_directories('./include')

utils_lib = library('lutils', utils_sources, 
                     include_directories : [utils_incdir], 
                            dependencies : [gst_dep], 
                            install : true)

utils_dep = declare_dependency(
                include_directories : utils_incdir,
                          link_with : utils_lib           
                                    )