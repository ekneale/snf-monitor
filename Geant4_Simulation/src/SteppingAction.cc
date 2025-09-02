
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
#include "G4Gamma.hh"
#include "G4VParticleChange.hh"
//#include "TrackingAction.hh"
#include "Annihilation.hh"
#include "G4GenericAnalysisManager.hh"


namespace G4_BREMS {

    // global vectors
    std::vector<SipmHit> gSipmHits;
    std::vector<neutronCaptureSipmHit> gneutronCaptureSipmHits;
    std::vector<AnnihilationEvent> gAnnihilationEvents;
    std::vector<AnnihilationSipmHit> gAnnihilationSipmHits;
    std::vector<GammaInfo> fAnnihilationGammas;
    std::vector<NeutronCaptureEvent> gNeutronCaptureEvents;
    
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

        const G4VProcess* creatorProcess = track->GetCreatorProcess();
        //G4String volName = step->GetPostStepPoint() ->GetTouchableHandle() ->GetVolume()


        const G4StepPoint* post = step->GetPostStepPoint();
        if (!post) return;

        G4String procName = post->GetProcessDefinedStep()->GetProcessName();
        
        
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
            captureDaughters.emplace_back(trackID, name);
            //gNeutronCaptureEvents.push_back({neutronCapturePos, neutronCapturetime, physVolName});
            //gNeutronCaptureEvents.push_back(G4_BREMS::NeutronCaptureEvent{neutronCapturetime, neutronCapturePos, physVolName});

            NeutronCaptureEvent nc_hit;
            nc_hit.time = neutronCapturetime;
            nc_hit.position = neutronCapturePos;
            nc_hit.volume = physVolName;
            auto man = G4AnalysisManager::Instance();
  
            man->FillH1(22, nc_hit.time / ns);
            
            man->FillH1(23, nc_hit.position.x() / mm);
            man->FillH1(24, nc_hit.position.y() / mm);
            man->FillH1(25, nc_hit.position.z() / mm);

            man->FillH2(24, nc_hit.position.x() / mm, nc_hit.position.y() / mm, nc_hit.time);
            man->FillH2(25, nc_hit.position.y() / mm, nc_hit.position.z() / mm, nc_hit.time);
            man->FillH2(26, nc_hit.position.x() / mm, nc_hit.position.z() / mm, nc_hit.time);

            //man->FillNtupleIColumn(1, 0, an_hit.sipmID);
            man->FillNtupleDColumn(3, 0, nc_hit.position.x() / mm);
            man->FillNtupleDColumn(3, 1, nc_hit.position.y() / mm);
            man->FillNtupleDColumn(3, 2, nc_hit.position.z() / mm);
            man->FillNtupleDColumn(3, 3, nc_hit.time / ns);
            //man->FillNtupleDColumn(3, 4, nc_hit.volume);

            man->AddNtupleRow(3);

        }
        /*
        if (step->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName() == "annihil") {
            //G4String name = track->GetDefinition()->GetParticleName();
            //G4int id = track->GetTrackID();
            G4cout << "Name : " << track->GetDefinition()->GetParticleName() << " " << "TrackID: " << track->GetTrackID() << G4endl;

        }
        */
       /*
        if (track->GetDefinition()->GetParticleName() == "e+"){  
            G4VParticleChange* AtRestDoIt(const G4Track track, const G4Step step){

            
                G4VParticleChange* particleChange = new particleChange();
                particleChange->Initialize(track);


                if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "annihil"){

                    theListOfSecondaries =  new G4TrackFastVector();
                    theNumberOfSecondaries = right.theNumberOfSecondaries;
                    for (G4int index = 0; index<theNumberOfSecondaries; index++){
                        G4Track* newTrack =  new G4Track(*((*right.theListOfSecondaries)[index] ));
                        theListOfSecondaries->SetElement(index, newTrack);                      
                    }


                }
            }

        }
        */
        if (track->GetDefinition()->GetParticleName() == "e+" &&
            track->GetTrackStatus() == fStopAndKill &&
            step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "annihil" &&
            step->GetPostStepPoint()->GetKineticEnergy() < 1 * keV &&
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName() != "World") 
        {
            /*
            (step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "Tile" ||
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "Sipm" ||
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "FiberCore" ||
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "FiberClad")

            */
            //(step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() != "BNfoil" &&
               //step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() != "AlLayer" &&
               //step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() != "World")) {
           
            annihilationTime = track->GetGlobalTime();
            G4ThreeVector annihilationPosition = step->GetPostStepPoint()->GetPosition();

            G4String volName = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName();

            //auto touchable = step->GetPostStepPoint()->GetTouchableHandle();
            //G4VPhysicalVolume* physVol = touchable->GetVolume();
            //G4String           name = physVol->GetName();
            //G4cout << "Track " << track->GetTrackID()
                //<< " born in physical volume: " << name << G4endl;

           // G4cout << "ANNIHILATION Time: " << annihilationTime / ns << " ns, "
                //<< "Position: " << annihilationPosition / mm << " mm, "
                //<< "Volume: " << volName << G4endl;
                //<< G4endl;
            //gAnnihilationEvents.push_back({ annihilationTime, annihilationPosition, volName });


            AnnihilationEvent an_hit;
            an_hit.position = annihilationPosition;
            an_hit.time = annihilationTime;
            an_hit.volume = volName;


            auto man = G4AnalysisManager::Instance();
            //man->FillNtupleIColumn(1, 0, an_hit.sipmID);
            man->FillH1(18, an_hit.time / ns);
            
            man->FillH1(19, an_hit.position.x() / mm);
            man->FillH1(20, an_hit.position.y() / mm);
            man->FillH1(21, an_hit.position.z() / mm);

            man->FillH2(21, an_hit.position.x() / mm, an_hit.position.y() / mm, an_hit.time);
            man->FillH2(22, an_hit.position.y() / mm, an_hit.position.z() / mm, an_hit.time);
            man->FillH2(23, an_hit.position.x() / mm, an_hit.position.z() / mm, an_hit.time);


            man->FillNtupleDColumn(2, 0, an_hit.position.x() / mm);
            man->FillNtupleDColumn(2, 1, an_hit.position.y() / mm);
            man->FillNtupleDColumn(2, 2, an_hit.position.z() / mm);
            man->FillNtupleDColumn(2, 3, an_hit.time / ns);
            //man->FillNtupleDColumn(2, 4, an_hit.volume);

            man->AddNtupleRow(2);
            //fAnnihilationGammas = fAnni->Annihilation::annihilationOutput();
            //G4cout << 
            // Store details of daughters from e+ annihilation
            /*
            for (auto* sec : *(step->GetSecondaryInCurrentStep())) {
                //if (sec->GetDefinition()==G4Gamma::GammaDefinition()) {
                  //G4int  gid = sec->GetTrackID();
                    
                    G4int  gid = sec->GetTrackID();
                    G4String sec_name = sec->GetDefinition()->GetParticleName();
                    G4double create_t  = sec->GetGlobalTime();
                    G4double local_t = sec->GetLocalTime();
                    //G4ThreeVector create_position = sec->GetPostStepPoint()->GetPosition();
                    G4ThreeVector create_position = sec->GetVertexPosition();
                    //G4double gEnergy = sec->GetKineticEnergy();
                    //fAnnihilationGammaIDs.insert(gid);
                    //fGammaEnergies[gid] = gEnergy;
                    //fAnnihilationGammas.push_back({ gid, gEnergy });
                    fAnnihilationGammas.emplace_back( create_t, sec_name );
                    G4cout<<" Annihilation particle ID="<< gid
                          <<"  name ="<< sec_name 
                          << "Creation_time_sec =" << create_t
                          //<< "Creation_Pos =" << annihilationPosition
                          << "Local Time: " << local_t
                          << G4endl;
                //}
            }
            
            */

            
        }
      
        if (!track || track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return;
      //fOpticalphotontrack.push_back({track->GetTrackID(), track->GetGlobalTime()});
       
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

        auto creator = track->GetCreatorProcess();
        

        
        if (creatorProcess && creatorProcess->GetProcessName()=="Scintillation") {
            G4int scintParent = track->GetParentID();
            auto it = std::find_if(captureDaughters.begin(), captureDaughters.end(), 
                                                 [&](auto &p){ return p.first==scintParent; });
            if (it!=captureDaughters.end()) {
                captureScintPhotonIDs.push_back(track->GetTrackID());
                //G4cout<<" scintPhoton PID="<< track->GetTrackID() <<" from "<< it->second << G4endl;

                    
            }
        }
       
            
        // track WLS events
        if (creatorName == "OpWLS") {
            // a re-emitted photon
            G4double reEmitEnergy = track->GetKineticEnergy();
            G4double reEmitWavelength = (1239.84193 * eV) / reEmitEnergy;
            
            auto man = G4AnalysisManager::Instance();
            man->FillH1(3, reEmitEnergy / eV);
            man->FillH1(5, reEmitWavelength);
            man->FillH1(10, reEmitWavelength);
            
        }
        else if (processName == "OpWLS") {
            // photon about to be absorbed by WLS
            G4double absorbEnergy = track->GetKineticEnergy();
            G4double absorbWavelength = (1239.84193 * eV) / absorbEnergy;

            auto man = G4AnalysisManager::Instance();
            man->FillH1(2, absorbEnergy / eV);
            man->FillH1(4, absorbWavelength);
           
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
                    

                    G4int wlsParent = track->GetParentID();
                    
                    if ( std::find(captureScintPhotonIDs.begin(), captureScintPhotonIDs.end(), wlsParent)!=captureScintPhotonIDs.end())
                    {
                        auto post = step->GetPostStepPoint();
                        auto pos = post->GetPosition();
                        auto t = post->GetGlobalTime();
                        auto sipm_id = post->GetTouchableHandle()->GetCopyNumber();

                        neutronCaptureSipmHit nhit;
                        nhit.sipmID = post->GetTouchableHandle()->GetCopyNumber();
                        nhit.position = pos;
                        nhit.time = t;
                        nhit.creatorProcess = "Inelastic";

                    
                        auto man = G4AnalysisManager::Instance();
                        //auto man = G4GenericAnalysisManager::Instance();
                        man->FillH1(13, t / ns);
                        man->FillH2(15, pos.x() / mm, pos.y() / mm, t / ns);
                        man->FillH2(16, pos.y() / mm, pos.z() / mm, t / ns);
                        man->FillH2(17, pos.x() / mm, pos.z() / mm, t / ns);
                        

                       man->FillNtupleIColumn(1, 0, nhit.sipmID);
                       man->FillNtupleDColumn(1, 1, nhit.position.x() / mm);
                       man->FillNtupleDColumn(1, 2, nhit.position.y() / mm);
                       man->FillNtupleDColumn(1, 3, nhit.position.z() / mm);
                       man->FillNtupleDColumn(1, 4, nhit.time / ns);

                       man->AddNtupleRow(1);
                       
                    }
                    
                    

                    G4int wlsParent_from_annihilation = track->GetParentID();
                    
                    if ( std::find(fScintPhotonIDsfromannihilation.begin(), fScintPhotonIDsfromannihilation.end(), 
                                                wlsParent_from_annihilation)!=fScintPhotonIDsfromannihilation.end()){      
                        G4double ahitTime = step->GetPostStepPoint()->GetGlobalTime();
                        G4double ahitTimeLocal = step->GetPostStepPoint()->GetLocalTime();
                        G4ThreeVector ahitPosition = step->GetPreStepPoint()->GetPosition();
                        G4ThreeVector ahitPositionSipm = step->GetPostStepPoint()->GetPosition();

                        G4double ahitEnergy = track->GetTotalEnergy();
                        G4double ahitWavelength = (1239.84193 * eV) / ahitEnergy; // wavelength in nm

                        G4VPhysicalVolume* physVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
                        G4String afullSipmName = physVolume->GetName();
                        G4int asipmID = step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber();
                       
                        G4cout << "asipmID " << asipmID << G4endl;
                        AnnihilationSipmHit ahit;
                        ahit.sipmID = asipmID;
                        //ahit.sipmName = afullSipmName;
                    
                        ahit.position = ahitPositionSipm;
                        ahit.time = ahitTime;
                        
                        auto man = G4AnalysisManager::Instance();
                        man->FillH1(14, ahitTime / ns);
                        man->FillH2(18, ahitPositionSipm.x() / mm, ahitPositionSipm.y() / mm, ahitTime);
                        man->FillH2(19, ahitPositionSipm.y() / mm, ahitPositionSipm.z() / mm, ahitTime);
                        man->FillH2(20, ahitPositionSipm.x() / mm, ahitPositionSipm.z() / mm, ahitTime);

                        
                    }


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
                    
                    auto man = G4AnalysisManager::Instance();
                    man->FillH1(11, hitTime / ns);
                    man->FillH1(12, hitWavelength);
                    man->FillH1(15, hit.position.x() / mm);
                    man->FillH1(16, hit.position.y() / mm);
                    man->FillH1(17, hit.position.z() / mm);

                    man->FillH2(12, hitPositionSipm.x() / mm, hitPositionSipm.y() / mm, hitTime);
                    man->FillH2(13, hitPositionSipm.y() / mm, hitPositionSipm.z() / mm, hitTime);
                    man->FillH2(14, hitPositionSipm.x() / mm, hitPositionSipm.z() / mm, hitTime);

                    man->FillNtupleDColumn(0, 0, hit.time);
                    man->FillNtupleDColumn(0, 1, hit.wavelength);
                    man->FillNtupleDColumn(0, 2, hit.position.x() / mm);
                    man->FillNtupleDColumn(0, 3, hit.position.y() / mm);
                    man->FillNtupleDColumn(0, 4, hit.position.z() / mm);
                    

                    man->AddNtupleRow(0);
                       
                    
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

                    //G4cout << "Hit Time: " << hitTime << " " << "Hit Position: " << hitPosition << " " << "Hit Energy: "
                        //<< hitEnergy << " " << "Hit Wavelength: " << hitWavelength << " " << "Pre Volume: " << preVolumeName
                        //<< " Step Number in Sipm: " << stepNum << " SiPM ID: " << sipmID << G4endl;
                }

            }
        }
        auto man = G4AnalysisManager::Instance();
        //auto man = G4GenericAnalysisManager::Instance();
        man->FillH1(0, edep / MeV);
        man->FillH1(1, globalTime / ns);
        
        man->FillH2(0, position.x()/mm, position.y()/mm, globalTime/ns);
        man->FillH2(1, position.y()/mm, position.z()/mm, globalTime/ns);
        man->FillH2(2, position.x()/mm, position.z()/mm, globalTime/ns);
        man->FillH2(3, position.x()/mm, position.y()/mm, edep / MeV);
        man->FillH2(4, position.y()/mm, position.z()/mm, edep / MeV);
        man->FillH2(5, position.x()/mm, position.z()/mm, edep / MeV);
        
        // fill volume-specific histograms
        if (volumeName == "FiberClad") {
            man->FillH1(6, wavelength);
            man->FillH1(7, energy / eV);
            man->FillH2(6, position.x()/mm, position.y()/mm, edep / MeV);
            man->FillH2(7, position.y()/mm, position.z()/mm, edep / MeV);
            man->FillH2(8, position.x()/mm, position.z()/mm, edep / MeV);
            
        }
        else if (volumeName == "FiberCore") {
            man->FillH1(8, wavelength);
            man->FillH1(9, energy / eV);
            man->FillH2(9, position.x()/mm, position.y()/mm, edep / MeV);
            man->FillH2(10, position.y()/mm, position.z()/mm, edep / MeV);
            man->FillH2(11, position.x()/mm, position.z()/mm, edep / MeV);
            
        }
    }
       
} // namespace G4_BREMS
                           