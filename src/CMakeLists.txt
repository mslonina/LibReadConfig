include_directories(.)
add_library (readconfig SHARED libreadconfig.c)

install (TARGETS readconfig DESTINATION lib)
install (FILES libreadconfig.h DESTINATION include)

if (BUILD_HDF5)
  target_link_libraries (readconfig hdf5 m)
  install (FILES libreadconfig_hdf5.h DESTINATION include)
endif (BUILD_HDF5)

