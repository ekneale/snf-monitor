#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"

class G4Event;

namespace G4_BREMS
{

  /// Event action class

  class EventAction : public G4UserEventAction
  {
    public:
      EventAction();
      virtual ~EventAction();

      virtual void BeginOfEventAction(const G4Event* anEvent);
      virtual void EndOfEventAction(const G4Event* anEvent);
  };

}  // namespace G4_BREMS

#endif
