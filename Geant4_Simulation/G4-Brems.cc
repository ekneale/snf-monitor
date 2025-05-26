// G4-Brems.cc : Defines the entry point for the application.
//
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4RunManagerFactory.hh"
#include "G4VisExecutive.hh"
#include "G4SteppingVerbose.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInit.hh"

using namespace G4_BREMS;

#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	G4UIExecutive* ui = nullptr;
	if (argc == 1) {
		ui = new G4UIExecutive(argc, argv);
	}
	auto* runManager =
		G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
	
	runManager->SetUserInitialization(new PhysicsList());
	runManager->SetUserInitialization(new DetectorConstruction());
	runManager->SetUserInitialization(new ActionInit());


	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();

	//G4TrajectoryDrawByAttribute* model = new G4TrajectoryDrawByAttribute;
	//model->Set("CPN")

	// random seed
	long seed = 12345;
	
	CLHEP::HepRandom::setTheSeed(seed);
	G4Random::setTheSeed(seed);

	// START UI =============================================================

	// get pointer to UI manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Run macro or start UI
	if (!ui) {
		// batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command + fileName);
	}
	else {
		// run visualization
		UImanager->ApplyCommand("/control/execute vis.mac");

		// use UI
		ui->SessionStart();
		delete ui;
	}

	// clean up
	delete visManager;
	delete runManager;
	return 0;
}



