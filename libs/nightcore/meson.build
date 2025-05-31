nightcore_sources = [
    './src/nightcore.c'
]

nightcore_incdir = include_directories('./include')

nightcore_lib = library('lnightcore', nightcore_sources, 
                     include_directories : [nightcore_incdir], 
                            dependencies : [gst_dep], 
                            install : true)

nightcore_dep = declare_dependency(
                include_directories : nightcore_incdir,
                          link_with : nightcore_lib           
                                    )