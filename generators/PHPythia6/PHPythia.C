#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <algorithm>
#include <cctype>

#include <TPythia6.h>

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,8) 
#include <TMCParticle.h>
#else
#include <TMCParticle6.h>
#endif

#include <phool/PHIODataNode.h>
#include <phool/PHObject.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHNodeReset.h>
#include <phool/PHTimeStamp.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <PHPythia.h>
#include <PHPythiaHeaderV2.h>
#include <PHPythiaContainerV2.h>

#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>

#define pytune pytune_
extern "C" int  pytune(int *itune);

using namespace std;

typedef PHIODataNode<PHObject> PHObjectNode_t;

static const Float_t CM2MM = 10.;	// cm to mm comversion

//__________________________________________________________
PHPythia::PHPythia(const std::string &name): 
  SubsysReco(name),
  eventcount( 0 ),
  _configFile( "pythia.cfg" ),
  _xsecFile( "phpy_xsec.root" ),
  tpythia6( new TPythia6() ),
  phpythiaheader(0),
  phpythia( 0 ),
  fSeed(-1)
{}

//__________________________________________________________
PHPythia::~PHPythia()
{ if (tpythia6!=0) { delete tpythia6; } }

//__________________________________________________________
int PHPythia::Init(PHCompositeNode *topNode)
{
  
  ReadConfig();  
  CreateNodeTree(topNode);

  // event numbering will start from 1
  eventcount = 0;
  
  if ( fSeed < 0 )
  {
    // first try getting seed from /dev/random
    ifstream devrandom;
    devrandom.open("/dev/random",ios::binary);
    devrandom.read((char*)&fSeed,sizeof(fSeed));
    devrandom.close();
    
    if ( fSeed != -1 )
    {
      cout << PHWHERE << " Got seed from /dev/random" << endl;
      fSeed = abs(fSeed)%900000000;
    }
    else
    {
      // /dev/random failed, get the random seed from the time of day, to the microsecond
      //fSeed = (Int_t)(time(NULL)/3);
      cout << PHWHERE << " Getting seed from gettimeofday()" << endl;
      timeval xtime;
      int status = gettimeofday(&xtime,NULL);
      if ( status==0 )
      {
        fSeed = ((xtime.tv_sec << 12) + (xtime.tv_usec&0xfff))%900000000;
      }
      else
      {
        cout << PHWHERE << " something wrong with gettimeofday()" << endl;
      }
    }
  }
  
  
  if ( (fSeed>=0) && (fSeed<=900000000) ) {
    tpythia6->SetMRPY(1, fSeed);                    // set seed
    tpythia6->SetMRPY(2, 0);                       // use new seed
  } else {
    cout << PHWHERE << " ERROR: seed " << fSeed << " is not valid" << endl;
    exit(2); 
  }

  PrintConfig();

  TDirectory *orig_dir = gDirectory;

  xsecfile = new TFile(_xsecFile.c_str(),"RECREATE");
  
  // Set up the normalization TTree
  Tp = new TTree("Tp","Pythia Normalization Tree");
  Tp->Branch("isub",&tp_isub,"isub/i");
  //Tp->Branch("proc",&tp_proc,"proc/C");
  Tp->Branch("nevt",&tp_nevt,"nevt/i");
  Tp->Branch("sigma",&tp_sigma,"sigma/D");
  Tp->Branch("nevt_sigma",&tp_nevt_sigma,"nevt_sigma/D");
  Tp->Branch("integlumi",&tp_integlumi,"integlumi/D");

  orig_dir->cd();

  return Fun4AllReturnCodes::EVENT_OK;

}

//__________________________________________________________
int PHPythia::End(PHCompositeNode *topNode)
{
  //-* dump out closing info (cross-sections, etc)
  tpythia6->Pystat(1);

  // Write out the normalization TTree, which has the x-sections, etc
  Pyint5_t *pyint5 = tpythia6->GetPyint5();

  for (int i=0; i<500; i++)
    {
      if ( (i!=0) && (tpythia6->GetMSUB(i) != 1) ) continue;

      /*
      cout << i << "\t"
           << tpythia6->GetPROC(i) << "\t"
           << tpythia6->GetNGENPD() << "\t"
           << pyint5->NGEN[2][i] << "\t"
           << pyint5->NGEN[0][i] << "\t"
           << pyint5->XSEC[2][i] << endl;;
      */

      tp_isub = i;
      tp_proc = tpythia6->GetPROC(i);
      tp_nevt = pyint5->NGEN[2][i];
      tp_sigma = pyint5->XSEC[2][i];
      tp_nevt_sigma = tp_nevt*tp_sigma;
      if ( tp_sigma!=0. ) tp_integlumi = 1e-9*tp_nevt/tp_sigma;		// in pb^-1
      else                tp_integlumi = 0.;
      Tp->Fill();
    }

  xsecfile->Write();
  xsecfile->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//__________________________________________________________
int PHPythia::ReadConfig(const char *cfg_file)
{

  if( cfg_file ) _configFile = cfg_file;
  cout << "PHPythia::ReadConfig - Reading " << _configFile << endl;
  
  ifstream infile( _configFile.c_str() ); 
  if (infile.fail ())
  {
    cout << "PHPythia::ReadConfig - Failed to open file " << _configFile << endl;
    exit(2);
  }

  // initialize variables
  Int_t   _nevents(0);
  Float_t _roots(0); 
  string  _proj;
  string  _targ;
  string  _frame;

  string FullLine;      // a complete line in the config file
  string label;         // the label

  int index = 999999;
  int ivalue = 999999;
  double value = 1e9;

  // get one line first 
  getline(infile, FullLine);
  while ( !infile.eof() )
  {
    
    // skip lines that begin with #, or "//"
    if ( FullLine[0]=='#' || FullLine.substr(0, 2) == "//" )
    {
      getline(infile,FullLine);
      continue;
    }
    
    // make FullLine an istringstream
    istringstream line( FullLine.c_str() );
    
    // get label
    line >> label;
    
    // to lower case
    std::transform(label.begin(), label.end(), label.begin(), (int(*)(int)) std::tolower);
    
    // based on label, fill correct item
    if ( label == "nevents" )
    {
      line >> _nevents;
      cout << "nevents\t" << _nevents << endl;
    }
    else if ( label == "roots" )
    {
      line >> _roots;
      cout << "roots\t" << _roots << endl;
    }
    else if ( label == "proj" )
    {
      line >> _proj;
      cout << "proj\t" << _proj << endl;
    }
    else if ( label == "targ" )
    {
      line >> _targ;
      cout << "targ\t" << _targ << endl;
    }
    else if ( label == "frame" )
    {
      line >> _frame;
      cout << "frame\t" << _frame << endl;
    }
    else if ( label == "p1" || label == "p2")
    {
      int entry = 0;
      if ( label=="p1") entry = 1;
      else if ( label=="p2") entry = 2;

      char temp_line[10000];
      strcpy(temp_line,FullLine.c_str());
      char *sptr = strtok(temp_line," \t"); // skip first word
      sptr = strtok(NULL," \t");
      cout << label;
      int index = 1;
      while ( sptr != NULL )
        {
          double val = atof(sptr);
          tpythia6->SetP(entry,index,val);
          index++;
          cout << "\t" << val;
          sptr = strtok(NULL," \t");
        }
      cout << endl;
    }
    else if ( label == "msel" )
    {
      line >> ivalue;
      tpythia6->SetMSEL(ivalue);
      cout << "msel\t" << ivalue << endl;
    }
    else if ( label == "msub" )
    {
      line >> index >> ivalue;
      tpythia6->SetMSUB(index,ivalue);
      cout << "msub\t" << index << " " << ivalue << endl;
    }
    else if ( label == "mstp" )
    {
      line >> index >> ivalue;
      tpythia6->SetMSTP(index,ivalue);
      cout << "mstp\t" << index << " " << ivalue << endl;
    }
    else if ( label == "mstj" )
    {
      line >> index >> ivalue;
      tpythia6->SetMSTJ(index,ivalue);
      cout << "mstj\t" << index << " " << ivalue << endl;
    }
    else if ( label == "mstu" )
    {
      line >> index >> ivalue;
      tpythia6->SetMSTU(index,ivalue);
      cout << "mstu\t" << index << " " << ivalue << endl;
    }
    else if ( label == "ckin" )
    {
      line >> index >> value;
      tpythia6->SetCKIN(index,value);
      cout << "ckin\t" << index << " " << value << endl;
    }
    else if ( label == "parp" )
    {
      line >> index >> value;
      tpythia6->SetPARP(index,value);
      cout << "parp\t" << index << " " << value << endl;
    }
    else if ( label == "parj" )
    {
      line >> index >> value;
      tpythia6->SetPARJ(index,value);
      cout << "parj\t" << index << " " << value << endl;
    }
    else if ( label == "paru" )
    {
      line >> index >> value;
      tpythia6->SetPARU(index,value);
      cout << "paru\t" << index << " " << value << endl;
    }
    else if ( label == "parf" )
    {
      line >> index >> value;
      tpythia6->SetPARF(index,value);
      cout << "parf\t" << index << " " << value << endl;
    }
    else if ( label == "pytune" )
    {
      line >> ivalue;
      pytune(&ivalue);
      cout << "pytune\t" << ivalue << endl;
    }
    else if ( label == "imss" )
    {
      line >> index >> ivalue;
      tpythia6->SetIMSS(index,ivalue);
      cout << "imss\t" << index << " " << ivalue << endl;
    }
    else if ( label == "rmss" )
    {
      line >> index >> value;
      tpythia6->SetRMSS(index,value);
      cout << "rmss\t" << index << " " << value << endl;
    }
    else if ( label == "mdcy" )
    {
      int kc = 0;          // particle kc code
      line >> kc >> index >> ivalue;
      
      // if (index==1) turn on/off decay channel idc
      tpythia6->SetMDCY(kc,index,ivalue);
      cout << "mdcy\t" << kc << " " << index << " " << ivalue << endl;
    }
    else if ( label == "mdme" )
    {
      int idc = 0;          // decay channel
      line >> idc >> index >> ivalue;
      
      // if (index==1) turn on/off decay channel idc
      tpythia6->SetMDME(idc,index,ivalue);
      cout << "mdme\t" << idc << " " << index << " " << ivalue << endl;
    }
    else if ( label == "kfin" )
    {
      int i;	// 1=proj side, 2=target
      int j;	// flavor code (KF)
      line >> i >> j >> value;
      tpythia6->SetKFIN(i,j,ivalue);
      cout << "kfin\t" << i << " " << j << " " << value << endl;
    }
    else if ( label == "kfpr")
    {
      int idc = 0;
      line >> index >> ivalue >> idc;
      tpythia6->GetPyint2()->KFPR[ivalue-1][index-1] = idc;
      cout << "kfpr\t" << index << " " << ivalue << " " << idc << endl;
    }
    else if ( label == "pmas" )
    {
      int quark = 0;        // quark type
      line >> quark >> index >> value;
      
      // Pycomp converts KF to KC (different
      // particle indexing understood by pythia)
      tpythia6->SetPMAS(tpythia6->Pycomp(quark),index,value);
      cout << "pmas\t" << quark << " " << index << " " << value << endl;
    }
    else if ( label == "brat" )
    {
      line >> index >> value;
      tpythia6->SetBRAT(index,ivalue);
      cout << "brat\t" << index << " " << value << endl;
    }
    else if ( label == "vtxfile" )
    {
      cout << "vtxfile is ignored. Use PHPyVertexShift instead." << endl;
    }
    else
    {
      // label was not understood
      cout << "************************************************************" << endl;
      cout << "PHPythia::ReadConfig(), ERROR this option is not supported: " << FullLine << endl;
      cout << "Please email chiu@bnl.gov to correct this" << endl;
      cout << "or just add it yourself to " << PHWHERE << endl;
      cout << "************************************************************" << endl;
    }
    
    // get next line in file
    getline( infile, FullLine );
  }
  
  tpythia6->Initialize(_frame.c_str(), _proj.c_str(), _targ.c_str(), _roots);
  
  infile.close();
  
  return _nevents;
}

//-* print pythia config info
void PHPythia::PrintConfig() const
{
  tpythia6->Pystat(2);
  tpythia6->Pystat(3);
  tpythia6->Pystat(4);
  tpythia6->Pystat(5);
  tpythia6->Pystat(6);
  //tpythia6->Pylist(11);  // list of all decay modes for all defined particles
  //tpythia6->Pylist(12);  // list of all decay modes for all defined particles
  
  cout << "Using seed " << fSeed << endl;
}

int PHPythia::process_event(PHCompositeNode *topNode)
{
  ++eventcount;
  
  tpythia6->GenerateEvent();

  TClonesArray *particleArray = (TClonesArray *)tpythia6->ImportParticles();
  
  // number of generated particles
  Int_t numParticles = particleArray->GetLast() + 1;	
  int nstable = 0;
  for (Int_t ipart=0; ipart<numParticles; ipart++)
  { 
    
    // get the particle information
    TMCParticle *particle = (TMCParticle *)particleArray->At(ipart);	
    phpythia->addParticle(*particle);
    
    if ( phpythia->isStable(ipart) ) ++nstable;
    
  }
  
  phpythiaheader->SetEvt(eventcount);		// Event number
  phpythiaheader->SetNpart(numParticles);	// Number of particle entries in entire history
  phpythiaheader->SetProcessid(tpythia6->GetMSTI(1));	// Process ID
  phpythiaheader->SetParId_1(tpythia6->GetMSTI(15));	// KF codes for partons participating in hard scattering
  phpythiaheader->SetParId_2(tpythia6->GetMSTI(16));	// KF codes for partons participating in hard scattering
  phpythiaheader->SetX1(tpythia6->GetPARI(33));		// Bjorken x1,x2
  phpythiaheader->SetX2(tpythia6->GetPARI(34));
  phpythiaheader->SetSvar(tpythia6->GetPARI(14));	// partonic s,t,u
  phpythiaheader->SetTvar(tpythia6->GetPARI(15));
  phpythiaheader->SetUvar(tpythia6->GetPARI(16));
  phpythiaheader->SetQsqr(tpythia6->GetPARI(22));	// Q squared
  phpythiaheader->SetPt(tpythia6->GetPARI(17));		// transverse momentum

  // primary vertex information. Assume position is at zero by default
  phpythiaheader->SetPrimaryVertexX(0);
  phpythiaheader->SetPrimaryVertexY(0);
  phpythiaheader->SetPrimaryVertexZ(0);

  // rapidity of parton parton com frame
  phpythiaheader->SetYcom(tpythia6->GetPARI(37));	


  if (( eventcount<2 )||( verbosity > 1 ))
  {
    cout << " **************************" << endl; 
    cout << " ****** event = " << eventcount   << endl;
    cout << " **************************" << endl;
    // print out particle listing information if desired (this shows decay modes and code numbers)
    tpythia6->Pylist(1);   // list full pythia generated event
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int PHPythia::CreateNodeTree(PHCompositeNode *topNode)
{
  PHCompositeNode *dstNode;
  PHNodeIterator iter(topNode);
  dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    cout << PHWHERE << "DST Node missing doing nothing" << endl;
    return -1;
  }

  //-* pythia header information
  phpythiaheader = new PHPythiaHeaderV2();
  PHObjectNode_t *PHPythiaHeaderNode = new PHObjectNode_t(phpythiaheader, "PHPythiaHeader", "PHObject");
  dstNode->addNode(PHPythiaHeaderNode);

  //-* pythia particle information
  phpythia = new PHPythiaContainerV2();
  PHObjectNode_t *PHPythiaNode = new PHObjectNode_t(phpythia, "PHPythia", "PHObject");
  dstNode->addNode(PHPythiaNode);

  return 0;
}

int PHPythia::ResetEvent(PHCompositeNode *topNode)
{
  PHNodeIterator mainIter(topNode);
  PHNodeReset reset;
  if (mainIter.cd(Name()))
    {
      mainIter.forEach(reset);
    }
  return 0;
}

