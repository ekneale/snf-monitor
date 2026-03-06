#ifndef TrackInformation_h
#define TrackInformation_h 1

#include "G4VUserTrackInformation.hh"
#include "G4Track.hh"
#include "globals.hh"

class TrackInformation : public G4VUserTrackInformation {
public:
    TrackInformation(const G4Track *aTrack);
    TrackInformation(TrackInformation *aTrackInfo);
    virtual ~TrackInformation() {}
    void Print() const;

    // Methods to interact with your custom data
    void SetPassed(G4bool val) { fHasPassedThroughSensitive = val; }
    G4bool HasPassed() const { return fHasPassedThroughSensitive; }

    void SetOriginalTrackID(G4int trackID) {fOriginalTrackID = trackID;}
    G4int GetOriginalTrackID() const {return fOriginalTrackID; }

    void SetParentTrackID(G4int trackID) {fParentTrackID = trackID;}
    G4int GetParentTrackID() const {return fParentTrackID; }

    void SetOriginalParticleName(G4String particleName) {fOriginalParticleName = particleName; }
    G4String GetOriginalParticleName() const {return fOriginalParticleName; }

    void SetParentParticleName(G4String particleName) {fParentParticleName = particleName; }
    G4String GetParentParticleName() const {return fParentParticleName; }

private:
    G4bool fHasPassedThroughSensitive = false;
    G4String fCreatorProcessName = "";
    G4String fOriginalParticleName = "";
    G4String fParentParticleName = "";
    G4int fOriginalTrackID = -1;
    G4int fParentTrackID = -1;
};

#endif