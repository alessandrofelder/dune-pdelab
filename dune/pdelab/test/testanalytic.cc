#ifdef HAVE_CONFIG_H
#include "config.h"     
#endif
#include <iostream>
#include <dune/common/mpihelper.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/fvector.hh>
#include<dune/grid/yaspgrid.hh>
#include"../common/function.hh"
#include"../common/vtkexport.hh"

// beautiful version for analytic functions
template<typename GV, typename RF>
class Velocity 
  : public Dune::PDELab::AnalyticGridFunctionBase<Dune::PDELab::AnalyticGridFunctionTraits<GV,RF,3>,
													  Velocity<GV,RF> >
{
public:
  typedef Dune::PDELab::AnalyticGridFunctionTraits<GV,RF,3> Traits;
  typedef Dune::PDELab::AnalyticGridFunctionBase<Traits,Velocity<GV,RF> > BaseT;

  Velocity (const GV& gv) : BaseT(gv) {}
  inline void evaluateGlobal (const typename Traits::DomainType& x, 
							  typename Traits::RangeType& y) const
  {  
	y[0] = x[1]*(1-x[1])*x[2]*(1-x[2])*16.0;
	y[1] = 0;
	y[2] = 0;
  }
};

template<typename GV, typename RF>
class Pressure 
  : public Dune::PDELab::AnalyticGridFunctionBase<Dune::PDELab::AnalyticGridFunctionTraits<GV,RF,1>,
													  Pressure<GV,RF> >
{
public:
  typedef Dune::PDELab::AnalyticGridFunctionTraits<GV,RF,1> Traits;
  typedef Dune::PDELab::AnalyticGridFunctionBase<Traits,Pressure<GV,RF> > BaseT;

  Pressure (const GV& gv) : BaseT(gv) {}
  inline void evaluateGlobal (const typename Traits::DomainType& x, 
							  typename Traits::RangeType& y) const
  {  
	y[0] = 4.0-x[0];
  }
};

// test function trees
template<class GV> 
void testuserfriendly (const GV& gv)
{
  typedef Pressure<GV,double> P;
  P p(gv);
  typedef Velocity<GV,double> V;
  V v(gv);
  typedef Dune::PDELab::CompositeGridFunction<V,P> TH;
  TH th(v,p);
  Dune::VTKWriter<GV> vtkwriter(gv,Dune::VTKOptions::conforming);
  Dune::PDELab::vtkwriter_tree_addvertexdata(vtkwriter,th);
  vtkwriter.write("channel",Dune::VTKOptions::ascii);
}

int main(int argc, char** argv)
{
  try{
    //Maybe initialize Mpi
    Dune::MPIHelper& helper = Dune::MPIHelper::instance(argc, argv);

	// need a 3D grid for user friendly version
	Dune::FieldVector<double,3> L3(1.0); L3[0] = 4;
	Dune::FieldVector<int,3> N3(1); N3[0] = 4;
	Dune::FieldVector<bool,3> B3(false);
	Dune::YaspGrid<3> grid(L3,N3,B3,0);
    grid.globalRefine(4);
	testuserfriendly(grid.leafView());

	// test passed
	return 0;

  }
  catch (Dune::Exception &e){
    std::cerr << "Dune reported error: " << e << std::endl;
	return 1;
  }
  catch (...){
    std::cerr << "Unknown exception thrown!" << std::endl;
	return 1;
  }
} 
