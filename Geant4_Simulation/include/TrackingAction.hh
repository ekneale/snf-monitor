#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "TrackInformation.hh"
#include "globals.hh"

class G4Track;

/**
 * Tracking Action class to manage track-level logic.
 * Derived from G4UserTrackingAction.
 */
class TrackingAction : public G4UserTrackingAction {
public:
    TrackingAction();
    virtual ~TrackingAction();

    // Triggered before the track starts moving
    virtual void PreUserTrackingAction(const G4Track* aTrack) override;

    // Triggered after the track has finished its trajectory
    virtual void PostUserTrackingAction(const G4Track* aTrack) override;

private:

    TrackInformation *info;
    G4int debug_trackingAction = 0;
};

#endif