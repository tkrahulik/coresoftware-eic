int
make_piston(string name, PHG4Reco* g4Reco);

int Min_si_layer = 1;
int Max_si_layer = 6;
int Min_cemc_layer = 1;
int Max_cemc_layer = 30;
int Min_hcal_layer = 1;
int Max_hcal_layer = 2;

double Min_forward_eta = 1.22;
int N_forward_sector = 8;

double no_overlapp = 0.0001; // added to radii to avoid overlapping volumes
bool overlapcheck = false; // set to true if you want to check for overlaps
//bool overlapcheck = true; // set to true if you want to check for overlaps

using namespace std;

void
G4Init()
{
  //Jin: Imported from G4Setup_ref_sPHENIX.C v1.1

  // load detector macros and execute Init() function
  gROOT->LoadMacro("G4_Svtx.C");
  SvtxInit();
  gROOT->LoadMacro("G4_CEmc_Spacal.C");
  gROOT->LoadMacro("G4_Hcal_ref.C");
  HCalInit();

  gROOT->LoadMacro("G4_FHcal.C");
  FHCalInit();
  gROOT->LoadMacro("G4_FGEM_fsPHENIX.C");
  FGEM_fsPHENIXInit();
}

//! Fully tuned for fsPHENIX setup with central barrel
//! Jin Huang <jhuang@bnl.gov>

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

  ///////////////////////////////////////////////////////////////////

  PHG4Reco* g4Reco = new PHG4Reco();
  g4Reco->set_rapidity_coverage(1.1); // according to drawings
  g4Reco->SetWorldSizeZ(2000.0); // Jin - larger size (+-10m) to allow for MuID
  g4Reco->SetWorldSizeX(2000.0);
  g4Reco->SetWorldSizeY(2000.0);
  //  g4Reco->set_rapidity_coverage(1.1); // temperary value assigned by ePHENIX LOI
  if (field < 0)
    {
      const char * map =
          "/direct/phenix+hhj/slash/simulation/sPHENIX/macro/sPHENIX_magnetic_field_BaBar_piston.root";

      cout << "G4Setup - Using Cesar's fsPHENIX field map with piston : ";
      cout << map;
      cout << endl;

      g4Reco->set_field_map(map, 2);
    }
  else
    {
      g4Reco->set_field(field); // use const soleniodal field
    }

  /////////////////////////////////////////////////
  //Jin: Imported from G4Setup_ref_sPHENIX.C v1.1
  /////////////////////////////////////////////////

  PHG4CylinderSubsystem *cyl;

  double magnet_inner_radius = 140;
  double magnet_outer_radius = 173;
  double magnet_thickness = magnet_outer_radius - magnet_inner_radius;
  double magnet_length = 373.98;
  double emc_inner_radius = 95.; // emc inner radius from engineering drawing
  double hcal_in_inner_radius = 180.0; // inner hcal inner radius from engineering spreadsheet
  double hcal_out_inner_radius = 210.; // inner hcal inner radius from engineering spreadsheet
  double preshower_inner_radius = 90.;

  int ilayer = -1; // just to declare this varialbe outside of detector setups

  double radius = 0.;
  radius = Svtx(g4Reco, radius, absorberactive);

  if (radius > preshower_inner_radius)
    {
      cout << "inconsistency: svx radius+thickness: " << radius
          << " larger than preshower inner radius: " << preshower_inner_radius
          << endl;
      gSystem->Exit(-1);
    }
  // radius = preshower_inner_radius;
  // radius = PreShower(g4Reco, radius, absorberactive);

  if (radius > emc_inner_radius)
    {
      cout << "inconsistency: preshower radius+thickness: " << radius
          << " larger than emc inner radius: " << emc_inner_radius << endl;
      gSystem->Exit(-1);
    }

  radius = emc_inner_radius;
  int ncross = 8;
  radius = CEmc(g4Reco, radius, ncross, absorberactive);

  // moved into CEmc description
  // cyl = new PHG4CylinderSubsystem("EMCELECTRONICS", 0);
  // cyl->SetRadius(radius);
  // cyl->SetMaterial("G4_TEFLON"); // plastic
  // cyl->SetThickness(0.5);
  // if (absorberactive)
  //   cyl->SetActive();
  // g4Reco->registerSubsystem(cyl);

  //double saveradius = radius + 0.5 + no_overlapp;
  double saveradius = radius + no_overlapp;

  radius = magnet_inner_radius;
  cyl = new PHG4CylinderSubsystem("MAGNET", 0);
  cyl->SetRadius(radius);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(magnet_length);
  cyl->SetMaterial("AL_BABAR_MAG"); // use 1 radiation length Al for magnet thickness
  cyl->SetThickness(magnet_thickness);
  if (absorberactive)
    cyl->SetActive();
  g4Reco->registerSubsystem(cyl);

  radius = magnet_inner_radius + magnet_thickness; // outside of magnet

  ncross = 4;
  radius = HCal(g4Reco, saveradius, ncross, absorberactive);

  // swallow all particles coming out of the backend of sPHENIX
  PHG4CylinderSubsystem *blackhole = new PHG4CylinderSubsystem("BH", 1);
  blackhole->SetRadius(radius + 100); // add 100 cm
  blackhole->SetLengthViaRapidityCoverage(false);
  blackhole->SetLength(g4Reco->GetWorldSizeZ() - no_overlapp); // make it cover the world in length
  blackhole->BlackHole();
  blackhole->SetThickness(0.1); // it needs some thickness
  blackhole->SetActive(); // always see what leaks out
  blackhole->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(blackhole);

  /////////////////////////////////////////////////
  //  Beam Pipe
  /////////////////////////////////////////////////
  PHG4CylinderSubsystem *beamcyl;
  beamcyl = new PHG4CylinderSubsystem("BEAMPIPE", 1);
  beamcyl->SetRadius(2.0);
  beamcyl->SetThickness(0.1);
  beamcyl->SetLengthViaRapidityCoverage(false);
  cout << "WORLD SIZE = " << g4Reco->GetWorldSizeZ() << endl;
  beamcyl->SetLength(g4Reco->GetWorldSizeZ() / 2); //g4Reco->GetWorldSizeZ()); // go through our entire world in z directions
  beamcyl->SetMaterial("G4_Be");
  //   beamcyl->SetPosition(0, 0, 250);
  //  if (absorberactive) beamcyl->SetActive();
  beamcyl->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(beamcyl);

  /////////////////////////////////////////////////
  //  Build hadron going detectors
  /////////////////////////////////////////////////
  double vtxpos[] =
    { 20.0, 25.0, 30.0, 60.0, 65.0, 70.0 };

  make_fvtx("FVTXSTATION1", g4Reco, vtxpos[0], 1.1, 3.0);
  make_fvtx("FVTXSTATION2", g4Reco, vtxpos[1], 1.1, 3.0);
  make_fvtx("FVTXSTATION3", g4Reco, vtxpos[2], 1.1, 3.0);
  make_fvtx("FVTXSTATION4", g4Reco, vtxpos[3], 2.9, 4.2);
  make_fvtx("FVTXSTATION5", g4Reco, vtxpos[4], 2.9, 4.2);
  make_fvtx("FVTXSTATION6", g4Reco, vtxpos[5], 2.9, 4.2);

  // tracker
  G4_FGEM_fsPHENIX(g4Reco, N_forward_sector, Min_forward_eta);

//  // HCal
  G4_FHCal(g4Reco, 350.0, 1.1, 5.0, 100.);

  // implementing the current MuTr provisorially with a GEM
  make_MuTr_station("MUTR", g4Reco, 465, 1.2, 4.1, 1.0);

  make_MuID("MUID", g4Reco, 712, 1.2, 4.1);

  make_piston("magpiston", g4Reco);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);
  se->registerSubsystem(g4Reco);
}

int
make_VTX(PHG4Reco* g4Reco)
{
  // here only the pixel layers are implemented since the strips are out of the forward coverage
  PHG4CylinderSubsystem *cyl;

  float magnet_inner_radius = 140;
  float inner_si_thickness = 0.02;
  float outer_si_thickness = 0.0625;

  // here is our silicon:
  cyl = new PHG4CylinderSubsystem("SVTX", 1);
  cyl->SetRadius(2.5);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(20);
  cyl->SetMaterial("G4_Si");
  cyl->SetThickness(0.02);
  cyl->SetActive();
  cyl->SuperDetector("SVTX");
  g4Reco->registerSubsystem(cyl);
  cyl = new PHG4CylinderSubsystem("SVTXSUPPORT", 1);
  cyl->SetRadius(2.5 + inner_si_thickness);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(20);
  cyl->SetMaterial("G4_Cu");
  cyl->SetThickness(0.022);
  g4Reco->registerSubsystem(cyl);

  cyl = new PHG4CylinderSubsystem("SVTX", 2);
  cyl->SetRadius(5);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(20);
  cyl->SetMaterial("G4_Si");
  cyl->SetThickness(0.02);
  cyl->SetActive();
  cyl->SuperDetector("SVTX");
  g4Reco->registerSubsystem(cyl);
  cyl = new PHG4CylinderSubsystem("SVTXSUPPORT", 2);
  cyl->SetRadius(5 + inner_si_thickness);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(20);
  cyl->SetMaterial("G4_Cu");
  cyl->SetThickness(0.022);
  g4Reco->registerSubsystem(cyl);

  return 0;
}

int
make_piston(string name, PHG4Reco* g4Reco)
{
  const double zpos0 = 120.0;
  const double zpos1 = 350.0;
  const double beampipe_radius = 2.1;
  const double number_of_wteeth = 100;
  const double teeth_thickness = 0.3504 * 2; //2 X0
  const double eta_inner = -log(tan(atan((beampipe_radius + 0.1) / zpos0) / 2));
  const double eta_outter = 4.2;
  const double eta_teeth_outter = 4.05;
  double pos = zpos0 + (zpos1 - zpos0) / 2;
  cout << "MAGNETIC PISTON:" << eta_inner << " " << eta_outter << " " << pos
      << endl;

  PHG4ConeSubsystem *magpiston = new PHG4ConeSubsystem("Piston", 0);
  magpiston->SetZlength((zpos1 - zpos0) / 2);
  magpiston->SetPlaceZ((zpos1 + zpos0) / 2);
  magpiston->SetR1(beampipe_radius,
      tan(PHG4Sector::Sector_Geometry::eta_to_polar_angle(eta_outter)) * zpos0);
  magpiston->SetR2(beampipe_radius,
      tan(PHG4Sector::Sector_Geometry::eta_to_polar_angle(eta_outter)) * zpos1);
  magpiston->SetMaterial("G4_Fe");
  magpiston->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(magpiston);

//  PHG4ConeSubsystem *magpiston = new PHG4ConeSubsystem(name.c_str(), 1);
//  magpiston->SetZlength((zpos1 - zpos0) / 2);
//  magpiston->SetPlaceZ(pos);
//  magpiston->Set_eta_range(eta_outter, eta_inner);
//  magpiston->SetMaterial("G4_Fe");
//  magpiston->SuperDetector(name.c_str());
//  magpiston->SetActive(false);
//  g4Reco->registerSubsystem(magpiston);

  pos = zpos0 + 1.0 + teeth_thickness / 2;
  for (int i = 0; i < number_of_wteeth; i++)
    {
      stringstream s;
      s << name;
      s << "_teeth_";
      s << i;

      magpiston = new PHG4ConeSubsystem(s.str(), i);
      magpiston->SuperDetector(name);
      magpiston->SetZlength(teeth_thickness / 2);
      magpiston->SetPlaceZ(pos);
      magpiston->SetR1(
          //
          tan(PHG4Sector::Sector_Geometry::eta_to_polar_angle(eta_outter - .01))
              * (pos - teeth_thickness / 2), //
          tan(PHG4Sector::Sector_Geometry::eta_to_polar_angle(eta_teeth_outter))
              * (pos - teeth_thickness / 2) //
              );
      magpiston->SetR2(
          //
          tan(PHG4Sector::Sector_Geometry::eta_to_polar_angle(eta_outter - .01))
              * (pos + teeth_thickness / 2), //
          tan(PHG4Sector::Sector_Geometry::eta_to_polar_angle(eta_outter - .01))
              * (pos + teeth_thickness / 2) + .1 //
              );
      magpiston->SetMaterial("G4_W");
      magpiston->SuperDetector(name.c_str());
      magpiston->SetActive(false);
      magpiston->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(magpiston);
      pos += ((320 - zpos0) / number_of_wteeth);
    }
  return 0;
}

// FVTX, according to current specifications
// first and fourth FVTX stations with phi fine segmentation don't exist.
// they should be with a new material budget
int
make_fvtx(string name, PHG4Reco* g4Reco, double zpos, double etamin,
    double etamax)
{
  float si_thickness = 0.02;
  float kapton_thickness = 0.0156;

  PHG4ConeSubsystem *fvtx;
  int layer = 0;
  double pos = zpos + si_thickness / 2;
  fvtx = new PHG4ConeSubsystem(name.c_str(), layer++);
  fvtx->SetZlength(si_thickness / 2);
  fvtx->SetPlaceZ(pos);
  fvtx->Set_eta_range(etamin, etamax);
  fvtx->SetMaterial("G4_Si");
  fvtx->SetActive(true);
  fvtx->OverlapCheck(overlapcheck);
  fvtx->SuperDetector(name.c_str());
  g4Reco->registerSubsystem(fvtx);

  // electronics
  pos += (si_thickness / 2 + kapton_thickness / 2);
  fvtx = new PHG4ConeSubsystem(name.c_str(), layer++);
  fvtx->SetZlength(kapton_thickness / 2);
  fvtx->SetPlaceZ(pos);
  fvtx->Set_eta_range(etamin, etamax);
  fvtx->SetMaterial("G4_KAPTON");
  fvtx->SetActive(false);
  fvtx->OverlapCheck(overlapcheck);
  fvtx->SuperDetector(name.c_str());
  g4Reco->registerSubsystem(fvtx);

  pos += (kapton_thickness / 2 + 0.5); // space for staggering

  fvtx = new PHG4ConeSubsystem(name.c_str(), layer++);
  fvtx->SetZlength(si_thickness / 2);
  fvtx->SetPlaceZ(pos);
  fvtx->Set_eta_range(etamin, etamax);
  fvtx->SetMaterial("G4_Si");
  fvtx->SetActive(true);
  fvtx->OverlapCheck(overlapcheck);
  fvtx->SuperDetector(name.c_str());
  g4Reco->registerSubsystem(fvtx);

  // electronics
  pos += (si_thickness / 2 + kapton_thickness / 2);
  fvtx = new PHG4ConeSubsystem(name.c_str(), layer++);
  fvtx->SetZlength(kapton_thickness / 2);
  fvtx->SetPlaceZ(pos);
  fvtx->Set_eta_range(etamin, etamax);
  fvtx->SetMaterial("G4_KAPTON");
  fvtx->SetActive(false);
  fvtx->OverlapCheck(overlapcheck);
  fvtx->SuperDetector(name.c_str());
  g4Reco->registerSubsystem(fvtx);

  return 0;
}

int
make_MuTr_station(string name, PHG4Reco* g4Reco, double zpos, double etamin,
    double etamax, double gem_chamber_thick)
{
  const double gem_myler_thick = 0.01;
  const double gem_kapton_thick = 0.005;
  const double gem_cu_thick = 0.0005;
  const int n_gem_planes = 3;
  const double gem_plane_spacing = 0.15; //inducing gap
  const double drift_distance = gem_chamber_thick
      - gem_plane_spacing * (n_gem_planes + 1);
  const double drift_velocity = 4; // cm/us http://www-ekp.physik.uni-karlsruhe.de/~ilctpc/Publications/kaminski_rom_paper.pdf 
  const double time_resolution = 0.005; // us  5ns time resolution, 40ns dead time from the VMM2 readout chip
  const int Nsamples = drift_distance / drift_velocity / time_resolution;

  PHG4ConeSubsystem *gem;
  int layer = 0;
  double pos = zpos + gem_myler_thick / 2;
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(gem_myler_thick / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G4_MYLAR");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);

  // this is the active area since the track will produce ionization along the gas
  // volume. The readout will take Nsamples ionization samples
  const double drift_distance_layer = drift_distance / Nsamples;
  pos += (gem_myler_thick / 2 + drift_distance_layer / 2);
  for (int idlayer = 0; idlayer < Nsamples; idlayer++)
    {
      gem = new PHG4ConeSubsystem(name.c_str(), layer++);
      gem->SetZlength(drift_distance_layer / 2);
      gem->SetPlaceZ(pos);
      gem->Set_eta_range(etamin, etamax);
      gem->SetMaterial("G4_METHANE");
      gem->SuperDetector(name.c_str());
      gem->SetActive(true);
      g4Reco->registerSubsystem(gem);
      pos += drift_distance_layer;
    }

  for (int ipl = 0; ipl < n_gem_planes; ipl++)
    {
      pos += gem_cu_thick / 2;
      gem = new PHG4ConeSubsystem(name.c_str(), layer++);
      gem->SetZlength(gem_cu_thick / 2);
      gem->SetPlaceZ(pos);
      gem->Set_eta_range(etamin, etamax);
      gem->SetMaterial("G4_Cu");
      gem->SuperDetector(name.c_str());
      gem->SetActive(false);
      g4Reco->registerSubsystem(gem);

      pos += (gem_cu_thick / 2 + gem_kapton_thick / 2);
      gem = new PHG4ConeSubsystem(name.c_str(), layer++);
      gem->SetZlength(gem_kapton_thick / 2);
      gem->SetPlaceZ(pos);
      gem->Set_eta_range(etamin, etamax);
      gem->SetMaterial("G4_KAPTON");
      gem->SuperDetector(name.c_str());
      gem->SetActive(false);
      g4Reco->registerSubsystem(gem);

      pos += (gem_kapton_thick / 2 + gem_cu_thick / 2);
      gem = new PHG4ConeSubsystem(name.c_str(), layer++);
      gem->SetZlength(gem_cu_thick / 2);
      gem->SetPlaceZ(pos);
      gem->Set_eta_range(etamin, etamax);
      gem->SetMaterial("G4_Cu");
      gem->SuperDetector(name.c_str());
      gem->SetActive(false);
      g4Reco->registerSubsystem(gem);

      pos += gem_cu_thick / 2 + gem_plane_spacing / 2;
      gem = new PHG4ConeSubsystem(name.c_str(), layer++);
      gem->SetZlength(gem_plane_spacing / 2);
      gem->SetPlaceZ(pos);
      gem->Set_eta_range(etamin, etamax);
      gem->SetMaterial("G4_METHANE");
      gem->SuperDetector(name.c_str());
      gem->SetActive(false);
      g4Reco->registerSubsystem(gem);

      pos += gem_plane_spacing / 2;
    }

  // Material budget from HBD [ doi:10.1016/j.nima.2011.04.015 ]
  // PCB
  pos += gem_kapton_thick / 2;
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(gem_kapton_thick / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G4_KAPTON");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);

  pos += (gem_kapton_thick / 2 + gem_cu_thick / 2);
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(gem_cu_thick / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G4_Cu");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);

  // facesheet
  pos += (gem_cu_thick / 2 + 0.025 / 2);
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(0.025 / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G10");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);

  // honeycomb, commented out for now since it has a unsignificant effect
  pos += (0.025 / 2 + 1.905 / 2);
  //  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  //  gem->SetZlength( 1.905/2 );
  //  gem->SetPlaceZ(pos);
  //  gem->Set_eta_range(etamin, etamax);
  //  gem->SetMaterial("??????"); 
  //  gem->SuperDetector(name.c_str());
  //  gem->SetActive(false);
  //  g4Reco->registerSubsystem( gem );

  // facesheet
  pos += (1.905 / 2 + 0.025 / 2);
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(0.025 / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G10");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);

  // readout
  //board 
  pos += (0.025 / 2 + 0.05 / 2);
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(0.05 / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G10");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);

  // readout
  pos += (0.05 / 2 + 0.001 / 2);
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(0.001 / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G4_Cu");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);

  // preamps + sockets
  pos += (0.001 / 2 + gem_cu_thick / 2);
  gem = new PHG4ConeSubsystem(name.c_str(), layer++);
  gem->SetZlength(gem_cu_thick / 2);
  gem->SetPlaceZ(pos);
  gem->Set_eta_range(etamin, etamax);
  gem->SetMaterial("G4_Cu");
  gem->SuperDetector(name.c_str());
  gem->SetActive(false);
  g4Reco->registerSubsystem(gem);
}

int
make_MuID(string name, PHG4Reco* g4Reco, double zpos, double etamin,
    double etamax)
{
  double ironwidth[] =
    { 10, 10, 20, 20, 0, 0, 0 };
  float scintiwidth = 1;
  double pos = zpos + ironwidth[0] / 2;
//  double pos = zpos +scintiwidth/2.;
  bool absorberactive = false;
  PHG4ConeSubsystem* muid = NULL;

  int i = 0;
  // we assume the first layer of MUID is removed.
//  cout <<"make_MuID - MUID readout @ z = "<<pos<<endl;
//  muid = new PHG4ConeSubsystem("MUID", i);
//  muid->SetPlaceZ(pos);
//  muid->SetMaterial("G4_POLYSTYRENE");
//  muid->SetZlength(scintiwidth/2);
//  muid->Set_eta_range(etamin, etamax);
//  muid->SetActive(true);
//        muid->SuperDetector("MUID");
//  muid->OverlapCheck(overlapcheck);
//  g4Reco->registerSubsystem( muid );
//  pos += (scintiwidth/2. + ironwidth[i]/2. + no_overlapp);

  for (int i = 1; i <= 4; i++)
    {
      cout << "make_MuID - MUID absorber @ z = " << pos << ", ["
          << pos - ironwidth[i - 1] / 2 << ", " << pos + ironwidth[i - 1] / 2
          << "]" << endl;
      muid = new PHG4ConeSubsystem("ABSORBER_MUID", i);
      muid->SetPlaceZ(pos);
      muid->SetMaterial("G4_Fe");
      muid->SetZlength(ironwidth[i - 1] / 2);
      muid->Set_eta_range(etamin, etamax);
      if (absorberactive)
        muid->SetActive();
      muid->SuperDetector("ABSORBER_MUID");
      muid->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(muid);
      pos += (ironwidth[i - 1] / 2. + scintiwidth / 2. + no_overlapp);

      cout << "make_MuID - MUID readout @ z = " << pos << ", ["
          << pos - scintiwidth / 2 << ", " << pos + scintiwidth / 2 << "]"
          << endl;
      muid = new PHG4ConeSubsystem("MUID", i);
      muid->SetPlaceZ(pos);
      muid->SetMaterial("G4_POLYSTYRENE");
      muid->SetZlength(scintiwidth / 2);
      muid->Set_eta_range(etamin, etamax);
      muid->SetActive(true);
      muid->SuperDetector("MUID");
      muid->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(muid);
      pos += (scintiwidth / 2. + ironwidth[i] / 2. + no_overlapp);
    }
  return 0;
}
