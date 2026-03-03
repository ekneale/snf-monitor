#include "RunActionMessenger.hh"
#include "RunAction.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"

namespace G4_BREMS
{

RunActionMessenger::RunActionMessenger(RunAction* pointerToAction) : G4UImessenger(), fRunAction(pointerToAction)
{
	fDir = new G4UIdirectory("/set/");
	fDir->SetGuidance("RunAction control commands.");

    fOutfileCmd = new G4UIcmdWithAString("/set/outfile",this);
    fOutfileCmd->SetGuidance("Set output root file name.");
    fOutfileCmd->SetParameterName("outfile",true);
    fOutfileCmd->SetDefaultValue("outputfile.root");
    SetNewValue(fOutfileCmd,"outputfile.root");

    fSeedCmd = new G4UIcmdWithAnInteger("/set/seed",this);
    fSeedCmd->SetGuidance("Set random number generator seed.");
    fSeedCmd->SetParameterName("seed",false);
    fSeedCmd->SetDefaultValue(0);
}

RunActionMessenger::~RunActionMessenger()
{
    delete fOutfileCmd;
    delete fSeedCmd;
}

void RunActionMessenger::SetNewValue(G4UIcommand * command, G4String newValue)
{
    if(command == fOutfileCmd)
    {
        fRunAction->SetOutfileName(newValue);
        G4cout << "Output root file name set to " << newValue << G4endl;
    }
    if(command == fSeedCmd)
    {
        fRunAction->SetSeed(std::stoi(newValue));
        G4cout << "Seed set to " << newValue << G4endl;
    }
}

G4String RunActionMessenger::GetCurrentValue(G4UIcommand * command)
{
    G4String cv = "";

    if (command==fOutfileCmd)
    {
        cv = fRunAction->GetOutfileName();
    }
    if (command==fSeedCmd)
    {
        cv = fRunAction->GetSeed();
    }

    return cv;
}

} //namespace G4_BREMS
