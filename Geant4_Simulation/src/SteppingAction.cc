#include "SipmHit.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "Annihilation.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"

#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4VParticleChange.hh"
#include "G4GenericAnalysisManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4UserTrackingAction.hh"

#include <fstream>
#include <sstream>

namespace G4_BREMS
{

    SteppingAction::SteppingAction(RunAction *runAction)
        : G4UserSteppingAction(), fRunAction(runAction), fSensitiveVolume(nullptr)
    {
    }

    SteppingAction::~SteppingAction()
    {
    }

    void SteppingAction::UserSteppingAction(const G4Step *step)
    {
        if (G4Threading::IsMasterThread())
            return;

        G4Track *track = step->GetTrack();
        if (!track)
            return;

        const G4StepPoint *post = step->GetPostStepPoint();
        if (!post)
            return;

        G4StepStatus stepStatus = post->GetStepStatus();
        G4bool transmit = (stepStatus == fGeomBoundary || stepStatus == fWorldBoundary);
        if (transmit)
            return;

        auto analysisManager = G4AnalysisManager::Instance();

        G4String name = track->GetDefinition()->GetParticleName();
        G4int trackID = track->GetTrackID();

        // Get the creator process
        const G4VProcess *creatorProcess = track->GetCreatorProcess();
        G4String creatorName = "Primary";
        if (creatorProcess != nullptr)
        {
            creatorName = creatorProcess->GetProcessName();
        }

        G4String procName = post->GetProcessDefinedStep()->GetProcessName();



         // Find and save the neutron capture
        // TODO Currently includes two-alpha production with no Li7
        NeutronCaptureEvent nCapture;
        if (track->GetDefinition() == G4Neutron::Definition())
        {
            auto secondaries = step->GetSecondaryInCurrentStep();
            if (secondaries && !secondaries->empty())
            {
                for (auto sec : *secondaries)
                {
                    G4String secname = sec->GetDefinition()->GetParticleName();
                    if ((secname == "Li7" || secname == "alpha" || secname == "gamma"))
                    {
                        nCapture.time = post->GetGlobalTime();
                        nCapture.position = post->GetPosition();
                        nCapture.volume = post->GetTouchableHandle()->GetVolume()->GetName();
                        nCapture.parentID = trackID;

                        if (debug_steppingaction)
                        {
                            G4cout << "ncapture neutron track ID " << trackID << G4endl;
                            G4cout << "Name: " << secname << ", " << procName << ", " << creatorName
                                   << "neutronCapturePos_X = " << nCapture.position.x() / mm << ", " << "neutronCapturePos_Y = " << nCapture.position.y() / mm << ", "
                                   << "neutronCapturePos_Z = " << nCapture.position.z() / mm << " mm,  t=" << nCapture.time / ns << G4endl;
                        }

                        if (generate_histograms)
                        {
                            analysisManager->FillH1(22, nCapture.time / ns);

                            analysisManager->FillH1(23, nCapture.position.x() / mm);
                            analysisManager->FillH1(24, nCapture.position.y() / mm);
                            analysisManager->FillH1(25, nCapture.position.z() / mm);

                            analysisManager->FillH2(24, nCapture.position.x() / mm, nCapture.position.y() / mm, nCapture.time);
                            analysisManager->FillH2(25, nCapture.position.y() / mm, nCapture.position.z() / mm, nCapture.time);
                            analysisManager->FillH2(26, nCapture.position.x() / mm, nCapture.position.z() / mm, nCapture.time);
                        }

                        // Fill ntuple assuming only one neutron capture
                        analysisManager->FillNtupleDColumn(26, nCapture.position.x() / mm);
                        analysisManager->FillNtupleDColumn(27, nCapture.position.y() / mm);
                        analysisManager->FillNtupleDColumn(28, nCapture.position.z() / mm);
                        analysisManager->FillNtupleDColumn(29, nCapture.time / ns);

                        // TODO consider saving to ntuple later, in case of multiple neutrons from muons
                        // gNeutronCaptureEvents.push_back(nCapture);
                    } // Li, alpha or gamma
                } // Loop over secondaries
            }
            // analysisManager->FillNtupleDColumn(30, nCapture.edep / MeV);
        } // Neutron track

        if (track->GetDefinition()->GetParticleName() == "e+" &&
            track->GetTrackStatus() == fStopAndKill &&
            step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "annihil" &&
            step->GetPostStepPoint()->GetKineticEnergy() < 1 * keV &&
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName() != "World")
        {

            AnnihilationEvent annihilation;
            annihilation.position = step->GetPostStepPoint()->GetPosition();
            annihilation.time = track->GetGlobalTime(); // TODO should this be step->GetPostStepPoint()->GetGlobalTime()???
            annihilation.volume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName();

            if (generate_histograms)
            {
                analysisManager->FillH1(18, annihilation.time / ns);

                analysisManager->FillH1(19, annihilation.position.x() / mm);
                analysisManager->FillH1(20, annihilation.position.y() / mm);
                analysisManager->FillH1(21, annihilation.position.z() / mm);

                analysisManager->FillH2(21, annihilation.position.x() / mm, annihilation.position.y() / mm, annihilation.time);
                analysisManager->FillH2(22, annihilation.position.y() / mm, annihilation.position.z() / mm, annihilation.time);
                analysisManager->FillH2(23, annihilation.position.x() / mm, annihilation.position.z() / mm, annihilation.time);
            }
            G4cout << "Found annihilation event" << G4endl;
            analysisManager->FillNtupleDColumn(28, annihilation.position.x() / mm);
            analysisManager->FillNtupleDColumn(29, annihilation.position.y() / mm);
            analysisManager->FillNtupleDColumn(30, annihilation.position.z() / mm);
            analysisManager->FillNtupleDColumn(31, annihilation.time / ns);

            gAnnihilationEvents.push_back(annihilation);
        }

        if (!track || track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
            return;

        G4VPhysicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        if (!volume)
            return;
        G4LogicalVolume *logicalVolume = volume->GetLogicalVolume();
        if (!logicalVolume)
            return;
        G4String volumeName = logicalVolume->GetName();

        if (generate_histograms)
        {

            // Save some track info to fill histograms later
            G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
            G4double globalTime = step->GetPreStepPoint()->GetGlobalTime();
            G4double edep = step->GetTotalEnergyDeposit();
            G4double energy = track->GetTotalEnergy();
            G4double wavelength = (1239.84193 * eV) / energy;

            // fill 1D histograms
            analysisManager->FillH1(0, edep / MeV);
            analysisManager->FillH1(1, globalTime / ns);

            // fill 2D histograms with timing
            analysisManager->FillH2(0, position.x() / mm, position.y() / mm, globalTime / ns);
            analysisManager->FillH2(1, position.y() / mm, position.z() / mm, globalTime / ns);
            analysisManager->FillH2(2, position.x() / mm, position.z() / mm, globalTime / ns);

            // fill 2D histograms with energy deposition
            analysisManager->FillH2(3, position.x() / mm, position.y() / mm, edep / MeV);
            analysisManager->FillH2(4, position.y() / mm, position.z() / mm, edep / MeV);
            analysisManager->FillH2(5, position.x() / mm, position.z() / mm, edep / MeV);

            // fill volume-specific histograms
            if (volumeName == "FiberClad")
            {
                analysisManager->FillH1(6, wavelength);  // wavelength in cladding
                analysisManager->FillH1(7, energy / eV); // energy in cladding
                analysisManager->FillH2(6, position.x() / mm, position.y() / mm, edep / MeV);
                analysisManager->FillH2(7, position.y() / mm, position.z() / mm, edep / MeV);
                analysisManager->FillH2(8, position.x() / mm, position.z() / mm, edep / MeV);
            }
            else if (volumeName == "FiberCore")
            {
                analysisManager->FillH1(8, wavelength);  // wavelength in core
                analysisManager->FillH1(9, energy / eV); // energy in core
                analysisManager->FillH2(9, position.x() / mm, position.y() / mm, edep / MeV);
                analysisManager->FillH2(10, position.y() / mm, position.z() / mm, edep / MeV);
                analysisManager->FillH2(11, position.x() / mm, position.z() / mm, edep / MeV);
            }
        }

        G4String processName = "Unknown";
        const G4StepPoint *postStepPoint = step->GetPostStepPoint();
        if (postStepPoint)
        {
            const G4VProcess *process = postStepPoint->GetProcessDefinedStep();
            if (process)
            {
                processName = process->GetProcessName();
            }
        }

        // Save all scintillation photons
        // if (creatorProcess && creatorProcess->GetProcessName() == "Scintillation")
        //{
        //    G4int scintParent = track->GetParentID();
        //    auto it = std::find_if(captureDaughters.begin(), captureDaughters.end(),
        //                           [&](auto &p)
        //                           { return p.first == scintParent; });
        //    if (it != captureDaughters.end())
        //    {
        //        fCaptureScintPhotonIDs.push_back(track->GetTrackID());
        //    }
        //}
        // SetCaptureScintPhotonIDs(fCaptureScintPhotonIDs);

        if (generate_histograms)
        {
            // track WLS events
            if (creatorName == "OpWLS")
            {
                // a re-emitted photon
                G4double reEmitEnergy = track->GetKineticEnergy();
                G4double reEmitWavelength = (1239.84193 * eV) / reEmitEnergy;

                analysisManager->FillH1(3, reEmitEnergy / eV); // energy after WLS
                analysisManager->FillH1(5, reEmitWavelength);  // wavelength after WLS
                analysisManager->FillH1(10, reEmitWavelength);
            }
            else if (processName == "OpWLS")
            {
                // photon about to be absorbed by WLS
                G4double absorbEnergy = track->GetKineticEnergy();
                G4double absorbWavelength = (1239.84193 * eV) / absorbEnergy;

                analysisManager->FillH1(2, absorbEnergy / eV); // energy before WLS
                analysisManager->FillH1(4, absorbWavelength);  // wavelength before WLS
            }
        }

        // update process counts
        fRunAction->AddProcessCount(volumeName, creatorName, true);
        fRunAction->AddProcessCount(volumeName, processName, false);

        if (volumeName == "Tile")
        {
            fRunAction->IncrementTileCount();
        }
        else if (volumeName == "FiberClad")
        {
            fRunAction->IncrementCladCount();
        }
        else if (volumeName == "FiberCore")
        {
            fRunAction->IncrementCoreCount();
        }
        else if (volumeName == "Sipm")
        {
            fRunAction->IncrementSipmCount();
        }
        else
        {
            fRunAction->IncrementOtherCount();
        }

        // Use pre-step and post-step volumes to identify sipm hits
        G4VPhysicalVolume *preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        G4VPhysicalVolume *postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
        G4double stepNum = step->GetTrack()->GetCurrentStepNumber();

        if (preVolume && postVolume)
        {
            G4String preVolumeName = preVolume->GetLogicalVolume()->GetName();
            G4String postVolumeName = postVolume->GetLogicalVolume()->GetName();

            if (preVolumeName != postVolumeName)
            {
                // photons entering the fiber
                // TODO this could count a photon entering the fiber twice -
                // once when it enters the fiber from the tile, and once when it enters the core
                // from the surrounding cladding.
                if ((postVolumeName == "FiberCore" && preVolumeName == "FiberClad") ||
                    (postVolumeName == "FiberClad" && preVolumeName == "Tile") && creatorName != "OpWLS")
                {
                    // G4double initial_energy = track->GetKineticEnergy();
                    // G4double initial_vertex_energy = track->GetVertexKineticEnergy();
                    // G4double initial_Wavelength = (1239.84193 * eV) / initial_energy;
                    // G4ThreeVector initial_momentum = track->GetMomentum();
                    // G4ThreeVector initial_direction = track->GetMomentumDirection();
                    fRunAction->IncrementPhotonsEnteredFiber();
                }

                // photons trapped in the fiber
                // TODO could also count photons going from the fiber to the sipm
                if ((postVolumeName != "Tile" && postVolumeName != "World") &&
                    (preVolumeName == "FiberCore" || preVolumeName == "FiberClad") && creatorName == "OpWLS")
                {
                    // G4double final_energy = track->GetKineticEnergy();
                    // G4double final_vertex_energy = track->GetVertexKineticEnergy();
                    // G4double final_Wavelength = (1239.84193 * eV) / final_energy;
                    // G4ThreeVector final_momentum = track->GetMomentum();
                    // G4ThreeVector final_direction = track->GetMomentumDirection();
                    fRunAction->IncrementPhotonsAbsorbedFiber();
                }

                //
                if ((preVolumeName == "BNfoil" || preVolumeName == "AlLayer") && (postVolumeName == "Tile" || postVolumeName == "FiberCore" || postVolumeName == "FiberClad" || postVolumeName == "Sipm") && creatorName == "nCapture" || creatorName == "nCaptureHP")
                {

                    G4double stepTime = step->GetPostStepPoint()->GetGlobalTime();
                    G4double stepTimeLocal = step->GetPostStepPoint()->GetLocalTime();
                    G4ThreeVector step_start_position = step->GetPreStepPoint()->GetPosition();
                    G4ThreeVector step_end_position = step->GetPostStepPoint()->GetPosition();

                    G4double stepEnergy = track->GetTotalEnergy();
                    G4double stepWavelength = (1239.84193 * eV) / stepEnergy;

                    G4VPhysicalVolume *physVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
                    G4String volumeName = physVolume->GetName();
                    // G4int sipmID = step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber(); // not necessarily sipm hit

                    if (debug_steppingaction)
                    {

                        G4cout << "Step Time: " << stepTime << " " << "Step Start Position: " << step_start_position << " "
                               << "Step End Position: " << step_end_position << " "
                               << "Step Energy: " << stepEnergy << " " << "Step Wavelength: " << stepWavelength << " " << "Pre Volume: " << volumeName
                               << " Step Number in Sipm: " << stepNum << G4endl;
                    }
                } // if neutron capture in neutron-detecting foils
            } // if change of volume
        } // if pre-step and post-step volume

    } // UserSteppingAction

} // namespace G4_BREMS
