#include <PHPythiaContainerV2.h>

#include <TBuffer.h>
#include <TClass.h>
#include <TClonesArray.h>

#include <RVersion.h> // root version 

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,8) 
#include <TMCParticle.h>
#else
#include <TMCParticle6.h>
#endif

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <vector>

ClassImp(PHPythiaContainerV2)

using namespace std;

PHPythiaContainerV2::PHPythiaContainerV2()
{
  const int MAXPARTICLES = 1000;
  part_array = new TClonesArray("TMCParticle",MAXPARTICLES);
  py_npart = 0;

  Reset();
}

PHPythiaContainerV2::~PHPythiaContainerV2()
{
  if ( part_array ) delete part_array;
}

TMCParticle* PHPythiaContainerV2::addParticle(const TMCParticle &particle)
{
  // First check if TC array exists (it does not for the base class)
  if (!GetArray())
    {
      return NULL;
    }

  new ((*part_array)[py_npart]) TMCParticle( particle );
  TMCParticle *temp = (TMCParticle*)GetArray()->At(py_npart);

  py_npart++;

  return temp;
}

TMCParticle*
PHPythiaContainerV2::getParticle(unsigned int itow) const
{
  assert( itow <= (unsigned int)GetArray()->GetLast() );
  return static_cast<TMCParticle*>(GetArray()->At(itow));
}

unsigned int PHPythiaContainerV2::getLineNumber(TMCParticle* part) const
{
  unsigned int npart = size();
  
  for (unsigned int ipart = 0; ipart < npart; ipart++) {
    TMCParticle *temp = (TMCParticle*)GetArray()->At(ipart);
    if (temp == part) return ipart+1;
  }

  return 9999;
}

TMCParticle* PHPythiaContainerV2::getParent(TMCParticle* part) const
{
  unsigned int parentLineNumber = part->GetParent();
  if (parentLineNumber == 0) return 0; // Top particle no parent

  TMCParticle *temp = getParticle(parentLineNumber-1); 

  return temp;
}

TMCParticle* PHPythiaContainerV2::getDifferentParent(TMCParticle* part) const
{
/*
  unsigned int parentLineNumber = part->GetParent();
  if (parentLineNumber == 0) return 0; // Top particle no parent
*/

  Int_t part_kf = part->GetKF();

  TMCParticle *temp = part;
  TMCParticle *temp_parent = 0;
  do {
    temp_parent = getParent(temp); 
    temp = temp_parent;
  } while ( (temp != 0) && (temp->GetKF() == part_kf) );

  return temp;
}

unsigned int PHPythiaContainerV2::getChildNumber(TMCParticle* part) const
{
  unsigned int childNumber = part->GetLastChild() - part->GetFirstChild() + 1;
  if (part->GetLastChild() == 0 && part->GetFirstChild() == 0) childNumber = 0; 

  return childNumber;
}

TMCParticle* PHPythiaContainerV2::getChild(TMCParticle* part, unsigned int iChild) const
{
  unsigned int firstChildLineNumber = part->GetFirstChild();
  unsigned int lastChildLineNumber  = part->GetLastChild();
  if (iChild > (lastChildLineNumber - firstChildLineNumber)) {
    cout << "error: " << iChild << "th child particle not found" << endl;
    exit(2);
  }

  TMCParticle *temp = getParticle(firstChildLineNumber + iChild - 1);

  return temp;
}

TString PHPythiaContainerV2::getHistoryString(TMCParticle* part) const
{
  TString history;

  TMCParticle* temp = part;
  int count = 0;
  while (getParent(temp) != 0) {
    stringstream istr;
    istr << " -> " << std::setw(10) << temp->GetName();
    history = TString(istr.str()) + history;    

    temp = getParent(temp);
    count++;
    if (count>100) break;
  }
  history = temp->GetName() + history;
  if (getChildNumber(part) == 0) return history;

  history += " -> (";
  for (unsigned int iChild = 0; iChild < getChildNumber(part); iChild++) {
    history += getChild(part, iChild)->GetName();
    if (iChild != getChildNumber(part)-1) history += ", ";
  }
  history += ")";
 
  return history;
}

TMCParticle* PHPythiaContainerV2::hasAncestor(TMCParticle* part, int pid) const
{
  TMCParticle* temp = part;
  int count = 0;
  while(temp != 0) {
    //std::cout << aPar->GetKF() << std::endl;
    if (temp->GetKF() == pid) return temp;
    temp = getParent(temp);
    count++;
    if (count>100) break;
  }

  return 0;
}

bool PHPythiaContainerV2::hasAncestor(TMCParticle* part, int pid1, int pid2) const
{
  if (hasAncestor(part, pid1) == 0 ) return false;
  if (hasAncestor(part, pid2) == 0 ) return false;

  return true;
}
  
bool PHPythiaContainerV2::hasAncestor(TMCParticle* part, int pid1, int pid2, int pid3) const
{
  if (hasAncestor(part, pid1) == 0 ) return false;
  if (hasAncestor(part, pid2) == 0 ) return false;
  if (hasAncestor(part, pid3) == 0 ) return false;

  return true;
}

TMCParticle* PHPythiaContainerV2::hasAncestorAbs(TMCParticle* part, unsigned int pid) const
{
  TMCParticle* temp = hasAncestor(part, pid);
  if (temp) return temp;
  else temp = hasAncestor(part, -pid);

  if (temp) return temp;
  else return 0;
}

TMCParticle* PHPythiaContainerV2::hasAncestorRange(TMCParticle* part, int pidStart, int pidEnd) const
{
  int tempPidStart = pidStart;
  int tempPidEnd   = pidEnd;
  if (pidStart > pidEnd) {
    tempPidStart = pidEnd;
    tempPidEnd   = pidStart;
  }

  TMCParticle* temp = part;
  int count = 0;
  while(temp != 0) {
    if (temp->GetKF() >= tempPidStart && temp->GetKF() <= tempPidEnd) return temp;
    temp = getParent(temp);
    count++;
    if (count>100) break;
  }

  return 0;
}

TMCParticle* PHPythiaContainerV2::hasAncestorAbsRange(TMCParticle* part, unsigned int pidStart, unsigned int pidEnd) const
{
  unsigned int tempPidStart = pidStart;
  unsigned int tempPidEnd   = pidEnd;
  if (pidStart > pidEnd) {
    tempPidStart = pidEnd;
    tempPidEnd   = pidStart;
  }

  TMCParticle *temp = 0;
  temp = hasAncestorRange(part, tempPidStart, tempPidEnd);
  if (temp) return temp;
  else temp = hasAncestorRange(part, -tempPidEnd, -tempPidStart);

  if (temp) return temp;
  else return 0;
}

// Note: this assumes primary_true_track starts from 0.
TMCParticle* PHPythiaContainerV2::getPrimaryParticle(const unsigned int primary_true_track) const
{
  // Build a list of stable particles, using the definition of stable provided by the
  // selector.  TODO: cache this list upon construction of the container, then we
  // don't need to do it every time the thing is looked up.  See "Streamers with Special Additions"
  // of the ROOT User's manual, for example.
  //std::vector<TMCParticle*> stables;
  if ( _stables.size() == 0 ) 
    {
      for (unsigned int ipart=0; ipart<size(); ipart++)
	{
	  TMCParticle* p = getParticle(ipart);
	  //if ( _primaryPred(p) ) stables.push_back(p);
          if ( _primaryPred(p) ) _stables.push_back(p);
	}
    }

  // Use std::vector::at() to ensure that an error in indexing gets
  // thrown back to the user (at() checks the access, and throws a
  // range_error if needed)
  //return stables.at(primary_true_track);
  return _stables.at(primary_true_track);
}

// return the true_track number of the ith pythia particle in the primary node
// ipythia counts from 0
Int_t PHPythiaContainerV2::getPrimaryNumber(const unsigned int ipythia) const
{
  int primary = 0;
  for (unsigned int ipart=0; ipart<ipythia; ipart++)
    {
      TMCParticle* p = getParticle(ipart);
      if ( _primaryPred(p) ) ++primary;
    }

  // primary was not found, this is an unstable particle
  if ( primary==0 ) return -1;

  return primary;
}

void PHPythiaContainerV2::moveVertex(const float x, const float y, const float z)
{

  static const float CM2MM = 10.;
  
  // go through all particles and add x,y,z to the vertex
  // x,y,z are in cm
  for (unsigned int ipart=0; ipart<size(); ipart++)
  {
    
    TMCParticle *p = getParticle(ipart);
    p->SetVx( p->GetVx() + x*CM2MM );
    p->SetVy( p->GetVy() + y*CM2MM );
    p->SetVz( p->GetVz() + z*CM2MM );
    
  }
}

unsigned int PHPythiaContainerV2::size(void) const
{ 
  unsigned int s;
  TClonesArray *p = GetArray();
  if (p)
    {
      s = p->GetLast()+1; 
    }
  else
    {
      s = -1;
    }
  return s;
}

void PHPythiaContainerV2::Reset()
{
  GetArray()->Clear();
  py_npart = 0;
  _stables.clear();
}

//_____________________________________________________________________________
void PHPythiaContainerV2::identify(std::ostream& os ) const
{
  os << "identify yourself: PHPythiaContainerV2 Object" << endl;
  os << "No of Entries: " << size() << endl;
  return;
}

void PHPythiaContainerV2::print(std::ostream& out) const
{
  identify(out);
/*
  for ( unsigned int i = 0; i < size(); ++i ) 
    {
      getTower(i)->print(out);
    }
*/
}
