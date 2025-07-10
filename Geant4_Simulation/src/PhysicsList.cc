
#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4LossTableManager.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysicsWVI.hh"
#include "G4EmStandardPhysicsSS.hh"
#include "G4EmStandardPhysicsGS.hh"

#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

//#include "QGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
//#include "G4NeutronTrackingCut.hh"
//#include "G4NeutronHPCapture.hh"
//#include "G4Neutron.hh"
//#include "G4ProcessManager.hh"
//#include "G4VDiscreteProcess.hh"

//#include "G4NeutronRadCapture"
//#include "G4NeutronCaptureXS"





#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessTable.hh"

// Processes
//#include "globals.hh"
#include "G4HadronElasticProcess.hh"
#include "G4ParticleHPElasticData.hh"
#include "G4ParticleHPThermalScatteringData.hh"
#include "G4ParticleHPElastic.hh"
#include "G4ParticleHPThermalScattering.hh"

//#include "G4NeutronInelasticProcess.hh"
#include "G4ParticleHPInelasticData.hh"
#include "G4ParticleHPInelastic.hh"

//#include "G4HadronCaptureProcess.hh"
#include "G4ParticleHPCaptureData.hh"
#include "G4ParticleHPCapture.hh"

//#include "G4HadronFissionProcess.hh"
#include "G4ParticleHPFissionData.hh"
#include "G4ParticleHPFission.hh"


namespace G4_BREMS
{
    PhysicsList::PhysicsList() : G4VModularPhysicsList()
    {

        RegisterPhysics(new G4EmStandardPhysics_option4());
        RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
        

        G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();

        auto opticalParams = G4OpticalParameters::Instance();
        opticalParams->SetWLSTimeProfile("delta");
        opticalParams->SetProcessActivation("OpWLS", true);
        
        RegisterPhysics(opticalPhysics);
       
        
        
    }
    void PhysicsList::ConstructParticle()
    {
        G4VModularPhysicsList::ConstructParticle();
    }

    void PhysicsList::ConstructProcess()
    {
        G4VModularPhysicsList::ConstructProcess();

        /*
        G4ParticleDefinition* neutron = G4Neutron::Neutron();
        G4ProcessManager* pManager = neutron->GetProcessManager();

        
        G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
        G4VProcess* process1 = 0;
        process1 = processTable->FindProcess("hadElastic", neutron);
        pManager->AddProcess(process1);
        
        G4VProcess* process2 = 0;
        process2 = processTable->FindProcess("neutronInelastic", neutron);
        pManager->AddProcess(process2);
        
        G4VProcess* process3 = 0;
        process3 = processTable->FindProcess("nCapture", neutron);
        pManager->AddProcess(process3);
        
        G4VProcess* process4 = 0;
        process4 = processTable->FindProcess("nFission", neutron);
        pManager->AddProcess(process4);
        */
        /*
        auto neutron = G4Neutron::NeutronDefinition();
        auto pManager = neutron->GetProcessManager();
        auto table = G4ProcessTable::GetProcessTable();
        auto elastic = table->FindProcess("neutronHPElastic", neutron);
        if (elastic) {
            pManager->AddProcess(elastic, ordDefault, ordDefault, ordDefault);
        }

        G4VProcess* inelastic = table->FindProcess("neutronHPInelastic", neutron);
        if (inelastic) {
            // discrete inelastic interactions happen post-step
            pManager->AddDiscreteProcess(inelastic);
        }

        auto capture = table->FindProcess("nCapture", neutron);
        if (capture) {
            pManager->AddDiscreteProcess(capture);
        }
        G4VProcess* fission = table->FindProcess("neutronHPFission", neutron);
        if (fission) {
            pManager->AddDiscreteProcess(fission);
        }
        this->SetVerboseLevel(1);

        */

    }
}





