#include "CustomSteppingAction.hh"

#include "G4AnalysisManager.hh"
#include "G4HadronicProcess.hh"
#include "G4Ions.hh"
#include "G4ParticleTable.hh"
#include "G4Step.hh"
#include "G4Threading.hh"

#include "RMGEventAction.hh"
#include "RMGLog.hh"
#include "RMGOutputManager.hh"

namespace u = CLHEP;

CustomSteppingAction::CustomSteppingAction() {
const auto ana_man = G4AnalysisManager::Instance();
  auto vid = RMGOutputManager::Instance()
                 ->CreateAndRegisterAuxNtuple("steps", "CustomSteppingAction", ana_man);

  ana_man->CreateNtupleIColumn(vid, "Z_of_Target");
  ana_man->CreateNtupleIColumn(vid, "N_of_Target");
  ana_man->CreateNtupleDColumn(vid, "Prestep_ekin_in_MeV");
  ana_man->CreateNtupleDColumn(vid, "Poststep_ekin_in_MeV");

  ana_man->FinishNtuple(vid);
}

void CustomSteppingAction::UserSteppingAction(const G4Step* step) {

  G4Track *track = step->GetTrack();
  
  const G4ParticleDefinition *particle = track->GetParticleDefinition();
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable(); // Load the particle table with all stored properties
  G4String particleName ="neutron";
  G4ParticleDefinition *requiredParticle = particleTable->FindParticle(particleName);

  if(particle == requiredParticle && track->GetTrackStatus() == fStopAndKill) {
    const G4StepPoint* endPoint = step->GetPostStepPoint();
    G4VProcess* process = const_cast<G4VProcess*>(endPoint->GetProcessDefinedStep());
    if(process->GetProcessName() == "nCapture") {
        G4HadronicProcess* hproc = dynamic_cast<G4HadronicProcess*>(process);
        const G4Isotope* target = NULL;
        if (hproc) target = hproc->GetTargetIsotope();

        G4int targetZ = target->GetZ();
        G4int targetN = target->GetN();
        G4double PreStep_energy = step->GetPreStepPoint()->GetKineticEnergy() / u::MeV;
        G4double PostStep_energy = step->GetPostStepPoint()->GetKineticEnergy() / u::MeV;

        const auto ana_man = G4AnalysisManager::Instance();
        auto rmg_man = RMGOutputManager::Instance();
        auto ntupleid = rmg_man->GetAuxNtupleID("steps");

        int col_id = 0;
        ana_man->FillNtupleIColumn(ntupleid, col_id++, targetZ);
        ana_man->FillNtupleIColumn(ntupleid, col_id++, targetN);
        ana_man->FillNtupleDColumn(ntupleid, col_id++, PreStep_energy);
        ana_man->FillNtupleDColumn(ntupleid, col_id++, PostStep_energy);
        ana_man->AddNtupleRow(ntupleid);
    }
  }

}


// vim: tabstop=2 shiftwidth=2 expandtab
