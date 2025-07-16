
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <sstream>
#include "G4Neutron.hh"


namespace {
    G4Mutex mutex = G4MUTEX_INITIALIZER;

}

namespace G4_BREMS {

    // global vectors
    std::vector<SipmHit> gSipmHits;
    std::vector<neutronCaptureSipmHit> gneutronCaptureSipmHits;
    std::vector<AnnihilationEvent> gAnnihilationEvents;
    //std::vector<neutronCaptureEvent> captureDaughterIDs;
    G4Mutex sipmHitsMutex = G4MUTEX_INITIALIZER;
    G4Mutex neutronCaptureSipmHitsMutex = G4MUTEX_INITIALIZER;

    SteppingAction::SteppingAction(RunAction* runAction)
        : G4UserSteppingAction(), fRunAction(runAction), fSensitiveVolume(nullptr) {
    }

    SteppingAction::~SteppingAction() {}

    void SteppingAction::UserSteppingAction(const G4Step* step)
    {
        if (G4Threading::IsMasterThread()) return;

        G4Track* track = step->GetTrack();
        if (!track) return;


        G4String name = track->GetDefinition()->GetParticleName();
        G4int trackID = track->GetTrackID();


        //G4String creatorProcess1 = track->GetCreatorProcess()->GetProcessName();
        //G4cout << "Particle Name: " << name << " " << "TrackID: " << trackID << G4endl;
        //"Creator Process: " << creatorProcess1 << G4endl;
        /*
        if (track->GetDefinition()->GetParticleName() == "neutron") {
            G4String name = track->GetDefinition()->GetParticleName();
            auto proc = step->GetPostStepPoint()->GetProcessDefinedStep();
            if (proc) {
                G4cout << "particle name" << name << "[DEBUG] Neutron post-step process: "
                    << proc->GetProcessName() << G4endl;
            }
        }
        */

        const G4VProcess* creatorProcess = track->GetCreatorProcess();
        //G4String volName = step->GetPostStepPoint() ->GetTouchableHandle() ->GetVolume()


        const G4StepPoint* post = step->GetPostStepPoint();
        if (!post) return;

        G4String procName = post->GetProcessDefinedStep()->GetProcessName();
        //G4Track* track = step->GetTrack();
        //if (track->GetDefinition() == G4Neutron::NeutronDefinition()
        //if (track->GetDefinition()->GetParticleName() == "neutron"
            //&& (procName == "nCapture" || procName == "nCaptureHP" || procName.contains("Capture"))) {
        //if (procName == "nCapture" || procName == "nCaptureHP" || procName.contains("Capture")) {
        if ((name == "Li7" || name == "alpha") && track->GetCurrentStepNumber() == 1){
            G4ThreeVector pos = post->GetPosition();
            //G4double       time = post->GetGlobalTime();
            G4double neutronCapturetime = step->GetPreStepPoint()->GetGlobalTime();
            G4ThreeVector neutronCapturePos = track->GetVertexPosition();
            G4String  physVolName = post->GetTouchableHandle()->GetVolume()->GetName();
            const G4VProcess* creatorProcess1 = track->GetCreatorProcess();
            /*
            G4cout << "Name: " << name << " " << "Creator Process: " << creatorProcess1->GetProcessName() << " " << 
                "neutronCapturePos_X = "
                << neutronCapturePos.x() / mm << " " <<
                "neutronCapturePos_Y = " << neutronCapturePos.y() / mm
                << " " << "neutronCapturePos_Z = " << neutronCapturePos.z() / mm << " mm  t=" << neutronCapturetime / ns 
                << " ns  vol="
                << physVolName << " " << "TrackID: " << trackID << G4endl;
            */
            G4cout << "Particle Name: " << name << " " << "TrackID: " << trackID << G4endl;
            //captureDaughterIDs.push_back(track->GetTrackID());
            captureDaughterIDs.push_back(trackID);
            //captureDaughterIDs.push_back({name, neutronCapturePos, neutronCapturetime});
            for (G4int i = 0; i < captureDaughterIDs.size(); i++) {
                G4cout << "Size: " << captureDaughterIDs.size() << "TrackID: " << captureDaughterIDs[i] << G4endl;
            }
            //G4cout << "Neutron_Capture_X_Pos = " << pos.x() / mm << " " << "Neutron_Capture_Y_Pos = " << pos.y() / mm
                //<< " " << "Neutron_Capture_Z_Pos = " << pos.z() / mm << " mm  t=" << time / ns << " ns  vol="
                //<< physVolName << G4endl;

        }
        
          
        if (track->GetDefinition()->GetParticleName() == "e+" &&
            track->GetTrackStatus() == fStopAndKill &&
            step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "annihil" &&
            step->GetPostStepPoint()->GetKineticEnergy() < 1 * keV) {
            /*
            (step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "Tile" ||
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "Sipm" ||
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "FiberCore" ||
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "FiberClad")

            */
            //(step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() != "BNfoil" &&
               //step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() != "AlLayer" &&
               //step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() != "World")) {
           
            G4double annihilationTime = track->GetGlobalTime();
            G4ThreeVector annihilationPosition = step->GetPostStepPoint()->GetPosition();

            G4String volName = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName();

            auto touchable = step->GetPostStepPoint()->GetTouchableHandle();
            G4VPhysicalVolume* physVol = touchable->GetVolume();
            G4String           name = physVol->GetName();
            G4cout << "Track " << track->GetTrackID()
                << " born in physical volume: " << name << G4endl;

            G4cout << "ANNIHILATION Time: " << annihilationTime / ns << " ns, "
                << "Position: " << annihilationPosition / mm << " mm, "
                << "Volume: " << volName << G4endl;

            gAnnihilationEvents.push_back({ annihilationTime, annihilationPosition, volName });
        }

        if (!track || track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return;

        G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
        G4double globalTime = step->GetPreStepPoint()->GetGlobalTime();
        G4double edep = step->GetTotalEnergyDeposit();
        G4double energy = track->GetTotalEnergy();
        G4double wavelength = (1239.84193 * eV) / energy;

        G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        if (!volume) return;
        G4LogicalVolume* logicalVolume = volume->GetLogicalVolume();
        if (!logicalVolume) return;
        G4String volumeName = logicalVolume->GetName();

        G4String creatorName = "Primary";
        //const G4VProcess* creatorProcess = track->GetCreatorProcess();
        if (creatorProcess != nullptr) {
            creatorName = creatorProcess->GetProcessName();
        }

        G4String processName = "Unknown";
        const G4StepPoint* postStepPoint = step->GetPostStepPoint();
        if (postStepPoint) {
            const G4VProcess* process = postStepPoint->GetProcessDefinedStep();
            if (process) {
                processName = process->GetProcessName();
            }
        }

        /*
        if (track->GetDefinition()->GetParticleName() == "neutron") {
                //G4String creatorProcess1 = track->GetCreatorProcess()->GetProcessName();
                G4cout <<
                    "Creator Process: " << creatorProcess->GetProcessName() << G4endl;
        }
        */
         
        

        // track WLS events
        if (creatorName == "OpWLS") {
            // a re-emitted photon
            G4double reEmitEnergy = track->GetKineticEnergy();
            G4double reEmitWavelength = (1239.84193 * eV) / reEmitEnergy;

            auto analysisManager = G4AnalysisManager::Instance();
            analysisManager->FillH1(3, reEmitEnergy / eV);    // energy after WLS
            analysisManager->FillH1(5, reEmitWavelength);   // wavelength after WLS
            analysisManager->FillH1(10, reEmitWavelength);
        }
        else if (processName == "OpWLS") {
            // photon about to be absorbed by WLS
            G4double absorbEnergy = track->GetKineticEnergy();
            G4double absorbWavelength = (1239.84193 * eV) / absorbEnergy;

            auto analysisManager = G4AnalysisManager::Instance();
            analysisManager->FillH1(2, absorbEnergy / eV);    // energy before WLS
            analysisManager->FillH1(4, absorbWavelength);   // wavelength before WLS
        }

        // update process counts
        fRunAction->AddProcessCount(volumeName, creatorName, true);
        fRunAction->AddProcessCount(volumeName, processName, false);

        if (volumeName == "Tile") {
            fRunAction->IncrementTileCount();
        }
        else if (volumeName == "FiberClad") {
            fRunAction->IncrementCladCount();
        }
        else if (volumeName == "FiberCore") {
            fRunAction->IncrementCoreCount();
        }
        else if (volumeName == "Sipm") {
            fRunAction->IncrementSipmCount();
        }
        else {
            fRunAction->IncrementOtherCount();
        }

        G4VPhysicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        G4VPhysicalVolume* postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
        G4double stepNum = step->GetTrack()->GetCurrentStepNumber();

        if (preVolume && postVolume) {
            G4String preVolumeName = preVolume->GetLogicalVolume()->GetName();
            G4String postVolumeName = postVolume->GetLogicalVolume()->GetName();

            if (preVolumeName != postVolumeName) {

                if ((postVolumeName == "FiberCore" && preVolumeName == "FiberClad") ||
                    (postVolumeName == "FiberClad" && preVolumeName == "Tile") && creatorName != "OpWLS") {
                    G4double initial_energy = track->GetKineticEnergy();
                    G4double initial_vertex_energy = track->GetVertexKineticEnergy();
                    G4double initial_Wavelength = (1239.84193 * eV) / initial_energy;
                    G4ThreeVector initial_momentum = track->GetMomentum();
                    G4ThreeVector initial_direction = track->GetMomentumDirection();

                    fRunAction->IncrementPhotonsEnteredFiber();
                }

                if ((postVolumeName != "Tile" && postVolumeName != "World") &&
                    (preVolumeName == "FiberCore" || preVolumeName == "FiberClad")
                    && creatorName == "OpWLS") {

                    G4double final_energy = track->GetKineticEnergy();
                    G4double final_vertex_energy = track->GetVertexKineticEnergy();
                    G4double final_Wavelength = (1239.84193 * eV) / final_energy;
                    G4ThreeVector final_momentum = track->GetMomentum();
                    G4ThreeVector final_direction = track->GetMomentumDirection();

                    fRunAction->IncrementPhotonsAbsorbedFiber();
                }

                if ((postVolumeName == "Sipm") && (preVolumeName == "FiberCore" || preVolumeName == "FiberClad")
                    && creatorName == "OpWLS") {
                    
                    G4int parentID = track->GetParentID();
                    if (count(captureDaughterIDs.begin(), captureDaughterIDs.end(), parentID) != 0) {
                        //return;
                        auto post = step->GetPostStepPoint();
                        auto pos = post->GetPosition();
                        auto t = post->GetGlobalTime();

                        neutronCaptureSipmHit hit;
                        hit.sipmID = post->GetTouchableHandle()->GetCopyNumber();
                        hit.position = pos;
                        hit.time = t;
                        hit.creatorProcess = "Neutroncapture_induced";

                        //gSipmHits.push_back(hit);
                        fneutroncaptureSipmHits.push_back(hit);


                        G4AutoLock lock(&neutronCaptureSipmHitsMutex);
                        gneutronCaptureSipmHits.push_back(hit);
                        lock.unlock();


                        auto analysisManager = G4AnalysisManager::Instance();
                        analysisManager->FillH1(20, t / ns);
                        analysisManager->FillH2(21, pos.x() / mm, pos.y() / mm, t / ns);
                        analysisManager->FillH2(22, pos.y() / mm, pos.z() / mm, t / ns);
                        analysisManager->FillH2(23, pos.x() / mm, pos.z() / mm, t / ns);
                    
                    }

                    /*
                    auto post = step->GetPostStepPoint();
                    auto pos = post->GetPosition();
                    auto t = post->GetGlobalTime();

                    SipmHit hit;
                    hit.sipmID = post->GetTouchableHandle()->GetCopyNumber();
                    hit.position = pos;
                    hit.time = t;
                    hit.creatorProcess = "capture?induced";

                    gSipmHits.push_back(hit);

                    auto mgr = G4AnalysisManager::Instance();
                    mgr->FillH1(20, t / ns);
                    mgr->FillH2(21, pos.x() / mm, pos.y() / mm, t / ns);
                    */


                    G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
                    G4double hitTimeLocal = step->GetPostStepPoint()->GetLocalTime();
                    G4ThreeVector hitPosition = step->GetPreStepPoint()->GetPosition();
                    G4ThreeVector hitPositionSipm = step->GetPostStepPoint()->GetPosition();

                    G4double hitEnergy = track->GetTotalEnergy();
                    G4double hitWavelength = (1239.84193 * eV) / hitEnergy; // wavelength in nm

                    G4VPhysicalVolume* physVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
                    G4String fullSipmName = physVolume->GetName();
                    G4int sipmID = step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber();
                    //G4cout << "Hit SiPM with name: " << fullSipmName
                        //<< ", Copy Number: " << sipmID << G4endl;
                    // store hit information
                    SipmHit hit;
                    hit.sipmID = sipmID;
                    hit.sipmName = fullSipmName;
                    hit.time = hitTime;
                    hit.position = hitPosition;
                    hit.energy = hitEnergy;
                    hit.wavelength = hitWavelength;
                    fSipmHits.push_back(hit);

                    G4AutoLock lock(&sipmHitsMutex);
                    gSipmHits.push_back(hit);
                    lock.unlock();


                    /*
                    G4cout << "Hit SiPM with name: " << fullSipmName << " " << "Hit Time: " << hitTime << " "
                        << "Hit Local Time: " << hitTimeLocal << " " << "Hit Position: "
                        << hitPosition << " " << "Hit Wavelength: " << hitWavelength << " " << "Pre Volume: " << preVolumeName
                        << " " << "Hit Position Sipm: " << hitPositionSipm
                        //<< " " << "Energy Deposition in Sipm: "
                        //<< edepSipm
                        << G4endl;

                    */
                    //G4cout << "Hit Time: " << hitTime << " " << "Hit Position: " << hitPosition << " " << "Hit Energy: "
                        //<< hitEnergy << " " << "Hit Wavelength: " << hitWavelength << " " << "Pre Volume: " << preVolumeName
                        //<< " Step Number in Sipm: " << stepNum << " SiPM ID: " << sipmID << G4endl;

                    //G4cout << "Adding SiPM hit to collection, current size: " << fSipmHits.size() << G4endl;
                    //fSipmHits.push_back(hit);
                    //G4cout << "New collection size: " << fSipmHits.size() << G4endl;

                    auto analysisManager = G4AnalysisManager::Instance();
                    analysisManager->FillH1(11, hitTime / ns);
                    analysisManager->FillH1(12, hitWavelength);
                    analysisManager->FillH2(12, hitPositionSipm.x() / mm, hitPositionSipm.y() / mm, hitTime);
                    analysisManager->FillH2(13, hitPositionSipm.y() / mm, hitPositionSipm.z() / mm, hitTime);
                    analysisManager->FillH2(14, hitPositionSipm.x() / mm, hitPositionSipm.z() / mm, hitTime);

                }

                if ((preVolumeName == "BNfoil" || preVolumeName == "AlLayer") && (postVolumeName == "Tile" ||
                    postVolumeName == "FiberCore" || postVolumeName == "FiberClad" || postVolumeName == "Sipm") &&
                    creatorName == "nCapture" || creatorName == "nCaptureHP") {


                    G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
                    G4double hitTimeLocal = step->GetPostStepPoint()->GetLocalTime();
                    G4ThreeVector hitPosition = step->GetPreStepPoint()->GetPosition();
                    G4ThreeVector hitPositionSipm = step->GetPostStepPoint()->GetPosition();

                    G4double hitEnergy = track->GetTotalEnergy();
                    G4double hitWavelength = (1239.84193 * eV) / hitEnergy;

                    G4VPhysicalVolume* physVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
                    G4String fullSipmName = physVolume->GetName();
                    G4int sipmID = step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber();

                    G4cout << "Hit Time: " << hitTime << " " << "Hit Position: " << hitPosition << " " << "Hit Energy: "
                        << hitEnergy << " " << "Hit Wavelength: " << hitWavelength << " " << "Pre Volume: " << preVolumeName
                        << " Step Number in Sipm: " << stepNum << " SiPM ID: " << sipmID << G4endl;
                }

            }
        }

        auto analysisManager = G4AnalysisManager::Instance();

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
        if (volumeName == "FiberClad") {
            analysisManager->FillH1(6, wavelength);  // wavelength in cladding
            analysisManager->FillH1(7, energy / eV); // energy in cladding
            analysisManager->FillH2(6, position.x() / mm, position.y() / mm, edep / MeV);
            analysisManager->FillH2(7, position.y() / mm, position.z() / mm, edep / MeV);
            analysisManager->FillH2(8, position.x() / mm, position.z() / mm, edep / MeV);
        }
        else if (volumeName == "FiberCore") {
            analysisManager->FillH1(8, wavelength);  // wavelength in core
            analysisManager->FillH1(9, energy / eV); // energy in core
            analysisManager->FillH2(9, position.x() / mm, position.y() / mm, edep / MeV);
            analysisManager->FillH2(10, position.y() / mm, position.z() / mm, edep / MeV);
            analysisManager->FillH2(11, position.x() / mm, position.z() / mm, edep / MeV);
        }
    }

} // namespace G4_BREMS
                           