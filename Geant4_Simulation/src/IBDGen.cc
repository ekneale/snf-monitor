

#include "IBDGen.hh"
#include "DetectorConstruction.hh"

#include <fstream>
#include <nlohmann/json.hpp>

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"

// Set json to nlohmann::json
using json = nlohmann::json;

namespace G4_BREMS{

IBDGen::IBDGen()
{
	// Things to initialise
	snfmondir = std::string(getenv("SNF_MONITOR_DATADIR"));	
	myIBDGun = new G4ParticleGun();
	// Initialised
	G4cout << "IBDGen: [INFO] Initialised IBDGen" << G4endl;

}

IBDGen::~IBDGen() 
{
	// Delete things here
	if (myIBDGun) delete myIBDGun;
}

void IBDGen::ReadSpectrumFromDB(G4String spectrum_database, G4String spectrum_name) 
{
	// Read model from database
	G4cout << "IBDGen: [INFO] spectrum database " << spectrum_database << " found." << G4endl;
	G4cout << "IBDGen: [INFO] using spectrum " << spectrum_name << G4endl;

	// Define the database to read from
	std::string db = spectrum_database;

	if (db.empty())
	{
		db = snfmondir + "IBDSpectra.json";
	}
	else
	{
		db = snfmondir + spectrum_database;
	}

	// Open the database
	std::ifstream spectrum_json(db.c_str());

	// Check that the database has been found
	if (!spectrum_json.is_open()) 
	{
		G4cerr << "IBDGen: \033[31m[ERROR]\033[0m spectrum database " << db << " not found."  << G4endl;
		exit(-1);
	}

	std::stringstream buffer;
	buffer << spectrum_json.rdbuf();

	json data = json::parse(buffer.str());

	// Loop over the json
	for (const auto &spectrum : data["spectra"]) 
	{
		if (spectrum["name"].get<std::string>() == spectrum_name) 
		{
			energyVector = spectrum["energy"].get<std::vector<float>>();
			fluxVector = spectrum["flux"].get<std::vector<float>>();

			// If the energy vector is not monotonically increasing wrt elements then exit
//			if (!std::is_sorted(energyVector.begin(), energyVector.end())) 
//			{
//				G4cerr << "IBDGen: \033[31m[ERROR]\033[0m energy vector is not monotonically increasing. Check "  
//					<< spectrum_database << G4endl;
//				exit(-1);
//			}

			// Check if the flux vector and energy vector have the same size
			if (energyVector.size() != fluxVector.size()) 
			{
				G4cerr << "IBDGen: \033[31m[ERROR]\033[0m energy and flux vectors have different sizes. Energy: "
					<< energyVector.size() << ", flux: " << fluxVector.size() << ". Check " << spectrum_database << G4endl;
				exit(-1);
			}

			// Set the minimum and maximum energy
			e_min = *std::min_element(energyVector.begin(),energyVector.end());
			e_max = *std::max_element(energyVector.begin(),energyVector.end());

			// Set the maximum flux
			flux_max = *std::max_element(fluxVector.begin(), fluxVector.end());
			xsec_flux_max = MaxXSecFlux();
			
			if (debug)
				G4cout << "IBDGen: [INFO] emin, emax, flux max, dsigma/dt(flux_max): " << e_min << ", " << e_max << ", " << flux_max << ", " << xsec_flux_max << G4endl;
			return;

		}
	}

	// If we get here, the spectrum was not found
	G4cerr << "IBDGen: \033[31m[ERROR]\033[0m spectrum \033[31m" << spectrum_name << "\033[0m not found in database "
		<< spectrum_database << G4endl;
	exit(-1);
	return;
}

void IBDGen::GenerateEvent(G4LorentzVector &neutrino, G4LorentzVector &positron, G4LorentzVector &neutron, G4Event* event)
{

	// Generate neutrino direction
	// TODO set neutrino direction from macro
	// For now, let's take the neutrino direction as +y
	G4ThreeVector nu_dir;
	nu_dir.set(0.0,1.0,0.0);
        
	// Generate neutrino interaction vertex
	G4ThreeVector vtx = GenerateRandomPosition();
        
	// Pick energy of neutrino and relative direction of positron
	float e_nu, cos_theta;
	GenerateNeutrinoKinematics(e_nu, cos_theta);

	// Compute neutrino 4-momentum
	neutrino.setVect(nu_dir * e_nu);
	neutrino.setE(e_nu);

        // Calculate positron and neutron kinematics
	
	GeneratePositronKinematics(positron, e_nu, cos_theta,nu_dir);
	GenerateNeutronKinematics(neutron, positron, neutrino);

	// Generate primary vertices
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	// Generate the positron
        myIBDGun->SetParticlePosition(vtx);
	myIBDGun->SetParticleDefinition(particleTable->FindParticle("e+"));
	myIBDGun->SetParticleEnergy(positron.getT());
	myIBDGun->SetParticleMomentumDirection(positron.getV());
	myIBDGun->SetParticleTime(0.);
	if (event)
	{
		myIBDGun->GeneratePrimaryVertex(event);
	}

	// Generate the neutron
	myIBDGun->SetParticlePosition(vtx);
	myIBDGun->SetParticleDefinition(particleTable->FindParticle("neutron"));
	myIBDGun->SetParticleEnergy(neutron.getT());
	myIBDGun->SetParticleMomentumDirection(neutron.getV());
	myIBDGun->SetParticleTime(0.);
	if (event)
	{
		myIBDGun->GeneratePrimaryVertex(event);
	}

}

void IBDGen::GenerateNeutrinoKinematics(float &rand_ene, float &rand_cos_theta) {

	G4bool passed = false;

	// Rejection sampling
	while (!passed) {
		
		// Pick energy and direction uniformly in correct range
		rand_ene = e_min + (e_max - e_min) * G4UniformRand();
		rand_cos_theta = -1.0 + 2.0 * G4UniformRand();

		// Sample point from a uniform distribution defined by the 
		// maximum value of cross section * flux, with the
		// width of the energy range defined in the spectrum model
		double g_of_x = (1 / (e_max - e_min));
		double scale = (g_of_x / (1.005 * xsec_flux_max));
		double xs_flux_test = (1 / scale) * g_of_x * G4UniformRand();

		// Cross section
		G4double xs_weight = dSigmaBydCosTheta(rand_ene, rand_cos_theta);

		// Skip if xs_weight is nan. This happens below the IBD energy threshold of ~1.8 MeV
		if (std::isnan(xs_weight)) {
		    continue;
		}

		// Flux at rand_ene
		G4double flux_weight = InterpolateSpectrum(rand_ene);

		passed = (xs_flux_test < xs_weight * flux_weight);
	}
}

double IBDGen::MatrixElement(double e_nu, double e_e) {
	
	// Calculates and return the value of the absolute matrix element squared

	// Calculate the Mandelstam variables. See above equation 11
	double s_minus_u = 2 * CLHEP::proton_mass_c2 * (e_nu + e_e) - pow(CLHEP::electron_mass_c2, 2.);
	double t = pow(CLHEP::neutron_mass_c2, 2) - pow(CLHEP::proton_mass_c2, 2) - 2 * CLHEP::proton_mass_c2 * (e_nu - e_e);

	// Calculate f1, f2, g1 and g2. See equation 7
	double x = 0 + t / (4 * pow(M_av, 2));
	double y = 1 - t / pow(M_V_squared, 2);
	double z = 1 - t / pow(M_A_squared, 2);
	double f1 = (1 - (1 + xi) * x) / ((1 - x) * pow(y, 2));
	double f2 = xi / ((1 - x) * pow(y, 2));
	double g1 = g_1_0 / pow(z, 2);
	double g2 = 2 * pow(M_av, 2) * g1 / (pow(139.56995, 2) - t);

	// Calculate A, B and C. See equation 10
	double A = 1. / 16. *
	       ((t - pow(CLHEP::electron_mass_c2, 2)) *
		    (4 * pow(f1, 2) * (4 * pow(M_av, 2) + t + pow(CLHEP::electron_mass_c2, 2)) +
		     4 * pow(g1, 2) * (-4 * pow(M_av, 2) + t + pow(CLHEP::electron_mass_c2, 2)) +
		     pow(f2, 2) * (pow(t, 2) / pow(M_av, 2) + 4 * t + 4 * pow(CLHEP::electron_mass_c2, 2)) +
		     4 * pow(CLHEP::electron_mass_c2, 2) * t * pow(g2, 2) / pow(M_av, 2) +
		     8 * f1 * f2 * (2 * t + pow(CLHEP::electron_mass_c2, 2)) +
		     16 * pow(CLHEP::electron_mass_c2, 2) * g1 * g2) -
		pow(delta, 2) * ((4 * pow(f1, 2) + t * pow(f2, 2) / pow(M_av, 2)) *
				     (4 * pow(M_av, 2) + t - pow(CLHEP::electron_mass_c2, 2)) +
				 4 * pow(g1, 2) * (4 * pow(M_av, 2) - t + pow(CLHEP::electron_mass_c2, 2)) +
				 4 * pow(CLHEP::electron_mass_c2, 2) * pow(g2, 2) *
				     (t - pow(CLHEP::electron_mass_c2, 2)) / pow(M_av, 2) +
				 8 * f1 * f2 * (2 * t - pow(CLHEP::electron_mass_c2, 2)) +
				 16 * pow(CLHEP::electron_mass_c2, 2) * g1 * g2) -
		32 * pow(CLHEP::electron_mass_c2, 2) * M_av * delta * g1 * (f1 + f2));

	double B = 1. / 16. *
	       (16 * t * g1 * (f1 + f2) +
		4 * pow(CLHEP::electron_mass_c2, 2) * delta * (pow(f2, 2) + f1 * f2 + 2 * g1 * g2) / M_av);

	double C = 1. / 16. * (4 * (pow(f1, 2) + pow(g1, 2)) - t * pow(f2, 2) / pow(M_av, 2));

	// Calculate the absolute value of the matrix element squared. See equation 5.
	double abs_M_squared = A - (s_minus_u)*B + pow((s_minus_u), 2) * C;

	return abs_M_squared;
}

double IBDGen::dSigmaBydt(double e_nu, double e_e) {

	// Mandelstam variable s. See equation 7.
	double s = 2 * CLHEP::proton_mass_c2 * e_nu + pow(CLHEP::proton_mass_c2, 2);

	// Calculate dSigma/dt. See equation 3.
	double dsigma_dt = pow(G_f, 2) * pow(cos_cabibbo, 2) /
	       (2 * CLHEP::pi * pow((s - pow(CLHEP::proton_mass_c2, 2)), 2)) * MatrixElement(e_nu, e_e);

	return dsigma_dt;
}

double IBDGen::dSigmaBydEe(double e_nu, double e_e) {

	// Calculates dSigma/dE_e and applies the one-loop radiative correction

	// Calculate dSigma/dEe. See equation 11
	double dSigma_by_dEe = 2 * CLHEP::proton_mass_c2 * dSigmaBydt(e_nu, e_e);

	// Apply the one-loop radiative correction. See equation 14.
	double dSigma_by_dEe_corrected = RadiativeCorrection(dSigma_by_dEe, e_e);

	return dSigma_by_dEe_corrected;
}

double IBDGen::RadiativeCorrection(double dSigma_by_dEe, double e_e) {
	
	// Calculate and apply the radiative correction. See equation 14
	// This correction is valued for E_nu << m_p
	double rad_corrected = dSigma_by_dEe * (1 + alpha / CLHEP::pi *
		    (6.00 + 3. / 2. * log10(CLHEP::proton_mass_c2 / (2 * e_e)) +
		     1.2 * pow(CLHEP::electron_mass_c2 / e_e, 1.5)));

	return rad_corrected;
}

double IBDGen::dSigmaBydCosTheta(double e_nu, double cos_theta) {

	// Simplifies the cross section expression when expanding in powers of epsilon. See equation 8.
	double epsilon = e_nu / CLHEP::proton_mass_c2;

	// Get the positron energy corresponding to the neutrino energy and scattering angle.
	double e_e = GetEe(e_nu, cos_theta);

	// Energy momentum relation. See equation 21
	double p_e = sqrt(pow(e_e, 2) - pow(CLHEP::electron_mass_c2, 2));

	// Calculate dSigma/dcos(theta). See equation 20
	double dSigma_by_dCosTheta =
		p_e * epsilon / (1 + epsilon * (1 - e_e / p_e * cos_theta)) * dSigmaBydEe(e_nu, e_e);

	return dSigma_by_dCosTheta * CLHEP::hbarc * CLHEP::hbarc;
}

double IBDGen::GetEe(double e_nu, double cos_theta) {

	// See above comment and equation 8.
	double epsilon = e_nu / CLHEP::proton_mass_c2;

	// See below equation 21 for kappa definition
	double kappa = pow(1 + epsilon, 2) - pow(epsilon * cos_theta, 2);

	// Positron energy calculation. See equation 21
	double e_e = ((e_nu - delta_cm) * (1 + epsilon) +
		epsilon * cos_theta * sqrt(pow(e_nu - delta_cm, 2) - 
			pow(CLHEP::electron_mass_c2, 2) * kappa)) / kappa;

	return e_e;
}

G4ThreeVector IBDGen::GenerateRandomPosition() {

        G4cout << "IBDGen: [INFO] Generating random position" << G4endl;

	// Generate random neutrino position
	// TODO access detector size dynamically
	// Pick random position in the detector
	double x_nu = -20*CLHEP::cm + (40*CLHEP::cm * G4UniformRand());
	double y_nu = -20*CLHEP::cm + (40*CLHEP::cm * G4UniformRand());
	double z_nu = -1*CLHEP::cm + (2*CLHEP::cm * G4UniformRand());

	G4ThreeVector nu_pos;

	// Set the nu_pos vector to the values generated above
	nu_pos.setX(x_nu);
	nu_pos.setY(y_nu);
	nu_pos.setZ(z_nu);

	return nu_pos;
}

void IBDGen::GeneratePositronKinematics(G4LorentzVector &positron, float e_nu, float cos_theta, G4ThreeVector nu_dir){

        G4cout << "IBDGen: [INFO] Generating positron" << G4endl;
	// First order correction to positron quantities
	// for finite nucleon mass
	double e1 = GetEe(e_nu, cos_theta);

	double p1 = sqrt(e1 * e1 - CLHEP::electron_mass_c2 * CLHEP::electron_mass_c2);

	// Compute positron 4-momentum
	G4ThreeVector pos_momentum(p1 * nu_dir);

	// Rotation from nu direction to positron direction.
	double theta = acos(cos_theta);
	double phi = 2 * CLHEP::pi * G4UniformRand(); // Random phi
	G4ThreeVector rotation_axis = nu_dir.orthogonal();
	rotation_axis.rotate(phi, nu_dir);
	pos_momentum.rotate(theta, rotation_axis);

	positron.setVect(pos_momentum);
	positron.setE(e1);

}

void IBDGen::GenerateNeutronKinematics(G4LorentzVector &neutron, G4LorentzVector positron, G4LorentzVector neutrino){

        G4cout << "IBDGen: [INFO] Generating neutron" << G4endl;
	// Compute neutron 4-momentum
	neutron.setVect(neutrino.vect() - positron.vect());
	neutron.setE(sqrt(neutron.vect().mag2() + CLHEP::neutron_mass_c2 * CLHEP::neutron_mass_c2));

}

double IBDGen::InterpolateSpectrum(float ene) {

    // Interpolate the spectrum at the energies in ener_vec
    // The spectrum is given by the vectors ener_vec and flux_vec

    // Loop over the energies in energy
    for (size_t i = 1; i < energyVector.size(); i++) {
        if (ene <= energyVector[i]) {
            // Perform linear interpolation
            double e1 = energyVector[i - 1];
            double e2 = energyVector[i];
            double f1 = fluxVector[i - 1];
            double f2 = fluxVector[i];

            double interpolated_value = f1 + (f2 - f1) * (ene - e1) / (e2 - e1);
            return interpolated_value;
        }
    }

    // If energy is larger than the maximum energy in the spectrum, return the final flux value
    return fluxVector.back();
}

double IBDGen::MaxXSecFlux() {
    // Loops over the energies in the flux model and calculates the maximum value of dSigma/dCosTheta * flux.
    double xs_flux_max = 0.0;

    for (size_t i = 0; i < energyVector.size(); i++) {
        // Calculate the cross section for forward scattered positrons and backward scattered positrons. One of
        // these values will always represent the maximum cross section across the energy range (forward at high
        // energies and backward at low energies)
        double x_sec_forward = dSigmaBydCosTheta(energyVector[i], 1.0);
        double x_sec_backward = dSigmaBydCosTheta(energyVector[i], -1.0);

        // Get the flux from the flux vector
        double flux_at_ene = fluxVector[i];

        // Calculate the max cross section * flux
        if (std::max(x_sec_forward, x_sec_backward) * flux_at_ene > xs_flux_max) {
            xs_flux_max = std::max(x_sec_forward, x_sec_backward) * flux_at_ene;
        }
    }

    // Check if xs_flux_max is still 0.0. If it is then something has gone wrong.
    if (xs_flux_max == 0.0) {
        G4cerr << "IBDGen: \033[31m[ERROR]\033[0m maximum value of (cross section * flux) is 0.0. Check the flux "
                  "database."
               << G4endl;
        exit(-1);
    }

    return xs_flux_max;
}

} // namespace G4_BREMS
