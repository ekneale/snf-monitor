#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"

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
