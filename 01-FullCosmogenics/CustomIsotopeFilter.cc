// Copyright (C) 2022 Luigi Pertoldi <gipert@pm.me>
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "CustomIsotopeFilter.hh"

#include <set>

#include "G4Electron.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4Positron.hh"
#include "G4RunManager.hh"

#include "RMGLog.hh"

std::optional<G4ClassificationOfNewTrack>
CustomIsotopeFilter::StackingActionClassify(const G4Track *aTrack, int stage) {
  // we are only interested in stacking into stage 1 after stage 0 finished.
  if (stage != 0)
    return std::nullopt;

  // defer tracking of optical photons and charged particles in the optical
  // medium (to reduce memory consumption).
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    return fWaiting;
  if (aTrack->GetParentID() < 1)
    return std::nullopt;
  const auto pv_name = aTrack->GetTouchableHandle()->GetVolume()->GetName();
  auto particle = aTrack->GetDefinition();
  if (pv_name == "InnerWater_phys" &&
      (particle == G4Electron::ElectronDefinition() ||
       particle == G4Positron::PositronDefinition()))
    return fWaiting;
  return std::nullopt;
}

void CustomIsotopeFilter::StoreEvent(const G4Event *event) {
  // stores the random seed for this event to be reproducible
  if (!ShouldDiscardEvent(event))
    G4RunManager::GetRunManager()->rndmSaveThisEvent();
}

// vim: tabstop=2 shiftwidth=2 expandtab
