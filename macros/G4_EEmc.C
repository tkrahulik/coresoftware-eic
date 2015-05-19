int Min_eemc_layer = 0;
int Max_eemc_layer = 60;

void EEmcInit()
{
  Min_eemc_layer = 0;
  Max_eemc_layer = 60;
}

// Forward EMC crystal calorimeter
// longitudinal segmentation just for creatinn of 'longitudinal shower profiles'- no 'physical' barrier!
// zpos is starting position in z
double G4_EEmc(PHG4Reco* g4Reco, double zpos, const double etamin, const double etamax, const double thickness)
{
  double layerwidth = thickness/(Max_eemc_layer-Min_eemc_layer);
  double pos = zpos - layerwidth/2;
  PHG4ConeSubsystem* eemc;

  for (int i = Min_eemc_layer; i <  Max_eemc_layer; i++)
    {
      eemc = new PHG4ConeSubsystem("EEMC", i);
      eemc->SetPlaceZ(pos);
      eemc->SetMaterial("G4_PbWO4");
      eemc->SetZlength(layerwidth/2);
      eemc->Set_eta_range(etamin, etamax);
      eemc->SetActive(true);
      eemc->SuperDetector("EEMC");
      g4Reco->registerSubsystem( eemc );
      pos -= (layerwidth);
    }

  return 0;
}
