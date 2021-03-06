int Min_si_layer = 1;
int Max_si_layer = 6;
int Min_cemc_layer = 1;
int Max_cemc_layer = 30;
int Min_hcal_in_layer = 1;
int Max_hcal_in_layer = 1;
int Min_hcal_out_layer = 1;
int Max_hcal_out_layer = 1;

double no_overlapp = 0.0001; // added to radii to avoid overlapping volumes
bool overlapcheck = false; // set to true if you want to check for overlaps

void G4Init(bool do_svtx = true,
	    bool do_preshower = false,
	    bool do_cemc = true,
	    bool do_hcalin = true,
	    bool do_magnet = true,
	    bool do_hcalout = true) {
  
  // load detector macros and execute Init() function
  gROOT->LoadMacro("G4_Svtx.C");           // default
  //gROOT->LoadMacro("G4_Svtx_ladders.C"); // testing
  //gROOT->LoadMacro("G4_Svtx_ITS.C");     // testing
  if (do_svtx) SvtxInit();

  if (do_preshower) {
    gROOT->LoadMacro("G4_PreShower.C");   // testing
    PreShowerInit();
  }

  gROOT->LoadMacro("G4_CEmc_Spacal.C");    // default
  //gROOT->LoadMacro("G4_CEmc_cross.C");   // obselete
  //gROOT->LoadMacro("G4_CEmc_Alice.C");   // obselete
  //gROOT->LoadMacro("G4_CEmc_Alice_W.C"); // obselete
  if (do_cemc) CEmcInit(Cemc_slats_per_cell);

  //gROOT->LoadMacro("G4_Hcal_ref.C");     // deprecated
  //gROOT->LoadMacro("G4_Hcal_cross.C");   // deprecated

  gROOT->LoadMacro("G4_HcalIn_ref.C");       // default 
  if (do_hcalin) HCalInnerInit();

  gROOT->LoadMacro("G4_Magnet.C");
  if (do_magnet) MagnetInit();

  gROOT->LoadMacro("G4_HcalOut_ref.C");       // default 
  if (do_hcalout) HCalOuterInit();
}


int G4Setup(const int absorberactive = 0,
	    const string &field ="1.5",
	    const EDecayType decayType = TPythia6Decayer::kAll,
	    const bool do_svtx = true,
	    const bool do_preshower = false,
	    const bool do_cemc = true,
	    const bool do_hcalin = true,
	    const bool do_magnet = true,
	    const bool do_hcalout = true) {
  
  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();

  PHG4Reco* g4Reco = new PHG4Reco();
  g4Reco->set_rapidity_coverage(1.1); // according to drawings
  if (decayType != TPythia6Decayer::kAll) {
    g4Reco->set_force_decay(decayType);
  }
  
  double fieldstrength;
  istringstream stringline(field);
  stringline >> fieldstrength;
  if (stringline.fail()) { // conversion to double fails -> we have a string

    if (field.find("sPHENIX.root") != string::npos) {
      g4Reco->set_field_map(field, 1);
    } else {
      g4Reco->set_field_map(field, 2);
    }
  } else {
    g4Reco->set_field(fieldstrength); // use const soleniodal field
  }

  double radius = 0.;

  //----------------------------------------
  // SVTX
  if (do_svtx) radius = Svtx(g4Reco, radius, absorberactive);

  //----------------------------------------
  // PRESHOWER
  
  if (do_preshower) radius = PreShower(g4Reco, radius, absorberactive);

  //----------------------------------------
  // CEMC
  
  if (do_cemc) radius = CEmc(g4Reco, radius, 8, 0);
  
  //----------------------------------------
  // HCALIN
  
  if (do_hcalin) radius = HCalInner(g4Reco, radius, 4, absorberactive);

  //----------------------------------------
  // MAGNET
  
  if (do_magnet) radius = Magnet(g4Reco, radius, 0, absorberactive);

  //----------------------------------------
  // HCALOUT
  
  if (do_hcalout) radius = HCalOuter(g4Reco, radius, 4, absorberactive);

  //----------------------------------------
  // BLACKHOLE
  
  // swallow all particles coming out of the backend of sPHENIX
  PHG4CylinderSubsystem *blackhole = new PHG4CylinderSubsystem("BH", 1);
  blackhole->SetRadius(radius + 10); // add 10 cm
  blackhole->SetLengthViaRapidityCoverage(false);
  blackhole->SetLength(g4Reco->GetWorldSizeZ() - no_overlapp); // make it cover the world in length
  blackhole->BlackHole();
  blackhole->SetThickness(0.1); // it needs some thickness
  blackhole->SetActive(); // always see what leaks out
  blackhole->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(blackhole);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);
  se->registerSubsystem( g4Reco );
}

int Get_Min_si_layer()
{
  return Min_si_layer;
}

int Get_Max_si_layer()
{
  return Max_si_layer;
}

int Get_Min_cemc_layer()
{
  return Min_cemc_layer;
}

int Get_Max_cemc_layer()
{
  return Max_cemc_layer;
}

int Get_Min_hcal_layer()
{
  return Min_hcal_layer;
}

int Get_Max_hcal_layer()
{
  return Max_hcal_layer;
}


