
void FHCalInit()
{
}

// Forward HCAL dedicated model
double G4_FHCal(PHG4Reco* g4Reco, const int absorberactive = 0, const char * name = "FHCAL")
{
  PHG4ForwardHcalSubsystem *hhcal = new PHG4ForwardHcalSubsystem(name);

  // automatically generate path to mapping file
  ostringstream mapping_hhcal;
  mapping_hhcal << getenv("OFFLINE_MAIN") <<
    "/share/calibrations/ForwardHcal/mapping/towerMap_FHCAL_v001.txt";
  cout << mapping_hhcal.str() << endl;

  // configure detector
  hhcal->SetTowerMappingFile( mapping_hhcal.str() );
  hhcal->OverlapCheck(overlapcheck);

  // register detector
  g4Reco->registerSubsystem( hhcal );
}
