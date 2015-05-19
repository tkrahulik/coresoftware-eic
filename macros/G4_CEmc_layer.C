// these are defined in the Fun4All macro, here we just override the values
// with what is used in this macro
Min_cemc_layer = 1;
Max_cemc_layer = 30;

void CEmcInit()
{
  Min_cemc_layer = 1;
  Max_cemc_layer = 30;
}

double
CEmc(PHG4Reco* g4Reco, double radius, const int crossings, const int absorberactive = 0)
{

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  double scintiwidth = 0.1;
  double tungstenwidth = 0.2;

  PHG4CylinderSubsystem *cemc;

  for (int ilayer = Min_cemc_layer; ilayer <= Max_cemc_layer; ilayer++)
    {

      cemc = new PHG4CylinderSubsystem("ABSORBER_CEMC", ilayer);
      cemc->SetRadius(radius);
      cemc->SetMaterial("G4_W");
      cemc->SetThickness(tungstenwidth);
      cemc->SuperDetector("ABSORBER_CEMC");
      if (absorberactive)  cemc->SetActive();
      cemc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem( cemc );
      radius += tungstenwidth;
      radius += no_overlapp;

      cemc =  new PHG4CylinderSubsystem("CEMC", ilayer);
      cemc->SetRadius(radius);
      cemc->SetMaterial("Scintillator");
      cemc->SetThickness(scintiwidth);
      cemc->SetActive();
      cemc->SuperDetector("CEMC");
      cemc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem( cemc );

      radius += scintiwidth;
      radius += no_overlapp;
    }
 
  PHG4CylinderSubsystem *cyl = new PHG4CylinderSubsystem("EMCELECTRONICS", 0);
  cyl->SetRadius(radius);
  cyl->SetMaterial("G4_TEFLON"); // plastic
  cyl->SetThickness(0.5);
  if (absorberactive)  cyl->SetActive();
  g4Reco->registerSubsystem( cyl );
  radius += 0.5;
  radius += no_overlapp;

  return radius;
}

void CEMC_Cells(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  PHG4CylinderCellReco *cemc_cells = new PHG4CylinderCellReco("CEMCCYLCELLRECO");
  cemc_cells->Detector("CEMC");
  cemc_cells->Verbosity(verbosity);
  for (int i = Min_cemc_layer; i <= Max_cemc_layer; i++) {
    cemc_cells->etaphisize(i, 0.024, 0.024);
  }
  se->registerSubsystem(cemc_cells);

  return;
}

void CEMC_Towers(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  RawTowerBuilder *TowerBuilder = new RawTowerBuilder("EmcRawTowerBuilder");
  TowerBuilder->Detector("CEMC");
  TowerBuilder->Verbosity(verbosity);
  se->registerSubsystem( TowerBuilder );
      
  return;
}

void CEMC_Clusters(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  
  RawClusterBuilder* ClusterBuilder = new RawClusterBuilder("EmcRawClusterBuilder");
  ClusterBuilder->Detector("CEMC");
  ClusterBuilder->Verbosity(verbosity);
  se->registerSubsystem( ClusterBuilder );
  
  return;
}

void CEMC_Eval(std::string outputfile, int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4eval.so");
  Fun4AllServer *se = Fun4AllServer::instance();
    
  PHG4CalEvaluator* eval = new PHG4CalEvaluator("PHG4CEMCEVALUATOR", outputfile.c_str());
  eval->Verbosity(0);
  eval->Detector("CEMC");
  se->registerSubsystem( eval );
      
  return;
}
