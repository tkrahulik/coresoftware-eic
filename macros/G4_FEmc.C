
void FEmcInit()
{
}

// Forward ECAL dedicated model
double G4_FEmc(PHG4Reco* g4Reco, const int absorberactive = 0, const char * name = "FEMC")
{
  PHG4ForwardEcalSubsystem *hemc = new PHG4ForwardEcalSubsystem(name);

  ostringstream mapping_hemc;
  //  mapping_hemc << "calibrations/ForwardEcal/mapping/towerMap_FEMC_v001.txt";

  // automatically generate path to mapping file
  mapping_hemc << getenv("OFFLINE_MAIN") <<
    "/share/calibrations/ForwardEcal/mapping/towerMap_FEMC_v001.txt";
  cout << mapping_hemc.str() << endl;

  // configure detector
  hemc->SetTowerMappingFile( mapping_hemc.str() );
  hemc->OverlapCheck(overlapcheck);

  // register detector
  g4Reco->registerSubsystem( hemc );

  return 0;
}
