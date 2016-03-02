
#include <PHPythiaAncestryHelper.h>

// PHPythia
#include <PHPythiaContainer.h>

// ROOT
#include <TMCParticle.h>

// std
#include <iostream>
#include <math.h>
#include <float.h>
#include <vector>

using namespace std;

PHPythiaAncestryHelper::PHPythiaAncestryHelper(PHPythiaContainer* particles) {
  _verbosity = 0;
  _particles  = particles;
  _traverse_mpi = true;
}

/// reports the origin parton by tracing through the string/clusters in Pythia
int PHPythiaAncestryHelper::get_particle_origin(int particle_id) {
	
  // traces beam remnant fragmentation to the beam remnant
  // traces ISR to the radiated particle
  // traces FSR to the outgoing parton (pass through the radiated particle)
  // traces fragmentation to the outgoing LO parton 
  // traces past MPI gluons to nearest ancestried parton (_traverse_mpi flag)

  // a requirement is made when tracing through a string that the parton is
  // closest in R=(eta,phi)
  // preference is given to partons with sufficient momentum to make the daughter particle
  // and if the parton has ancestry (controlled by the _traverse_mpi flag)

  if (_verbosity) {
    cout << endl;
    cout << "                               PHPythiaAncestryHelper Trace" << endl;
    cout << " ================================================================================================" << endl;
    cout << "    I particle/jet KS     KF  orig    p_x      p_y      p_z       E        m       eta      phi  " << endl;
    cout << " ================================================================================================" << endl;
  }

  // particle
  int pid1 = particle_id;
  TMCParticle *particle1 = (TMCParticle*)_particles->getParticle(pid1-1);

  // parent particle
  int pid2 = particle1->GetParent();
  TMCParticle *particle2 = (TMCParticle*)_particles->getParticle(pid2-1);

  // origin particle
  int pid = -1;
  TMCParticle *particle = NULL;
  
  // while the parent is not part of the diagram
  // or outside of the record
  while ((pid2 != 0)&&(particle2->GetKS() != 21)) {

    if (_verbosity) {
      cout << Form("%5i",pid1);
      cout << Form(" %-11s",particle1->GetName());
      cout << Form(" %3i",particle1->GetKS());
      cout << Form(" %7i",particle1->GetKF());
      cout << Form(" %4i",particle1->GetParent());
      cout << Form(" %8.3f",particle1->GetPx());
      cout << Form(" %8.3f",particle1->GetPy());
      cout << Form(" %8.3f",particle1->GetPz());
      cout << Form(" %8.3f",particle1->GetEnergy());
      cout << Form(" %8.3f",particle1->GetMass());
      cout << Form(" %8.3f",get_eta(particle1));
      cout << Form(" %8.3f",get_phi(particle1));
      cout << endl;
    }

    // handle multi-grandparent states (strings, clusters, etc)
    if (!has_single_parent(pid2)) {
      
      // print out string/cluster entry
      if (_verbosity) {
	cout << Form("%5i",pid2);
	cout << Form(" %-11s",particle2->GetName());
	cout << Form(" %3i",particle2->GetKS());
	cout << Form(" %7i",particle2->GetKF());
	cout << Form(" %4i",particle2->GetParent());
	cout << Form(" %8.3f",particle2->GetPx());
	cout << Form(" %8.3f",particle2->GetPy());
	cout << Form(" %8.3f",particle2->GetPz());
	cout << Form(" %8.3f",particle2->GetEnergy());
	cout << Form(" %8.3f",particle2->GetMass());
	cout << Form(" %8.3f",get_eta(particle2));
	cout << Form(" %8.3f",get_phi(particle2));
	cout << endl;
      }

      std::vector<int> candidates = get_multiple_parents(pid2);

      if (_verbosity) {
	cout << " ================================================================================================" << endl;
	for (unsigned int i = 0; i < candidates.size(); ++i) {
	  int candidate_id = candidates[i];
	  TMCParticle* candidate = (TMCParticle*)_particles->getParticle(candidate_id-1);
	  cout << Form("%5i",candidate_id);
	  cout << Form(" %-10s",candidate->GetName());
	  if (i==0) cout << "A";
	  else if (i!=candidates.size()-1) cout << "I";
	  else cout << "V";
	  cout << Form(" %3i",candidate->GetKS());
	  cout << Form(" %7i",candidate->GetKF());
	  cout << Form(" %4i",candidate->GetParent());
	  cout << Form(" %8.3f",candidate->GetPx());
	  cout << Form(" %8.3f",candidate->GetPy());
	  cout << Form(" %8.3f",candidate->GetPz());
	  cout << Form(" %8.3f",candidate->GetEnergy());
	  cout << Form(" %8.3f",candidate->GetMass());
	  cout << Form(" %8.3f",get_eta(candidate));
	  cout << Form(" %8.3f",get_phi(candidate));
	  cout << endl;
	}	   
	cout << " ================================================================================================" << endl;
      }
 
      int best_id = select_best_candidate(pid1,pid2,candidates);
      TMCParticle* best = (TMCParticle*)_particles->getParticle(best_id-1);

      // print out best selected entry
      if (_verbosity) {
	cout << Form("%5i",best_id);
	cout << Form(" %-11s",best->GetName());
	cout << Form(" %3i",best->GetKS());
	cout << Form(" %7i",best->GetKF());
	cout << Form(" %4i",best->GetParent());
	cout << Form(" %8.3f",best->GetPx());
	cout << Form(" %8.3f",best->GetPy());
	cout << Form(" %8.3f",best->GetPz());
	cout << Form(" %8.3f",best->GetEnergy());
	cout << Form(" %8.3f",best->GetMass());
	cout << Form(" %8.3f",get_eta(best));
	cout << Form(" %8.3f",get_phi(best));
	cout << endl;
      }

      // if the string component traces, keep following
      if (best_id != 0) { 
	TMCParticle *best = (TMCParticle*)_particles->getParticle(best_id-1);
	if ((best->GetParent() == 7)||(best->GetParent() == 8)) {
	  best_id = best->GetParent();
	  best = (TMCParticle*)_particles->getParticle(best_id-1);
	  if (_verbosity) {
	    cout << Form("%5i",best_id);
	    cout << Form(" %-11s",best->GetName());
	    cout << Form(" %3i",best->GetKS());
	    cout << Form(" %7i",best->GetKF());
	    cout << Form(" %4i",best->GetParent());
	    cout << Form(" %8.3f",best->GetPx());
	    cout << Form(" %8.3f",best->GetPy());
	    cout << Form(" %8.3f",best->GetPz());
	    cout << Form(" %8.3f",best->GetEnergy());
	    cout << Form(" %8.3f",best->GetMass());
	    cout << Form(" %8.3f",get_eta(best));
	    cout << Form(" %8.3f",get_phi(best));
	    cout << endl;
	  }
	}
      }

      // go back and record best grandparent as the origin particle
      // thus skipping through the string and going for the next particle
      pid2 = best_id;
      if (best_id != 0) {
	particle2 = (TMCParticle*)_particles->getParticle(pid2-1);
      }
    } // string handling	

    // advance a generation
    pid1 = pid2;
    if (pid1 != 0) {
      particle1 = (TMCParticle*)_particles->getParticle(pid1-1);
    } else {
      break;
    }

    pid2 = particle1->GetParent();
    if (pid2 != 0) {
      particle2 = (TMCParticle*)_particles->getParticle(pid2-1);
    } else {
      break;
    }
  }

  if (!particle) {
    pid = pid1;
    particle = (TMCParticle*)_particles->getParticle(pid-1);
  }

  if (_verbosity) {
    cout << " ================================================================================================" << endl;
  }

  return pid;
}

bool PHPythiaAncestryHelper::has_single_parent(int particle_id) {
  TMCParticle *particle = (TMCParticle*)_particles->getParticle(particle_id-1);
  if ((particle->GetKF() > 90)&&(particle->GetKF() < 95)) return false;
  return true;
}

std::vector<int> PHPythiaAncestryHelper::get_multiple_parents(int particle_id) {

  std::vector<int> parent_ids;

  if (particle_id <= 0) return parent_ids;
    
  TMCParticle *particle = (TMCParticle*)_particles->getParticle(particle_id-1);    
  int parent_id = particle->GetParent();

  bool last = false;
  while (!last) {	

    if (parent_id <= 0) break;
    parent_ids.push_back(parent_id);
    
    TMCParticle *parent = (TMCParticle*)_particles->getParticle(parent_id-1);
    if (parent->GetKS() != 12) {
      last = true;
    }

    ++parent_id;
  }

  return parent_ids;
}

int PHPythiaAncestryHelper::select_best_candidate(int particle_id, int parent_id, std::vector<int> candidates) {
 
  // nearest in phi and eta
  // best candidate should have more momentum than child particle
  // best candidate should have ancestry (not be MPI gluon)

  int best_id = -1;
  double best_distance = DBL_MAX;
  
  TMCParticle* particle = (TMCParticle*)_particles->getParticle(particle_id-1);
  TMCParticle* string   = (TMCParticle*)_particles->getParticle(parent_id-1);
  if (_traverse_mpi) {
    for (unsigned int i = 0; i < candidates.size(); ++i) {
      TMCParticle* candidate = (TMCParticle*)_particles->getParticle(candidates[i]-1);

      if (!passes_ancestry_cut(particle,string,candidate)) continue;
      if (!passes_kinematic_cut(particle,string,candidate)) continue;

      double distance = get_distance(particle,candidate);
      if (distance < best_distance) {
	best_id = candidates[i];
	best_distance = distance;      
      }
    }
  }

  // repeat search relaxing the ancestry requirement
  if (best_id == -1) {
    for (unsigned int i = 0; i < candidates.size(); ++i) {
      TMCParticle* candidate = (TMCParticle*)_particles->getParticle(candidates[i]-1);

      if (!passes_kinematic_cut(particle,string,candidate)) continue;      
      
      double distance = get_distance(particle,candidate);
      if (distance < best_distance) {
	best_id = candidates[i];
	best_distance = distance;      
      }
    }
  }

  // repeat search relaxing the ancestry requirement & kinematic requirement
  if (best_id == -1) {
    for (unsigned int i = 0; i < candidates.size(); ++i) {
      TMCParticle* candidate = (TMCParticle*)_particles->getParticle(candidates[i]-1);
      
      double distance = get_distance(particle,candidate);
      if (distance < best_distance) {
	best_id = candidates[i];
	best_distance = distance;      
      }
    }
  }

  return best_id;
}

bool PHPythiaAncestryHelper::passes_ancestry_cut(TMCParticle* particle, TMCParticle* string, TMCParticle* candidate) {
  if (get_mom(candidate) < get_mom(particle)) return false;
  return true;
}

bool PHPythiaAncestryHelper::passes_kinematic_cut(TMCParticle* particle, TMCParticle* string, TMCParticle* candidate) {
  if (get_mom(candidate) < get_mom(particle)) return false;
  return true;
}

double PHPythiaAncestryHelper::get_distance(TMCParticle* particleA, TMCParticle* particleB) {
  double distance = sqrt(pow(get_eta(particleA)-get_eta(particleB),2)+pow(get_phi(particleA)-get_phi(particleB),2));
  return distance;
}

double PHPythiaAncestryHelper::get_mom(TMCParticle* particle) {  
  return sqrt(pow(particle->GetPx(),2)+pow(particle->GetPy(),2)+pow(particle->GetPz(),2));
}

double PHPythiaAncestryHelper::get_pt(TMCParticle* particle) {  
  return sqrt(pow(particle->GetPx(),2)+pow(particle->GetPy(),2));
}

double PHPythiaAncestryHelper::get_eta(TMCParticle* particle) {  
  double pt = get_pt(particle);
  if (pt != 0.0) {
    return asinh(particle->GetPz()/pt);
  }
  if (particle->GetPz() > 0.0) return +9999.9;
  return -9999.9;
}

double PHPythiaAncestryHelper::get_phi(TMCParticle* particle) {
  
  if (particle->GetPx() == 0.0) return 0.0;
  return atan2(particle->GetPy(),particle->GetPx());  
}
