#ifndef G4_BREMS_PRIMARY_GENERATOR_ACTION_H
#define G4_BREMS_PRIMARY_GENERATOR_ACTION_H 1

#include "G4VUserPrimaryGeneratorAction.hh"

#include "IBDGen.hh"

#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleGun;
class G4Event;

namespace G4_BREMS
{
	class PrimaryGeneratorMessenger;

	// PrimaryGeneratorAction class
	class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
	{
	public:
		PrimaryGeneratorAction();
		~PrimaryGeneratorAction();

		virtual void GeneratePrimaries(G4Event*);
	private:
		G4ParticleGun*			fParticleGun;
		IBDGen* 			fIBDGen;
		PrimaryGeneratorMessenger*	fGenMessenger;

		// Variables set by the messenger
		G4bool 				useIBDGen;
		G4String 			spectrum_database;
		G4String 			spectrum_name;

		// Variables for saving vertex information
                G4double fNuEnergy;
                G4ThreeVector fNuDir;
                G4ThreeVector fVertex;
                G4double fPositronEnergy;
                G4ThreeVector fPositronDir;
                G4double fNeutronEnergy;
                G4ThreeVector fNeutronDir;

	public: 
		// Addition of IBD event generator
		inline void SetIBDGenerator(G4bool choice) { useIBDGen = choice; }
		inline G4bool IsUsingIBDGen() { return useIBDGen; }
		inline void SetIBDDatabase(G4String choice) { spectrum_database = choice; }
		inline G4String GetIBDDatabase()  { return spectrum_database; }
		inline void SetIBDSpectrum(G4String choice) { spectrum_name = choice; }
		inline G4String GetIBDSpectrum()  { return spectrum_name; }
	};
}// namespace G4_BREMS

#endif 
