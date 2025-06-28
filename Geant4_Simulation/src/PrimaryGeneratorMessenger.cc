#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

namespace G4_BREMS
{
PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* pointerToAction) : fAction(pointerToAction)
{
	fDir = new G4UIdirectory("/mygen/");
	fDir->SetGuidance("Generator control commands.");

	fGenCmd = new G4UIcmdWithAString("/mygen/generator",this);
	fGenCmd->SetGuidance("Select primary generator.");
	fGenCmd->SetGuidance(" Available generators : currently only IBD available. Defaults to positron particle gun if no generator specified.");
	fGenCmd->SetParameterName("generator",true);
	fGenCmd->SetDefaultValue("");
	fGenCmd->SetCandidates("ibd");

	fIBDDatabaseCmd = new G4UIcmdWithAString("/mygen/ibd_database",this);
	fIBDDatabaseCmd->SetGuidance("Select the IBD database file.");
	fIBDDatabaseCmd->SetParameterName("ibd_database", true);
	fIBDDatabaseCmd->SetDefaultValue("data/IBDSpectra.json");
	SetNewValue(fIBDDatabaseCmd, "data/IBDSpectra.json");

	fIBDSpectrumCmd = new G4UIcmdWithAString("/mygen/ibd_spectrum",this);
	fIBDSpectrumCmd->SetGuidance("Select the IBD spectrum.");
	fIBDSpectrumCmd->SetParameterName("ibd_spectrum", true);
	fIBDSpectrumCmd->SetDefaultValue("Flat");
	SetNewValue(fIBDSpectrumCmd, "Flat");

}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
	delete fGenCmd;
	delete fDir;
	delete fIBDDatabaseCmd;
	delete fIBDSpectrumCmd;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
	if( command==fGenCmd )
	{
		if (newValue == "ibd") // IBD (inverse beta decay) generator
		{
			fAction->SetIBDGenerator(true);
		}
	}
	if (command == fIBDDatabaseCmd)
	{
		fAction->SetIBDDatabase(newValue);
		G4cout << "IBD database file set to " << newValue << G4endl;
	}
	if (command == fIBDSpectrumCmd) {
		fAction->SetIBDSpectrum(newValue);
	}
}

G4String PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
	G4String cv = "";

	if( command==fGenCmd )
	{
		if(fAction->IsUsingIBDGen())
			{ cv = "ibd"; }
	}

	return cv;

}

}// namespace G4_BREMS



