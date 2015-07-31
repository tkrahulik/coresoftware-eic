
void FHCalInit()
{
}

// Forward HCAL dedicated model
double G4_FHCal(PHG4Reco* g4Reco, const int absorberactive = 0, const char * name = "FHCAL")
{
  PHG4ForwardHcalSubsystem *hhcal = new PHG4ForwardHcalSubsystem(name);
  hhcal->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem( hhcal );
}
