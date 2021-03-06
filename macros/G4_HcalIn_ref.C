// these are defined in the Fun4All macro, here we just override the values
// with what is used in this macro
Min_hcal_in_layer = 1;
Max_hcal_in_layer = 1;

void HCalInnerInit() {
  Min_hcal_in_layer = 1;
  Max_hcal_in_layer = 1;
}

double HCalInner(PHG4Reco* g4Reco,
		 double radius,
		 const int crossings,
		 const int absorberactive = 0,
		 int verbosity = 0) {
  
  double innerradius = 116.;
  double outerradius = 135.;
  double scintw = 0.7;
  int    nscint = 64*5;
  
  if (radius > innerradius) {
    cout << "inconsistency: radius: " << radius 
	 << " larger than HCALIN inner radius: " << innerradius << endl;
    gSystem->Exit(-1);
  }

  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");
  
  double hcal_inner_thickness = outerradius - innerradius;
  int ilayer = Min_hcal_in_layer;
  PHG4HcalSubsystem *hcal;
  hcal = new PHG4HcalSubsystem("HCALIN", ilayer);
  hcal->SetRadius(innerradius);
  hcal->SetMaterial("SS304"); // SS304 stainless steel
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

  HCalInner_SupportRing(g4Reco,absorberactive);
  
  if (verbosity >= 0) {
    cout << "==================== G4_HcalIn_ref.C::HCalInner() =========================" << endl;
    cout << " CVS Version: $Id: G4_HcalIn_ref.C,v 1.2 2015/04/21 21:21:14 pinkenbu Exp $" << endl;
    cout << " HCALIN Material Description:" << endl;
    cout << "  inner radius = " << innerradius << " cm" << endl;
    cout << "  outer radius = " << outerradius << " cm" << endl;
    cout << "  number of scintilators = " << nscint << endl;
    cout << "  width of scintilators = " << scintw << " cm" << endl;
    cout << "  number of crossings = " << crossings << endl;
    cout << "===========================================================================" << endl;
  }

  radius += no_overlapp;
  return radius;
}

//! A rough version of the inner HCal support ring, from Richie's CAD drawing. - Jin
void HCalInner_SupportRing(PHG4Reco* g4Reco,
			   const int absorberactive = 0) {
  
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
  
  return;
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
