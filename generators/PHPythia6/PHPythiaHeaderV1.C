#include "PHPythiaHeaderV1.h"

ClassImp(PHPythiaHeaderV1)

using namespace std;

PHPythiaHeaderV1::PHPythiaHeaderV1()
{
}

void PHPythiaHeaderV1::Reset()
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
}

void PHPythiaHeaderV1::identify(std::ostream& os) const
{
  os << "I am a PHPythiaHeaderV1 Object" << endl;
}

void PHPythiaHeaderV1::print(std::ostream& os) const
{
  os << "PHPythiaHeaderV1" << endl;
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
}

