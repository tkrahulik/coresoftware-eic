#ifndef __PHPythiaHeaderV2_H__
#define __PHPythiaHeaderV2_H__

#include "PHPythiaHeader.h"

class PHPythiaHeaderV2 : public PHPythiaHeader
{
public:

  PHPythiaHeaderV2();
  virtual ~PHPythiaHeaderV2() {}

  Int_t   GetEvt() const	{return py_event;}
  Int_t   GetNpart() const	{return py_npart;}
  Int_t   GetProcessid() const	{return processid;}
  Int_t   GetParId_1() const	{return par_id_1;}
  Int_t   GetParId_2() const	{return par_id_2;}
  Float_t GetX1() const		{return x1;}
  Float_t GetX2() const		{return x2;}
  Float_t GetSvar() const	{return svar;}
  Float_t GetTvar() const	{return tvar;}
  Float_t GetUvar() const	{return uvar;}
  Float_t GetQsqr() const	{return qsqr;}
  Float_t GetPt() const		{return pt;}
  Float_t GetYcom() const	{return y_com;}

  void    SetEvt(const Int_t Evt)               {py_event = Evt;}
  void    SetNpart(const Int_t Npart)           {py_npart = Npart;}
  void    SetProcessid(const Int_t Processid)   {processid = Processid;}
  void    SetParId_1(const Int_t ParId_1)       {par_id_1 = ParId_1;}
  void    SetParId_2(const Int_t ParId_2)       {par_id_2 = ParId_2;}
  void    SetX1(const Float_t X1)               {x1 = X1;}
  void    SetX2(const Float_t X2)               {x2 = X2;}
  void    SetSvar(const Float_t Svar)           {svar = Svar;}
  void    SetTvar(const Float_t Tvar)           {tvar = Tvar;}
  void    SetUvar(const Float_t Uvar)           {uvar = Uvar;}
  void    SetQsqr(const Float_t Qsqr)           {qsqr = Qsqr;}
  void    SetPt(const Float_t Pt)               {pt = Pt;}
  void    SetYcom(const Float_t y)              {y_com = y;}

  //!@name primary vertex position
  //@{
  Float_t GetPrimaryVertexX() const	{return primary_vertex_x;}
  Float_t GetPrimaryVertexY() const	{return primary_vertex_y;}
  Float_t GetPrimaryVertexZ() const	{return primary_vertex_z;}
  
  void SetPrimaryVertexX( Float_t value ) {primary_vertex_x = value;}
  void SetPrimaryVertexY( Float_t value ) {primary_vertex_y = value;}
  void SetPrimaryVertexZ( Float_t value ) {primary_vertex_z = value;}
  
  // move the vertex by x, y, z (in cm)
  void moveVertex(const float x, const float y, const float z);
  
  //@}
  
  // PHObject methods
  int isValid() const { return 1; }
  void Reset();
  void identify(std::ostream& os=std::cout) const;
  void print(std::ostream& os=std::cout) const;

private:
  Int_t   py_event;	// event number
  Int_t   py_npart;	// number of particles in event
  Int_t   processid;	// type of process
  Int_t   par_id_1;	// KF code of parton 1
  Int_t   par_id_2;	// KF code of parton 2
  Float_t x1;		// Bjorken x1
  Float_t x2;		// Bjorken x2
  Float_t svar;		// Mandelstam s
  Float_t tvar;		// Mandelstam t
  Float_t uvar;		// Mandelstam u
  Float_t qsqr;		// Q squared
  Float_t pt;		// transverse momentum
  Float_t y_com;	// parton-parton y in com frame

  //!@name primary vertex (millimeters)
  //@{
  Float_t primary_vertex_x; 
  Float_t primary_vertex_y; 
  Float_t primary_vertex_z; 
  //@}
  
  ClassDef (PHPythiaHeaderV2,1)
};

#endif	// __PHPythiaHeaderV2_H__

