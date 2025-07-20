#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "Randomize.hh"

#include "G4UnitsTable.hh"

namespace G4_BREMS
{
	PrimaryGeneratorAction::PrimaryGeneratorAction() {
		// set up particle gun
		G4int nParticles = 1;
		//fParticleGun1 = new G4ParticleGun(nParticles);
		fParticleGun2 = new G4ParticleGun(nParticles);

		// define particle properties
		//const G4String& particleName = "opticalphoton";
		//const G4String& particleName1 = "e+";
		const G4String& particleName2 = "neutron";

		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		//G4ParticleDefinition* particle1
			//= particleTable->FindParticle(particleName1);
		G4ParticleDefinition* particle2
			= particleTable->FindParticle(particleName2);

		//fParticleGun1->SetParticleDefinition(particle1);
		fParticleGun2->SetParticleDefinition(particle2);


		//G4ThreeVector momentumDirection = G4ThreeVector(1, 0, 0);
		//G4ThreeVector momentumDirection = G4ThreeVector(0, 1, 0);
		G4ThreeVector momentumDirection = G4ThreeVector(0, 0, 1);
		//G4ThreeVector momentumDirection = G4ThreeVector(0, 0, -1);

		//G4cout << "Particle direction: " << momentumDirection << G4endl;

		//fParticleGun1->SetParticleMomentumDirection(momentumDirection);
		fParticleGun2->SetParticleMomentumDirection(momentumDirection);
		G4double positron_energy = 10.0 * MeV;
		//G4double neutron_energy = 0.025 * eV;
		//G4double neutron_energy = 0.05 * eV;
		G4double neutron_energy = 0.5 * MeV;
		//G4double neutron_energy = 3 * MeV;
		//G4double neutron_energy = 1e-5 * eV;
		//G4cout << "Particle energy: " << G4BestUnit(energy, "Energy") << G4endl;

		//fParticleGun1->SetParticleEnergy(positron_energy);
		fParticleGun2->SetParticleEnergy(neutron_energy);

	}

	PrimaryGeneratorAction::~PrimaryGeneratorAction() {
		//delete fParticleGun1;
		delete fParticleGun2;
	}

	void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
	{

		//G4ThreeVector positron_position = G4ThreeVector(-200.0 * mm, 0 * mm, 0 * mm);

		//fParticleGun1->SetParticlePosition(positron_position);

		//fParticleGun1->GeneratePrimaryVertex(event);
		//G4ThreeVector neutron_position = G4ThreeVector(-200.0 * mm, 10 * mm, 0 * mm);
		//G4ThreeVector neutron_position = G4ThreeVector(-200.0 * mm, 0 * mm, 0 * mm);
		//G4ThreeVector neutron_position = G4ThreeVector(0.0 * mm, 100 * mm, 0 * mm);
		//G4ThreeVector neutron_position = G4ThreeVector(100.0 * mm, 100 * mm, 50 * mm); -works
		G4ThreeVector neutron_position = G4ThreeVector(0 * mm, 0 * mm, 0 * mm);
		fParticleGun2->SetParticlePosition(neutron_position);
		fParticleGun2->GeneratePrimaryVertex(event);

	}

}