// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_PDELAB_ORDERING_TRANSFORMATIONS_HH
#define DUNE_PDELAB_ORDERING_TRANSFORMATIONS_HH

#include <cstddef>

#include <dune/pdelab/common/typetree/traversal.hh>
#include <dune/pdelab/common/typetree/accumulate_static.hh>
#include <dune/pdelab/common/multiindex.hh>
#include <dune/pdelab/gridfunctionspace/tags.hh>
#include <dune/pdelab/gridfunctionspace/tags.hh>

/**
 * \file
 * \brief Define and register ordering related transformations.
 * This header defines the two transformations gfs_to_ordering and gfs_to_local_ordering.
 * As both the PowerGFS and the CompositeGFS transformations use a single descriptor that
 * is specialized for the individual orderings, the prototype of those descriptors are also
 * declared in this file and are registered with the TypeTree transformation system.
 */

namespace Dune {
  namespace PDELab {

#ifndef DOXYGEN

    struct extract_max_container_depth
    {

      typedef std::size_t result_type;

      template<typename Node, typename TreePath>
      struct doVisit
      {
        static const bool value = true;
      };

      template<typename Node, typename TreePath>
      struct visit
      {
        static const std::size_t result = Node::Traits::Backend::Traits::max_blocking_depth;
      };

    };


    //! GridFunctionSpace to Ordering transformation descriptor
    template<typename RootGFS>
    struct gfs_to_ordering
    {
      static const std::size_t ci_depth =
        TypeTree::AccumulateValue<RootGFS,
                                  extract_max_container_depth,
                                  TypeTree::max<std::size_t>,
                                  0,
                                  TypeTree::plus<std::size_t>
                                  >::result + 1;

      typedef typename gfs_to_lfs<RootGFS>::DOFIndex DOFIndex;
      typedef MultiIndex<std::size_t,ci_depth> ContainerIndex;
    };

    //! GridFunctionSpace to LocalOrdering transformation descriptor
    template<typename GlobalTransformation>
    struct gfs_to_local_ordering
    {
      typedef typename GlobalTransformation::DOFIndex DOFIndex;
      typedef typename GlobalTransformation::ContainerIndex ContainerIndex;
    };



    // Declare PowerGFS to ordering descriptor and register transformation

    template<typename GFS, typename Transformation, typename OrderingTag>
    struct power_gfs_to_ordering_descriptor;

    template<typename GridFunctionSpace, typename Params>
    power_gfs_to_ordering_descriptor<
      GridFunctionSpace,
      gfs_to_ordering<Params>,
      typename GridFunctionSpace::OrderingTag
      >
    lookupNodeTransformation(GridFunctionSpace* gfs, gfs_to_ordering<Params>* t, PowerGridFunctionSpaceTag tag);


    // Declare LeafGFS to ordering descriptor and register transformation

    template<typename GFS, typename Transformation, typename OrderingTag>
    struct leaf_gfs_to_ordering_descriptor;

    template<typename GridFunctionSpace, typename Params>
    leaf_gfs_to_ordering_descriptor<
      GridFunctionSpace,
      gfs_to_ordering<Params>,
      typename GridFunctionSpace::Traits::OrderingTag
      >
    lookupNodeTransformation(GridFunctionSpace* gfs, gfs_to_ordering<Params>* t, LeafGridFunctionSpaceTag tag);


    // Declare CompositeGFS to ordering descriptor and register transformation

    template<typename GFS, typename Transformation, typename OrderingTag>
    struct composite_gfs_to_ordering_descriptor;

    template<typename GridFunctionSpace, typename Params>
    composite_gfs_to_ordering_descriptor<
      GridFunctionSpace,
      gfs_to_ordering<Params>,
      typename GridFunctionSpace::OrderingTag
      >
    lookupNodeTransformation(GridFunctionSpace* gfs, gfs_to_ordering<Params>* t, CompositeGridFunctionSpaceTag tag);


    // Declare PowerGFS to local ordering descriptor and register transformation

   template<typename GFS, typename Transformation, typename OrderingTag>
   struct power_gfs_to_local_ordering_descriptor;

    template<typename GFS, typename Params>
    power_gfs_to_local_ordering_descriptor<
      GFS,
      gfs_to_local_ordering<Params>,
      typename GFS::OrderingTag
      >
    lookupNodeTransformation(GFS* gfs, gfs_to_local_ordering<Params>* t, PowerGridFunctionSpaceTag tag);


    // Declare CompositeGFS to ordering descriptor and register transformation

    template<typename GFS, typename Transformation, typename OrderingTag>
    struct composite_gfs_to_local_ordering_descriptor;

    template<typename GFS, typename Params>
    composite_gfs_to_local_ordering_descriptor<
      GFS,
      gfs_to_local_ordering<Params>,
      typename GFS::OrderingTag
      >
    lookupNodeTransformation(GFS* gfs, gfs_to_local_ordering<Params>* t, CompositeGridFunctionSpaceTag tag);


#endif // DOXYGEN

  } // namespace PDELab
} // namespace Dune

#endif // DUNE_PDELAB_ORDERING_TRANSFORMATIONS_HH