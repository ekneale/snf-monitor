#include "TrackInformation.hh"
#include "G4ios.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

TrackInformation::TrackInformation(const G4Track *aTrack)
    : G4VUserTrackInformation()
{
    //fOriginalTrackID = aTrack->GetTrackID();
    //fOriginalParticleName = aTrack->GetDefinition()->GetParticleName();
}

TrackInformation::TrackInformation(TrackInformation *aTrackInfo)
{
    //fParentTrackID = aTrackInfo->fOriginalTrackID;
   // fParentParticleName = aTrackInfo->fOriginalParticleName;
}

// Print method: Highly recommended for debugging complex tracking
void TrackInformation::Print() const
{
    //G4cout << "--- Custom Track Information ---" << G4endl;
    //G4cout << "  Original ID: " << fOriginalTrackID << G4endl;
    //if (fOriginalParticleName)
    //{
    //    G4cout << "  Particle: " << fOriginalParticleName << G4endl;
    //}
}
