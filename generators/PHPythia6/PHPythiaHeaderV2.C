#include "PHPythiaHeaderV2.h"

ClassImp(PHPythiaHeaderV2)

using namespace std;

PHPythiaHeaderV2::PHPythiaHeaderV2()
{
}

void PHPythiaHeaderV2::Reset()
{
  
  py_event = -9999;
  py_npart = -9999;
  processid = -9999;
  par_id_1 = -9999;
  par_id_2 = -9999;
  x1 = -9999.;
  x2 = -9999.;
  svar = -9999.;
  tvar = -9999.;
  uvar = -9999.;
  qsqr = -9999.;
  pt = -9999.;
  y_com = -9999.;

  // setting the primary vertex to zero here is safe
  // since this is usually what pythia does.
  primary_vertex_x = 0; 
  primary_vertex_y = 0;  
  primary_vertex_z = 0; 
  
}

void PHPythiaHeaderV2::moveVertex(const float x, const float y, const float z)
{

  static const float CM2MM = 10.;
  primary_vertex_x += x*CM2MM; 
  primary_vertex_y += y*CM2MM;  
  primary_vertex_z += z*CM2MM; 

}

void PHPythiaHeaderV2::identify(std::ostream& os) const
{ os << "I am a PHPythiaHeaderV2 Object" << endl;}

void PHPythiaHeaderV2::print(std::ostream& os) const
{
  os << "PHPythiaHeaderV2" << endl;
  os << "py_event\t" << py_event << endl;
  os << "py_npart\t" << py_npart << endl;
  os << "processid\t" << processid << endl;
  os << "par_id_1\t" << par_id_1 << endl;
  os << "par_id_2\t" << par_id_2 << endl;
  os << "x1\t" << x1 << endl;
  os << "x2\t" << x2 << endl;
  os << "svar\t" << svar << endl;
  os << "tvar\t" << tvar << endl;
  os << "uvar\t" << uvar << endl;
  os << "qsqr\t" << qsqr << endl;
  os << "pt\t" << pt << endl;
  os << "y_com\t" << y_com << endl;
  
  os << "primary_vertex_x\t " << primary_vertex_x << endl; 
  os << "primary_vertex_y\t " << primary_vertex_y << endl; 
  os << "primary_vertex_z\t " << primary_vertex_z << endl; 
  
}

