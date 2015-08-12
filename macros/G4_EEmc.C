void EEmcInit()
{
}

double G4_EEmc(PHG4Reco* g4Reco)
{
  PHG4CrystalCalorimeterSubsystem *eecal = new PHG4CrystalCalorimeterSubsystem("EEMC");

  /* Use non-projective geometry */
  ostringstream mapping_eecal;
  //mapping_eecal << "calibrations/CrystalCalorimeter/mapping/towerMap_EEMC_v001.txt";
  mapping_eecal << getenv("OFFLINE_MAIN") <<
    "/share/calibrations/CrystalCalorimeter/mapping/towerMap_EEMC_v001.txt";
  cout << mapping_eecal.str() << endl;
  eecal->SetTowerMappingFile( mapping_eecal.str() );

  /* register Ecal module */
  eecal->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem( eecal );

  return 0;
}
