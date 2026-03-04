#include "ActionInit.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "Annihilation.hh"
#include "EventAction.hh"  
#include "TrackingAction.hh"

namespace G4_BREMS {

void ActionInit::Build() const {
	
	SetUserAction(new PrimaryGeneratorAction);

	auto eventAction = new EventAction();
	auto runAction = new RunAction(eventAction);
    eventAction->SetRunAction(runAction);
	
	auto steppingAction = new SteppingAction(runAction, eventAction);
	auto trackingAction = new TrackingAction();

	SetUserAction(steppingAction);
	SetUserAction(eventAction);
	SetUserAction(runAction);
	SetUserAction(trackingAction);

}

void ActionInit::BuildForMaster() const {

	auto eventAction = new EventAction();
	auto runAction = new RunAction(eventAction);
	SetUserAction(runAction);
}

}// namespace G4_BREMS


