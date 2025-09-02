#include "ActionInit.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "Annihilation.hh"
//#include "EventAction.hh"  

namespace G4_BREMS {
	void ActionInit::Build() const {
		SetUserAction(new PrimaryGeneratorAction);
        
		SteppingAction* steppingAction = new SteppingAction(nullptr);

		RunAction* runAction = new RunAction(steppingAction);

		steppingAction->SetRunAction(runAction);
        
		//SetUserAction(new TrackingAction);
		SetUserAction(runAction);
		SetUserAction(steppingAction);

		//TrackingAction* trackingAction = new TrackingAction;
        //SetUserAction(trackingAction);

		//runManager->SetUserAction(new TrackingAction);
        

		

	}
	void ActionInit::BuildForMaster() const {
		SetUserAction(new RunAction(nullptr));

	}
}

