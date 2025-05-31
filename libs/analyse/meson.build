analyse_sources = [
    './src/analyse.c'
]

analyse_incdir = include_directories('./include')

analyse_lib = library('lanalyse', analyse_sources, 
                     include_directories : [analyse_incdir], 
                            dependencies : [gst_dep], 
                            install : true)

analyse_dep = declare_dependency(
                include_directories : analyse_incdir,
                          link_with : analyse_lib           
                                    )