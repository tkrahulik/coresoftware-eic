// $Id: PHPyOscarOutputManager.C,v 1.1 2011/08/05 17:47:36 pinkenbu Exp $

/*!
   \file PHPyOscarOutputManager.h
   \brief dedicated output manager to write phpythia contents into oscar formated text file
   \author Hugo Pereira
   \version $Revision: 1.1 $
   \date $Date: 2011/08/05 17:47:36 $
*/


#include <PHPyOscarOutputManager.h>
#include <PHPythiaContainer.h>
#include <PHPythiaHeader.h>

#include <phool/getClass.h>
#include <TMCParticle.h>

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

//___________________________________________________________________________________
PHPyOscarOutputManager::PHPyOscarOutputManager(const string &myname, const string &fname):
  Fun4AllOutputManager( myname )
{
  outfileopen( fname );
  return ;
}

//___________________________________________________________________________________
int PHPyOscarOutputManager::outfileopen(const string &fname)
{

  outfilename = fname;
  out.open( outfilename.c_str() );
  if( !out )
  {

    cout << PHWHERE << " Could not open " << fname << " exiting now" << endl;
    exit(1);

  } else {

    out << "# OSC1999A" << endl;
    out << "# final_id_p_x" << endl;
    out << "# PHPythia" << endl;
    out << "# " << endl;

  }
  return 0;

}

//___________________________________________________________________________________
PHPyOscarOutputManager::~PHPyOscarOutputManager()
{

  if( out.is_open() ) out.close();
  return;

}

//___________________________________________________________________________________
int PHPyOscarOutputManager::Write(PHCompositeNode *startNode)
{

  // Get PYTHIA Header
  PHPythiaHeader* pythia_header = findNode::getClass<PHPythiaHeader>(startNode, "PHPythiaHeader" );
  if (!pythia_header)
  {
    cout << "PHPyOscarOutputManager::Write - unable to get PHPythiaHeader, is Node missing?" << endl;
    return -1;
  }

  // Get PYTHIA Particles
  PHPythiaContainer* phpythia = findNode::getClass<PHPythiaContainer>(startNode, "PHPythia");
  if (!phpythia)
  {
    cout << "PHPyOscarOutputManager::Write - unable to get PHPythia, is Node missing?" << endl;
    return -1;
  }

  if( !out )
  {
    cout << "PHPyOscarOutputManager::Write - output stream is invalid." << endl;
    return -1;
  }

  return _Write( phpythia,
    pythia_header->GetPrimaryVertexX(),
    pythia_header->GetPrimaryVertexY(),
    pythia_header->GetPrimaryVertexZ() );

}

//_______________________________________________
int PHPyOscarOutputManager::_Write( PHPythiaContainer* phpythia, double x_primary, double y_primary, double z_primary )
{

  // convert millimeters into Fermi (needed for oscar output units
  static const double milimeters_to_fermi = 1e12;

  // total number of particles
  int numParticles( phpythia->size() );

  // number of stable particles, that gets written to output
  int nstable(0);
  for( int ipart=0; ipart<numParticles; ipart++)
  { if ( phpythia->isStable(ipart) ) nstable++; }

  // block header
  out << "0 " << nstable + 1 << endl;

  // particle index
  int nout(0);

  // add dummy particle (geantino) that contains primary vertex information
  {

    ++nout;
    out << setw(8) << left << nout << " "
      << setw(8) << right << "0" << " "
      << setw(4) << "0" << " "
      << setw(12) << "0" << " "
      << setw(12) << "0" << " "
      << setw(12) << "0" << " "
      << setw(12) << "0" << " "
      << setw(12) << "0" << " "
      << setw(14) << x_primary*milimeters_to_fermi << " "
      << setw(14) << y_primary*milimeters_to_fermi << " "
      << setw(14) << z_primary*milimeters_to_fermi << " "
      << setw(14) << "0"
      << endl;

  }


  for( int ipart=0; ipart<numParticles; ipart++)
  {

    if ( phpythia->isStable(ipart) )
    {

      // increment particle index
      ++nout;

      TMCParticle *particle = phpythia->getParticle(ipart);	// get the particle information
      out << setw(8) << left << nout << " "
        << setw(8) << right << particle->GetKF() << " "
        << setw(4) << "0" << " "
        << setw(12) << particle->GetPx() << " "
        << setw(12) << particle->GetPy() << " "
        << setw(12) << particle->GetPz() << " "
        << setw(12) << particle->GetEnergy() << " "
        << setw(12) << particle->GetMass() << " "
        << setw(14) << particle->GetVx()*milimeters_to_fermi << " "
        << setw(14) << particle->GetVy()*milimeters_to_fermi << " "
        << setw(14) << particle->GetVz()*milimeters_to_fermi << " "
        << setw(14) << particle->GetTime() << " "
        << endl;
    }
  }

  // end of block
  out << "0 0" << endl;
  return 0;

}
