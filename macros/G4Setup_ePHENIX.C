int
make_preshower(string name, PHG4Reco* g4Reco, double zpos, double etamin,
    double etamax);

int Min_si_layer = 1;
int Max_si_layer = 6;
int Min_cemc_layer = 1;
int Max_cemc_layer = 30;
int Min_hcal_layer = 1;
int Max_hcal_layer = 2;

double Min_forward_eta = 1.22;
double Min_forward_eta_RICH = Min_forward_eta+0.5;
int N_forward_sector = 8;

double no_overlapp = 0.0001; // added to radii to avoid overlapping volumes
bool overlapcheck = false; // set to true if you want to check for overlaps
//bool overlapcheck = true; // set to true if you want to check for overlaps

using namespace std;

void
G4Init()
{
  // load detector macros
  gROOT->LoadMacro("G4_Tpc.C");
  TpcInit();
  gROOT->LoadMacro("G4_EEmc.C");
  EEmcInit();
  gROOT->LoadMacro("G4_FEmc.C");
  FEmcInit();
  gROOT->LoadMacro("G4_FHcal.C");
  FHCalInit();
  gROOT->LoadMacro("G4_RICH.C");
  RICHInit();
  gROOT->LoadMacro("G4_DIRC.C");
  DIRCInit();
  gROOT->LoadMacro("G4_FGEM_ePHENIX.C");
  FGEM_ePHENIXInit();
  gROOT->LoadMacro("G4_AeroGel.C");
  AeroGelInit();
  //  gROOT->LoadMacro("G4_PreShower.C");
  // PreShowerInit();
  // The *_cross.C macros load our current real design but G4's drawing
  // barfs. For the drawing load our old layered geometries in the *_layer.C
  // macros. They implement the same function, so further changes in
  // this macro are not needed
//    gROOT->LoadMacro("G4_CEmc_cross.C");
//    gROOT->LoadMacro("G4_Hcal_cross.C");
//  gROOT->LoadMacro("G4_CEmc_layer.C");
  gROOT->LoadMacro("G4_CEmc_Spacal.C");
  CEmcInit();
//  gROOT->LoadMacro("G4_Hcal_layer.C");
//  gROOT->LoadMacro("G4_Hcal_cross.C");
  gROOT->LoadMacro("G4_Hcal_ref.C");
  HCalInit();
  gROOT->LoadMacro("G4_Svtx.C");
  SvtxInit();
}

//! Jin Huang <jhuang@bnl.gov> : Fully tuned for ePHENIX LOI setup
void
G4Setup(const int absorberactive = 0, const float field = -1.)
{
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
  g4Reco->set_rapidity_coverage(1.1); // temperary value assigned by ePHENIX LOI
  if (field < 0)
    {
      const char * map =
          "/direct/phenix+WWW/p/draft/jinhuang/sPHENIX/ePHENIX/BABAR_V11_GridOut_ePHENIX.SF7.root";

      cout << "G4Setup - Using ePHENIX LOI field map ";
      cout << map;
      cout << endl;

      g4Reco->set_field_map(map, 2);
    }
  else
    {
      g4Reco->set_field(field); // use const soleniodal field 
    }

  /////////////////////////////////////////////////
  //  Beam Pipe
  /////////////////////////////////////////////////
  PHG4CylinderSubsystem *beamcyl;
  beamcyl = new PHG4CylinderSubsystem("BEAMPIPE", 1);
  beamcyl->SetRadius(2.0);
  beamcyl->SetThickness(0.1);
  beamcyl->SetLengthViaRapidityCoverage(false);
  beamcyl->SetLength(g4Reco->GetWorldSizeZ() - 10.); //g4Reco->GetWorldSizeZ()); // go through our entire world in z directions
  beamcyl->SetMaterial("G4_Be");
  //   beamcyl->SetPosition(0, 0, 250);
  //  if (absorberactive) beamcyl->SetActive();
  beamcyl->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(beamcyl);

  /////////////////////////////////////////////////
  // Central magnet and detectors
  /////////////////////////////////////////////////

  PHG4CylinderSubsystem *cyl;

  double magnet_inner_radius = 140;
  double magnet_outer_radius = 173;
  double magnet_thickness = magnet_outer_radius - magnet_inner_radius;
  double emc_inner_radius = 95.; // emc inner radius from engineering spreadsheet
  double hcal_in_inner_radius = magnet_outer_radius + 15.; // inner hcal inner radius from engineering spreadsheet
  double hcal_out_inner_radius = hcal_in_inner_radius + 30.; // inner hcal inner radius from engineering spreadsheet
  double preshower_inner_radius = 90.;
  double tpc_inner_radius = 15.;
  double tpc_outer_radius = 80.;
  double tpc_thickness = tpc_outer_radius - tpc_inner_radius;

  int ilayer = -1; // just to declare this varialbe outside of detector setups

  double radius = tpc_inner_radius;
  radius = Tpc(g4Reco, radius, tpc_thickness, absorberactive);
//
  radius = G4_DIRC(g4Reco);

  if (radius > emc_inner_radius - 4.)
    {
      cout << "inconsistency: " << radius << " larger than emc inner radius: "
          << emc_inner_radius - 4. << endl;
      gSystem->Exit(-1);
    }

  // inside CEmc now...
  //radius = emc_inner_radius - 4.;
  //cyl = new PHG4CylinderSubsystem("EMCELECTRONICS", 0);
  //cyl->SetRadius(radius);
  //cyl->SetMaterial("G4_TEFLON"); // plastic
  //cyl->SetThickness(0.5);
  //if (absorberactive)
  //  cyl->SetActive();
  //cyl->OverlapCheck(overlapcheck);
  //g4Reco->registerSubsystem(cyl);

  radius = emc_inner_radius;
  int ncross = 4;
  radius = CEmc(g4Reco, radius, ncross, absorberactive);
  double saveradius = radius + no_overlapp;

//   double al_radlen =  8.897;
//   radius = magnet_inner_radius + magnet_thickness/2. - al_radlen/2.;
  // magnet (1 radlen Al = 8.897cm - pdg)
  // located half way between inner and outer radius
  radius = magnet_inner_radius;
  cyl = new PHG4CylinderSubsystem("MAGNET", 0);
  cyl->SetRadius(radius);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(385.0);
  cyl->SetMaterial("AL_BABAR_MAG"); // use 1 radiation length Al for magnet thickness
  cyl->SetThickness(magnet_thickness);
  cyl->OverlapCheck(overlapcheck);
  if (absorberactive)
    cyl->SetActive();
  g4Reco->registerSubsystem(cyl);

//  radius = magnet_inner_radius + magnet_thickness; // outside of magnet
//
//  radius += 10;
//  if (radius > hcal_in_inner_radius)
//    {
//      cout << "inconsistency: magnet radius+thickness: " << radius
//          << " larger than emc inner radius: " << hcal_in_inner_radius << endl;
//      gSystem->Exit(-1);
//    }
//  radius = hcal_in_inner_radius;
//  ncross = 3;
//  radius = HCal(g4Reco, radius, ncross, absorberactive);
  //radius += 60;

  cout <<"G4Setup - loading updated HCal setup"<<endl;
  ncross = 4;
  radius = HCal(g4Reco, saveradius, ncross, absorberactive);

  /////////////////////////////////////////////////
  //  Build hadron going detectors
  /////////////////////////////////////////////////

  // tracker
  G4_FGEM_ePHENIX(g4Reco, N_forward_sector, Min_forward_eta);

// RICH
  G4_RICH(g4Reco, N_forward_sector, Min_forward_eta_RICH);

  //AeroGel
  G4_AeroGel(g4Reco, N_forward_sector, Min_forward_eta);

  // double newzpos = G4_FEmc(g4Reco, 315.0, Min_forward_eta, 4.0, 10., "G4_W"); // 10cm thick W/Sci calorimeter = 20 X0
  double newzpos = G4_FEmc(g4Reco, 315.0, Min_forward_eta, 4.0, 17., "G4_Pb"); // 17cm thick Pb/Sci calorimeter = 20 X0

  if (newzpos > 350)
    {
      cout << "overlapp with hcal" << endl;
    }
  // HCal
  G4_FHCal(g4Reco, 350.0, Min_forward_eta, 5.0, 100.);

  PHG4ConeSubsystem *bbc = new PHG4ConeSubsystem("BBC", 0);
  bbc->SetZlength(2);
  bbc->SetPlaceZ(325);
  bbc->Set_eta_range(4, 5);
  bbc->SetMaterial("G4_Pyrex_Glass");
  bbc->SetActive();
  bbc->SuperDetector("BBC");
  bbc->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(bbc);

  /////////////////////////////////////////////////
  //  electron going detectors
  /////////////////////////////////////////////////
  double newzpos = G4_EEmc(g4Reco, -99., -1.2, -4.5, 18. ); // 18cm thick PbWO4 crystal calorimeter

  G4_eGEM_ePHENIX(g4Reco);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);
  se->registerSubsystem(g4Reco);
}

int
Get_Min_si_layer()
{
  return Min_si_layer;
}

int
Get_Max_si_layer()
{
  return Max_si_layer;
}

int
Get_Min_cemc_layer()
{
  return Min_cemc_layer;
}

int
Get_Max_cemc_layer()
{
  return Max_cemc_layer;
}

int
Get_Min_hcal_layer()
{
  return Min_hcal_layer;
}

int
Get_Max_hcal_layer()
{
  return Max_hcal_layer;
}

// pre-shower based on central barrel setup
int
make_preshower(string name, PHG4Reco* g4Reco, double zpos, double etamin,
    double etamax)
{
  float siwidth = 0.03;
  float airwidth = 0.17;
  float tungstenwidth = 0.2;
  float pos = zpos + siwidth / 2;
  PHG4ConeSubsystem* prsh;
  for (int i = 1; i < 9; i++)
    {
      prsh = new PHG4ConeSubsystem(Form("%s_SILICON", name.c_str()), i);
      prsh->SetPlaceZ(pos);
      prsh->SetMaterial("G4_Si");
      prsh->SetZlength(siwidth / 2);
      prsh->Set_eta_range(etamin, etamax);
      prsh->SetActive(true);
      g4Reco->registerSubsystem(prsh);
      pos += siwidth / 2 + airwidth / 2;

      prsh = new PHG4ConeSubsystem(Form("%s_AIR", name.c_str()), i);
      prsh->SetPlaceZ(pos);
      prsh->SetMaterial("G4_AIR");
      prsh->SetZlength(airwidth / 2);
      prsh->Set_eta_range(etamin, etamax);
      prsh->SetActive(0);
      g4Reco->registerSubsystem(prsh);
      pos += airwidth / 2 + tungstenwidth / 2;

      prsh = new PHG4ConeSubsystem(Form("%s_TUNGSTEN", name.c_str()), i);
      prsh->SetPlaceZ(pos);
      prsh->SetMaterial("G4_W");
      prsh->SetZlength(tungstenwidth / 2);
      prsh->Set_eta_range(etamin, etamax);
      prsh->SetActive(0);
      g4Reco->registerSubsystem(prsh);
      pos += tungstenwidth / 2 + siwidth / 2;
    }
  return 0;
}

// current central arm EMCAl
int
make_PbGl(PHG4Reco* g4Reco)
{
  double Emcleadthickness = 0.15; // cm
  double Emcscintithickness = 0.4; // cm
  double sizexy = 126 * 5.535;
  double zpos1 = 300.0; // - 33 * (0.4 + 0.15);
  PHG4BlockSubsystem* blk;
  for (int i = 0; i < 66; i++)
    {
      blk = new PHG4BlockSubsystem("PbAbsorber", i);
      blk->SetSize(6 * 12 * 5.535, 2 * 6 * 12 * 5.535, Emcleadthickness);
      blk->SetPlaceZ(zpos1 + Emcleadthickness / 2.);
      blk->SetMaterial("G4_Pb");
      if (overlapcheck)
        blk->OverlapCheck();
      if (absorberactive)
        {
          blk->SetActive();
          blk->SuperDetector("PbAbsorber");
        }
      g4Reco->registerSubsystem(blk);
      zpos1 += Emcleadthickness + no_overlap;
      blk = new PHG4BlockSubsystem("FEMC", i);
      blk->SetSize(6 * 12 * 5.535, 2 * 6 * 12 * 5.535, Emcscintithickness);
      blk->SetPlaceZ(zpos1 + Emcscintithickness / 2.);
      blk->SetActive();
      blk->SuperDetector("FEMC");
      blk->SetMaterial("G4_POLYSTYRENE");
      if (overlapcheck)
        blk->OverlapCheck();
      g4Reco->registerSubsystem(blk);
      zpos1 += Emcscintithickness + no_overlap;
    }
  return 0;
}

int
make_He_bag(string name, PHG4Reco* g4Reco, double zpos1, double zpos2,
    double etamin, double etamax)
{

  PHG4ConeSubsystem *gem;
  gem = new PHG4ConeSubsystem(name.c_str(), 0);
  gem->SetZlength(abs(zpos1 - zpos2) / 2);
  gem->SetPlaceZ((zpos1 + zpos2) / 2);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G4_He");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);
}

void
G4Setup_Sandbox(const int absorberactive = 0, const float field = -1.)
{
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

}


