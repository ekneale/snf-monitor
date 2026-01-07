
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4RunManagerFactory.hh"
#include "G4VisExecutive.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInit.hh"

#include <iostream>
#include <string>

using namespace G4_BREMS;

int main(int argc, char** argv)
{
    int fileIndex = 0;
    std::string macroFile = "";

    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.rfind("--file-index=", 0) == 0) {
            std::string value = arg.substr(std::string("--file-index=").size());
            try {
                fileIndex = std::stoi(value);
            } catch (...) {
                std::cerr << "ERROR: Invalid --file-index value: " << value << std::endl;
                return 1;
            }
        }
        // macro file (*.mac)
        else if (arg.size() > 4 && arg.substr(arg.size() - 4) == ".mac") {
            macroFile = arg;
        }
    }

    std::cout << "FileIndex = " << fileIndex << std::endl;
    if (!macroFile.empty()) {
        std::cout << "Macro file = " << macroFile << std::endl;
    }

    // UI setup
    G4UIExecutive* ui = nullptr;
    bool batchMode = !macroFile.empty();

    if (!batchMode) {
        ui = new G4UIExecutive(argc, argv);
    }

    
    // Run manager
    auto* runManager =
        G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserInitialization(new ActionInit(fileIndex));

    
    // Visualization
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Random seed
    long seed = 12345;
    CLHEP::HepRandom::setTheSeed(seed);
    G4Random::setTheSeed(seed);

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (batchMode) {
        UImanager->ApplyCommand("/control/execute " + macroFile);
    } else {
        ui->SessionStart();
        delete ui;
    }

    
    delete visManager;
    delete runManager;

    return 0;
}
