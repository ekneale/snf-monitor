#ifndef G4_BREMS_PrimaryGeneratorMessenger_h
#define G4_BREMS_PrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithAString;

namespace G4_BREMS
{

class PrimaryGeneratorAction;

// PrimaryGeneratorMessenger class
class PrimaryGeneratorMessenger: public G4UImessenger
{
	public:
		PrimaryGeneratorMessenger(PrimaryGeneratorAction*);
		~PrimaryGeneratorMessenger() override;

	public:
		void SetNewValue(G4UIcommand* command, G4String newValues) override;
		G4String GetCurrentValue(G4UIcommand* command) override;

	private:
		PrimaryGeneratorAction* fAction = nullptr;
		G4UIdirectory* fDir = nullptr;
	
	private:	
		// commands
		G4UIcmdWithAString* fGenCmd;
		// IBD event generator spectrum file
		G4UIcmdWithAString* fIBDDatabaseCmd;
		G4UIcmdWithAString* fIBDSpectrumCmd;
};

}// namespace G4_BREMS

#endif
