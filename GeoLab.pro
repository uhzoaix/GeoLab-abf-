#config += debug_and_release
TEMPLATE      = subdirs
CONFIG += ordered
SUBDIRS   = \
        Core \
        PluginCore \
        GUI \
        Main \
        Utilities/OpenMeshAlgorithms \
  #      Plugins/io_base \
        Plugins/filter_smooth_laplacian \
  #      Plugins/filter_remesh_optimization \

