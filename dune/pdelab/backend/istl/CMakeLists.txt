install(FILES
  bcrsmatrixbackend.hh
  bcrspattern.hh
  blockmatrixdiagonal.hh
  cg_to_dg_prolongation.hh
  descriptors.hh
  forwarddeclarations.hh
  matrixhelpers.hh
  ovlp_amg_dg_backend.hh
  parallelhelper.hh
  patternstatistics.hh
  seq_amg_dg_backend.hh
  tags.hh
  utility.hh
  vectorhelpers.hh
  vectoriterator.hh
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/pdelab/backend/istl)
