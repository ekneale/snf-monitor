
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

		if (fParticleGun1) delete fParticleGun1;
		if (fParticleGun2) delete fParticleGun2;
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
				if(debug_primarygenerator){
					G4cout << "PrimaryGeneratorAction::GeneratePrimaries  [INFO] Initialising IBD generator" << G4endl;
				}
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
			if(debug_primarygenerator){
				G4cout << "PrimaryGeneratorAction::GeneratePrimaries  [INFO] Generating IBD interaction" << G4endl;
			}
			fIBDGen->GenerateEvent(neutrino,positron,neutron,event);

			// Populate truth information to be written out
			fNuEnergy = neutrino.getT();
			fNuDir = neutrino.getV();
			fPositronEnergy = positron.getT();
			fPositronDir = positron.getV();
			fNeutronEnergy = neutron.getT();
			fNeutronDir = neutron.getV();
			fVertex = {neutrino.getX(),neutrino.getY(),neutrino.getZ()};

			if(debug_primarygenerator){
				G4cout << "PrimaryGeneratorAction::GeneratePrimaries: [INFO] Writing primary vertex information to root file..." << G4endl;
			}

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

		  // set up particle gun
		  G4int nParticles = 1;
		  fParticleGun1 = new G4ParticleGun(nParticles);
		  fParticleGun2 = new G4ParticleGun(nParticles);

		  // define particle properties
		  //const G4String& particleName = "opticalphoton";
		  const G4String& particleName1 = "e+";
		  const G4String& particleName2 = "neutron";

		  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		  G4ParticleDefinition* particle1
			  = particleTable->FindParticle(particleName1);
		  G4ParticleDefinition* particle2
			  = particleTable->FindParticle(particleName2);

		  fParticleGun1->SetParticleDefinition(particle1);
		  fParticleGun2->SetParticleDefinition(particle2);


		  //G4ThreeVector momentumDirection = G4ThreeVector(1, 0, 0);
		  //G4ThreeVector momentumDirection = G4ThreeVector(0, 1, 0);
		  G4ThreeVector momentumDirection = G4ThreeVector(0, 0, 1);
		  //G4ThreeVector momentumDirection = G4ThreeVector(0, 0, -1);
		
	    fParticleGun1->SetParticleMomentumDirection(momentumDirection);
		  fParticleGun2->SetParticleMomentumDirection(momentumDirection);
		  G4double positron_energy = 10.0 * MeV;
		  //G4double neutron_energy = 0.025 * eV;
		  //G4double neutron_energy = 0.05 * eV;
		  G4double neutron_energy = 0.5 * MeV;
		  //G4double neutron_energy = 1e-5 * eV;
		  //G4cout << "Particle energy: " << G4BestUnit(energy, "Energy") << G4endl;

		  fParticleGun1->SetParticleEnergy(positron_energy);
		  fParticleGun2->SetParticleEnergy(neutron_energy);
      G4ThreeVector positron_position = G4ThreeVector(-200.0 * mm, 0 * mm, 0 * mm);
		
		  fParticleGun1->SetParticlePosition(positron_position);

		  fParticleGun1->GeneratePrimaryVertex(event);
		  //G4ThreeVector neutron_position = G4ThreeVector(-200.0 * mm, 10 * mm, 0 * mm);
		  //G4ThreeVector neutron_position = G4ThreeVector(-200.0 * mm, 0 * mm, 0 * mm);
		  G4ThreeVector neutron_position = G4ThreeVector(0.0 * mm, 100 * mm, 0 * mm);
		  //G4ThreeVector neutron_position = G4ThreeVector(0 * mm, 0 * mm, 1 * mm);
		  fParticleGun2->SetParticlePosition(neutron_position);
		  fParticleGun2->GeneratePrimaryVertex(event);
    
   	} // else 
	} // GeneratePrimaries
} // G4_BREMS

