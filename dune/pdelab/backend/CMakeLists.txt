set(commondir  ${CMAKE_INSTALL_INCLUDEDIR}/dune/pdelab/backend)
set(common_HEADERS  backendselector.hh
                 eigen.hh
                 eigenmatrixbackend.hh
                 eigensolverbackend.hh
                 eigenvectorbackend.hh
                 istlmatrixbackend.hh
                 istlsolverbackend.hh
                 istlvectorbackend.hh
                 novlpistlsolverbackend.hh
                 ovlpistlsolverbackend.hh
                 petscmatrixbackend.hh
                 petscnestedvectorbackend.hh
                 petscutility.hh
                 petscvectorbackend.hh
                 seqistlsolverbackend.hh
                 simple.hh
                 solver.hh
                 tags.hh
                 )

add_subdirectory(common)
add_subdirectory(eigen)
add_subdirectory(istl)
add_subdirectory(simple)

if(NOT HAVE_EIGEN)
  exclude_from_headercheck(
    eigen.hh
    eigenmatrixbackend.hh
    eigensolverbackend.hh
    eigenvectorbackend.hh)
endif()

install(FILES ${common_HEADERS} DESTINATION ${commondir})
