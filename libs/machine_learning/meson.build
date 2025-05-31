machine_learning_sources = [
    './src/machine_learning.c'
]

machine_learning_incdir = include_directories('./include')

machine_learning_lib = library('lmachinelearning', machine_learning_sources, 
                     include_directories : [machine_learning_incdir], 
                            dependencies : [gst_dep], 
                            install : true)

machine_learning_dep = declare_dependency(
                include_directories : machine_learning_incdir,
                          link_with : machine_learning_lib           
                                    )