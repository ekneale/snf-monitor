#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "G4AnalysisManager.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "G4UnitsTable.hh"

namespace G4_BREMS
{
	PrimaryGeneratorAction::PrimaryGeneratorAction() {
		
		// Create a new messenger for this class
		fGenMessenger = new PrimaryGeneratorMessenger(this);
		useIBDGen = false;
	
	}

	PrimaryGeneratorAction::~PrimaryGeneratorAction() {

		if (fParticleGun) delete fParticleGun;
		if (fIBDGen) delete fIBDGen;
		if (fGenMessenger) delete fGenMessenger;
	}

	void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
	{
		
		if (useIBDGen){

			fNuEnergy = 0.;
			fNuDir = {0.,0.,0.};
			fPositronEnergy = 0.;
			fPositronDir = {0.,0.,0.};
			fNeutronEnergy = 0.;
			fNeutronDir = {0.,0.,0.};
			fVertex={0.,0.,0.};
                	
			// populate event from IBDGen.cc
			
			// Initialise the IBD generator once per simulation
			// This will read the spectrum from the db
			// and produce a 2D histogram of the spectrum.
			if(!fIBDGen){
				G4cout << "PrimaryGeneratorAction::GeneratePrimaries  [INFO] Initialising IBD generator" << G4endl;
				fIBDGen = new IBDGen();
				fIBDGen->ReadSpectrumFromDB(spectrum_database,spectrum_name);
			}

			// Create the particles
			G4LorentzVector neutrino;
			G4LorentzVector positron;
			G4LorentzVector neutron;

			// Generate event
			// Fills the Lorentz vectors with the 
			// direction and energy of the particles,
			// with a random position in the detector
			// and will populate the G4Event
			G4cout << "PrimaryGeneratorAction::GeneratePrimaries  [INFO] Generating IBD interaction" << G4endl;
			fIBDGen->GenerateEvent(neutrino,positron,neutron,event);

			// Populate truth information to be written out
			fNuEnergy = neutrino.getT();
			fNuDir = neutrino.getV();
			fPositronEnergy = positron.getT();
			fPositronDir = positron.getV();
			fNeutronEnergy = neutron.getT();
			fNeutronDir = neutron.getV();
			fVertex = {neutrino.getX(),neutrino.getY(),neutrino.getZ()};

			G4cout << "EventAction: [INFO] Writing primary vertex information to root file..." << G4endl;
			// get analysis manager
			auto analysisManager = G4AnalysisManager::Instance();

			// fill ntuple
			analysisManager->FillNtupleDColumn(0, fNuEnergy);
			analysisManager->FillNtupleDColumn(1, fNuDir[0]);
			analysisManager->FillNtupleDColumn(2, fNuDir[1]);
			analysisManager->FillNtupleDColumn(3, fNuDir[2]);
			analysisManager->FillNtupleDColumn(4, fVertex[0]);
			analysisManager->FillNtupleDColumn(5, fVertex[1]);
			analysisManager->FillNtupleDColumn(6, fVertex[2]);
			analysisManager->FillNtupleDColumn(7, fPositronEnergy);
			analysisManager->FillNtupleDColumn(8, fPositronDir[0]);
			analysisManager->FillNtupleDColumn(9, fPositronDir[1]);
			analysisManager->FillNtupleDColumn(10, fPositronDir[2]);
			analysisManager->FillNtupleDColumn(11, fNeutronEnergy);
			analysisManager->FillNtupleDColumn(12, fNeutronDir[0]);
			analysisManager->FillNtupleDColumn(13, fNeutronDir[1]);
			analysisManager->FillNtupleDColumn(14, fNeutronDir[2]);
			analysisManager->AddNtupleRow();


		} else {

			// set up particle gun by default
			G4int nParticles = 1;
			fParticleGun = new G4ParticleGun(nParticles);

			// define particle properties
			const G4String& particleName = "e+";

			G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
			G4ParticleDefinition* particle
				= particleTable->FindParticle(particleName);

			fParticleGun->SetParticleDefinition(particle);

			G4ThreeVector momentumDirection = G4ThreeVector(1, 0, 0);
			//G4ThreeVector momentumDirection = G4ThreeVector(0, 1, 0);

			//G4cout << "Particle direction: " << momentumDirection << G4endl;

			fParticleGun->SetParticleMomentumDirection(momentumDirection);

			G4double energy = 10.0 * MeV;
			//G4cout << "Particle energy: " << G4BestUnit(energy, "Energy") << G4endl;

			fParticleGun->SetParticleEnergy(energy);

			G4ThreeVector position = G4ThreeVector(-200.0 * mm, 0 * mm, 0 * mm);
		
			fParticleGun->SetParticlePosition(position);

			fParticleGun->GeneratePrimaryVertex(event);
		}
	}
}
