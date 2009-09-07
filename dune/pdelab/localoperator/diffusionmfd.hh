#ifndef DUNE_PDELAB_DIFFUSIONMFD_HH
#define DUNE_PDELAB_DIFFUSIONMFD_HH

#include "../gridoperatorspace/gridoperatorspaceutilities.hh"
#include "../gridoperatorspace/localmatrix.hh"
#include "pattern.hh"
#include "flags.hh"
#include "mfdcommon.hh"

namespace Dune
{
    namespace PDELab
    {

        //! \addtogroup LocalOperator
        //! \ingroup PDELab
        //! \{

        /** a local operator for solving the diffusion equation
         *
         * \f{align*}{
         * - \nabla\cdot\{K(x) \nabla u\} + a_0 u &=& f \mbox{ in } \Omega,          \\
         *                                      u &=& g \mbox{ on } \partial\Omega_D \\
         *              -(K(x)\nabla u) \cdot \nu &=& j \mbox{ on } \partial\Omega_N \\
         * \f}
         * with a mimetic finite difference method on all types of grids in any dimension
         * \tparam Data contains methods K, a_0, f, g and j giving the equation data
         * and bcType to define the boundary condition type
         * \tparam WBuilder builds the local scalar product matrix
         */
        template<class Data, class WBuilder = MimeticBrezziW<typename Data::ctype,Data::dimension> >
        class DiffusionMFD
            : public FullVolumePattern
            , public LocalOperatorDefaultFlags
        {
            static const unsigned int dim = Data::dimension;
            typedef typename Data::ctype ctype;
            typedef typename Data::rtype rtype;

        public:
            // pattern assembly flags
            enum { doPatternVolume = true };
            enum { doPatternSkeleton = false };

            // residual assembly flags
            enum { doAlphaVolume = true };
            enum { doSkeletonTwoSided = true };
            enum { doAlphaSkeleton = true };
            enum { doAlphaBoundary = true };
            enum { doAlphaVolumePostSkeleton = true };

            enum { doLambdaVolume = true };
            enum { doLambdaBoundary = true };

            DiffusionMFD(const Data& data_, const WBuilder& wbuilder_ = WBuilder())
                : data(data_), wbuilder(wbuilder_)
            {}

            // alpha ***********************************************

            template<typename EG, typename LFSU, typename X, typename LFSV, typename R>
            void alpha_volume(const EG& eg, const LFSU& lfsu, const X& x, const LFSV& lfsv, R& r) const
            {
                cell.init(eg.entity());
            }

            template<typename IG, typename LFSU, typename X, typename LFSV, typename R>
            void alpha_skeleton(const IG& ig,
                                const LFSU& lfsu_s, const X& x_s, const LFSV& lfsv_s,
                                const LFSU& lfsu_n, const X& x_n, const LFSV& lfsv_n,
                                R& r_s, R& r_n) const
            {
                cell.add_face(ig.intersection());
            }

            template<typename IG, typename LFSU, typename X, typename LFSV, typename R>
            void alpha_boundary(const IG& ig, const LFSU& lfsu_s, const X& x_s,
                                const LFSV& lfsv_s, R& r_s) const
            {
                cell.add_face(ig.intersection());
            }

            template<typename EG, typename LFSU, typename X, typename LFSV, typename R>
            void alpha_volume_post_skeleton(const EG& eg, const LFSU& lfsu, const X& x,
                                            const LFSV& lfsv, R& r) const
            {
                // extract subspaces
                typedef typename LFSU::template Child<0>::Type CellUnknowns;
                const CellUnknowns& cell_space = lfsu.template getChild<0>();
                typedef typename LFSU::template Child<1>::Type FaceUnknowns;
                const FaceUnknowns& face_space = lfsu.template getChild<1>();

                // get permeability for current cell
                GeometryType gt = eg.geometry().type();
                FieldVector<ctype,dim> localcenter = GenericReferenceElements<ctype,dim>::general(gt).position(0,0);
                FieldMatrix<rtype,dim,dim> K = data.K(eg.entity(), localcenter);

                // build matrix W
                wbuilder.build_W(cell, K, W);

                // Compute residual
                for(int e = 0, m = 0; e < cell.num_faces; ++e)
                    for(int f = 0; f < cell.num_faces; ++f, ++m)
                    {
                        r[cell_space.localIndex(0)] += W[m]
                            * (x[cell_space.localIndex(0)] - x[face_space.localIndex(f)]);
                        r[face_space.localIndex(e)] -= W[m]
                            * (x[cell_space.localIndex(0)] - x[face_space.localIndex(f)]);
                    }

                // Helmholtz term
                r[cell_space.localIndex(0)] += data.a_0(eg.entity(), localcenter)
                    * x[cell_space.localIndex(0)];
            }

            // jacobian ********************************************

            template<typename EG, typename LFSU, typename X, typename LFSV, typename R>
            void jacobian_volume(const EG& eg, const LFSU& lfsu, const X& x,
                                 const LFSV& lfsv, LocalMatrix<R>& mat) const
            {
                cell.init(eg.entity());
            }

            template<typename IG, typename LFSU, typename X, typename LFSV, typename R>
            void jacobian_skeleton(const IG& ig,
                                   const LFSU& lfsu_s, const X& x_s, const LFSV& lfsv_s,
                                   const LFSU& lfsu_n, const X& x_n, const LFSV& lfsv_n,
                                   LocalMatrix<R>& mat_ss, LocalMatrix<R>& mat_sn,
                                   LocalMatrix<R>& mat_ns, LocalMatrix<R>& mat_nn) const
            {
                cell.add_face(ig.intersection());
            }

            template<typename IG, typename LFSU, typename X, typename LFSV, typename R>
            void jacobian_boundary(const IG& ig, const LFSU& lfsu_s, const X& x_s, const LFSV& lfsv_s,
                                   LocalMatrix<R>& mat_ss) const
            {
                cell.add_face(ig.intersection());
            }

            template<typename EG, typename LFSU, typename X, typename LFSV, typename R>
            void jacobian_volume_post_skeleton(const EG& eg, const LFSU& lfsu, const X& x,
                                               const LFSV& lfsv, LocalMatrix<R>& mat) const
            {
                // extract subspaces
                typedef typename LFSU::template Child<0>::Type CellUnknowns;
                const CellUnknowns& cell_space = lfsu.template getChild<0>();
                typedef typename LFSU::template Child<1>::Type FaceUnknowns;
                const FaceUnknowns& face_space = lfsu.template getChild<1>();

                // get permeability for current cell
                GeometryType gt = eg.geometry().type();
                FieldVector<ctype,dim> localcenter = GenericReferenceElements<ctype,dim>::general(gt).position(0,0);
                FieldMatrix<rtype,dim,dim> K = data.K(eg.entity(), localcenter);

                // build matrix W
                wbuilder.build_W(cell, K, W);

                // Compute residual
                for(int e = 0, m = 0; e < cell.num_faces; ++e)
                    for(int f = 0; f < cell.num_faces; ++f, ++m)
                    {
                        mat(cell_space.localIndex(0), cell_space.localIndex(0)) += W[m];
                        mat(cell_space.localIndex(0), face_space.localIndex(f)) -= W[m];
                        mat(face_space.localIndex(f), cell_space.localIndex(0)) -= W[m];
                        mat(face_space.localIndex(f), face_space.localIndex(f)) += W[m];
                    }

                // Helmholtz term
                mat(cell_space.localIndex(0), cell_space.localIndex(0))
                    += data.a_0(eg.entity(), localcenter);
            }

            // lambda **********************************************

            template<typename EG, typename LFSV, typename R>
            void lambda_volume(const EG& eg, const LFSV& lfsv, R& r) const
            {
                // extract subspaces
                typedef typename LFSV::template Child<0>::Type CellUnknowns;
                const CellUnknowns& cell_space = lfsv.template getChild<0>();

                GeometryType gt = eg.geometry().type();
                FieldVector<ctype,dim> localcenter = GenericReferenceElements<ctype,dim>::general(gt).position(0,0);
                r[cell_space.localIndex(0)] -= cell.volume * data.f(eg.entity(), localcenter);
            }

            template<typename IG, typename LFSV, typename R>
            void lambda_boundary(const IG& ig, const LFSV& lfsv, R& r) const
            {
                // local index of current face
                unsigned int e = cell.num_faces - 1;

                GeometryType gt = ig.intersection().type();
                FieldVector<ctype,dim-1> center = GenericReferenceElements<ctype,dim-1>::general(gt).position(0,0);
                FieldVector<ctype,dim> local_face_center = ig.geometryInInside().global(center);

                if (data.bcType(ig, center) == Data::bcNeumann)
                {
                    typedef typename LFSV::template Child<1>::Type FaceUnknowns;
                    const FaceUnknowns& face_space = lfsv.template getChild<1>();

                    r[face_space.localIndex(e)] += cell.face_areas[e]
                        * data.j(*(ig.inside()), local_face_center);
                }
            }

        private:
            const Data& data;
            const WBuilder wbuilder;
            mutable MimeticCellProperties<ctype,dim> cell;
            mutable std::vector<rtype> W;
        };

        //! \} group LocalOperator
    }
}

#endif
