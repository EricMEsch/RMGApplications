
#include "IsotopeOutputScheme.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

#include "RMGLog.hh"
#include "RMGManager.hh"

namespace u = CLHEP;

IsotopeOutputScheme::IsotopeOutputScheme() {}

void IsotopeOutputScheme::ClearBeforeEvent() {
  zOfEvent.clear();
  aOfEvent.clear();
  OriginVolumeIDs.clear();
}

// invoked in RMGRunAction::SetupAnalysisManager()
void IsotopeOutputScheme::AssignOutputNames(G4AnalysisManager *ana_man) {

  auto rmg_man = RMGManager::Instance();

  auto id = rmg_man->RegisterNtuple(
      OutputRegisterID, ana_man->CreateNtuple("IsotopeOutput", "Event data"));

  ana_man->CreateNtupleIColumn(id, "evtid");
  // Could be done with particle PDG but this way it is human readable
  ana_man->CreateNtupleIColumn(id, "Z");
  ana_man->CreateNtupleIColumn(id, "A");
  ana_man->CreateNtupleIColumn(id, "OriginVolumeID"); // 1 for in Water volume (water, GdPMMA etc.), 2 for in Steel, 0 elsewhere

  ana_man->FinishNtuple(id);
}

void IsotopeOutputScheme::TrackingActionPre(const G4Track *aTrack) {
  const auto particle = aTrack->GetParticleDefinition();
  // if (!particle->IsGeneralIon()) return;
  const int z = particle->GetAtomicNumber();
  const int a = particle->GetAtomicMass();
  if (z == 0 || a == 0)
    return; // not an isotope, but any other particle.

  G4String CreatorName = aTrack->GetCreatorProcess()->GetProcessName();
  G4int id =
      G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

  
  if (CreatorName == "RMGnCapture" || CreatorName == "nCapture") {
    zOfEvent.push_back(z);
    aOfEvent.push_back(a);
    auto OriginVolume = aTrack->GetLogicalVolumeAtVertex();
    G4String OriginVolumeName;
    if(OriginVolume)
      OriginVolumeName= OriginVolume->GetName();
    else
      OriginVolumeName = "Unknown";

    if(G4StrUtil::contains(OriginVolumeName, "Water") || G4StrUtil::contains(OriginVolumeName, "PMMA"))
      OriginVolumeIDs.push_back(1);
    else if(G4StrUtil::contains(OriginVolumeName, "Cout") || G4StrUtil::contains(OriginVolumeName, "Tank"))
      OriginVolumeIDs.push_back(2);
    else
      OriginVolumeIDs.push_back(0);
  }
}

// invoked in RMGEventAction::EndOfEventAction()
void IsotopeOutputScheme::StoreEvent(const G4Event *event) {
  auto rmg_man = RMGManager::Instance();
  if (rmg_man->IsPersistencyEnabled()) {
    RMGLog::OutDev(RMGLog::debug, "Filling persistent data vectors");
    const auto ana_man = G4AnalysisManager::Instance();

    for (size_t i = 0; i < zOfEvent.size(); ++i) {
      auto ntupleid = rmg_man->GetNtupleID(OutputRegisterID);

      int col_id = 0;
      ana_man->FillNtupleIColumn(ntupleid, col_id++, event->GetEventID());
      ana_man->FillNtupleIColumn(ntupleid, col_id++, zOfEvent[i]);
      ana_man->FillNtupleIColumn(ntupleid, col_id++, aOfEvent[i]);
      ana_man->FillNtupleIColumn(ntupleid, col_id++, OriginVolumeIDs[i]);

      // NOTE: must be called here for hit-oriented output
      ana_man->AddNtupleRow(ntupleid);
    }
  }
}

// vim: tabstop=2 shiftwidth=2 expandtab
