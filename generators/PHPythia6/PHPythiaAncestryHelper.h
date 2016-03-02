#ifndef __PHPYTHIAANCESTRYHELPER_H__
#define __PHPYTHIAANCESTRYHELPER_H__

/**
 * @class  PHPythiaAncestryHelper
 * @author Mike McCumber <mccumber@bnl.gov>
 * @date   May 2014
 *
 * @brief  This class contains a helper method for tracing the PHPythiaContainer
 *         and assigning a partonic origin for a stable entry in the Pythia event record
 *         It has been tested primarily on hard scattering events.
 *
 */

// traversing through MPI within a string is controlled by the traverse_mpi command
// it is not clear at the moment if these gluons are real MPI or some additional modelling
// from PYTHIA for hadronic collisions

/* get_particle_origin ends on [highlighted] items:

   (1) outgoing LO partons
   (2) initial state radiation off of incoming parton
   (3) beam remnant
 
       [BR]     [ISR]
        /        /
       /        /
    p+ ---------           [parton #7]
                \             /
                 \__LO pQCD__/
                 /           \
                /             \
    p+ ---------           [parton #8]
       \        \
        \        \
       [BR]     [ISR]
*/

#include <PHPythiaContainer.h>
#include <vector>

class PHPythiaAncestryHelper {

public:
  PHPythiaAncestryHelper(PHPythiaContainer* particles);
  virtual ~PHPythiaAncestryHelper() {}
  
  void verbosity(int i) {_verbosity = i;}
  
  int get_particle_origin(int particle_id);

  void traverse_mpi(bool b) {_traverse_mpi = b;}

private:
  
  bool has_single_parent(int particle_id);
  std::vector<int> get_multiple_parents(int particle_id);
  int select_best_candidate(int particle_id, int parent_id, std::vector<int> candidates); 
  bool passes_ancestry_cut(TMCParticle* particle, TMCParticle* string, TMCParticle* candidate);
  bool passes_kinematic_cut(TMCParticle* particle, TMCParticle* string, TMCParticle* candidate);
  double get_distance(TMCParticle* particleA, TMCParticle* particleB);

  double get_mom(TMCParticle* particle);
  double get_pt(TMCParticle* particle);
  double get_eta(TMCParticle* particle);
  double get_phi(TMCParticle* particle);

  int _verbosity;
  bool _traverse_mpi;

  PHPythiaContainer* _particles;
};

#endif /* __PHPYTHIAANCESTRYHELPER_H__ */
