// $$Id: Fun4All_G4_fsPHENIX.C,v 1.7 2015/03/01 19:52:33 nfeege Exp $$
#include <iostream>

using namespace std;

//! fsPHENIX simulation loading script
//! \param[in] nEvents Number of events to run. If nEvents=-1, then a event display will be shown
//! \param[in] nSkip Number of event to skip before start processing
//! \param[in] inputFile Input file. Depending on the "Input options" as in the beginning of macro.
//! \param[in] embed_input_file Second input file for embedding. Also depending on the "Input options" as in the beginning of macro.
void
Fun4All_G4_fsPHENIX(int nEvents = 3, int nSkip = 0, const char * input_file =
    "test/ID1001_phpythia_jet_eta0_E1.root.lst",
//    "input",//
    const char * embed_input_file = NULL
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/fsPHENIX_work/JetG4/pA_sPHENIX_HcalIn_fHcal/G4sPHENIX-4fm.lst"//
    )
{
  /*
   //Seed for event generation
   if (seed == 0)
   {
   seed = atoi((const char *) (gSystem->GetFromPipe("/bin/date +'%N'")));
   }
   */
  //-----------------------------------------------------------------
  // Input options
  //-----------------------------------------------------------------
  // Either:
  // read previously generated g4-hits or g4-tower DST files, in this case it opens a DST and skips
  // the simulations step completely. The G4Setup macro is only loaded to get information
  // about the number of layers used for the cell reco code
  const bool readhits = false;
  const bool readtowers = false;
  // Or:
  // read files in HepMC format (typically output from event generators like hijing or pythia)
  const bool readhepmc = false; // read HepMC files
  // Or:
  // read DST files containing PHPythia nodes
  const bool readpythiaDST = true; // read PHPythia files
//  const bool readpythiaDST = false; // read PHPythia files
  // Or:
  // Generate pythia event
  const bool runpythia = false; // read HepMC files
  // Or:
  // Run a event display automatically when nEvents<0
  const bool event_display = (nEvents < 0 && !(readhits || readtowers)); // read HepMC files
  // Save G4 raw info?
//  const bool save_g4_raw = readhits ? false : true;
  const bool save_g4_raw = false;

  //Files for evaluator output
  TString outputFile = (TString(input_file) + TString("_G4.root"));
  TString svx_evalname =
      (TString(input_file) + TString(Form("_svx_%i.root", 0)));
  TString cemc_evalname = (TString(input_file)
      + TString(Form("_cemc_%i.root", 0)));
  TString hcalin_evalname = (TString(input_file)
      + TString(Form("_hcal1_%i.root", 0)));
  TString hcalout_evalname = (TString(input_file)
      + TString(Form("_hcal2_%i.root", 0)));
  TString fhcal_evalname = (TString(input_file)
      + TString(Form("_fhcal_%i.root", 0)));
  TString jet_evalname_base = (TString(input_file) + TString("_jet_%i.root"));
  TString jet_embed_evalname_base = (TString(input_file)
      + TString("_jet_embed_%i.root"));
  TString jet_embed_bgd_sub_evalname_base = (TString(input_file)
      + TString("_jet_embed_bgd_sub_%i.root"));

  std::string embed_top_node = "TOP_EMBED";


  //-----------------------------------------------------------------
  // flag set for G4 production
  //-----------------------------------------------------------------

//  bool do_g4_eval = true;
//
//  bool do_svx_cell = true;
//  bool do_svx_track = false;
//  bool do_svx_eval = false;
//
//  bool do_cemc_cell = true;
//  bool do_cemc_twr = true;
//  bool do_cemc_cluster = false;
//  bool do_cemc_eval = false;
//
//  bool do_hcal_cell = true;
//  bool do_hcal_twr = true;
//  bool do_hcal_cluster = false;
//  bool do_hcal_eval = false;
//
//  bool do_fhcal_twr = true;
//  bool do_fhcal_cluster = false;
//  bool do_fhcal_eval = false;
//
//  bool do_mc_twr = false;
//  bool do_jet_reco = false;
//  bool do_jet_bgd_sub = false;

//  //-----------------------------------------------------------------
//  // flag set for jet reconstruction from G4 hits
//  //-----------------------------------------------------------------
//
  bool do_g4_eval = true;

  bool do_svx_cell = true;
  bool do_svx_track = false;
  bool do_svx_eval = false;

  bool do_cemc_cell = true;
  bool do_cemc_twr = true;
  bool do_cemc_cluster = false;
  bool do_cemc_eval = false;

  bool do_hcal_cell = true;
  bool do_hcal_twr = true;
  bool do_hcal_cluster = false;
  bool do_hcal_eval = false;

  bool do_fhcal_twr = true;
  bool do_fhcal_cluster = false;
  bool do_fhcal_eval = false;

  bool do_mc_twr = true;
  bool do_jet_reco = true;
  bool do_jet_bgd_sub = false;

  //-----------------------------------------------------------------
  // flag set for jet embedding and reconstruction from towers
  //-----------------------------------------------------------------

//  bool do_g4_eval = true;
//
//  bool do_svx_cell = false;
//  bool do_svx_track = false;
//  bool do_svx_eval = false;
//
//  bool do_cemc_cell = false;
//  bool do_cemc_twr = false;
//  bool do_cemc_cluster = false;
//  bool do_cemc_eval = false;
//
//  bool do_hcal_cell = false;
//  bool do_hcal_twr = false;
//  bool do_hcal_cluster = false;
//  bool do_hcal_eval = false;
//
//  bool do_fhcal_twr = false;
//  bool do_fhcal_cluster = false;
//  bool do_fhcal_eval = false;
//
//  bool do_mc_twr = false;
//  bool do_jet_reco = true;
//  bool do_jet_bgd_sub = true;
//

  // use binning in eta and number of slats - closer to what we will do
  // if set to false use binning in eta phi bins (needed if you use
  // a layered geometry instead of our tilted slats)
  bool cemc_use_slat_cell = false;
  bool hcal_use_slat_cell = true;

  //-----------------------------------------------------------------
  // Load libraries
  //-----------------------------------------------------------------
  gSystem->Load("libphool.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libcemc.so");
  gSystem->Load("libg4eval.so");

  //-----------------------------------------------------------------
  // Load detector configuration
  //-----------------------------------------------------------------
  gROOT->LoadMacro("G4Setup_fsPHENIX.C"); // Inner Hcal inside Babar magnet + forward HCAL
  G4Init();

  int absorberactive = 0;

  //-----------------------------------------------------------------
  // Fun4All Server
  //-----------------------------------------------------------------
  Fun4AllServer *se = Fun4AllServer::instance();

  //-----------------------------------------------------------------
  // Event generation
  //-----------------------------------------------------------------
  if (event_display)
    {
      cout
          << "======================================================================="
          << endl;
      cout << "Make an event display of one forward jet" << endl;
      cout
          << "======================================================================="
          << endl;

      //Jet maker
      MakeAJet();
    }
  if (!save_g4_raw)
    {
      cout
          << "======================================================================="
          << endl;
      cout << "Do not save G4 raw information to DST" << endl;
      cout
          << "======================================================================="
          << endl;

      //Jet maker
//      MakeAJet();
    }

  if (readhits)
    {
      cout
          << "======================================================================="
          << endl;
      cout << "Do NOT run simulation, but read in G4Hit DST file: "
          << input_file << endl;
      cout
          << "======================================================================="
          << endl;

    }
  else if (readtowers)
    {
      cout
          << "======================================================================="
          << endl;
      cout << "Do NOT run simulation, but read in G4Tower DST file: "
          << input_file << endl;
      cout
          << "======================================================================="
          << endl;

    }
  else if (readhepmc)
    {
      cout
          << "======================================================================="
          << endl;
      cout << "Read in HepMC file: " << input_file << endl;
      cout
          << "======================================================================="
          << endl;

      HepMCNodeReader *hr = new HepMCNodeReader();
      se->registerSubsystem(hr);
    }
  else if (readpythiaDST)
    {
      cout
          << "======================================================================="
          << endl;
      cout << "Read in Pythia DST file: " << input_file << endl;
      cout
          << "======================================================================="
          << endl;

      // -------------
      //  PHPythia Input
      // -------------

      PHG4PhPyReader *phpyread = new PHG4PhPyReader();
      if (nEvents > 0 && nEvents < 3)
        phpyread->Verbosity(2);
      //phpyread->set_vtx_sigma(0.008, 0.008, 30);
      se->registerSubsystem(phpyread);

    }
  else if (runpythia)
    {
      cout
          << "======================================================================="
          << endl;
      cout << "Run Pythia with configuraiton file: " << input_file << endl;
      cout
          << "======================================================================="
          << endl;
      // -------------
      //  PHPythia Input
      // -------------

      gSystem->Load("libPHPythiaEventGen");
      gSystem->Load("libPHPyTrigger");

      PHPythia *phpythia = new PHPythia();
      phpythia->ReadConfig(input_file);
      se->registerSubsystem(phpythia);

      PHG4PhPyReader *phpyread = new PHG4PhPyReader();
      if (nEvents > 0 && nEvents < 3)
        phpyread->Verbosity(2);
      //phpyread->set_vtx_sigma(0.008, 0.008, 30);
      se->registerSubsystem(phpyread);

      Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
      se->registerInputManager(in);
    }

  cout
      << "======================================================================="
      << endl;
  if (embed_input_file)
    {
      cout << "Embed the signal event to " << embed_input_file << endl;
    }
  else
    {
      cout << "Do NOT perform embedding" << endl;
    }
  cout
      << "======================================================================="
      << endl;

  if (!(readhits || readtowers))
    {
      G4Setup(absorberactive);

      //PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
      //g4->G4Seed(seed);
    }

  //-----------------------------------------------------------------
  // Detector Division
  //-----------------------------------------------------------------

  if (do_svx_cell)
    Svtx_Cells();

  if (do_cemc_cell)
    {
      // Compact Emc
      int minlayer = Get_Min_cemc_layer();
      int maxlayer = Get_Max_cemc_layer();
      if (cemc_use_slat_cell) // for hcal tilted slat geo
        {
          PHG4SlatCellReco *cemc_cells = new PHG4SlatCellReco(
              "CEMCSLATCELLRECO");
          cemc_cells->Detector("CEMC");
          cemc_cells->Verbosity(0);
          for (int i = minlayer; i <= maxlayer; i++)
            {
              cemc_cells->etasize_nslat(i, 0.024, Cemc_slats_per_cell / 8);
            }
          se->registerSubsystem(cemc_cells);
        }
      else // old layered geometry
        {
          PHG4CylinderCellReco *cemc_cells = new PHG4CylinderCellReco(
              "CEMCCYLCELLRECO");
          cemc_cells->Detector("CEMC");
          cemc_cells->Verbosity(0);
          for (int i = minlayer; i <= maxlayer; i++)
            {
              cemc_cells->etaphisize(i, 0.024, 0.024);
            }
          se->registerSubsystem(cemc_cells);
        }
    }

  if (do_hcal_cell)
    {

      char *detname[1] =
        { "HCALIN" };
      int minhcallayer[2];
      int maxhcallayer[2];
      // Inner Hcal
      minhcallayer[0] = Get_Min_hcal_in_layer();
      maxhcallayer[0] = Get_Max_hcal_in_layer();
      minhcallayer[1] = Get_Min_hcal_out_layer();
      maxhcallayer[1] = Get_Max_hcal_out_layer();
      for (int ihcal = 0; ihcal < 1; ihcal++)
        {
          if (hcal_use_slat_cell) // for hcal tilted slat geo
            {
              PHG4SlatCellReco *hcal_slats = new PHG4SlatCellReco(
                  "HCALSLATCELLRECO");
              hcal_slats->Detector(detname[ihcal]);
              hcal_slats->Verbosity(0);
              for (int i = minhcallayer[ihcal]; i <= maxhcallayer[ihcal]; i++)
                {
                  // update from 8 to 6 slate cell to be corresponding
                  // to reference design in 2014 scientific review - Jin
                  hcal_slats->etasize_nslat(i, 0.1, 6);
                }
              se->registerSubsystem(hcal_slats);
            }
          else // old layered geometry
            {
              PHG4CylinderCellReco *hcal_cells = new PHG4CylinderCellReco(
                  "HCALCYLCELLRECO");
              hcal_cells->Detector(detname[ihcal]);
              hcal_cells->Verbosity(0);
              for (int i = minhcallayer[ihcal]; i <= maxhcallayer[ihcal]; i++)
                {
                  hcal_cells->etaphisize(i, 0.1, 0.1);
                }
              se->registerSubsystem(hcal_cells);
            }
        }

      PHG4HcalCellReco *hc = new PHG4HcalCellReco();
      hc->Detector("HCALOUT");
      hc->etasize_nslat(0, 0, 5);
      se->registerSubsystem(hc);

    }

  //-------------------------
  // Preshower reconstruction
  //-------------------------

  if (do_cemc_twr)
    {
      //-----------------------------
      // Compact EMCal towers
      //-----------------------------
      RawTowerBuilder *TowerBuilder = new RawTowerBuilder("EmcRawTowerBuilder");
      TowerBuilder->Detector("CEMC");
      se->registerSubsystem(TowerBuilder);
    }
  if (do_cemc_cluster)
    {
      RawClusterBuilder* ClusterBuilder = new RawClusterBuilder(
          "EmcRawClusterBuilder");
      ClusterBuilder->Detector("CEMC");
      ClusterBuilder->Verbosity(0);
      se->registerSubsystem(ClusterBuilder);
    }

  if (do_hcal_twr)
    {
      //--------------------
      // HCal reconstruction
      //--------------------
      RawTowerBuilder *TowerBuilder = new RawTowerBuilder(
          "HcalInRawTowerBuilder");
      TowerBuilder->Detector("HCALIN");
      se->registerSubsystem(TowerBuilder);
//      if (do_outer_hcal_cell)
        {
          TowerBuilder = new RawTowerBuilder("HcalOutRawTowerBuilder");
          TowerBuilder->Detector("HCALOUT");
          se->registerSubsystem(TowerBuilder);
        }
    }
  if (do_fhcal_twr)
    {

      //--------------------
      // FHCal reconstruction
// tower groups, doubled azimuthal segmentation from the 2m FHCal design as below
//      Index R0  Rin Rout  etain etaout  Circ  Ndiv ideal  Ndiv assign Width assign  Area
//      1 10.1692857143 5 15.3385714286 4.3821828481  3.2625610985  63.8955065845 6.3895506585  8 7.9869383231  82.5735323486
//      2 20.5078571429 15.3385714286 25.6771428571 3.2625610985  2.749958817 128.8546666817  12.8854666682 16  8.0534166676  83.2608234621
//      3 30.8464285714 25.6771428571 36.0157142857 2.749958817 2.4155194212  193.813826779 19.3813826779 32  6.0566820868  62.617440375
//      4 41.185  36.0157142857 46.3542857143 2.4155194212  2.1683175662  258.7729868762  25.8772986876 32  8.0866558399  83.6044690189
//      5 51.5235714286 46.3542857143 56.6928571429 2.1683175662  1.9733247024  323.7321469734  32.3732146973 32  10.1166295929 104.5914976628
//      6 61.8621428571 56.6928571429 67.0314285714 1.9733247024  1.8132713715  388.6913070706  38.8691307071 64  6.073301673 62.7892631534
//      7 72.2007142857 67.0314285714 77.37 1.8132713715  1.6783381229  453.6504671679  45.3650467168 64  7.0882885495  73.2827774753
//      8 82.5392857143 77.37 87.7085714286 1.6783381229  1.5623867281  518.6096272651  51.8609627265 64  8.103275426 83.7762917973
//      9 92.8778571429 87.7085714286 98.0471428571 1.5623867281  1.4613096351  583.5687873623  58.3568787362 64  9.1182623025  94.2698061192
//      10  103.2164285714  98.0471428571 108.3857142857  1.4613096351  1.372212818 648.5279474596  64.852794746  64  10.1332491791 104.7633204412
//      11  113.555 108.3857142857  118.7242857143  1.372212818 1.2929733396  713.4871075568  71.3487107557 64  11.1482360556 115.2568347631
//      12  123.8935714286  118.7242857143  129.0628571429  1.2929733396  1.2219827685  778.446267654 77.8446267654 64  12.1632229321 125.7503490851
//      13  134.2321428571  129.0628571429  139.4014285714  1.2219827685  1.1579900073  843.4054277512  84.3405427751 64  13.1782098086 136.243863407
//      14  144.5707142857  139.4014285714  149.74  1.1579900073  1.1000005917  908.3645878485  90.8364587848 64  14.1931966851 146.737377729

      RawTowerBuilderCone *TowerBuilderCone = NULL;
      //--------------------
      TowerBuilderCone = new RawTowerBuilderCone("RawTowerBuilderCone1");
      TowerBuilderCone->Detector("FHCAL");
      TowerBuilderCone->SetZMin(350);
      TowerBuilderCone->SetZMax(450);

      TowerBuilderCone->SetGroupID("1");
      TowerBuilderCone->SetEtaMin(4.0);
      TowerBuilderCone->SetEtaMax(5.0);
      TowerBuilderCone->SetEtaNBins(2);
      TowerBuilderCone->SetPhiNBins(16);
      se->registerSubsystem(TowerBuilderCone);
      //--------------------
      TowerBuilderCone = new RawTowerBuilderCone("RawTowerBuilderCone2");
      TowerBuilderCone->Detector("FHCAL");
      TowerBuilderCone->SetZMin(350);
      TowerBuilderCone->SetZMax(450);

      TowerBuilderCone->SetGroupID("2");
      TowerBuilderCone->SetEtaMin(2.749958817);
      TowerBuilderCone->SetEtaMax(4.0);
      TowerBuilderCone->SetEtaNBins(5);
      TowerBuilderCone->SetPhiNBins(32);
      se->registerSubsystem(TowerBuilderCone);
      //--------------------
      TowerBuilderCone = new RawTowerBuilderCone("RawTowerBuilderCone3");
      TowerBuilderCone->Detector("FHCAL");
      TowerBuilderCone->SetZMin(350);
      TowerBuilderCone->SetZMax(450);

      TowerBuilderCone->SetGroupID("3");
      TowerBuilderCone->SetEtaMin(1.9733247024);
      TowerBuilderCone->SetEtaMax(2.749958817);
      TowerBuilderCone->SetEtaNBins(6);
      TowerBuilderCone->SetPhiNBins(64);
      se->registerSubsystem(TowerBuilderCone);
      //--------------------
      TowerBuilderCone = new RawTowerBuilderCone("RawTowerBuilderCone4");
      TowerBuilderCone->Detector("FHCAL");
      TowerBuilderCone->SetZMin(350);
      TowerBuilderCone->SetZMax(450);

      TowerBuilderCone->SetGroupID("4");
      TowerBuilderCone->SetEtaMin(1.1000005917);
      TowerBuilderCone->SetEtaMax(1.9733247024);
      TowerBuilderCone->SetEtaNBins(9);
      TowerBuilderCone->SetPhiNBins(64*2);
      se->registerSubsystem(TowerBuilderCone);

    }
  if (do_hcal_cluster)
    {
      RawClusterBuilder* ClusterBuilder = new RawClusterBuilder(
          "HcalInRawClusterBuilder");
      ClusterBuilder->Detector("HCALIN");
      ClusterBuilder->Verbosity(0);
      se->registerSubsystem(ClusterBuilder);
//      if (do_outer_hcal_cell)
        {
          ClusterBuilder = new RawClusterBuilder("HcalOutRawClusterBuilder");
          ClusterBuilder->Detector("HCALOUT");
          ClusterBuilder->Verbosity(0);
          se->registerSubsystem(ClusterBuilder);
        }
    }
  //--------------
  // SVTX tracking
  //--------------

  if (do_svx_track)
    Svtx_Reco();

  //----------------------
  // Simulation evaluation
  //----------------------

  if (do_svx_eval)
    Svtx_Eval(svx_evalname);

  if (do_cemc_eval)
    {
      PHG4CalEvaluator* eval1 = new PHG4CalEvaluator("PHG4CEMCEVALUATOR",
          cemc_evalname);
      eval1->Verbosity(0);
      eval1->Detector("CEMC");
      se->registerSubsystem(eval1);
    }

  if (do_hcal_eval)
    {
      PHG4CalEvaluator* eval1 = new PHG4CalEvaluator("PHG4HCALINEVALUATOR",
          hcalin_evalname);
      eval1->Verbosity(0);
      eval1->Detector("HCALIN");
      se->registerSubsystem(eval1);
      eval1 = new PHG4CalEvaluator("PHG4HCALOUTEVALUATOR", hcalout_evalname);
      eval1->Verbosity(0);
      eval1->Detector("HCALOUT");
      se->registerSubsystem(eval1);
    }

  if (do_fhcal_eval)
    {
      PHG4CalEvaluator* eval1 = new PHG4CalEvaluator("PHG4FHCALEVALUATOR",
          fhcal_evalname);
      eval1->Verbosity(0);
      eval1->Detector("FHCAL");
      se->registerSubsystem(eval1);
    }

  // truth jet
  if (do_jet_reco && (readpythiaDST || runpythia || readhits))
    {
      gSystem->Load("libPHPythiaJet.so");

      PHPyJetParticleAcceptor *inperfect = new PHPyJetParticleAcceptInPerfect(
          5.0);

      PHPyJetMaker *jetReco_antikt06_inperfect = new PHPyJetMaker("anti-kt",
          0.6, "AntiKt06JetsInPerfect", inperfect, "PHPythia");
      jetReco_antikt06_inperfect->Verbosity(0);
      se->registerSubsystem(jetReco_antikt06_inperfect);

      PHPyJetMaker *jetReco_antikt04_inperfect = new PHPyJetMaker("anti-kt",
          0.4, "AntiKt04JetsInPerfect", inperfect, "PHPythia");
      jetReco_antikt04_inperfect->Verbosity(0);
      se->registerSubsystem(jetReco_antikt04_inperfect);

      PHPyJetMaker *jetReco_antikt03_inperfect = new PHPyJetMaker("anti-kt",
          0.3, "AntiKt03JetsInPerfect", inperfect, "PHPythia");
      jetReco_antikt03_inperfect->Verbosity(0);
      se->registerSubsystem(jetReco_antikt03_inperfect);

      PHPyJetMaker *jetReco_antikt02_inperfect = new PHPyJetMaker("anti-kt",
          0.2, "AntiKt02JetsInPerfect", inperfect, "PHPythia");
      jetReco_antikt02_inperfect->Verbosity(0);
      se->registerSubsystem(jetReco_antikt02_inperfect);

    }

  // MC towers for G4 jet
  if (do_jet_reco && do_mc_twr)
    {
      gSystem->Load("libPHJetBackgroundSubtract.so");

      // new calibration from 200 GeV jet simulation
      /*
       root [3] T->Draw("CEMC_Sample>>h1(100,0,0.15)")
       root [4]
       ****************************************
       Minimizer is Minuit2 / Migrad
       Chi2                      =      4.91196
       NDf                       =            8
       Edm                       =  9.54003e-06
       NCalls                    =           63
       Constant                  =      19.2626   +/-   2.84198
       Mean                      =    0.0357682   +/-   0.000317554
       Sigma                     =   0.00297688   +/-   0.000334113     (limited)
       root [5] T->Draw("HCALIN_Sample>>h1(100,0,0.15)")
       root [6] h1->Fit("gaus")

       ****************************************
       Minimizer is Minuit2
       Chi2                      =      29.2065
       NDf                       =           32
       Edm                       =  2.34549e-10
       NCalls                    =          100
       Constant                  =      2.68738   +/-   0.43543
       Mean                      =    0.0995591   +/-   0.0039091
       Sigma                     =    0.0219078   +/-   0.00540857      (limited)
       (class TFitResultPtr)163703936
       root [7] T->Draw("HCALOUT_Sample>>h1(100,0,0.15)")
       root [8] h1->Fit("gaus")

       ****************************************
       Minimizer is Minuit2
       Chi2                      =      22.2465
       NDf                       =           37
       Edm                       =  3.13956e-07
       NCalls                    =           88
       Constant                  =      2.75798   +/-   0.487613
       Mean                      =    0.0691385   +/-   0.00344568
       Sigma                     =    0.0220639   +/-   0.00565252      (limited)

       root [10] T->Draw("FHCAL_Sample>>h1(100,0,0.15)")
       root [11]  FCN=7.97701 FROM MIGRAD    STATUS=CONVERGED     455 CALLS         456 TOTAL
       EDM=5.18067e-07    STRATEGY= 1      ERROR MATRIX ACCURATE
       EXT PARAMETER                                   STEP         FIRST
       NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE
       1  Constant     1.46182e+01   2.26216e+00   2.22774e-03  -6.37056e-04
       2  Mean         4.66219e-02   4.21616e-04   6.10112e-07   5.25939e-01
       3  Sigma        3.83293e-03   4.73975e-04   4.63658e-05  -1.98290e-02

       */

      PHMCTowerReco *mctowers = new PHMCTowerReco("G4Towers", 1.0 / 0.0357682,
          1.0 / 0.0995591, 1.0 / 0.0691385);
      mctowers->Verbosity(0);
      se->registerSubsystem(mctowers);

      const double fhcal_sample = 4.66219e-02;
      se->registerSubsystem(
          new PHMCForwardTowerReco("TOWER_FHCAL_1", "G4Towers",
              1 / fhcal_sample));
      se->registerSubsystem(
          new PHMCForwardTowerReco("TOWER_FHCAL_2", "G4Towers",
              1 / fhcal_sample));
      se->registerSubsystem(
          new PHMCForwardTowerReco("TOWER_FHCAL_3", "G4Towers",
              1 / fhcal_sample));
      se->registerSubsystem(
          new PHMCForwardTowerReco("TOWER_FHCAL_4", "G4Towers",
              1 / fhcal_sample));

    }

  if (do_jet_reco && (readpythiaDST || runpythia || readhits))
    {
      gSystem->Load("libPHJetBackgroundSubtract");

      PHJetTowerAcceptor *inupgrade = new PHJetTowerAcceptInDetector(5.0);

      PHTowerJetMaker *towerjets2 = new PHTowerJetMaker("anti-kt", 0.2,
          "G4TowerJets_2", inupgrade, "G4Towers");
      PHTowerJetMaker *towerjets3 = new PHTowerJetMaker("anti-kt", 0.3,
          "G4TowerJets_3", inupgrade, "G4Towers");
      PHTowerJetMaker *towerjets4 = new PHTowerJetMaker("anti-kt", 0.4,
          "G4TowerJets_4", inupgrade, "G4Towers");
      PHTowerJetMaker *towerjets6 = new PHTowerJetMaker("anti-kt", 0.6,
          "G4TowerJets_6", inupgrade, "G4Towers");

      se->registerSubsystem(towerjets2);
      se->registerSubsystem(towerjets3);
      se->registerSubsystem(towerjets4);
      se->registerSubsystem(towerjets6);

    }

  // match jets
  if (do_jet_reco && (readpythiaDST || runpythia || readhits))
    {
      gSystem->Load("libPHJetBackgroundSubtract");

      PHJetMatch *jm2 = new PHJetMatch(
          std::string(Form(jet_evalname_base.Data(), 2)).c_str(),
          "AntiKt02JetsInPerfect", "G4TowerJets_2", 0.2, "");
      jm2->SetTrueEtCut(1);
      jm2->SetTrueEtaCut(5);
      jm2->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm2);

      PHJetMatch *jm3 = new PHJetMatch(
          std::string(Form(jet_evalname_base.Data(), 3)).c_str(),
          "AntiKt03JetsInPerfect", "G4TowerJets_3", 0.3, "");
      jm3->SetTrueEtCut(1);
      jm3->SetTrueEtaCut(5);
      jm3->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm3);

      PHJetMatch *jm4 = new PHJetMatch(
          std::string(Form(jet_evalname_base.Data(), 4)).c_str(),
          "AntiKt04JetsInPerfect", "G4TowerJets_4", 0.4, "");
      jm4->SetTrueEtCut(1);
      jm4->SetTrueEtaCut(5);
      jm4->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm4);

      PHJetMatch *jm6 = new PHJetMatch(
          std::string(Form(jet_evalname_base.Data(), 6)).c_str(),
          "AntiKt06JetsInPerfect", "G4TowerJets_6", 0.6, "");
      jm6->SetTrueEtCut(1);
      jm6->SetTrueEtaCut(5);
      jm6->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm6);
    }

  // embedding
  if (embed_input_file)
    {
      gSystem->Load("libPHJetBackgroundSubtract");

      PHMCTowerMerge *combine = new PHMCTowerMerge("PHMCTowerMerge",
          "G4Towers_combined");
      se->registerSubsystem(combine);
      combine->AddInputMCTower(embed_top_node + "/G4Towers"); // towers from embeding DST
      combine->AddInputMCTower("G4Towers"); // towers from this simulation
    }

  // make jet after embedding
  if (embed_input_file && do_jet_reco)
    {
      gSystem->Load("libPHJetBackgroundSubtract");
      PHJetTowerAcceptor *inupgrade = new PHJetTowerAcceptInDetector(5.0);

      PHTowerJetMaker *towerjets2 = new PHTowerJetMaker("anti-kt", 0.2,
          "G4TowerJets_combined_2", inupgrade, "G4Towers_combined");
      PHTowerJetMaker *towerjets3 = new PHTowerJetMaker("anti-kt", 0.3,
          "G4TowerJets_combined_3", inupgrade, "G4Towers_combined");
      PHTowerJetMaker *towerjets4 = new PHTowerJetMaker("anti-kt", 0.4,
          "G4TowerJets_combined_4", inupgrade, "G4Towers_combined");
      PHTowerJetMaker *towerjets6 = new PHTowerJetMaker("anti-kt", 0.6,
          "G4TowerJets_combined_6", inupgrade, "G4Towers_combined");

      se->registerSubsystem(towerjets2);
      se->registerSubsystem(towerjets3);
      se->registerSubsystem(towerjets4);
      se->registerSubsystem(towerjets6);

    }

  // match jets
  if (embed_input_file && do_jet_reco
      && (readpythiaDST || runpythia || readtowers || readhits))
    {
      gSystem->Load("libPHJetBackgroundSubtract");

      PHJetMatch *jm2 = new PHJetMatch(
          std::string(Form(jet_embed_evalname_base.Data(), 2)).c_str(),
          "AntiKt02JetsInPerfect", "G4TowerJets_combined_2", 0.2, "");
      jm2->SetTrueEtCut(1);
      jm2->SetTrueEtaCut(5);
      jm2->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm2);

      PHJetMatch *jm3 = new PHJetMatch(
          std::string(Form(jet_embed_evalname_base.Data(), 3)).c_str(),
          "AntiKt03JetsInPerfect", "G4TowerJets_combined_3", 0.3, "");
      jm3->SetTrueEtCut(1);
      jm3->SetTrueEtaCut(5);
      jm3->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm3);

      PHJetMatch *jm4 = new PHJetMatch(
          std::string(Form(jet_embed_evalname_base.Data(), 4)).c_str(),
          "AntiKt04JetsInPerfect", "G4TowerJets_combined_4", 0.4, "");
      jm4->SetTrueEtCut(1);
      jm4->SetTrueEtaCut(5);
      jm4->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm4);

      PHJetMatch *jm6 = new PHJetMatch(
          std::string(Form(jet_embed_evalname_base.Data(), 6)).c_str(),
          "AntiKt06JetsInPerfect", "G4TowerJets_combined_6", 0.6, "");
      jm6->SetTrueEtCut(1);
      jm6->SetTrueEtaCut(5);
      jm6->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm6);

    }

  // background subtraction
  if (embed_input_file && do_jet_bgd_sub
      && (readpythiaDST || runpythia || readtowers || readhits))
    {
      gSystem->Load("libPHJetBackgroundSubtract");

      const int verbosity = 0;

      // jet
      PHBackgroundExclusion* ex0 = new PHBackgroundExclusion("BkgdEx",
          "G4TowerJets_combined_4", "G4Towers_combined", 2);
      ex0->Verbosity(verbosity);
      ex0->SetExclusionCut(3.0);
      ex0->SetEtCut(10.0);
      se->registerSubsystem(ex0);

//      sPHv2 *sphv2 = new sPHv2("sPHv2","sPHv2","HEPMC","BkgdEx","G4Towers_combined","","vn_hist.root",1,true);
//      sphv2->Verbosity(verbosity);
//      sphv2->setAbortnoV2(false);
//      sphv2->setDetector(0); // 0 is EMCal, 1 is HCal
//      se->registerSubsystem(sphv2);

      PHUnderlyingEventCalc *jb = new PHUnderlyingEventCalc("BkgdEx",
          "G4Towers_combined", "background0", "", 2);
      jb->Verbosity(verbosity);
      jb->SetAbsEtaRange(3);
      jb->SetEMCalSeg(64, 60);
      jb->SetHCalSeg(64, 60);
      se->registerSubsystem(jb);

      PHJetBackgroundSubtract *sb = new PHJetBackgroundSubtract("background0",
          "G4TowerJets_combined_4", "G4Towers_combined", "", "",
          "tower_bkgd0_jets", "towers_bkgd0");
      sb->Verbosity(verbosity);
      sb->SetEtaBinning(61, 1.1); // 61 bin from -1.1 t +5
      se->registerSubsystem(sb);

      PHBackgroundExclusion *ex1 = new PHBackgroundExclusion("BkgdExSub",
          "tower_bkgd0_jets", "G4Towers_combined", 2);
      ex1->Verbosity(verbosity);
      ex1->SetEtCut(15);
      ex1->SetOutputName("sub_ex_histos.root");
      se->registerSubsystem(ex1);

//      sPHv2 *sphv2_1 = new sPHv2("sPHv2_1","sPHv2_1","HEPMC","BkgdExSub","PHMCTower","","vn_hist1.root",1,true);
//      sphv2_1->Verbosity(verbosity);
//      sphv2_1->setAbortnoV2(false);
//      sphv2_1->setDetector(0); // 0 is EMCal, 1 is HCal
//      se->registerSubsystem(sphv2_1);

      PHUnderlyingEventCalc *jb_1 = new PHUnderlyingEventCalc("BkgdExSub",
          "G4Towers_combined", "background1", "", 2);
      jb_1->Verbosity(verbosity);
      jb_1->SetAbsEtaRange(3);
      jb_1->SetEMCalSeg(64, 60);
      jb_1->SetHCalSeg(64, 60);
      se->registerSubsystem(jb_1);

      PHJetBackgroundSubtract *sb_1 = new PHJetBackgroundSubtract("background1",
          "G4TowerJets_combined_4", "G4Towers_combined", "", "",
          "tower_bkgd1_jets", "towers_bkgd1");
      sb_1->Verbosity(verbosity);
      sb_1->SetEtaBinning(61, 1.1); // 61 bin from -1.1 t +5
      se->registerSubsystem(sb_1);

      PHJetTowerAcceptor *inupgrade = new PHJetTowerAcceptInDetector(5.0);

      PHTowerJetMaker *towerjets2 = new PHTowerJetMaker("anti-kt", 0.2,
          "G4TowerJets_combined_bgd_sub_2", inupgrade, "towers_bkgd1");
      PHTowerJetMaker *towerjets3 = new PHTowerJetMaker("anti-kt", 0.3,
          "G4TowerJets_combined_bgd_sub_3", inupgrade, "towers_bkgd1");
      PHTowerJetMaker *towerjets4 = new PHTowerJetMaker("anti-kt", 0.4,
          "G4TowerJets_combined_bgd_sub_4", inupgrade, "towers_bkgd1");
      PHTowerJetMaker *towerjets6 = new PHTowerJetMaker("anti-kt", 0.6,
          "G4TowerJets_combined_bgd_sub_6", inupgrade, "towers_bkgd1");

      se->registerSubsystem(towerjets2);
      se->registerSubsystem(towerjets3);
      se->registerSubsystem(towerjets4);
      se->registerSubsystem(towerjets6);

      PHJetMatch *jm2 = new PHJetMatch(
          std::string(Form(jet_embed_bgd_sub_evalname_base.Data(), 2)).c_str(),
          "AntiKt02JetsInPerfect", "G4TowerJets_combined_bgd_sub_2", 0.2, "");
      jm2->SetTrueEtCut(1);
      jm2->SetTrueEtaCut(5);
      jm2->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm2);

      PHJetMatch *jm3 = new PHJetMatch(
          std::string(Form(jet_embed_bgd_sub_evalname_base.Data(), 3)).c_str(),
          "AntiKt03JetsInPerfect", "G4TowerJets_combined_bgd_sub_3", 0.3, "");
      jm3->SetTrueEtCut(1);
      jm3->SetTrueEtaCut(5);
      jm3->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm3);

      PHJetMatch *jm4 = new PHJetMatch(
          std::string(Form(jet_embed_bgd_sub_evalname_base.Data(), 4)).c_str(),
          "AntiKt04JetsInPerfect", "G4TowerJets_combined_bgd_sub_4", 0.4, "");
      jm4->SetTrueEtCut(1);
      jm4->SetTrueEtaCut(5);
      jm4->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm4);

      PHJetMatch *jm6 = new PHJetMatch(
          std::string(Form(jet_embed_bgd_sub_evalname_base.Data(), 6)).c_str(),
          "AntiKt06JetsInPerfect", "G4TowerJets_combined_bgd_sub_6", 0.6, "");
      jm6->SetTrueEtCut(1);
      jm6->SetTrueEtaCut(5);
      jm6->SetPythiaParticles("PHPythia");
      se->registerSubsystem(jm6);

    }
  //--------------
  // IO management
  //--------------

  if (readhits)
    {
      // Hits file
      Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
//      hitsin->fileopen(input_file);
      hitsin->AddListFile(input_file);
      se->registerInputManager(hitsin);
    }
  if (readtowers)
    {
      gSystem->Load("libPHJetBackgroundSubtract");
      // Hits file
      Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
//      hitsin->fileopen(input_file);
      hitsin->AddListFile(input_file);
      se->registerInputManager(hitsin);
    }
  else if (readhepmc)
    {
      Fun4AllInputManager *in = new Fun4AllHepMCInputManager("DSTIN");
      se->registerInputManager(in);
      in->fileopen(input_file);
    }
  else if (readpythiaDST)
    {

      // -------------
      //  PHPythia Input
      // -------------
      gSystem->Load("libPHPythiaJet.so");

      Fun4AllDstInputManager *in = new Fun4AllDstInputManager("DSTIN");
//      in->fileopen(input_file);
      in->AddListFile(input_file);
      se->registerInputManager(in);

    }

  if (embed_input_file)
    {

      Fun4AllDstInputManager *in1 = new Fun4AllNoSyncDstInputManager("DSTin1",
          "DST", embed_top_node);
//      in1->AddFile(embed_input_file);
      in1->AddListFile(embed_input_file);
      se->registerInputManager(in1);

    }

  //Save hits to disk
  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT",
      outputFile.Data());
  se->registerOutputManager(out);
  out->StripNode("PHG4INEVENT");

  // further strip all G4hit nodes if input is G4 hits
  if (!save_g4_raw)
    {
      out->StripNode("G4HIT_ABSORBER_FHCAL");
      out->StripNode("G4HIT_FHCAL");
      out->StripNode("G4HIT_ABSORBER_FHCAL_B");
      out->StripNode("G4HIT_FHCAL_B");
      out->StripNode("G4HIT_SVTX");
      out->StripNode("G4HIT_SVTXSUPPORT");
      out->StripNode("G4HIT_CEMC");
      out->StripNode("G4HIT_ABSORBER_CEMC");
      out->StripNode("G4HIT_EMCELECTRONICS_0");
      out->StripNode("G4HIT_HCALIN");
      out->StripNode("G4HIT_ABSORBER_HCALIN");
      out->StripNode("G4HIT_MAGNET_0");
      out->StripNode("G4HIT_HCALOUT");
      out->StripNode("G4HIT_ABSORBER_HCALOUT");
      out->StripNode("G4HIT_BH_1");
      out->StripNode("G4TruthInfo");
    }

  //--------------
  // G4hit evaluator
  //--------------

  if (do_g4_eval)
    {
      if (gSystem->Load("libg4analysis.so") == 0)
        {
          G4DSTReader* ana = new G4DSTReader(
              string(outputFile) + string("_DSTReader.root"));
          ana->set_save_particle(false);
          ana->set_load_all_particle(false);
          ana->set_load_active_particle(false);

          ana->set_save_vertex(false);

          if (nEvents > 0 && nEvents < 2)
            {
              ana->Verbosity(2);
            }

          if (save_g4_raw)
            {
              ana->AddNode("CEMC");
              if (absorberactive)
                ana->AddNode("ABSORBER_CEMC");

              ana->AddNode("HCALIN");
              if (absorberactive)
                ana->AddNode("ABSORBER_HCALIN");

              ana->AddNode("HCALOUT");
              if (absorberactive)
                ana->AddNode("ABSORBER_HCALOUT");

              ana->AddNode("FHCAL");
              if (absorberactive)
                ana->AddNode("ABSORBER_FHCAL");

              ana->AddNode("SVTX");
              ana->AddNode("MUTR");

              ana->AddNode("FVTXSTATION1");
              ana->AddNode("FVTXSTATION2");
              ana->AddNode("FVTXSTATION3");
              ana->AddNode("FVTXSTATION4");
              ana->AddNode("FVTXSTATION5");
              ana->AddNode("FVTXSTATION6");

              ana->AddNode("GEMSTATION1");
              ana->AddNode("GEMSTATION2");
              ana->AddNode("GEMSTATION3");
            }

          ana->set_tower_zero_sup(1e-6);
          if (do_cemc_twr)
            ana->AddTower("CEMC");
          if (do_hcal_twr)
            {
              ana->AddTower("HCALIN");
              ana->AddTower("HCALOUT");
            }

          if (do_fhcal_twr)
            {
              ana->AddTower("FHCAL_1");
              ana->AddTower("FHCAL_2");
              ana->AddTower("FHCAL_3");
              ana->AddTower("FHCAL_4");
            }

          if (do_jet_reco)
            {

              ana->AddJet("AntiKt06JetsInPerfect");
              ana->AddJet("G4TowerJets_6");
            }
          if (embed_input_file && do_jet_reco)
            {
              ana->AddJet("G4TowerJets_combined_6");
            }

          se->registerSubsystem(ana);
        }
      else
        {
          cout
              << "======================================================================="
              << endl;
          cout
              << "Please ignore the above error if you do not want to use G4DSTReader"
              << endl;
          cout
              << "======================================================================="
              << endl;
        }
    }

  gSystem->ListLibraries();

  if (event_display)
    {
      PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
      g4->ApplyCommand("/control/execute fsPHENIX.mac");

//      se->skip(4);
      se->Verbosity(10);
      se->run(1);
    }
  else
    {
      if (nEvents == 1)
        se->Verbosity(10);

      //-----------------
      // Event processing
      //-----------------
      se->skip(nSkip);
      se->run(nEvents);

      //-----
      // Exit
      //-----

      se->End();
      std::cout << "All done" << std::endl;
      delete se;
      gSystem->Exit(0);
    }
}

int
Get_Min_cemc_layer()
{
  return Min_cemc_layer;
}

int
Get_Max_cemc_layer()
{
  return Max_cemc_layer;
}

int
Get_Min_hcal_in_layer()
{
  return Min_hcal_in_layer;
}

int
Get_Max_hcal_in_layer()
{
  return Max_hcal_in_layer;
}

int
Get_Min_hcal_out_layer()
{
  return Min_hcal_out_layer;
}

int
Get_Max_hcal_out_layer()
{
  return Max_hcal_out_layer;
}

int
Get_Min_si_layer()
{
  return Min_si_layer;
}

int
Get_Max_si_layer()
{
  return Max_si_layer;
}

int
Get_Min_preshower_layer()
{
  return Min_preshower_layer;
}

int
Get_Max_preshower_layer()
{
  return Max_preshower_layer;
}

void
ParticleGun(const string name, const double pt, const double eta, double phi)
{
  phi += 1.3 - (-1.318265);

  const double px = pt * cos(phi);
  const double py = pt * sin(phi);
  const double pz = pt
      / tan(PHG4Sector::Sector_Geometry::eta_to_polar_angle(eta));
  TString g_name = TString("PHG4ParticleGun")
      + TString(Form("_PT_%.0f", pt * 1000));

  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4ParticleGun *gun = new PHG4ParticleGun(g_name.Data());
  gun->set_name(name);
  gun->set_mom(px, py, pz);
  se->registerSubsystem(gun);
}

void
MakeAJet()
{

  ///////////////////////////////////////////////////////////////////
  // Quick jet 43.6 GeV, eta = 3
  ///////////////////////////////////////////////////////////////////

  //JET IN FCAL

//  *    Row   *     event *    flavor *         e *        pt *       eta *       phi *       jid *        je *       jpt *      jeta *      jphi *
//  *     8228 *      1850 *      -211 * 4.2047877 * 0.5818077 * 2.6656084 * -0.915076 *        37 * 43.596885 * 4.0995669 * 3.0536193 * -1.245874 *
  ParticleGun("pi-", 0.5818077, 2.6656084, -0.915076);
//   ,    8229 *      1850 *        22 * 2.9098682 * 0.2201119 * 3.2734403 * -1.277455 *        37 * 43.596885 * 4.0995669 * 3.0536193 * -1.245874 *
  ParticleGun("gamma", 0.2201119, 3.2734403, -1.277455);
//  *     8230 *      1850 *        22 * 6.7528739 * 0.6606621 * 3.0152266 * -1.519383 *        37 * 43.596885 * 4.0995669 * 3.0536193 * -1.245874 *
  ParticleGun("gamma", 0.6606621, 3.0152266, -1.519383);
//  *     8231 *      1850 *       211 * 6.9534483 * 0.7269603 * 2.9483218 * -1.141218 *        37 * 43.596885 * 4.0995669 * 3.0536193 * -1.245874 *
  ParticleGun("pi+", 0.7269603, 2.9483218, -1.141218);
//  *     8232 *      1850 *        22 * 1.2386536 * 0.1310818 * 2.9362936 * -1.477361 *        37 * 43.596885 * 4.0995669 * 3.0536193 * -1.245874 *
  ParticleGun("gamma", 0.1310818, 2.9362936, -1.477361);
//  *     8233 *      1850 *      -211 * 8.1639041 * 0.7929520 * 3.0223486 * -1.318403 *        37 * 43.596885 * 4.0995669 * 3.0536193 * -1.245874 *
  ParticleGun("pi-", 0.7929520, 3.0223486, -1.318403);
//  *     8234 *      1850 *      2212 * 11.805546 * 0.8194502 * 3.3564555 * -1.318265 *        37 * 43.596885 * 4.0995669 * 3.0536193 * -1.245874 *
  ParticleGun("proton", 0.8194502, 3.3564555, -1.318265);

  Fun4AllServer *se = Fun4AllServer::instance();
  // for single particle generators we just need something which drives
  // the event loop, the Dummy Input Mgr does just that
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  se->registerInputManager(in);
}

void
G4Cmd(const char * cmd)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
  g4->ApplyCommand(cmd);
}
