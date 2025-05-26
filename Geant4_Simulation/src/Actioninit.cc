
#include "ActionInit.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"

namespace G4_BREMS {
	void ActionInit::Build() const {
		SetUserAction(new PrimaryGeneratorAction);

		SteppingAction* steppingAction = new SteppingAction(nullptr);

		RunAction* runAction = new RunAction(steppingAction);

		steppingAction->SetRunAction(runAction);

		SetUserAction(runAction);
		SetUserAction(steppingAction);

	}
	void ActionInit::BuildForMaster() const {
		SetUserAction(new RunAction(nullptr));

	}
}


