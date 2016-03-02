#ifndef __PHPYTHIAHEADER_H__
#define __PHPYTHIAHEADER_H__

#include <phool/phool.h>
#include <phool/PHObject.h>

class PHPythiaHeader : public PHObject
{
public:

  PHPythiaHeader();
  virtual ~PHPythiaHeader() {}

  /// event number
  virtual Int_t GetEvt() const		{ PHOOL_VIRTUAL_WARNING; return -9999; }
  /// number of particles in event
  virtual Int_t GetNpart() const	{ PHOOL_VIRTUAL_WARNING; return -9999; }
  /// type of process
  virtual Int_t GetProcessid() const	{ PHOOL_VIRTUAL_WARNING; return -9999; }
  /// KF code of parton 1
  virtual Int_t GetParId_1() const	{ PHOOL_VIRTUAL_WARNING; return -9999; }
  /// KF code of parton 2
  virtual Int_t GetParId_2() const	{ PHOOL_VIRTUAL_WARNING; return -9999; }
  /// Bjorken x1
  virtual Float_t GetX1() const		{ PHOOL_VIRTUAL_WARNING; return -9999.; }
  /// Bjorken x2
  virtual Float_t GetX2() const		{ PHOOL_VIRTUAL_WARNING; return -9999.; }
  /// Mandelstam s
  virtual Float_t GetSvar() const	{ PHOOL_VIRTUAL_WARNING; return -9999.; }
  /// Mandelstam t
  virtual Float_t GetTvar() const	{ PHOOL_VIRTUAL_WARNING; return -9999.; }
  /// Mandelstam u
  virtual Float_t GetUvar() const	{ PHOOL_VIRTUAL_WARNING; return -9999.; }
  /// Q squared
  virtual Float_t GetQsqr() const	{ PHOOL_VIRTUAL_WARNING; return -9999.; }
  /// transverse momentum
  virtual Float_t GetPt() const		{ PHOOL_VIRTUAL_WARNING; return -9999.; }
  /// parton-parton y in com frame
  virtual Float_t GetYcom() const	{ PHOOL_VIRTUAL_WARNING; return -9999.; }

  virtual void SetEvt(const Int_t Evt) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetNpart(const Int_t Npart) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetProcessid(const Int_t Processid) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetParId_1(const Int_t ParId_1) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetParId_2(const Int_t ParId_2) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetX1(const Float_t X1) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetX2(const Float_t X2) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetSvar(const Float_t Svar) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetTvar(const Float_t Tvar) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetUvar(const Float_t Uvar) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetQsqr(const Float_t Qsqr) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetPt(const Float_t Pt) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetYcom(const Float_t y) { PHOOL_VIRTUAL_WARNING; }

  //!@name primary vertex position
  //@{
  
  /*! 
  it is safe to return zero here
  since it is what pythia uses by default
  */
  virtual Float_t GetPrimaryVertexX() const	{ return 0.; }
  virtual Float_t GetPrimaryVertexY() const	{ return 0.; }
  virtual Float_t GetPrimaryVertexZ() const	{ return 0.; }
  
  virtual void SetPrimaryVertexX( Float_t value ) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetPrimaryVertexY( Float_t value ) { PHOOL_VIRTUAL_WARNING; }
  virtual void SetPrimaryVertexZ( Float_t value ) { PHOOL_VIRTUAL_WARNING; }
  
  // move the vertex by x, y, z (in cm)
  virtual void moveVertex(const float x, const float y, const float z)
  { PHOOL_VIRTUAL_WARNING; }
  
  //@}

  // PHObject methods
  int isValid() const { PHOOL_VIRTUAL_WARNING; return 0; }
  void Reset() { PHOOL_VIRTUAL_WARNING; }
  void identify(std::ostream& os=std::cout) const { PHOOL_VIRTUAL_WARNING; }
  void print(std::ostream& os=std::cout) const { PHOOL_VIRTUAL_WARNING; }

private:

  ClassDef (PHPythiaHeader,1)
};

#endif	// __PHPYTHIAHEADER_H__

