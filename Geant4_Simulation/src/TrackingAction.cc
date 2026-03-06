#include "TrackingAction.hh"
#include "TrackInformation.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"

TrackingAction::TrackingAction()
{
}

TrackingAction::~TrackingAction()
{
}

void TrackingAction::PreUserTrackingAction(const G4Track *aTrack)
{
    if ((aTrack->GetDefinition()->GetParticleName() == "neutron") || (aTrack->GetDefinition()->GetParticleName() == "e+"))
    {
        // Cast away constness to set user information
        G4Track *mutableTrack = const_cast<G4Track *>(aTrack);
        if (!aTrack->GetUserInformation())
        {
            info = new TrackInformation(aTrack);

            // Use the tracking manager to set the information
            fpTrackingManager->SetUserTrackInformation(info);
        }

        info->SetOriginalTrackID(aTrack->GetTrackID());
        info->SetOriginalParticleName(aTrack->GetDefinition()->GetParticleName());

        mutableTrack->SetUserInformation(info);
    }
}

void TrackingAction::PostUserTrackingAction(const G4Track *aTrack)
{
    if (aTrack->GetDefinition()->GetParticleName() == "neutron" || aTrack->GetDefinition()->GetParticleName() == "e+")
    {
        G4TrackVector *secondaries = fpTrackingManager->GimmeSecondaries();
        if (secondaries && !secondaries->empty())
        {
            TrackInformation *parentInfo = (TrackInformation *)(aTrack->GetUserInformation());
            if (parentInfo)
            {
                size_t nSeco = secondaries->size();

                for (size_t i = 0; i < nSeco; i++)
                {
                    if ((*secondaries)[i])
                    {
                        if (!(*secondaries)[i]->GetUserInformation())
                        {
    
                            TrackInformation *newParentInfo = new TrackInformation(parentInfo);
                            G4String secName = (*secondaries)[i]->GetDefinition()->GetParticleName();
                            if ( secName == "Li7" || secName == "alpha" || secName == "gamma" )
                            {
                                if (debug_trackingAction)
                                    G4cout << "TrackingAction::PostUserTrackingAction [INFO] Parent particle: " << aTrack->GetDefinition()->GetParticleName() << ", daughter particle " << (*secondaries)[i]->GetDefinition()->GetParticleName() << G4endl;
                                (*secondaries)[i]->SetUserInformation(newParentInfo);
                            }
                        }
                    }
                }
            }
        }
    }
}