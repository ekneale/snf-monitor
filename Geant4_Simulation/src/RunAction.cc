
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "G4AnalysisManager.hh"
#include "G4AccumulableManager.hh"
#include "G4GenericAnalysisManager.hh"

#include <iomanip>
#include <string>
#include <fstream>
#include <map>
#include <vector>

#include "Randomize.hh"   
#include <chrono>         
#include <cstdlib>        
#include <cmath>

namespace G4_BREMS
{

    G4_BREMS::RunAction::RunAction(SteppingAction *steppingAction)
        : G4UserRunAction(),
          fTileCount(0), fCladCount(0), fCoreCount(0), fSipmCount(0), fOtherCount(0),
          fPhotonsEnteredFiber(0), fPhotonsExitedFiber(0), fPhotonsAbsorbedFiber(0),
          fAccTileCount("TileCount", 0),
          fAccCladCount("CladCount", 0),
          fAccCoreCount("CoreCount", 0),
          fAccSipmCount("SipmCount", 0),
          fAccOtherCount("OtherCount", 0),
          fAccPhotonsEnteredFiber("PhotonsEnteredFiber", 0),
          fAccPhotonsExitedFiber("PhotonsExitedFiber", 0),
          fAccPhotonsAbsorbedFiber("PhotonsAbsorbedFiber", 0),
          fSteppingAction(steppingAction)
    {
        std::vector<G4String> volumes = {"Tile", "FiberCore", "FiberClad", "Sipm"};
        for (const auto &volume : volumes)
        {
            auto name = volume + "_Creation_Cerenkov";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Creation_Scintillation";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Creation_OpWLS";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);

            name = volume + "_Interaction_OpAbsorption";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Interaction_OpWLS";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Interaction_Transportation";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
        }

        G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->Register(fAccTileCount);
        accumulableManager->Register(fAccCladCount);
        accumulableManager->Register(fAccCoreCount);
        accumulableManager->Register(fAccSipmCount);
        accumulableManager->Register(fAccOtherCount);
        accumulableManager->Register(fAccPhotonsEnteredFiber);
        accumulableManager->Register(fAccPhotonsExitedFiber);
        accumulableManager->Register(fAccPhotonsAbsorbedFiber);

        for (auto &pair : fAccCreationCounts)
        {
            accumulableManager->Register(*pair.second);
        }
        for (auto &pair : fAccInteractionCounts)
        {
            accumulableManager->Register(*pair.second);
        }
        
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetVerboseLevel(1);
        analysisManager->SetDefaultFileType("root");
        analysisManager->SetNtupleMerging(true);

        analysisManager->CreateH1("edep", "Energy Deposition Distribution", 100, 0., 2.0E-5 * CLHEP::MeV);
        analysisManager->SetH1XAxisTitle(0, "Energy Deposition [MeV]");
        analysisManager->SetH1YAxisTitle(0, "Counts");
       
	// TODO move this to a separate function
        if (generate_histograms)
        {
            // create histograms
            analysisManager->CreateH1("edep", "Energy Deposition Distribution",
                                      100, 0., 2.0E-5 * CLHEP::MeV);
            analysisManager->SetH1XAxisTitle(0, "Energy Deposition [MeV]");
            analysisManager->SetH1YAxisTitle(0, "Counts");

            analysisManager->CreateH1("time", "Time Distribution",
                                      100, 0., 3.0);
            analysisManager->SetH1XAxisTitle(1, "Photon Time [ns]");
            analysisManager->SetH1YAxisTitle(1, "Counts");

            // create 2D histograms
            analysisManager->CreateH2("timing_xy", "XY Timing",
                                      100, -400., 300., 100, -400., 300.);
            analysisManager->SetH2XAxisTitle(0, "x [mm]");
            analysisManager->SetH2YAxisTitle(0, "y [mm]");
            analysisManager->SetH2ZAxisTitle(0, "Time [ns]");

            analysisManager->CreateH2("timing_yz", "YZ Timing",
                                      // 100, -400., 300., 100, -10., 10.);
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(1, "y [mm]");
            analysisManager->SetH2YAxisTitle(1, "z [mm]");
            analysisManager->SetH2ZAxisTitle(1, "Time [ns]");

            analysisManager->CreateH2("timing_xz", "XZ Timing",
                                      // 100, -400., 300., 100, -10., 10.);
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(2, "x [mm]");
            analysisManager->SetH2YAxisTitle(2, "z [mm]");
            analysisManager->SetH2ZAxisTitle(2, "Time [ns]");

            analysisManager->CreateH2("edep_xy", "XY Energy Deposition",
                                      100, -400., 300., 100, -400., 300.);
            analysisManager->SetH2XAxisTitle(3, "x [mm]");
            analysisManager->SetH2YAxisTitle(3, "y [mm]");
            analysisManager->SetH2ZAxisTitle(3, "Energy [MeV]");

            analysisManager->CreateH2("edep_yz", "YZ Energy Deposition",
                                      // 100, -400., 300., 100, -10., 10.);
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(4, "y [mm]");
            analysisManager->SetH2YAxisTitle(4, "z [mm]");
            analysisManager->SetH2ZAxisTitle(4, "Energy [MeV]");

            analysisManager->CreateH2("edep_xz", "XZ Energy Deposition",
                                      // 100, -400., 300., 100, -10., 10.);
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(5, "x [mm]");
            analysisManager->SetH2YAxisTitle(5, "z [mm]");
            analysisManager->SetH2ZAxisTitle(5, "Energy [MeV]");

            analysisManager->CreateH2("clad_xy", "Cladding XY",
                                      100, -400., 300., 100, -400., 300.);
            analysisManager->SetH2XAxisTitle(6, "x [mm]");
            analysisManager->SetH2YAxisTitle(6, "y [mm]");
            analysisManager->SetH2ZAxisTitle(6, "Energy [MeV]");

            analysisManager->CreateH2("clad_yz", "Cladding YZ",
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(7, "y [mm]");
            analysisManager->SetH2YAxisTitle(7, "z [mm]");
            analysisManager->SetH2ZAxisTitle(7, "Energy [MeV]");

            analysisManager->CreateH2("clad_xz", "Cladding XZ",
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(8, "x [mm]");
            analysisManager->SetH2YAxisTitle(8, "z [mm]");
            analysisManager->SetH2ZAxisTitle(8, "Energy [MeV]");

            analysisManager->CreateH2("core_xy", "Core XY",
                                      100, -400., 300., 100, -400., 300.);
            analysisManager->SetH2XAxisTitle(9, "x [mm]");
            analysisManager->SetH2YAxisTitle(9, "y [mm]");
            analysisManager->SetH2ZAxisTitle(9, "Energy [MeV]");

            analysisManager->CreateH2("core_yz", "Core YZ",
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(10, "y [mm]");
            analysisManager->SetH2YAxisTitle(10, "z [mm]");
            analysisManager->SetH2ZAxisTitle(10, "Energy [MeV]");

            analysisManager->CreateH2("core_xz", "Core XZ",
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(11, "x [mm]");
            analysisManager->SetH2YAxisTitle(11, "z [mm]");
            analysisManager->SetH2ZAxisTitle(11, "Energy [MeV]");

            analysisManager->CreateH1("PhotonEnergyBeforeWLS", "Photon Energy Before WLS",
                                      100, 2.0, 3.5);
            analysisManager->SetH1XAxisTitle(2, "Energy [eV]");
            analysisManager->SetH1YAxisTitle(2, "Counts");

            analysisManager->CreateH1("PhotonEnergyAfterWLS", "Photon Energy After WLS",
                                      100, 2.0, 3.5);
            analysisManager->SetH1XAxisTitle(3, "Energy [eV]");
            analysisManager->SetH1YAxisTitle(3, "Counts");

            analysisManager->CreateH1("PhotonWavelengthBeforeWLS", "Photon Wavelength Before WLS",
                                      100, 350., 600.);
            analysisManager->SetH1XAxisTitle(4, "Wavelength [nm]");
            analysisManager->SetH1YAxisTitle(4, "Counts");

            analysisManager->CreateH1("PhotonWavelengthAfterWLS", "Photon Wavelength After WLS",
                                      100, 350., 600.);
            analysisManager->SetH1XAxisTitle(5, "Wavelength [nm]");
            analysisManager->SetH1YAxisTitle(5, "Counts");

            analysisManager->CreateH1("CladdingWavelength", "Photon Wavelength in Cladding",
                                      100, 300., 600.);
            analysisManager->SetH1XAxisTitle(6, "Wavelength [nm]");
            analysisManager->SetH1YAxisTitle(6, "Counts");

            analysisManager->CreateH1("CladdingEnergy", "Photon Energy in Cladding",
                                      100, 1.5, 4.1);
            analysisManager->SetH1XAxisTitle(7, "Energy [eV]");
            analysisManager->SetH1YAxisTitle(7, "Counts");

            analysisManager->CreateH1("CoreWavelength", "Photon Wavelength in Core",
                                      100, 300., 600.);
            analysisManager->SetH1XAxisTitle(8, "Wavelength [nm]");
            analysisManager->SetH1YAxisTitle(8, "Counts");

            analysisManager->CreateH1("CoreEnergy", "Photon Energy in Core",
                                      100, 1.5, 4.1);
            analysisManager->SetH1XAxisTitle(9, "Energy [eV]");
            analysisManager->SetH1YAxisTitle(9, "Counts");

            analysisManager->CreateH1("WLSEmissionSpectrum", "WLS Emission Spectrum",
                                      200, 300., 600.);
            analysisManager->SetH1XAxisTitle(10, "Wavelength [nm]");
            analysisManager->SetH1YAxisTitle(10, "Counts");

            analysisManager->CreateH1("SipmTimeSpectrum", "Sipm Time Spectrum",
                                      100, 0., 300.0);
            analysisManager->SetH1XAxisTitle(11, "Time [ns]");
            analysisManager->SetH1YAxisTitle(11, "Counts");

            analysisManager->CreateH1("SipmWavelength", "Photon Wavelenght in Sipm",
                                      200, 300, 600);
            analysisManager->SetH1XAxisTitle(12, "Wavelength [nm]");
            analysisManager->SetH1YAxisTitle(12, "Counts");

            analysisManager->CreateH2("Sipm_Timing_xy", "Sipm XY Timing",
                                      100, -400., 300., 100, -400., 300.);
            analysisManager->SetH2XAxisTitle(12, "x [mm]");
            analysisManager->SetH2YAxisTitle(12, "y [mm]");
            analysisManager->SetH2ZAxisTitle(12, "Time [ns]");

            analysisManager->CreateH2("Sipm_Timing_yz", "Sipm YZ Timing",
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(13, "y [mm]");
            analysisManager->SetH2YAxisTitle(13, "z [mm]");
            analysisManager->SetH2ZAxisTitle(13, "Time [ns]");

            analysisManager->CreateH2("Sipm_Timing_xz", "Sipm XZ Timing",
                                      100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(14, "x [mm]");
            analysisManager->SetH2YAxisTitle(14, "z [mm]");
            analysisManager->SetH2ZAxisTitle(14, "Time [ns]");

            analysisManager->CreateH1("Sipm_Timing_NeutronCapture", "Sipm Timing NeutronCapture",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(13, "Time [ns]");
            analysisManager->SetH1YAxisTitle(13, "Counts");

        
            analysisManager->CreateH1("Sipm_Timing_Annihilation", "Sipm Timing Annihilation",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(14, "Time [ns]");
            analysisManager->SetH1YAxisTitle(14, "Counts");

            analysisManager->CreateH1("Sipm_Timing_NeutronCapture", "Sipm Timing NeutronCapture",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(13, "Time [ns]");
            analysisManager->SetH1YAxisTitle(13, "Counts");

        
            analysisManager->CreateH1("Sipm_Timing_Annihilation", "Sipm Timing Annihilation",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(14, "Time [ns]");
            analysisManager->SetH1YAxisTitle(14, "Counts");

            analysisManager->CreateH1("Sipm_Hit_XPosition", "Sipm Hit XPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(15, "X_hitPosition");
            analysisManager->SetH1YAxisTitle(15, "Counts");

            analysisManager->CreateH1("Sipm_Hit_YPosition", "Sipm Hit YPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(16, "Y_hitPosition");
            analysisManager->SetH1YAxisTitle(16, "Counts");

            analysisManager->CreateH1("Sipm_Hit_ZPosition", "Sipm Hit ZPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(17, "Z_hitPosition");
            analysisManager->SetH1YAxisTitle(17, "Counts");

            analysisManager->CreateH1("Timing_Annihilation", "Timing Annihilation",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(18, "Time [ns]");
            analysisManager->SetH1YAxisTitle(18, "Counts");

            analysisManager->CreateH1("Annihilation_XPosition", "Annihilation XPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(19, "X_hitPosition");
            analysisManager->SetH1YAxisTitle(19, "Counts");

            analysisManager->CreateH1("Annihilation_YPosition", "Annihilation YPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(20, "Y_hitPosition");
            analysisManager->SetH1YAxisTitle(20, "Counts");

            analysisManager->CreateH1("Annihilation_ZPosition", "Annihilation ZPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(21, "Z_hitPosition");
            analysisManager->SetH1YAxisTitle(21, "Counts");
        

            analysisManager->CreateH1("Timing_Neutron_Capture", "Timing Neutron_Capture",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(22, "Time [ns]");
            analysisManager->SetH1YAxisTitle(22, "Counts");

            analysisManager->CreateH1("Neutron_Capture_XPosition", "Neutron_Capture XPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(23, "X_hitPosition");
            analysisManager->SetH1YAxisTitle(23, "Counts");

            analysisManager->CreateH1("Neutron_Capture_YPosition", "Neutron_Capture YPosition",
                                        200, 300, 600);
            analysisManager->SetH1XAxisTitle(24, "Y_hitPosition");
            analysisManager->SetH1YAxisTitle(24, "Counts");

            analysisManager->CreateH1("Neutron_Capture_ZPosition", "Neutron_Capture ZPosition",
                200, 300, 600);
            analysisManager->SetH1XAxisTitle(25, "Z_hitPosition");
            analysisManager->SetH1YAxisTitle(25, "Counts");

            analysisManager->CreateH2("Sipm_Timing_NeutronCapture_xy", "Sipm Timing NeutronCapture XY",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(15, "x [mm]");
            analysisManager->SetH2YAxisTitle(15, "y [mm]");
            analysisManager->SetH2ZAxisTitle(15, "Time [ns]");

            analysisManager->CreateH2("Sipm_Timing_NeutronCapture_yz", "Sipm Timing NeutronCapture YZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(16, "y [mm]");
            analysisManager->SetH2YAxisTitle(16, "z [mm]");
            analysisManager->SetH2ZAxisTitle(16, "Time [ns]");

            analysisManager->CreateH2("Sipm_Timing_NeutronCapture_xz", "Sipm Timing NeutronCapture XZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(17, "x [mm]");
            analysisManager->SetH2YAxisTitle(17, "z [mm]");
            analysisManager->SetH2ZAxisTitle(17, "Time [ns]");

            analysisManager->CreateH2("Sipm_Timing_Annihilation_xy", "Sipm Timing Annihilation XY",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(18, "x [mm]");
            analysisManager->SetH2YAxisTitle(18, "y [mm]");
            analysisManager->SetH2ZAxisTitle(18, "Time [ns]");

            analysisManager->CreateH2("Sipm_Timing_Annihilation_yz", "Sipm Timing Annihilation YZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(19, "y [mm]");
            analysisManager->SetH2YAxisTitle(19, "z [mm]");
            analysisManager->SetH2ZAxisTitle(19, "Time [ns]");

            analysisManager->CreateH2("Sipm_Timing_Annihilation_xz", "Sipm Timing Annihilation XZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(20, "x [mm]");
            analysisManager->SetH2YAxisTitle(20, "z [mm]");
            analysisManager->SetH2ZAxisTitle(20, "Time [ns]"); 
            
            analysisManager->CreateH2("Annihilation_xy", "Annihilation XY",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(21, "x [mm]");
            analysisManager->SetH2YAxisTitle(21, "y [mm]");
            analysisManager->SetH2ZAxisTitle(21, "Time [ns]");

            analysisManager->CreateH2("Annihilation_yz", "Annihilation YZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(22, "y [mm]");
            analysisManager->SetH2YAxisTitle(22, "z [mm]");
            analysisManager->SetH2ZAxisTitle(22, "Time [ns]");

            analysisManager->CreateH2("Annihilation_xz", "Annihilation XZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(23, "x [mm]");
            analysisManager->SetH2YAxisTitle(23, "z [mm]");
            analysisManager->SetH2ZAxisTitle(23, "Time [ns]"); 

            analysisManager->CreateH2("NeutronCapture_xy", "NeutronCapture XY",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(24, "x [mm]");
            analysisManager->SetH2YAxisTitle(24, "y [mm]");
            analysisManager->SetH2ZAxisTitle(24, "Time [ns]");

            analysisManager->CreateH2("NeutronCapture_yz", "NeutronCapture YZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(25, "y [mm]");
            analysisManager->SetH2YAxisTitle(25, "z [mm]");
            analysisManager->SetH2ZAxisTitle(25, "Time [ns]");

            analysisManager->CreateH2("NeutronCapture_xz", "NeutronCapture XZ",
                100, -400., 300., 100, -40., 40.);
            analysisManager->SetH2XAxisTitle(26, "x [mm]");
            analysisManager->SetH2YAxisTitle(26, "z [mm]");
            analysisManager->SetH2ZAxisTitle(26, "Time [ns]"); 

        } // endif generate_histograms

        analysisManager->CreateNtuple("simdata", "MC truth tree");
        analysisManager->CreateNtupleIColumn("runID");                     // column ID = 0
        analysisManager->CreateNtupleIColumn("eventID");                   // column ID = 1
	// IBD event
        analysisManager->CreateNtupleDColumn("nu_energy");                 // column ID = 2
        analysisManager->CreateNtupleDColumn("nu_dir_x");                  // column ID = 3
        analysisManager->CreateNtupleDColumn("nu_dir_y");                  // column ID = 4
        analysisManager->CreateNtupleDColumn("nu_dir_z");                  // column ID = 5
        analysisManager->CreateNtupleDColumn("vtx_x");                     // column ID = 6
        analysisManager->CreateNtupleDColumn("vtx_y");                     // column ID = 7
        analysisManager->CreateNtupleDColumn("vtx_z");                     // column ID = 8
        analysisManager->CreateNtupleDColumn("positron_energy");           // column ID = 9
        analysisManager->CreateNtupleDColumn("positron_dir_x");            // column ID = 10
        analysisManager->CreateNtupleDColumn("positron_dir_y");            // column ID = 11
        analysisManager->CreateNtupleDColumn("positron_dir_z");            // column ID = 12
        analysisManager->CreateNtupleDColumn("neutron_energy");            // column ID = 13
        analysisManager->CreateNtupleDColumn("neutron_dir_x");             // column ID = 14
        analysisManager->CreateNtupleDColumn("neutron_dir_y");             // column ID = 15
        analysisManager->CreateNtupleDColumn("neutron_dir_z");             // column ID = 16
	// All hits
        analysisManager->CreateNtupleIColumn("n_sipm_hits");               // column ID = 17
        analysisManager->CreateNtupleDColumn("sipm_x");                    // column ID = 18
        analysisManager->CreateNtupleDColumn("sipm_y");                    // column ID = 19
        analysisManager->CreateNtupleDColumn("sipm_z");                    // column ID = 20
        analysisManager->CreateNtupleDColumn("sipm_t");                    // column ID = 21
        analysisManager->CreateNtupleDColumn("sipm_q");                    // column ID = 22
        analysisManager->CreateNtupleDColumn("sipm_wl");                   // column ID = 23
	// Neutron capture
        analysisManager->CreateNtupleDColumn("vtx_x_ncapture");            // column ID = 24
        analysisManager->CreateNtupleDColumn("vtx_y_ncapture");            // column ID = 25
        analysisManager->CreateNtupleDColumn("vtx_z_ncapture");            // column ID = 26
        analysisManager->CreateNtupleDColumn("t_ncapture");                // column ID = 27
        analysisManager->CreateNtupleIColumn("n_sipm_hits_ncapture");      // column ID = 28
        analysisManager->CreateNtupleDColumn("sipm_x_ncapture");           // column ID = 29
        analysisManager->CreateNtupleDColumn("sipm_y_ncapture");           // column ID = 30
        analysisManager->CreateNtupleDColumn("sipm_z_ncapture");           // column ID = 31
        analysisManager->CreateNtupleDColumn("sipm_t_ncapture");           // column ID = 32
        analysisManager->CreateNtupleDColumn("sipm_q_ncapture");           // column ID = 33
        analysisManager->CreateNtupleDColumn("sipm_wl_ncapture");          // column ID = 34
	// Annihilation
        analysisManager->CreateNtupleDColumn("vtx_x_annihilation");        // column ID = 35 
        analysisManager->CreateNtupleDColumn("vtx_y_annihilation");        // column ID = 36
        analysisManager->CreateNtupleDColumn("vtx_z_annihilation");        // column ID = 37
        analysisManager->CreateNtupleDColumn("t_annihilation");            // column ID = 38
        analysisManager->CreateNtupleIColumn("n_sipm_hits_annihilation");  // column ID = 39
        analysisManager->CreateNtupleDColumn("sipm_x_annihilation");       // column ID = 40
        analysisManager->CreateNtupleDColumn("sipm_y_annihilation");       // column ID = 41
        analysisManager->CreateNtupleDColumn("sipm_z_annihilation");       // column ID = 42
        analysisManager->CreateNtupleDColumn("sipm_t_annihilation");       // column ID = 43
        analysisManager->CreateNtupleDColumn("sipm_q_annihilation");       // column ID = 44
        analysisManager->CreateNtupleDColumn("sipm_wl_annihilation");      // column ID = 45

        analysisManager->FinishNtuple();

    }

    G4_BREMS::RunAction::~RunAction()
    {
        for (auto &pair : fAccCreationCounts)
        {
            delete pair.second;
        }
        for (auto &pair : fAccInteractionCounts)
        {
            delete pair.second;
        }
    }

    void G4_BREMS::RunAction::BeginOfRunAction(const G4Run *run)
    {
        G4AccumulableManager::Instance()->Reset();

        if (fSteppingAction)
        {
            fSteppingAction->ClearHits();
            fSteppingAction->ClearEvents();
        }

        long s1 = 0, s2 = 0;

	
        if (const char* e1 = std::getenv("G4_SEED1")) s1 = std::strtol(e1, nullptr, 10);
        if (const char* e2 = std::getenv("G4_SEED2")) s2 = std::strtol(e2, nullptr, 10);
        if (s1 == 0 || s2 == 0) {
            auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            long base = static_cast<long>(now) ^ (static_cast<long>(run->GetRunID()) << 24);
            if (s1 == 0) s1 = std::llabs(base) + 11;
            if (s2 == 0) s2 = std::llabs(base * 1812433253L + 12345L) + 17;
        }

        long seeds[2] = { s1, s2 };
        if (G4Threading::IsMasterThread()) {
             G4Random::setTheSeeds(seeds, 2);
             G4cout << "Run " << run->GetRunID()
               << " using RNG seeds: " << s1 << " , " << s2 << G4endl;
        }

        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->Reset();
        //TODO set file name in macro
        analysisManager->OpenFile("nuwim_output.root");
        
    }

    void G4_BREMS::RunAction::AddProcessCount(const G4String &volume,
                                              const G4String &processName, bool isCreationProcess)
    {

        G4String key = volume + (isCreationProcess ? "_Creation_" : "_Interaction_") + processName;

        if (isCreationProcess)
        {
            auto it = fAccCreationCounts.find(key);
            if (it != fAccCreationCounts.end())
            {
                *(it->second) += 1;
            }
        }
        else
        {
            auto it = fAccInteractionCounts.find(key);
            if (it != fAccInteractionCounts.end())
            {
                *(it->second) += 1;
            }
        }
    }

    G4double G4_BREMS::RunAction::CalculateTrappingEfficiency() const
    {
        G4double photonsEntered = fAccPhotonsEnteredFiber.GetValue();
        G4double photonsAbsorbed = fAccPhotonsAbsorbedFiber.GetValue();
        if (photonsEntered == 0)
            return 0.0;
        return photonsAbsorbed / photonsEntered;
    }

    void G4_BREMS::RunAction::EndOfRunAction(const G4Run *run)
    {
        G4AccumulableManager::Instance()->Merge();
        if (write_to_csv)
        {
            if (G4Threading::IsMasterThread())
            {
                fTileCount = fAccTileCount.GetValue();
                fCladCount = fAccCladCount.GetValue();
                fCoreCount = fAccCoreCount.GetValue();
                fSipmCount = fAccSipmCount.GetValue();
                fOtherCount = fAccOtherCount.GetValue();
                fPhotonsEnteredFiber = fAccPhotonsEnteredFiber.GetValue();
                fPhotonsAbsorbedFiber = fAccPhotonsAbsorbedFiber.GetValue();
                if (debug_runaction)
                {
                    G4cout << "Tile Count: " << fTileCount << G4endl;
                    G4cout << "Clad Count: " << fCladCount << G4endl;
                    G4cout << "Core Count: " << fCoreCount << G4endl;
                    G4cout << "Sipm Count: " << fSipmCount << G4endl;
                    G4cout << "Other Count: " << fOtherCount << G4endl;

                    G4cout << "Trapping Efficiency: " << CalculateTrappingEfficiency() * 100.0 << " %" << G4endl;
                }

                std::vector<G4String> volumes = {"Tile", "FiberCore", "FiberClad", "Sipm"};

                for (const auto &volume : volumes)
                {
                    if (debug_runaction)
                    {
                        G4cout << "\nVolume: " << volume << G4endl;

                        // creation processes
                        G4cout << "Creation Processes:" << G4endl;
                    }
                }
            }
            
        }
        
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->Write();
        analysisManager->CloseFile();   

    }

} // namespace G4_BREMS
