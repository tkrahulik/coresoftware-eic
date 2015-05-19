// $$Id: Fun4All_G4_ePHENIX.C,v 1.9 2014/09/23 15:00:10 mccumber Exp $$

int Min_cemc_layer = -1;
int Max_cemc_layer = -1;
int Min_hcal_layer = -1;
int Max_hcal_layer = -1;
int Min_tpc_layer = -1;
int Max_tpc_layer = -1;

//! ePHENIX simulation loading script
//! \param[in] nEvents Number of events to run. If nEvents=1, then a event display will be shown
//! \param[in] outputFile output for G4DSTReader
//! \param[in] inputFile HepMC input files, not activated by default
int
Fun4All_G4_ePHENIX( //
//        int nEvents = 0, //
    //    int nEvents = 1, //
    int nEvents = 2, //
//    int nEvents = 100000, //
    const char * outputFile = "G4ePHENIX.root", //
    const char * inputFile = "MyPythia.dat" //
    )
{
  //---------------
  // Load libraries
  //---------------

  const bool readhepmc = false; // read HepMC files
  const int absorberactive = 1; // set to 1 to make all absorbers active volumes
  const bool verbosity = false; // very slow but very detailed logs
//  const bool verbosity = true; // very slow but very detailed logs

  gSystem->Load("libg4testbench.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libcemc.so");
  gSystem->Load("libg4eval.so");

  gROOT->LoadMacro("G4Setup_ePHENIX.C");

  G4Init(); // initialize layer numbers for barrel

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  //-----------------
  // Event generation
  //-----------------
  if (readhepmc)
    {
      // this module is needed to read the HepMC records into our G4 sims
      // but only if you read HepMC input files
      HepMCNodeReader *hr = new HepMCNodeReader();
      se->registerSubsystem(hr);
    }
  else
    {
      // The PHG4ParticleGun runs the same particle(s) in
      // every event

      PHG4ParticleGun *gun = new PHG4ParticleGun();
//          gun->set_name("geantino+");
//      gun->set_name("chargedgeantino");
      gun->set_name("mu-");
//      gun->set_name("pi-"); // e,pi,mu,p,gamma
      //    gun->set_vtx(0,12.09,200);
      gun->set_vtx(0, 0, 0);
//        gun->set_mom(0, 0, 10);
      gun->set_mom(1.12641e-16, 1.83962, 13.6021);
      se->registerSubsystem(gun);

      PHG4ParticleGenerator *gen = new PHG4ParticleGenerator();

      //      gen->set_seed(TRandom3(0).GetSeed());
      gen->set_seed(1234);
//      gen->set_name("geantino");
      gen->set_name("mu-");
//      gun->set_name("pi-"); // e,pi,mu,p,gamma
      gen->set_vtx(0, 0, 0);
      gen->set_z_range(0, 0);
//      gen->set_eta_range(3.5, 3.5);
      gen->set_eta_range(-4, 4);
      gen->set_phi_range(TMath::Pi() / 2, TMath::Pi() / 2);
      gen->set_mom_range(1.0, 50.0);
      gen->Verbosity(1);
//      se->registerSubsystem(gen);
//
      //! high Q2 250x10 GeV pythia file from Kieran
      ReadEICFiles *eic = new ReadEICFiles();
      const char *infile =
          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/display/pythia.ep.250x010.10000000events.seed679586890.root";
      eic->OpenInputFile(infile);
      eic->SetFirstEntry(565);
//      se->registerSubsystem(eic);

    }
  //---------------------
  // Detector description from loaded macro
  //---------------------

  G4Setup(absorberactive, -1);
//  G4Setup_Sandbox(absorberactive, -1);

//  MaterialScan();
  //----------------------
  // Simulation evaluation
  //----------------------

//  SubsysReco* eval = new PHG4Evaluator("PHG4EVALUATOR","g4eval.root");
  //eval->Verbosity(0);
  //se->registerSubsystem( eval );

  //SubsysReco* eval = new PHG4CEMCEvaluator("PHG4CEMCEVALUATOR","out/g4eval.root");
  //eval->Verbosity(1);
  //se->registerSubsystem( eval );

  //     HitNtuples *hits = new HitNtuples();
  //        hits->AddNode("FEMCABS",0);
  //        hits->AddNode("FEMC",3);
  //        hits->AddNode("FPRESHOWER",1);
  //        hits->AddNode("GEM",2);
  //        hits->AddNode("GEMLIDFRONT",4);
  //        hits->AddNode("GEMLIDBACK",5);
  //    se->registerSubsystem(hits);

  //----------------------
  // Save selected nodes to root file
  //----------------------
    {
      // require offline/analysis/g4analysis_fsphenix

    PHG4DSTReader* ana = new PHG4DSTReader(outputFile);
      if (nEvents > 0 && nEvents < 3)
        ana->Verbosity(2);
      ana->AddNode("GEMSTATION0");
      ana->AddNode("GEMSTATION1");
      ana->AddNode("GEMSTATION2");
      ana->AddNode("GEMSTATION3");
      ana->AddNode("GEMSTATION4");
//      ana->AddNode("TestDetector_0");
      se->registerSubsystem(ana);

    }

  //--------------
  // IO management
  //--------------

  if (readhepmc)
    {
      Fun4AllInputManager *in = new Fun4AllHepMCInputManager("DSTIN");
      se->registerInputManager(in);
      se->fileopen(in->Name(), inputFile);
    }
  else
    {
      // for single particle generators we just need something which drives
      // the event loop, the Dummy Input Mgr does just that
      Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
      se->registerInputManager(in);
    }

  // File Managers
  bool save_dst = false;
  if (save_dst)
    {
      Fun4AllDstOutputManager *dstManager = new Fun4AllDstOutputManager(
          "DSTOUT", "DST.root");
      //FVTX nodes
      dstManager->AddNode("PHG4INEVENT");
      dstManager->AddNode("G4TruthInfo");

      se->registerOutputManager(dstManager);
    }

  //-----------------
  // Event processing
  //-----------------

  if (nEvents == 1)
    {
      PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
      g4->ApplyCommand("/control/execute eic.mac");
//      g4->StartGui();
      se->run(1);

      se->End();
      std::cout << "All done" << std::endl;
    }
  else
    {
      if (verbosity)
        {
          se->Verbosity(3);
          PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
          g4->Verbosity(3);
          g4->ApplyCommand("/control/verbose 5");
          g4->ApplyCommand("/run/verbose  5");
          g4->ApplyCommand("/tracking/verbose 5");
        }

      se->run(nEvents);

      se->End();
      std::cout << "All done" << std::endl;
      delete se;
      gSystem->Exit(0);
    }

  //   //-----
  //   // Exit
  //   //-----

  return 0;
}

void
G4Cmd(const char * cmd)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
  g4->ApplyCommand(cmd);
}

void
MaterialScan()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");

  g4->ApplyCommand("/control/matScan/phi 4 -22.5 45 deg");
  g4->ApplyCommand("/control/matScan/phi 5 -22.5 45 deg");
  g4->ApplyCommand("/control/matScan/theta 90 0 180 deg");
  g4->ApplyCommand("/control/matScan/scan");

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
Get_Min_hcal_layer()
{
  return Min_hcal_layer;
}

int
Get_Max_hcal_layer()
{
  return Max_hcal_layer;
}

