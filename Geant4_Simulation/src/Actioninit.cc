#include "ActionInit.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "Annihilation.hh"
#include "EventAction.hh"  

namespace G4_BREMS {

void ActionInit::Build() const {
	
	SetUserAction(new PrimaryGeneratorAction);

	auto steppingAction = new SteppingAction(nullptr);
	SetUserAction(steppingAction);
	auto eventAction = new EventAction;
	SetUserAction(eventAction);
	auto runAction = new RunAction(eventAction);
	SetUserAction(runAction);

	steppingAction->SetRunAction(runAction);

}

void ActionInit::BuildForMaster() const {

	auto eventAction = new EventAction;
	SetUserAction(new RunAction(eventAction));

}

}// namespace G4_BREMS


