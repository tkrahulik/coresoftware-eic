// $Id: G4_AeroGel.C,v 1.2 2013/10/09 01:08:17 jinhuang Exp $                                                                                             

/*!
 * \file G4_AeroGel.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision: 1.2 $
 * \date $Date: 2013/10/09 01:08:17 $
 */

void
AeroGelInit()
{

}

void
G4_AeroGel(PHG4Reco* g4Reco, const int N_Sector = 8, //
    const double min_eta = 1.1 //
    )
{

  PHG4SectorSubsystem *ag;
  ag = new PHG4SectorSubsystem("AeroGel");

  ag->get_geometry().set_normal_polar_angle(
      (PHG4Sector::Sector_Geometry::eta_to_polar_angle(min_eta)
          + PHG4Sector::Sector_Geometry::eta_to_polar_angle(2)) / 2);
//  ag->get_geometry().set_normal_polar_angle(0);
  ag->get_geometry().set_normal_start(
      307 * PHG4Sector::Sector_Geometry::Unit_cm());
  ag->get_geometry().set_min_polar_angle(
      PHG4Sector::Sector_Geometry::eta_to_polar_angle(1.9));
  ag->get_geometry().set_max_polar_angle(
      PHG4Sector::Sector_Geometry::eta_to_polar_angle(min_eta));
  ag->get_geometry().set_min_polar_edge(
      PHG4Sector::Sector_Geometry::FlatEdge());
  ag->get_geometry().set_material("G4_AIR");
  ag->get_geometry().set_N_Sector(N_Sector);
  ag->OverlapCheck(overlapcheck);

  ag->get_geometry().AddLayers_AeroGel_ePHENIX();
  g4Reco->registerSubsystem(ag);

}

