#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "Randomize.hh"

#include "G4UnitsTable.hh"

namespace G4_BREMS
{
	PrimaryGeneratorAction::PrimaryGeneratorAction() {
		
		// Things to initialise here
//		messenger = new PrimaryGeneratorMessenger(this);
		useIBDGen = false;
	
	}

	PrimaryGeneratorAction::~PrimaryGeneratorAction() {

		if (particleGun) delete particleGun;
		if (ibdGen) delete ibdGen;
	}

	void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
	{
		
		if (useIBDGen){
                	
			// populate event from IBDGen.cc
			
			// Initialise the IBD generator once per simulation
			// This will read the spectrum from the db
			// and produce a 2D histogram of the spectrum.
			if(!ibdGen){
				G4cout << "PrimaryGeneratorAction::GeneratePrimaries  [INFO] Initialising IBD generator" << G4endl;
				ibdGen = new IBDGen();
				ibdGen->ReadSpectrumFromDB(spectrum_database,spectrum_name);
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
			ibdGen->GenerateEvent(neutrino,positron,neutron,event);

			// Populate truth information to be written out
			// vtx
			// dir
			// positron energy
			// neutron energy
			// positron pdg
			// neutron pdg
			// positron momentum/direction
			// neutron momentum/direcion

		} else {

			// set up particle gun by default
			G4int nParticles = 1;
			particleGun = new G4ParticleGun(nParticles);

			// define particle properties
			const G4String& particleName = "e+";

			G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
			G4ParticleDefinition* particle
				= particleTable->FindParticle(particleName);

			particleGun->SetParticleDefinition(particle);

			G4ThreeVector momentumDirection = G4ThreeVector(1, 0, 0);
			//G4ThreeVector momentumDirection = G4ThreeVector(0, 1, 0);

			//G4cout << "Particle direction: " << momentumDirection << G4endl;

			particleGun->SetParticleMomentumDirection(momentumDirection);

			G4double energy = 10.0 * MeV;
			//G4cout << "Particle energy: " << G4BestUnit(energy, "Energy") << G4endl;

			particleGun->SetParticleEnergy(energy);

			G4ThreeVector position = G4ThreeVector(-200.0 * mm, 0 * mm, 0 * mm);
		
			particleGun->SetParticlePosition(position);

			particleGun->GeneratePrimaryVertex(event);
		}
	}
}
