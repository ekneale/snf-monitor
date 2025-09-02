
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
//namespace {
    //G4Mutex mutex = G4MUTEX_INITIALIZER;

//}

namespace G4_BREMS {

    // global vectors
    std::vector<SipmHit> gSipmHits;
    std::vector<neutronCaptureSipmHit> gneutronCaptureSipmHits;
    std::vector<AnnihilationEvent> gAnnihilationEvents;
    std::vector<AnnihilationSipmHit> gAnnihilationSipmHits;
    std::vector<GammaInfo> fAnnihilationGammas;
    std::vector<NeutronCaptureEvent> gNeutronCaptureEvents;
    //auto fAnni = new SteppingAction();
    //std::vector<GammaInfo> f
  //std::vector<GammaInfo> fOpticalphotontrack;
    //td::vector<GammaInfo> fOpticalphotontrack;
    //std::vector<AnnihilationDaughters> gAnnihilationDaughters;
    //std::vector<neutronCaptureEvent> captureDaughterIDs;
    //std::vector<std::pair<G4int, G4String>> SteppingAction::fAnnihilationGammas{};
    //std::vector<G4int> SteppingAction::fScintPhotonIDsfromannihilation{};
    //G4ThreadLocal std::vector<G4int> SteppingAction::fScintPhotonIDsfromannihilation{};
   // extern TrackingAction* gTrackingAction;

    
    
    //G4Mutex sipmHitsMutex = G4MUTEX_INITIALIZER;
    //G4Mutex neutronCaptureSipmHitsMutex = G4MUTEX_INITIALIZER;
    //G4Mutex annihilationSipmHitsMutex = G4MUTEX_INITIALIZER;

    SteppingAction::SteppingAction(RunAction* runAction)
        : G4UserSteppingAction(), fRunAction(runAction), fSensitiveVolume(nullptr) {
    }

    SteppingAction::~SteppingAction() {}

    void SteppingAction::UserSteppingAction(const G4Step* step)
    {
        if (G4Threading::IsMasterThread()) return;

        G4Track* track = step->GetTrack();
        if (!track) return;


        //auto h1 = G4GenericAnalysisManager::Instance("H1");
        //auto h2 = G4GenericAnalysisManager::Instance("H2");
        //auto nt = G4GenericAnalysisManager::Instance("NT");

        //auto man = G4GenericAnalysisManager::Instance();
        //auto man = G4AnalysisManager::Instance();
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
            //G4cout<<"neutronCapture Daughter Name: "<< name<<" trackID="<< trackID << G4endl;
            //captureDaughterIDs.push_back({name, neutronCapturePos, neutronCapturetime});
            //for (G4int i = 0; i < captureDaughterIDs.size(); i++) {
                //G4cout << "Size: " << captureDaughterIDs.size() << "TrackID: " << captureDaughterIDs[i] << G4endl;
            //}
            //G4cout << "Neutron_Capture_X_Pos = " << pos.x() / mm << " " << "Neutron_Capture_Y_Pos = " << pos.y() / mm
                //<< " " << "Neutron_Capture_Z_Pos = " << pos.z() / mm << " mm  t=" << time / ns << " ns  vol="
                //<< physVolName << G4endl;

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
      //auto creator1 = track->GetCreatorProcess();
      //if ( track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() ){
          //track->GetParentID() == 0){  //&& creator1  && creator1->GetProcessName() == "annihil") {
        //  G4cout << ">>> Detected annihilation photons: ID=" << track->GetTrackID() << " at time=" << track->GetGlobalTime()/ns
         // << " ns,  pos=" << track->GetVertexPosition()/mm << " mm\n";
     // }


        
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

        /*
        if (track->GetDefinition()->GetParticleName() == "neutron") {
                //G4String creatorProcess1 = track->GetCreatorProcess()->GetProcessName();
                G4cout <<
                    "Creator Process: " << creatorProcess->GetProcessName() << G4endl;
        }
        */
        
        auto creator = track->GetCreatorProcess();
        /*
        if (creator) {
            G4String name = track->GetDefinition()->GetParticleName();
            G4int id = track->GetTrackID();
            if (creator->GetProcessName()=="annihil"){
                G4cout << "Name : " << track->GetDefinition()->GetParticleName() << " " << "TrackID: " << track->GetTrackID() << G4endl;
            } 
        }
        */

        /*
        if (creator && creator->GetProcessName()=="Scintillation") {
            G4int parentID = track->GetParentID();
            //auto &list = TrackingAction::fAnnihilationSecondaries;
            //auto it = std::find_if(list.begin(), list.end(),
                     //[&](auto const & tup){ return std::get<0>(tup) == parentID; });
            //if ( it != list.end() ) {
                //G4String gammaName = std::get<1>(*it);
                //fScintPhotonIDsfromannihilation.push_back(track->GetTrackID());
                //fScintPhotonIDsfromannihilation.push_back(step->GetTrack()->GetTrackID());
                //G4cout << " Scint photon ID="<< track->GetTrackID() 
                       //<< " from annihilation track "<<parentID << " ("<<gammaName<<")\n";
            //}
        
        


            //if (fAnnihilationGammaIDs.count(parentID)) {
                    //fScintPhotonIDsfromannihilation.insert(track->GetTrackID());
                    //G4cout<<"Scintillation Photon ID="<< track->GetTrackID()
                          //<<"  parent gamma="<< parentID << G4endl;
            //}
            //auto it = std::find_if( fAnnihilationGammas.begin(), fAnnihilationGammas.end(),
                                        //[parentID](const GammaInfo& info){ return info.trackID == parentID; });
                                       
            auto it = std::find_if( fAnnihilationGammas.begin(), fAnnihilationGammas.end(),
                                        [&](auto &p){ return p.first == parentID; });
            if (it != fAnnihilationGammas.end()) {
                //fScintPhotonIDsfromannihilation.insert(track->GetTrackID());
                fScintPhotonIDsfromannihilation.push_back(track->GetTrackID());
                //G4cout << "  Scint photon from gamma ID=" << it->trackID << " (E=" << it->energy/keV << " keV)\n";
                G4cout<<" scintPhoton PID="<< track->GetTrackID() << G4endl;
            }
            
        }
        */
        if (creatorProcess && creatorProcess->GetProcessName()=="Scintillation") {
            G4int scintParent = track->GetParentID();
            auto it = std::find_if(captureDaughters.begin(), captureDaughters.end(), 
                                                 [&](auto &p){ return p.first==scintParent; });
            if (it!=captureDaughters.end()) {
                captureScintPhotonIDs.push_back(track->GetTrackID());
                //G4cout<<" scintPhoton PID="<< track->GetTrackID() <<" from "<< it->second << G4endl;

                    
            }
        }
        /*
        if (creatorName != "OpWLS" && volumeName == "Tile"){
            
            fOpticalphotontrack.emplace_back(track->GetTrackID(), track->GetGlobalTime());
        }
    
        if (creator)//creator->GetProcessName()=="Scintillation")){ //&& track->GetCurrentStepNumber()==1) {
            //fScintPhotonIDsfromannihilation.push_back(track->GetTrackID());
            //G4cout << "Photons from annihilation: " << track->GetTrackID() << G4endl;
            //&& step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName() != "World"){
        {
          //G4double phoTime = track->GetGlobalTime();
            G4int tid = track->GetTrackID(); 
            G4double gtime = track->GetGlobalTime();
            G4ThreeVector create_position = track->GetVertexPosition();
            for ( auto &p : fOpticalphotontrack){
              //G4double annihTime = p.first;
              G4double phoTime = p.second;
             //G4double annihTime = p.second;
               for(auto &q : fAnnihilationGammas){
                  G4double annihTime = q.first;
                //if ( std::abs(phoTime - annihTime) < eps ) {
                    if (std::abs(phoTime - annihTime) < 0.0005*ns) {
                        if (std::find(fScintPhotonIDsfromannihilation.begin(), fScintPhotonIDsfromannihilation.end(), tid)
                                  == fScintPhotonIDsfromannihilation.end()  &&( std::find(fScintPhotontime.begin(),
                                   fScintPhotontime.end(), annihTime)
                                  == fScintPhotontime.end())) {
                           fScintPhotonIDsfromannihilation.push_back(track->GetTrackID());
                           fScintPhotontime.push_back(annihTime);
                         //G4cout << "Photon from annihilation: ID=" << track->GetTrackID() 
                        // << "  time=" << phoTime/ns << " ns" 
                        // << "Annihilation time: " << annihTime/ns << " ns" 
                          // << "Position: " << create_position
                        // << G4endl;
                       }
                       break;
                    
                    } 
               }
          
            }
            */ 
           /*
            G4int pid = track->GetParentID();
            bool fromAnnihilationGamma = false;
            while (pid>0) {
                auto itName   = gTrackingAction->fNameByTrackID.find(pid);
                if (itName!=gTrackingAction->fNameByTrackID.end() && itName->second=="gamma") {
                    fromAnnihilationGamma = true;
                    break;
                }
                auto itParent = gTrackingAction->fParentByTrackID.find(pid);
                if (itParent==gTrackingAction->fParentByTrackID.end()) break;
                pid = itParent->second;  
            }

            if (fromAnnihilationGamma) {
                fScintPhotonIDsfromannihilation.push_back(track->GetTrackID());
                //G4cout << "Photon from annihilation‑gamma: ID="<<track->GetTrackID()
                        //<< "  time=" << track->GetGlobalTime()/ns << " ns"
                        //<< G4endl;
            } 

            */
        //}


        



    
        
            
        // track WLS events
        if (creatorName == "OpWLS") {
            // a re-emitted photon
            G4double reEmitEnergy = track->GetKineticEnergy();
            G4double reEmitWavelength = (1239.84193 * eV) / reEmitEnergy;
            
            //h1->FillH1(3, reEmitEnergy / eV);
            //h1->FillH1(5, reEmitWavelength);
            //h1->FillH1(10, reEmitWavelength);
            //auto man = G4GenericAnalysisManager::Instance();
            auto man = G4AnalysisManager::Instance();
            man->FillH1(3, reEmitEnergy / eV);
            man->FillH1(5, reEmitWavelength);
            man->FillH1(10, reEmitWavelength);
            /*
            auto analysisManager = G4AnalysisManager::Instance();
            analysisManager->FillH1(3, reEmitEnergy / eV);    // energy after WLS
            analysisManager->FillH1(5, reEmitWavelength);   // wavelength after WLS
            analysisManager->FillH1(10, reEmitWavelength);
            */
        }
        else if (processName == "OpWLS") {
            // photon about to be absorbed by WLS
            G4double absorbEnergy = track->GetKineticEnergy();
            G4double absorbWavelength = (1239.84193 * eV) / absorbEnergy;

            auto man = G4AnalysisManager::Instance();
           // auto man = G4GenericAnalysisManager::Instance();
            man->FillH1(2, absorbEnergy / eV);
            man->FillH1(4, absorbWavelength);
            /*
            auto analysisManager = G4AnalysisManager::Instance();
            analysisManager->FillH1(2, absorbEnergy / eV);    // energy before WLS
            analysisManager->FillH1(4, absorbWavelength);   // wavelength before WLS
            */
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
                    /*
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
                    */

                    G4int wlsParent = track->GetParentID();
                    //if ( std::find(captureScintPhotonIDs.begin(), captureScintPhotonIDs.end(), wlsParent)==captureScintPhotonIDs.end()){
                       // return; 

                    //}
                    //G4cout << "wlsParent " << wlsParent << G4endl;
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

                    //gSipmHits.push_back(hit);
                        //fneutroncaptureSipmHits.push_back(nhit);


                        //G4AutoLock nlock(&neutronCaptureSipmHitsMutex);
                        //gneutronCaptureSipmHits.push_back(nhit);
                        //nlock.unlock();

                        /*
                        auto analysisManager = G4AnalysisManager::Instance();
                    //analysisManager->FillH1(20, t / ns);
                    //analysisManager->FillH2(21, pos.x() / mm, pos.y() / mm, t / ns);
                    //analysisManager->FillH2(22, pos.y() / mm, pos.z() / mm, t / ns);
                    //analysisManager->FillH2(23, pos.x() / mm, pos.z() / mm, t / ns);
                        analysisManager->FillH1(13, t / ns);
                        analysisManager->FillH2(15, pos.x() / mm, pos.y() / mm, t / ns);
                        analysisManager->FillH2(16, pos.y() / mm, pos.z() / mm, t / ns);
                        analysisManager->FillH2(17, pos.x() / mm, pos.z() / mm, t / ns);
                        */
                        //h1->FillH1(13, t / ns);
                        //h2->FillH2(15, pos.x() / mm, pos.y() / mm, t / ns);
                        //h2->FillH2(16, pos.y() / mm, pos.z() / mm, t / ns);
                        //h2->FillH2(17, pos.x() / mm, pos.z() / mm, t / ns);
                        auto man = G4AnalysisManager::Instance();
                        //auto man = G4GenericAnalysisManager::Instance();
                        man->FillH1(13, t / ns);
                        man->FillH2(15, pos.x() / mm, pos.y() / mm, t / ns);
                        man->FillH2(16, pos.y() / mm, pos.z() / mm, t / ns);
                        man->FillH2(17, pos.x() / mm, pos.z() / mm, t / ns);
                        /*
                        
                        auto ncSipmHitsManager = G4AnalysisManager::Instance();
                        ncSipmHitsManager->FillH1(0, t / ns);
                        ncSipmHitsManager->FillH2(0, pos.x() / mm, pos.y() / mm, t / ns);
                        ncSipmHitsManager->FillH2(1, pos.y() / mm, pos.z() / mm, t / ns);
                        ncSipmHitsManager->FillH2(2, pos.x() / mm, pos.z() / mm, t / ns);


                        ncSipmHitsManager->FillNtupleIColumn(0, step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber());
                        ncSipmHitsManager->FillNtupleDColumn(1, pos.x() / mm);
                        ncSipmHitsManager->FillNtupleDColumn(2, pos.y() / mm);
                        ncSipmHitsManager->FillNtupleDColumn(3, pos.z() / mm);
                        ncSipmHitsManager->FillNtupleDColumn(4, t / ns);
                        ncSipmHitsManager->AddNtupleRow();
                        */
                       /*
                        analysisManager->FillNtupleIColumn(0, 0, step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber());
                        analysisManager->FillNtupleDColumn(0, 1, pos.x() / mm);
                        analysisManager->FillNtupleDColumn(0, 2, pos.y() / mm);
                        analysisManager->FillNtupleDColumn(0, 3, pos.z() / mm);
                        analysisManager->FillNtupleDColumn(0, 4, t / ns);
                        analysisManager->AddNtupleRow();
                       */
                        
                       //man->FillNtupleIColumn(0, step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber());
                       
                       //man->FillNtupleIColumn(0, nhit.sipmID);
                       //man->FillNtupleDColumn(1, nhit.position.x() / mm);
                       //man->FillNtupleDColumn(2, nhit.position.y() / mm);
                       //man->FillNtupleDColumn(3, nhit.position.z() / mm);
                       //man->FillNtupleDColumn(4, nhit.time / ns);

                       //man->AddNtupleRow();

                       man->FillNtupleIColumn(1, 0, nhit.sipmID);
                       man->FillNtupleDColumn(1, 1, nhit.position.x() / mm);
                       man->FillNtupleDColumn(1, 2, nhit.position.y() / mm);
                       man->FillNtupleDColumn(1, 3, nhit.position.z() / mm);
                       man->FillNtupleDColumn(1, 4, nhit.time / ns);

                       man->AddNtupleRow(1);
                       
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
                    

                    G4int wlsParent_from_annihilation = track->GetParentID();
                    //G4cout << "wlsParent from_annihilation " << wlsParent_from_annihilation << G4endl;
                    //if (fScintPhotonIDsfromannihilation.count(wlsParent_from_annihilation)) {
                    //if ( std::find(fScintPhotonIDsfromannihilation.begin(), fScintPhotonIDsfromannihilation.end(), 
                                                //wlsParent_from_annihilation)==fScintPhotonIDsfromannihilation.end()){
                        //return; 

                    //} 
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
                        //ahit.energy  = ahitEnergy;
                        //ahit.wavelength = ahitWavelength;
                        //fannihilationSipmHits.push_back(ahit);
                        
                        /*
                        auto mgr = G4AnalysisManager::Instance();
                        mgr->FillH1(11, hit.time/ns);
                        mgr->FillH1(12, hit.wavelength);
                        mgr->FillH2(12, hit.position.x()/mm, hit.position.y()/mm, hit.time/ns);
                        G4cout<<"[SiPM] hit at "<<hit.position
                          <<"  t="<<hit.time/ns<<" ns"<<G4endl;
                        */
                        
                        //G4AutoLock alock(&annihilationSipmHitsMutex);
                        //gAnnihilationSipmHits.push_back(ahit);
                        //alock.unlock();
                        /*
                        auto analysisManager = G4AnalysisManager::Instance();
                        analysisManager->FillH1(14, ahitTime / ns);
                        analysisManager->FillH2(18, ahitPositionSipm.x() / mm, ahitPositionSipm.y() / mm, ahitTime / ns);
                        analysisManager->FillH2(19, ahitPositionSipm.y() / mm, ahitPositionSipm.z() / mm, ahitTime / ns);
                        analysisManager->FillH2(20, ahitPositionSipm.x() / mm, ahitPositionSipm.z() / mm, ahitTime / ns);
                        */
                        auto man = G4AnalysisManager::Instance();
                        //auto man = G4GenericAnalysisManager::Instance();
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
                    //fSipmHits.push_back(hit);

                    //G4AutoLock lock(&sipmHitsMutex);
                    //gSipmHits.push_back(hit);
                    //lock.unlock();


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
                    auto man = G4AnalysisManager::Instance();
                    //auto man = G4GenericAnalysisManager::Instance();
                    man->FillH1(11, hitTime / ns);
                    man->FillH1(12, hitWavelength);
                    man->FillH1(15, hit.position.x() / mm);
                    man->FillH1(16, hit.position.y() / mm);
                    man->FillH1(17, hit.position.z() / mm);

                    man->FillH2(12, hitPositionSipm.x() / mm, hitPositionSipm.y() / mm, hitTime);
                    man->FillH2(13, hitPositionSipm.y() / mm, hitPositionSipm.z() / mm, hitTime);
                    man->FillH2(14, hitPositionSipm.x() / mm, hitPositionSipm.z() / mm, hitTime);


                    //man->FillNtupleIColumn(1, 11, hit.time);
                    //man->FillNtupleDColumn(1, 12, hit.wavelength);
                    //man->FillNtupleDColumn(1, 15, hit.position.x() / mm);
                    //man->FillNtupleDColumn(1, 16, hit.position.y() / mm);
                    //man->FillNtupleDColumn(1, 17, hit.position.z() / mm);

                    //man->FillNtupleIColumn(0, hit.time);
                    //man->FillNtupleDColumn(1, hit.wavelength);
                    //man->FillNtupleDColumn(2, hit.position.x() / mm);
                    //man->FillNtupleDColumn(3, hit.position.y() / mm);
                    //man->FillNtupleDColumn(4, hit.position.z() / mm);
                    

                    //man->AddNtupleRow();

                    man->FillNtupleDColumn(0, 0, hit.time);
                    man->FillNtupleDColumn(0, 1, hit.wavelength);
                    man->FillNtupleDColumn(0, 2, hit.position.x() / mm);
                    man->FillNtupleDColumn(0, 3, hit.position.y() / mm);
                    man->FillNtupleDColumn(0, 4, hit.position.z() / mm);
                    

                    man->AddNtupleRow(0);
                       
                    /*
                    auto analysisManager = G4AnalysisManager::Instance();
                    analysisManager->FillH1(11, hitTime / ns);
                    analysisManager->FillH1(12, hitWavelength);
                    analysisManager->FillH2(12, hitPositionSipm.x() / mm, hitPositionSipm.y() / mm, hitTime);
                    analysisManager->FillH2(13, hitPositionSipm.y() / mm, hitPositionSipm.z() / mm, hitTime);
                    analysisManager->FillH2(14, hitPositionSipm.x() / mm, hitPositionSipm.z() / mm, hitTime);
                    */
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
        /*
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
        */
        // fill volume-specific histograms
        if (volumeName == "FiberClad") {
            //auto man = G4AnalysisManager::Instance();
            man->FillH1(6, wavelength);
            man->FillH1(7, energy / eV);
            man->FillH2(6, position.x()/mm, position.y()/mm, edep / MeV);
            man->FillH2(7, position.y()/mm, position.z()/mm, edep / MeV);
            man->FillH2(8, position.x()/mm, position.z()/mm, edep / MeV);
            /*
            analysisManager->FillH1(6, wavelength);  // wavelength in cladding
            analysisManager->FillH1(7, energy / eV); // energy in cladding
            
            analysisManager->FillH2(6, position.x() / mm, position.y() / mm, edep / MeV);
            analysisManager->FillH2(7, position.y() / mm, position.z() / mm, edep / MeV);
            analysisManager->FillH2(8, position.x() / mm, position.z() / mm, edep / MeV);
            */
        }
        else if (volumeName == "FiberCore") {
            //auto man = G4AnalysisManager::Instance();
            man->FillH1(8, wavelength);
            man->FillH1(9, energy / eV);
            man->FillH2(9, position.x()/mm, position.y()/mm, edep / MeV);
            man->FillH2(10, position.y()/mm, position.z()/mm, edep / MeV);
            man->FillH2(11, position.x()/mm, position.z()/mm, edep / MeV);
            /*
            analysisManager->FillH1(8, wavelength);  // wavelength in core
            analysisManager->FillH1(9, energy / eV); // energy in core
            analysisManager->FillH2(9, position.x() / mm, position.y() / mm, edep / MeV);
            analysisManager->FillH2(10, position.y() / mm, position.z() / mm, edep / MeV);
            analysisManager->FillH2(11, position.x() / mm, position.z() / mm, edep / MeV);
            */
        }
    }
    


    
    
    
} // namespace G4_BREMS
                           