#ifndef __PHPYTHIACONTAINERV2_H__
#define __PHPYTHIACONTAINERV2_H__

#include <PHPythiaContainer.h>
#include <TMCParticle.h> // in order to define the selection functor in the header

#include <vector>
#include <iostream>
#include <functional>

class TClonesArray;
class TMCParticle; 

class PHPythiaContainerV2 : public PHPythiaContainer
{
public:

  PHPythiaContainerV2();

  virtual ~PHPythiaContainerV2();
  
  TMCParticle* addParticle(const TMCParticle &c);

  TMCParticle* getParticle(unsigned int itow) const;

  // the line number of a particle in pythia event generation 
  unsigned int getLineNumber(TMCParticle* part) const;

  // return pointer to the parent of a give particle
  TMCParticle* getParent(TMCParticle* part) const;

  // return pointer to the parent of a given particle, parent must be of a different type than particle
  TMCParticle* getDifferentParent(TMCParticle* part) const;

  // number of direct children of a give particle 
  unsigned int getChildNumber(TMCParticle* part) const;

  // return pointer to one of the children
  TMCParticle* getChild(TMCParticle* part, unsigned int iChild) const;

  // get history string
  TString getHistoryString(TMCParticle* part) const;

  // find a given particle's ancestor particle with pid and return its pointer, if not found, return NULL
  TMCParticle* hasAncestor(TMCParticle* part, int pid) const;

  // whether a given particle's has ancestor with pid1 and pid2,  e.x  B+ -> D+ -> mu+
  bool hasAncestor(TMCParticle* part, int pid1, int pid2) const;

  // whether a given particle's has ancestor with pid1 and pid2 and pid3
  bool hasAncestor(TMCParticle* part, int pid1, int pid2, int pid3) const;

  // find a given particle's ancestor particle with abs(pid) and return its pointer, e.x 211 for both of pi+ and pi-
  TMCParticle* hasAncestorAbs(TMCParticle* part, unsigned int pid) const;

  // find a given particle's ancestor in pid range [pidStar, pidEnd],  e.x  [400,499] for any of  D+, D*+, D0, D0*, Ds+, Ds*+
  TMCParticle* hasAncestorRange(TMCParticle* part, int pidStart, int pidEnd) const;

  // find a given particle's ancestor in abs(pid) range [pidStar, pidEnd],  e.x  [400,499] for [-499,-400] and [400, 499]
  TMCParticle* hasAncestorAbsRange(TMCParticle* part, unsigned int pidStart, unsigned int pidEnd) const;

  bool isStable(const unsigned int iPart) const 
  { return _primaryPred( getParticle(iPart) ); }

  // return pointer to the particle with primary id (ie, the true_track in the primary node)
  TMCParticle* getPrimaryParticle(const unsigned int primary_true_track) const;

  // return the true_track number of particle i in the primary node
  Int_t getPrimaryNumber(const unsigned int i) const;

  // move the vertex by x, y, z (in cm)
  virtual void moveVertex(const float x, const float y, const float z);

  // the actual size of the object.
  unsigned int size(void) const;

  int isValid() const
  {
    // if size>0, return 1, otherwise 0
    return((size()>0) ? 1 : 0);
  }

  void Reset();

  void identify(std::ostream& os=std::cout) const;

  void print(std::ostream& os=std::cout) const;

protected:
  short py_npart;

  TClonesArray *part_array;	// array of particles
  TClonesArray *GetArray() const {return part_array;}
  void setSize(const short n) { py_npart = n; }

private:

  struct StableSelector : public std::unary_function<bool,TMCParticle*> 
  {
    
    bool operator()(const TMCParticle* p) const 
    { return p->GetKS()>0 && p->GetKS()<11; }

  };

  StableSelector _primaryPred; //! Predicate for selecting primary particles

  mutable std::vector<TMCParticle*> _stables; //! cached list of stable particles

  ClassDef(PHPythiaContainerV2,1) // Array of PHPythiaContainerV2 (ABC)
    
};

#endif	// __PHPYTHIACONTAINERV2_H__

