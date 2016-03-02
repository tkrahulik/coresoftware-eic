#ifndef __PHPYTHIACONTAINER__
#define __PHPYTHIACONTAINER__

//#include <iosfwd>
#include <phool/phool.h>
#include <phool/PHObject.h>
#include <TString.h>
#include <TClonesArray.h>
 
//class TClonesArray;
class TMCParticle;
class TString;

class PHPythiaContainer : public PHObject
{
public:

  PHPythiaContainer();
  virtual ~PHPythiaContainer(){}
  
  /** Add a new particle (using copy ctor). */
  virtual TMCParticle* addParticle(const TMCParticle &tpart)
  { PHOOL_VIRTUAL_WARNING; return NULL; }

  virtual TMCParticle* RemoveAt(Int_t idx)
  { setSize(size()-1); return (TMCParticle*)(GetArray()->RemoveAt(idx)); }

  virtual void Compress()
  { GetArray()->Compress(); return; }

  virtual TMCParticle* getParticle(unsigned int i) const 
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual unsigned int getLineNumber(TMCParticle* part) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual TMCParticle* getParent(TMCParticle* part) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual TMCParticle* getDifferentParent(TMCParticle* part) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual unsigned int getChildNumber(TMCParticle* part) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual TMCParticle* getChild(TMCParticle* part, unsigned int iChild) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual TString getHistoryString(TMCParticle* part) const
  { PHOOL_VIRTUAL_WARNING; return TString(""); }

  virtual TMCParticle* hasAncestor(TMCParticle* part, int pid) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual bool hasAncestor(TMCParticle* part, int pid1, int pid2) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual bool hasAncestor(TMCParticle* part, int pid1, int pid2, int pid3) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual TMCParticle* hasAncestorAbs(TMCParticle* part, unsigned int pid) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual TMCParticle* hasAncestorRange(TMCParticle* part, int pidStart, int pidEnd) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual TMCParticle* hasAncestorAbsRange(TMCParticle* part, unsigned int pidStart, unsigned int pidEnd) const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual bool isStable(const unsigned int iPart) const
  { PHOOL_VIRTUAL_WARNING; return false; }

  virtual TMCParticle* getPrimaryParticle(const unsigned int i) const 
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual Int_t getPrimaryNumber(const unsigned int i) const 
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual void moveVertex(const float x, const float y, const float z)
  { PHOOL_VIRTUAL_WARNING; return; }

  // the number of entries in the array of particles
  virtual unsigned int size(void) const 
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual int isValid() const
  { PHOOL_VIRTUAL_WARNING; return 0; }

  virtual void Reset()
  { PHOOL_VIRTUAL_WARNING; } 

  virtual void identify(std::ostream& os=std::cout) const
  { PHOOL_VIRTUAL_WARNING; }

  virtual void print(std::ostream& os=std::cout) const
  { PHOOL_VIRTUAL_WARNING; }

protected:
  virtual TClonesArray *GetArray() const { PHOOL_VIRTUAL_WARNING; return 0;}
  virtual void setSize(const short s) { PHOOL_VIRTUAL_WARNING; }

private:

  ClassDef(PHPythiaContainer,1)
    
};

#endif	// __PHPYTHIACONTAINER__
