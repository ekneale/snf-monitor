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

namespace G4_BREMS {

    G4_BREMS::RunAction::RunAction(SteppingAction* steppingAction)
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
        std::vector<G4String> volumes = { "Tile", "FiberCore", "FiberClad", "Sipm" };
        for (const auto& volume : volumes) {
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
        accumulableManager->RegisterAccumulable(fAccTileCount);
        accumulableManager->RegisterAccumulable(fAccCladCount);
        accumulableManager->RegisterAccumulable(fAccCoreCount);
        accumulableManager->RegisterAccumulable(fAccSipmCount);
        accumulableManager->RegisterAccumulable(fAccOtherCount);
        accumulableManager->RegisterAccumulable(fAccPhotonsEnteredFiber);
        accumulableManager->RegisterAccumulable(fAccPhotonsExitedFiber);
        accumulableManager->RegisterAccumulable(fAccPhotonsAbsorbedFiber);

        for (auto& pair : fAccCreationCounts) {
            accumulableManager->RegisterAccumulable(*pair.second);
        }
        for (auto& pair : fAccInteractionCounts) {
            accumulableManager->RegisterAccumulable(*pair.second);
        }
        
        //auto man = G4GenericAnalysisManager::Instance();
        auto man = G4AnalysisManager::Instance();
        man->SetVerboseLevel(1);
        //man->SetFileName("H1:Sipm_Hits1, H2:Sipm_Hits2, NT:NeutronCaptureSipmHits");
        man->SetFileName("SipmHits");
        //man->SetFileName("Sipm_Hits1");
        man->SetDefaultFileType("root");
        man->SetNtupleMerging(true);

        man->CreateH1("edep", "Energy Deposition Distribution", 100, 0., 2.0E-5 * CLHEP::MeV);
        man->SetH1XAxisTitle(0, "Energy Deposition [MeV]");
        man->SetH1YAxisTitle(0, "Counts");

        man->CreateH1("time", "Time Distribution", 100, 0., 3.0);
        man->SetH1XAxisTitle(1, "Photon Time [ns]");
        man->SetH1YAxisTitle(1, "Counts");

        man->CreateH1("PhotonEnergyBeforeWLS", "Photon Energy Before WLS",
            100, 2.0, 3.5);
        man->SetH1XAxisTitle(2, "Energy [eV]");
        man->SetH1YAxisTitle(2, "Counts");

        man->CreateH1("PhotonEnergyAfterWLS", "Photon Energy After WLS",
            100, 2.0, 3.5);
        man->SetH1XAxisTitle(3, "Energy [eV]");
        man->SetH1YAxisTitle(3, "Counts");

        man->CreateH1("PhotonWavelengthBeforeWLS", "Photon Wavelength Before WLS",
            100, 350., 600.);
        man->SetH1XAxisTitle(4, "Wavelength [nm]");
        man->SetH1YAxisTitle(4, "Counts");

        man->CreateH1("PhotonWavelengthAfterWLS", "Photon Wavelength After WLS",
            100, 350., 600.);
        man->SetH1XAxisTitle(5, "Wavelength [nm]");
        man->SetH1YAxisTitle(5, "Counts");
        

        man->CreateH1("CladdingWavelength", "Photon Wavelength in Cladding",
            100, 300., 600.);
        man->SetH1XAxisTitle(6, "Wavelength [nm]");
        man->SetH1YAxisTitle(6, "Counts");

        man->CreateH1("CladdingEnergy", "Photon Energy in Cladding",
            100, 1.5, 4.1);
        man->SetH1XAxisTitle(7, "Energy [eV]");
        man->SetH1YAxisTitle(7, "Counts");

        man->CreateH1("CoreWavelength", "Photon Wavelength in Core",
            100, 300., 600.);
        man->SetH1XAxisTitle(8, "Wavelength [nm]");
        man->SetH1YAxisTitle(8, "Counts");


        man->CreateH1("CoreEnergy", "Photon Energy in Core",
            100, 1.5, 4.1);
        man->SetH1XAxisTitle(9, "Energy [eV]");
        man->SetH1YAxisTitle(9, "Counts");

        man->CreateH1("WLSEmissionSpectrum", "WLS Emission Spectrum",
            200, 300., 600.);
        man->SetH1XAxisTitle(10, "Wavelength [nm]");
        man->SetH1YAxisTitle(10, "Counts");

        man->CreateH1("SipmTimeSpectrum", "Sipm Time Spectrum",
            100, 0., 300.0);
        man->SetH1XAxisTitle(11, "Time [ns]");
        man->SetH1YAxisTitle(11, "Counts");


        man->CreateH1("SipmWavelength", "Photon Wavelenght in Sipm",
            200, 300, 600);
        man->SetH1XAxisTitle(12, "Wavelength [nm]");
        man->SetH1YAxisTitle(12, "Counts");

        man->CreateH1("Sipm_Timing_NeutronCapture", "Sipm Timing NeutronCapture",
            200, 300, 600);
        man->SetH1XAxisTitle(13, "Time [ns]");
        man->SetH1YAxisTitle(13, "Counts");

        
        man->CreateH1("Sipm_Timing_Annihilation", "Sipm Timing Annihilation",
            200, 300, 600);
        man->SetH1XAxisTitle(14, "Time [ns]");
        man->SetH1YAxisTitle(14, "Counts");

        man->CreateH1("Sipm_Hit_XPosition", "Sipm Hit XPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(15, "X_hitPosition");
        man->SetH1YAxisTitle(15, "Counts");

        man->CreateH1("Sipm_Hit_YPosition", "Sipm Hit YPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(16, "Y_hitPosition");
        man->SetH1YAxisTitle(16, "Counts");

        man->CreateH1("Sipm_Hit_ZPosition", "Sipm Hit ZPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(17, "Z_hitPosition");
        man->SetH1YAxisTitle(17, "Counts");

        man->CreateH1("Timing_Annihilation", "Timing Annihilation",
            200, 300, 600);
        man->SetH1XAxisTitle(18, "Time [ns]");
        man->SetH1YAxisTitle(18, "Counts");

        man->CreateH1("Annihilation_XPosition", "Annihilation XPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(19, "X_hitPosition");
        man->SetH1YAxisTitle(19, "Counts");

        man->CreateH1("Annihilation_YPosition", "Annihilation YPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(20, "Y_hitPosition");
        man->SetH1YAxisTitle(20, "Counts");

        man->CreateH1("Annihilation_ZPosition", "Annihilation ZPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(21, "Z_hitPosition");
        man->SetH1YAxisTitle(21, "Counts");
        

        man->CreateH1("Timing_Neutron_Capture", "Timing Neutron_Capture",
            200, 300, 600);
        man->SetH1XAxisTitle(22, "Time [ns]");
        man->SetH1YAxisTitle(22, "Counts");

        man->CreateH1("Neutron_Capture_XPosition", "Neutron_Capture XPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(23, "X_hitPosition");
        man->SetH1YAxisTitle(23, "Counts");

        man->CreateH1("Neutron_Capture_YPosition", "Neutron_Capture YPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(24, "Y_hitPosition");
        man->SetH1YAxisTitle(24, "Counts");

        man->CreateH1("Neutron_Capture_ZPosition", "Neutron_Capture ZPosition",
            200, 300, 600);
        man->SetH1XAxisTitle(25, "Z_hitPosition");
        man->SetH1YAxisTitle(25, "Counts");

        man->CreateH2("timing_xy", "XY Timing", 
            100, -400.,300., 100, -400.,300.);
        man->SetH2XAxisTitle(0, "x [mm]");
        man->SetH2YAxisTitle(0, "y [mm]");
        man->SetH2ZAxisTitle(0, "Time [ns]");
        
        man->CreateH2("timing_yz", "YZ Timing",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(1, "y [mm]");
        man->SetH2YAxisTitle(1, "z [mm]");
        man->SetH2ZAxisTitle(1, "Time [ns]");

        man->CreateH2("timing_xz", "XZ Timing",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(2, "x [mm]");
        man->SetH2YAxisTitle(2, "z [mm]");
        man->SetH2ZAxisTitle(2, "Time [ns]");

        man->CreateH2("edep_xy", "XY Energy Deposition",
            100, -400., 300., 100, -400., 300.);
        man->SetH2XAxisTitle(3, "x [mm]");
        man->SetH2YAxisTitle(3, "y [mm]");
        man->SetH2ZAxisTitle(3, "Energy [MeV]");

        man->CreateH2("edep_yz", "YZ Energy Deposition",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(4, "y [mm]");
        man->SetH2YAxisTitle(4, "z [mm]");
        man->SetH2ZAxisTitle(4, "Energy [MeV]");

        man->CreateH2("edep_xz", "XZ Energy Deposition",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(5, "x [mm]");
        man->SetH2YAxisTitle(5, "z [mm]");
        man->SetH2ZAxisTitle(5, "Energy [MeV]");

        man->CreateH2("clad_xy", "Cladding XY",
            100, -400., 300., 100, -400., 300.);
        man->SetH2XAxisTitle(6, "x [mm]");
        man->SetH2YAxisTitle(6, "y [mm]");
        man->SetH2ZAxisTitle(6, "Energy [MeV]");

        man->CreateH2("clad_yz", "Cladding YZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(7, "y [mm]");
        man->SetH2YAxisTitle(7, "z [mm]");
        man->SetH2ZAxisTitle(7, "Energy [MeV]");

        man->CreateH2("clad_xz", "Cladding XZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(8, "x [mm]");
        man->SetH2YAxisTitle(8, "z [mm]");
        man->SetH2ZAxisTitle(8, "Energy [MeV]");


        man->CreateH2("core_xy", "Core XY",
            100, -400., 300., 100, -400., 300.);
        man->SetH2XAxisTitle(9, "x [mm]");
        man->SetH2YAxisTitle(9, "y [mm]");
        man->SetH2ZAxisTitle(9, "Energy [MeV]");

        man->CreateH2("core_yz", "Core YZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(10, "y [mm]");
        man->SetH2YAxisTitle(10, "z [mm]");
        man->SetH2ZAxisTitle(10, "Energy [MeV]");

        man->CreateH2("core_xz", "Core XZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(11, "x [mm]");
        man->SetH2YAxisTitle(11, "z [mm]");
        man->SetH2ZAxisTitle(11, "Energy [MeV]");

        man->CreateH2("Sipm_Timing_xy", "Sipm XY Timing",
            100, -400., 300., 100, -400., 300.);
        man->SetH2XAxisTitle(12, "x [mm]");
        man->SetH2YAxisTitle(12, "y [mm]");
        man->SetH2ZAxisTitle(12, "Time [ns]");

        man->CreateH2("Sipm_Timing_yz", "Sipm YZ Timing",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(13, "y [mm]");
        man->SetH2YAxisTitle(13, "z [mm]");
        man->SetH2ZAxisTitle(13, "Time [ns]");

        man->CreateH2("Sipm_Timing_xz", "Sipm XZ Timing",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(14, "x [mm]");
        man->SetH2YAxisTitle(14, "z [mm]");
        man->SetH2ZAxisTitle(14, "Time [ns]");

        man->CreateH2("Sipm_Timing_NeutronCapture_xy", "Sipm Timing NeutronCapture XY",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(15, "x [mm]");
        man->SetH2YAxisTitle(15, "y [mm]");
        man->SetH2ZAxisTitle(15, "Time [ns]");

        man->CreateH2("Sipm_Timing_NeutronCapture_yz", "Sipm Timing NeutronCapture YZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(16, "y [mm]");
        man->SetH2YAxisTitle(16, "z [mm]");
        man->SetH2ZAxisTitle(16, "Time [ns]");

        man->CreateH2("Sipm_Timing_NeutronCapture_xz", "Sipm Timing NeutronCapture XZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(17, "x [mm]");
        man->SetH2YAxisTitle(17, "z [mm]");
        man->SetH2ZAxisTitle(17, "Time [ns]");

        man->CreateH2("Sipm_Timing_Annihilation_xy", "Sipm Timing Annihilation XY",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(18, "x [mm]");
        man->SetH2YAxisTitle(18, "y [mm]");
        man->SetH2ZAxisTitle(18, "Time [ns]");

        man->CreateH2("Sipm_Timing_Annihilation_yz", "Sipm Timing Annihilation YZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(19, "y [mm]");
        man->SetH2YAxisTitle(19, "z [mm]");
        man->SetH2ZAxisTitle(19, "Time [ns]");

        man->CreateH2("Sipm_Timing_Annihilation_xz", "Sipm Timing Annihilation XZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(20, "x [mm]");
        man->SetH2YAxisTitle(20, "z [mm]");
        man->SetH2ZAxisTitle(20, "Time [ns]"); 
        
        man->CreateH2("Annihilation_xy", "Annihilation XY",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(21, "x [mm]");
        man->SetH2YAxisTitle(21, "y [mm]");
        man->SetH2ZAxisTitle(21, "Time [ns]");

        man->CreateH2("Annihilation_yz", "Annihilation YZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(22, "y [mm]");
        man->SetH2YAxisTitle(22, "z [mm]");
        man->SetH2ZAxisTitle(22, "Time [ns]");

        man->CreateH2("Annihilation_xz", "Annihilation XZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(23, "x [mm]");
        man->SetH2YAxisTitle(23, "z [mm]");
        man->SetH2ZAxisTitle(23, "Time [ns]"); 

        man->CreateH2("NeutronCapture_xy", "NeutronCapture XY",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(24, "x [mm]");
        man->SetH2YAxisTitle(24, "y [mm]");
        man->SetH2ZAxisTitle(24, "Time [ns]");

        man->CreateH2("NeutronCapture_yz", "NeutronCapture YZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(25, "y [mm]");
        man->SetH2YAxisTitle(25, "z [mm]");
        man->SetH2ZAxisTitle(25, "Time [ns]");

        man->CreateH2("NeutronCapture_xz", "NeutronCapture XZ",
            100, -400., 300., 100, -40., 40.);
        man->SetH2XAxisTitle(26, "x [mm]");
        man->SetH2YAxisTitle(26, "z [mm]");
        man->SetH2ZAxisTitle(26, "Time [ns]"); 
        

        man->CreateNtuple("SipmHits","SipmHits");
        man->CreateNtupleDColumn(0, "SipmTimeSpectrum");
        man->CreateNtupleDColumn(0, "SipmWavelength");
        man->CreateNtupleDColumn(0, "Sipm_Hit_XPosition");
        man->CreateNtupleDColumn(0, "Sipm_Hit_YPosition");
        man->CreateNtupleDColumn(0, "Sipm_Hit_ZPosition");
        
        man->FinishNtuple(0);
        
        man->CreateNtuple("NeutronCaptureSipmHits","NeutronCaptureSipmHits");
        man->CreateNtupleIColumn(1, "SipmID");
        man->CreateNtupleDColumn(1, "X(mm)");
        man->CreateNtupleDColumn(1, "Y(mm)");
        man->CreateNtupleDColumn(1, "Z(mm)");
        man->CreateNtupleDColumn(1, "Time(ns)");
        man->FinishNtuple(1);
        
        man->SetNtupleFileName(0, "NeutronCaptureSipmHits");

        man->CreateNtuple("AnnihilationEvents","AnnihilationEvents");
        man->CreateNtupleDColumn(2, "X(mm)");
        man->CreateNtupleDColumn(2, "Y(mm)");
        man->CreateNtupleDColumn(2, "Z(mm)");
        man->CreateNtupleDColumn(2, "Time(ns)");
        //man->CreateNtupleDColumn(2, "Volume");
        man->FinishNtuple(2);
        
        man->SetNtupleFileName(1, "AnnihilationEvents");

        man->CreateNtuple("NeutronCaptureEvents","NeutronCaptureEvents");
        man->CreateNtupleDColumn(3, "X(mm)");
        man->CreateNtupleDColumn(3, "Y(mm)");
        man->CreateNtupleDColumn(3, "Z(mm)");
        man->CreateNtupleDColumn(3, "Time(ns)");
        //man->CreateNtupleDColumn(3, "Volume");
        man->FinishNtuple(3);
        
        man->SetNtupleFileName(2, "NeutronCaptureEvents");
       
        
    }

    G4_BREMS::RunAction::~RunAction() {
        for (auto& pair : fAccCreationCounts) {
            delete pair.second;
        }
        for (auto& pair : fAccInteractionCounts) {
            delete pair.second;
        }
    }

    void G4_BREMS::RunAction::BeginOfRunAction(const G4Run*) {
        //G4AccumulableManager::Instance()->Reset();
        G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->Reset();

        if (fSteppingAction) {
           fSteppingAction->ClearHits();
        }

        if (G4Threading::IsMasterThread()) {
            gSipmHits.clear();
            //fSipmHits.clear();
            gAnnihilationEvents.clear();
        }
        
        auto man = G4AnalysisManager::Instance();
        man->Reset();
        //analysisManager->OpenFile("Sipm_Hits.root");
        man->OpenFile();
        //man->OpenFile("Sipm_Hits1.root");
        //man->OpenFile("Sipm_Hits2.root");
        //man->SetFileName("Sipm_Hits1.root");

    }

    void G4_BREMS::RunAction::AddProcessCount(const G4String& volume,
        const G4String& processName, bool isCreationProcess) {

        G4String key = volume + (isCreationProcess ? "_Creation_" : "_Interaction_") + processName;

        if (isCreationProcess) {
            auto it = fAccCreationCounts.find(key);
            if (it != fAccCreationCounts.end()) {
                *(it->second) += 1;
            }
        }
        else {
            auto it = fAccInteractionCounts.find(key);
            if (it != fAccInteractionCounts.end()) {
                *(it->second) += 1;
            }
        }
    }

    G4double G4_BREMS::RunAction::CalculateTrappingEfficiency() const {
        G4double photonsEntered = fAccPhotonsEnteredFiber.GetValue();
        G4double photonsAbsorbed = fAccPhotonsAbsorbedFiber.GetValue();
        if (photonsEntered == 0) return 0.0;
        return photonsAbsorbed / photonsEntered;
    }

    void G4_BREMS::RunAction::EndOfRunAction(const G4Run* run) {
        G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->Merge();
        auto man = G4AnalysisManager::Instance();

        if (G4Threading::IsMasterThread()) {
            fTileCount = fAccTileCount.GetValue();
            fCladCount = fAccCladCount.GetValue();
            fCoreCount = fAccCoreCount.GetValue();
            fSipmCount = fAccSipmCount.GetValue();
            fOtherCount = fAccOtherCount.GetValue();
            fPhotonsEnteredFiber = fAccPhotonsEnteredFiber.GetValue();
            fPhotonsAbsorbedFiber = fAccPhotonsAbsorbedFiber.GetValue();

            G4cout << "Tile Count: " << fTileCount << G4endl;
            G4cout << "Clad Count: " << fCladCount << G4endl;
            G4cout << "Core Count: " << fCoreCount << G4endl;
            G4cout << "Sipm Count: " << fSipmCount << G4endl;
            G4cout << "Other Count: " << fOtherCount << G4endl;

            G4cout << "Trapping Efficiency: " << CalculateTrappingEfficiency() * 100.0 << " %" << G4endl;

            std::vector<G4String> volumes = { "Tile", "FiberCore", "FiberClad", "Sipm" };
            for (const auto& volume : volumes) {
                G4cout << "\nVolume: " << volume << G4endl;

                // creation processes
                G4cout << "Creation Processes:" << G4endl;
                std::vector<G4String> creationProcesses = { "Cerenkov", "Scintillation", "OpWLS" };
                for (const auto& process : creationProcesses) {
                    G4String key = volume + "_Creation_" + process;
                    auto it = fAccCreationCounts.find(key);
                    if (it != fAccCreationCounts.end()) {
                        G4cout << std::setw(15) << process << ": "
                            << it->second->GetValue() << G4endl;
                    }
                }

                // Interaction processes
                G4cout << "Interaction Processes:" << G4endl;
                std::vector<G4String> interactionProcesses = { "OpAbsorption", "OpWLS", "Transportation" };
                for (const auto& process : interactionProcesses) {
                    G4String key = volume + "_Interaction_" + process;
                    auto it = fAccInteractionCounts.find(key);
                    if (it != fAccInteractionCounts.end()) {
                        G4cout << std::setw(15) << process << ": "
                            << it->second->GetValue() << G4endl;
                    }
                }
            }
            
            
        }
        
        man->Write();
        man->CloseFile(false);
        

    }

}  // namespace G4_BREMS

