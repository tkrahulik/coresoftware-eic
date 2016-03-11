#ifndef __PHPYTHIA_H__
#define __PHPYTHIA_H__

#include <fstream>
#include <string>
#include <fun4all/SubsysReco.h>
#include <Rtypes.h>

class PHCompositeNode;
class TPythia6;
class PHPythiaHeader;
class PHPythiaContainer;
class TTree;
class TFile;

class PHPythia: public SubsysReco
{
public:
  
  //! constructor
  PHPythia(const std::string &name = "PHPythia");
  
  //! destructor
  virtual ~PHPythia();

  //! configuration file
  void SetConfigFile( const char* cfg_file )
  { if( cfg_file ) _configFile = cfg_file; }
  
  //! Read Config File
  /*! if argument is 0 current _configFile is used. It is overwritten otherwise */
  int ReadConfig(const char *cfg_file = 0);

  //! Print Config File
  void PrintConfig() const;

  //! set xsec filename
  void SetXsecFile( const char* xsec_fname )
  { if( xsec_fname ) _xsecFile = xsec_fname; }
  
  //! Set Seed of random number generator
  void SetSeed(const int s) { fSeed = s; }

  //! Methods Derived from SubsysReco
  int Init(PHCompositeNode *topNode);
  
  //int InitRun(PHCompositeNode *topNode);
  
  //! event method
  int process_event(PHCompositeNode *topNode);
  
  //! event reset
  int ResetEvent(PHCompositeNode *topNode);
  
  //! end of job
  int End(PHCompositeNode *topNode);

protected:
  
  //! node tree
  int CreateNodeTree(PHCompositeNode *topNode);
  
  //! event
  int eventcount;
  
  //! configuration file. Default is "phpythia.cfg
  std::string _configFile;
  
  //! xsec file. Default is "phpy_xsec.root
  std::string _xsecFile;
  
  //! pythia interface
  TPythia6 *tpythia6;
  
  //! pythia header output node
  PHPythiaHeader *phpythiaheader;
  
  //! pythia container output node
  PHPythiaContainer *phpythia;

  //! seed to random number generator
  long int fSeed;		
  
  //! Tree for normalization
  TFile *xsecfile;
  TTree *Tp;
  UInt_t   tp_isub;
  Char_t  *tp_proc;
  UInt_t   tp_nevt;
  Double_t tp_sigma;
  Double_t tp_nevt_sigma;
  Double_t tp_integlumi;

};

#endif	/* __PHPYTHIA_H__ */

