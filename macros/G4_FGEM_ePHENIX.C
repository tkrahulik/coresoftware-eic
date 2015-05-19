// $Id: G4_FGEM_ePHENIX.C,v 1.4 2013/10/13 21:45:27 jinhuang Exp $                                                                                             

/*!
 * \file G4_FGEM_ePHENIX.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision: 1.4 $
 * \date $Date: 2013/10/13 21:45:27 $
 */

void
FGEM_ePHENIXInit()
{

}

void
G4_FGEM_ePHENIX(PHG4Reco* g4Reco, const int N_Sector = 8, //
    const double min_eta = 1.1 //
    )
{

  double zpos[] =
    { 17., 60., 140.0, 185, 297.5 }; // position of GEM tracker stations
  make_FGEM_station("GEMSTATION0", g4Reco, zpos[0], 1.01, 2.7);
  make_FGEM_station("GEMSTATION1", g4Reco, zpos[1], 2.15, 4.0);
  make_FGEM_station("GEMSTATION2", g4Reco, zpos[2], min_eta, 4.);
  make_FGEM_station("GEMSTATION3", g4Reco, zpos[3], 2., 4.3);
  make_FGEM_station("GEMSTATION4", g4Reco, zpos[4], 2., 4.8);

  PHG4SectorSubsystem *gem;
  gem = new PHG4SectorSubsystem("GEMSTATION4_LowerEta");
  gem->SuperDetector("GEMSTATION4");

  gem->get_geometry().set_normal_polar_angle(
      (PHG4Sector::Sector_Geometry::eta_to_polar_angle(min_eta)
          + PHG4Sector::Sector_Geometry::eta_to_polar_angle(2)) / 2);
  gem->get_geometry().set_normal_start(
      305 * PHG4Sector::Sector_Geometry::Unit_cm());
  gem->get_geometry().set_min_polar_angle(
      PHG4Sector::Sector_Geometry::eta_to_polar_angle(2.2));
  gem->get_geometry().set_max_polar_angle(
      PHG4Sector::Sector_Geometry::eta_to_polar_angle(min_eta));
  gem->get_geometry().set_material("G4_METHANE");
  gem->get_geometry().set_N_Sector(N_Sector);
  gem->get_geometry().set_min_polar_edge(
      PHG4Sector::Sector_Geometry::FlatEdge());

  gem->get_geometry().AddLayers_DriftVol_COMPASS();
  gem->get_geometry().AddLayers_HBD_GEM();
  gem->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(gem);

}

void
G4_eGEM_ePHENIX(PHG4Reco* g4Reco)
{
  make_FGEM_station("GEMSTATIONE1", g4Reco, -32., -1.6, -3.4);
  make_FGEM_station("GEMSTATIONE2", g4Reco, -58., -2.1, -4.0);
  make_FGEM_station("GEMSTATIONE3", g4Reco, -98., -1.2, -4.5);
}

int
make_FGEM_station(string name, PHG4Reco* g4Reco, double zpos, double etamin,
    double etamax)
{

  cout
      << "make_GEM_station - GEM construction with PHG4SectorSubsystem - make_GEM_station_EdgeReadout  of "
      << name << endl;

  double polar_angle = 0;

  if (zpos < 0)
    {
      zpos = -zpos;
      polar_angle = TMath::Pi();

    }
  if (etamax < etamin)
    {
      double t = etamax;
      etamax = etamin;
      etamin = t;
    }

  PHG4SectorSubsystem *gem;
  gem = new PHG4SectorSubsystem(name.c_str());

  gem->get_geometry().set_normal_polar_angle(polar_angle);
  gem->get_geometry().set_normal_start(
      zpos * PHG4Sector::Sector_Geometry::Unit_cm());
  gem->get_geometry().set_min_polar_angle(
      PHG4Sector::Sector_Geometry::eta_to_polar_angle(etamax));
  gem->get_geometry().set_max_polar_angle(
      PHG4Sector::Sector_Geometry::eta_to_polar_angle(etamin));
  gem->get_geometry().set_material("G4_METHANE");
  gem->get_geometry().set_N_Sector(1);
  gem->OverlapCheck(overlapcheck);

  gem->get_geometry().AddLayers_DriftVol_COMPASS();
  gem->get_geometry().AddLayers_HBD_GEM();
  g4Reco->registerSubsystem(gem);

}

