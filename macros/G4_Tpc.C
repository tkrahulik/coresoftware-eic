// these are defined in the Fun4All macro, here we just override the values
// with what is used in this macro
int Min_tpc_layer = 0;
int Max_tpc_layer = 65;

#include <TMath.h>

void
TpcInit()
{
  Min_tpc_layer = 0;
  Max_tpc_layer = 65;
}

//! ePHENIX TPC - Jin Huang <jhuang@bnl.gov>
//! Ref: B. Yu et al. A gem based tpc for the legs experiment. In Nuclear Science Symposium
//! Conference Record, 2005 IEEE, volume 2, pages 924-928, 2005. doi:10.1109/NSSMIC.2005.1596405.
double
Tpc(PHG4Reco* g4Reco, double radius=15, const double thickness=65,
    const int absorberactive = 0)
{
  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  PHG4CylinderSubsystem *cyl;

  double length = 190.; // 190 cm long
  double readoutthickness = (thickness - .5)
      / (Max_tpc_layer - Min_tpc_layer + 1);
  double inner_radius = radius;

  const double min_pol_angle = TMath::ATan2(inner_radius, length / 2);
  const double max_pol_angle = TMath::ATan2((inner_radius + thickness),      length / 2);

  cyl = new PHG4CylinderSubsystem("TPC_Inner_Window", 100);
  cyl->SetRadius(radius);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(length);
  cyl->SetMaterial("G4_MYLAR");
  cyl->SetThickness(.1);
  cyl->SetActive(false);
  cyl->SuperDetector("TPC");
  cyl->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(cyl);
  radius += readoutthickness;
  radius += no_overlapp;

  for (int ilayer = Min_tpc_layer; ilayer <= Max_tpc_layer; ilayer++)
    {
      cyl = new PHG4CylinderSubsystem("TPC", ilayer);
      cyl->SetRadius(radius);
      cyl->SetLengthViaRapidityCoverage(false);
      cyl->SetLength(length);
      cyl->SetMaterial("ePHEINX_TPC_Gas");
      cyl->SetThickness(readoutthickness);
      cyl->SetActive();
      cyl->SuperDetector("TPC");
      cyl->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(cyl);
      radius += readoutthickness;
      radius += no_overlapp;
    }

  cyl = new PHG4CylinderSubsystem("TPC_Outer_Window", 100);
  cyl->SetRadius(radius);
  cyl->SetLengthViaRapidityCoverage(false);
  cyl->SetLength(length);
  cyl->SetMaterial("G4_POLYCARBONATE");
  cyl->SetThickness(.25);
  cyl->SetActive(false);
  cyl->SuperDetector("TPC");
  cyl->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(cyl);
  radius += readoutthickness;
  radius += no_overlapp;

  PHG4SectorSubsystem *gem;

  gem = new PHG4SectorSubsystem("TPC_Readout_North");
  gem->get_geometry().set_normal_polar_angle(0);
  gem->get_geometry().set_normal_start(
      (length / 2) * PHG4Sector::Sector_Geometry::Unit_cm());
  gem->get_geometry().set_min_polar_angle(min_pol_angle);
  gem->get_geometry().set_max_polar_angle(max_pol_angle);
  gem->get_geometry().set_material("ePHEINX_TPC_Gas");
  gem->get_geometry().set_N_Sector(1);
  gem->OverlapCheck(overlapcheck);
  gem->get_geometry().AddLayers_HBD_GEM();
  gem->get_geometry().AddLayers_HBD_Readout();
  gem->SuperDetector("TPC");
  g4Reco->registerSubsystem(gem);
//
  gem = new PHG4SectorSubsystem("TPC_Readout_South");
  gem->get_geometry().set_normal_polar_angle(TMath::Pi());
  gem->get_geometry().set_normal_start(
      (length / 2) * PHG4Sector::Sector_Geometry::Unit_cm());
  gem->get_geometry().set_min_polar_angle(TMath::Pi() - max_pol_angle);
  gem->get_geometry().set_max_polar_angle(TMath::Pi() - min_pol_angle);
  gem->get_geometry().set_material("ePHEINX_TPC_Gas");
  gem->get_geometry().set_N_Sector(1);
  gem->OverlapCheck(overlapcheck);
  gem->get_geometry().AddLayers_HBD_GEM();
  gem->get_geometry().AddLayers_HBD_Readout();
  gem->SuperDetector("TPC");
  g4Reco->registerSubsystem(gem);

  return radius;
}
