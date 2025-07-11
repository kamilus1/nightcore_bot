utils_sources = [
    './src/machine_learning.c'
]

utils_incdir = include_directories('./include')

utils_lib = library('lmachinelearning', utils_sources, 
                     include_directories : [utils_incdir], 
                            dependencies : [gst_dep], 
                            install : true)

utils_dep = declare_dependency(
                include_directories : utils_incdir,
                          link_with : utils_lib           
                                    )