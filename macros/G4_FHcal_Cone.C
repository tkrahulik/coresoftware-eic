int Min_fhcal_layer = 0;
int Max_fhcal_layer = 30;

#include <string>

void FHCalInit()
{
  Min_fhcal_layer = 0;
  Max_fhcal_layer = 30;
}

// Forward HCAL dedicated model
double G4_FHCal(PHG4Reco* g4Reco, const int absorberactive = 0, const char * name = "FHCAL")
{
  PHG4ForwardHcalSubsystem *hhcal = new PHG4ForwardHcalSubsystem(name);
  hhcal->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem( hhcal );
}

// Forward HCAL based on central barrel G4 setup
// zpos is starting position in z
// this function uses cones to build up the detector
double G4_FHCal(PHG4Reco* g4Reco, double zpos, const double etamin, const double etamax, const double thickness, const int absorberactive = 0, const char * name = "FHCAL")
{
  const bool forward_backward_swith = (zpos>0);

  if (forward_backward_swith)
    cout <<"G4_FHCal - construct forward FHCAL with name "<<name<<endl;
  else
    cout <<"G4_FHCal - construct backward FHCAL with name "<<name<<endl;

  double construction_direction = forward_backward_swith? +1: -1;



  double layerwidth = thickness/(Max_fhcal_layer-Min_fhcal_layer);
  double ironwidth = layerwidth*4./5.; // 4/5th iron
  float scintiwidth = layerwidth/5.; // 1/5th scintillator
  double pos = zpos +construction_direction * (ironwidth/2);
  PHG4ConeSubsystem* fhcal;
  for (int i = Min_fhcal_layer; i <=  Max_fhcal_layer; i++)
    {
      fhcal = new PHG4ConeSubsystem(std::string("ABSORBER_") + name, i);
      fhcal->SetPlaceZ(pos);
      fhcal->SetMaterial("G4_Fe");
      fhcal->SetZlength(ironwidth/2);
      fhcal->Set_eta_range(etamin, etamax);
      if (absorberactive)  fhcal->SetActive();
      fhcal->SuperDetector(std::string("ABSORBER_") + name);
      fhcal->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem( fhcal );
      pos += construction_direction *  (ironwidth/2. + scintiwidth/2. + no_overlapp);

      fhcal = new PHG4ConeSubsystem(name, i);
      fhcal->SetPlaceZ(pos);
      fhcal->SetMaterial("G4_POLYSTYRENE");
      fhcal->SetZlength(scintiwidth/2);
      fhcal->Set_eta_range(etamin, etamax);
      fhcal->SetActive(true);
      fhcal->SuperDetector(name);
      fhcal->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem( fhcal );
      pos += construction_direction *  (scintiwidth/2. + ironwidth/2. + no_overlapp);
    }
  pos -= construction_direction * ironwidth/2.;
  return pos;
}
