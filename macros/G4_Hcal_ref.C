// these are defined in the Fun4All macro, here we just override the values
// with what is used in this macro
Min_hcal_in_layer = 1;
Max_hcal_in_layer = 1;
Min_hcal_out_layer = 0;
Max_hcal_out_layer = 0;

void HCalInit()
{
  Min_hcal_in_layer = 1;
  Max_hcal_in_layer = 1;
  Min_hcal_out_layer = 1;
  Max_hcal_out_layer = 1;
}

double HCal(PHG4Reco* g4Reco,
	    double radius,
	    const int crossings,
	    const int absorberactive = 0,
	    int verbosity = 0) {
  
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  HCalInner( g4Reco, radius, crossings, absorberactive);
  HCalInner_SupportRing( g4Reco,  absorberactive);

  double outer_hcal_outer_radius = 260.3;

  int ilayer = Min_hcal_out_layer;
  PHG4OuterHcalSubsystem *outerhcal = new PHG4OuterHcalSubsystem("HCALOUT");
  outerhcal->SetActive();
  //outerhcal->SetLightCorrection(178.0,0.65,260.3,1.0);
  outerhcal->SuperDetector("HCALOUT");
  if (absorberactive)  outerhcal->SetAbsorberActive();
  outerhcal->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem( outerhcal );

  if (ilayer > Max_hcal_in_layer)
    {
      cout << "layer discrepancy, current layer " << ilayer
           << " max hcal layer: " << Max_hcal_in_layer << endl;
    }

  radius = outer_hcal_outer_radius;
  radius += no_overlapp;  // if inner radius equal outer radius we have overlapps
  
  if (verbosity >= 0) {
    cout << "========================= G4_Hcal_ref.C::HCal() ===========================" << endl;
    cout << " CVS Version: $Id: G4_Hcal_ref.C,v 1.5 2015/04/03 20:22:19 mccumber Exp $" << endl;
    cout << " HCALOUT Material Description:" << endl;
    cout << "  outer radius = " << outer_hcal_outer_radius << " cm" << endl;
    cout << "===========================================================================" << endl;
  }
  
  return radius; 
}

double HCalInner(PHG4Reco* g4Reco,
		 double radius,
		 const int crossings,
		 const int absorberactive = 0,
		 int verbosity = 0) {
  
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  double innerradius = 116.;
  double outerradius = 135.;
  double scintw = 0.7;
  int    nscint = 64*5;
  
  double hcal_inner_thickness = outerradius - innerradius;
  int ilayer = Min_hcal_in_layer;
  PHG4HcalSubsystem *hcal;
  hcal = new PHG4HcalSubsystem("HCALIN", ilayer);
  hcal->SetRadius(innerradius);
  hcal->SetMaterial("G4_Cu");
  hcal->SetThickness(hcal_inner_thickness);
  hcal->SetScintWidth(scintw);
  hcal->SetNumScint(nscint);
  hcal->SetTiltViaNcross(+crossings); // Jin: correct for the sign for inner HCal tilting per request from Chris
  hcal->SetActive();
  hcal->SuperDetector("HCALIN");
  if (absorberactive)  hcal->SetAbsorberActive();
  hcal->OverlapCheck(overlapcheck);
  //hcal->SetLightCorrection(116.0,0.85,135.0,1.0);
  
  g4Reco->registerSubsystem( hcal );

  radius = outerradius;
  radius += no_overlapp;  // if inner radius equal outer radius we have overlapps  return radius;

  if (verbosity >= 0) {
    cout << "===================== G4_Hcal_ref.C::HCalInner() ==========================" << endl;
    cout << " CVS Version: $Id: G4_Hcal_ref.C,v 1.5 2015/04/03 20:22:19 mccumber Exp $" << endl;
    cout << " HCALIN Material Description:" << endl;
    cout << "  inner radius = " << innerradius << " cm" << endl;
    cout << "  outer radius = " << outerradius << " cm" << endl;
    cout << "  number of scintilators = " << nscint << endl;
    cout << "  width of scintilators = " << scintw << " cm" << endl;
    cout << "  number of crossings = " << crossings << endl;
    cout << "===========================================================================" << endl;
  }
  
  return radius;
}

//! A rough version of the inner HCal support ring, from Richie's CAD drawing. - Jin
void
HCalInner_SupportRing(PHG4Reco* g4Reco,
    const int absorberactive = 0)
{
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  const double z_ring1 = (2025 + 2050) / 2. / 10.;
  const double z_ring2 = (2150 + 2175) / 2. / 10.;
  const double dz = 25. / 10.;
  const double innerradius = 116.;
  const double maxradius = 178.0 - 0.001; // avoid touching the outer HCal envelop volumne

  const double z_rings[] =
    { -z_ring2, -z_ring1, z_ring1, z_ring2, 0, 0, 0, 0 };

  PHG4CylinderSubsystem *cyl;

  for (int i = 0; i < 4; i++)
    {
      cyl = new PHG4CylinderSubsystem("HCALIN_SPT_N1", 0);
      cyl->SetPosition(0, 0, z_rings[i]);
      cyl->SuperDetector("HCALIN_SPT");
      cyl->SetRadius(innerradius);
      cyl->SetLengthViaRapidityCoverage(false);
      cyl->SetLength(dz);
      cyl->SetMaterial("G4_Fe"); // use 1 radiation length Al for magnet thickness
      cyl->SetThickness(maxradius - innerradius);
      if (absorberactive)
        cyl->SetActive();
      g4Reco->registerSubsystem(cyl);
    }

}


void HCALInner_Cells(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();

  PHG4SlatCellReco *hcal_slats = new PHG4SlatCellReco("HCALSLATCELLRECO");
  hcal_slats->Detector("HCALIN");
  hcal_slats->Verbosity(verbosity);
  hcal_slats->etasize_nslat(Min_hcal_in_layer, 0.1, 6);
  se->registerSubsystem(hcal_slats);
  
  return;  
}

void HCALOuter_Cells(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  PHG4HcalCellReco *hc = new PHG4HcalCellReco();
  hc->Detector("HCALOUT");
  hc->etasize_nslat(0, 0, 5);
  se->registerSubsystem(hc);

  return;
}

void HCALInner_Towers(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  RawTowerBuilder *TowerBuilder = new RawTowerBuilder("HcalInRawTowerBuilder");
  TowerBuilder->Detector("HCALIN");
  TowerBuilder->Verbosity(verbosity);
  se->registerSubsystem( TowerBuilder );
      
  return;
}

void HCALOuter_Towers(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  RawTowerBuilder* TowerBuilder = new RawTowerBuilder("HcalOutRawTowerBuilder");
  TowerBuilder->Detector("HCALOUT");
  TowerBuilder->Verbosity(verbosity);
  se->registerSubsystem( TowerBuilder );
  
  return;
}

void HCALInner_Clusters(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  RawClusterBuilder* ClusterBuilder = new RawClusterBuilder("HcalInRawClusterBuilder");
  ClusterBuilder->Detector("HCALIN");
  ClusterBuilder->Verbosity(verbosity);
  se->registerSubsystem( ClusterBuilder );
  
  return;
}

void HCALOuter_Clusters(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  RawClusterBuilder* ClusterBuilder = new RawClusterBuilder("HcalOutRawClusterBuilder");
  ClusterBuilder->Detector("HCALOUT");
  ClusterBuilder->Verbosity(verbosity);
  se->registerSubsystem( ClusterBuilder );
  
  return;
}

void HCALInner_Eval(std::string outputfile, int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4eval.so");
  Fun4AllServer *se = Fun4AllServer::instance();
    
  PHG4CalEvaluator* eval = new PHG4CalEvaluator("PHG4HCALINEVALUATOR", outputfile.c_str());
  eval->Verbosity(verbosity);
  eval->Detector("HCALIN");
  se->registerSubsystem( eval );
      
  return;
}

void HCALOuter_Eval(std::string outputfile, int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4eval.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  PHG4CalEvaluator* eval = new PHG4CalEvaluator("PHG4HCALOUTEVALUATOR", outputfile.c_str());
  eval->Verbosity(verbosity);
  eval->Detector("HCALOUT");
  se->registerSubsystem( eval );
  
  return;
}
