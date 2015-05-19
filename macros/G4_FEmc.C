int Min_femc_layer = 0;
int Max_femc_layer = 60;

void FEmcInit()
{
  Min_femc_layer = 0;
  Max_femc_layer = 60;
}

// Forward EMC based on central barrel G4 setup
// zpos is starting position in z
double G4_FEmc(PHG4Reco* g4Reco, double zpos, const double etamin, const double etamax, const double thickness, const string absorbermat = "G4_Pb", const int absorberactive = 0)
{
  double layerwidth = thickness/(Max_femc_layer-Min_femc_layer);
  double absorberwidth = layerwidth*2./3.; // 2/3rd absorber
  float scintiwidth = layerwidth/3.; // 1/3rd scintillator
  double pos = zpos + absorberwidth/2;
  PHG4ConeSubsystem* femc;

  cout << "Abswidth: " << absorberwidth << endl;
  cout << "Scintwidth: " << scintiwidth << endl;

  for (int i = Min_femc_layer; i <  Max_femc_layer; i++)
    {
      cout << "Layer " << i << " : pos " << pos << endl;

      femc = new PHG4ConeSubsystem("ABSORBER_FEMC", i);
      femc->SetPlaceZ(pos);
      femc->SetMaterial( absorbermat );
      femc->SetZlength(absorberwidth/2);
      femc->Set_eta_range(etamin, etamax);
      if (absorberactive)  femc->SetActive();
      femc->SuperDetector("ABSORBER_FEMC");
      femc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem( femc );
      pos += (absorberwidth/2. + scintiwidth/2. + no_overlapp);

      femc = new PHG4ConeSubsystem("FEMC", i);
      femc->SetPlaceZ(pos);
      femc->SetMaterial("G4_POLYSTYRENE");
      femc->SetZlength(scintiwidth/2);
      femc->Set_eta_range(etamin, etamax);
      femc->SetActive(true);
      femc->SuperDetector("FEMC");
      femc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem( femc );
      pos += (scintiwidth/2. + absorberwidth/2. + no_overlapp);
    }
  pos -= absorberwidth/2.;
  return 0;
}
