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
#include "RMGOutputManager.hh"

namespace u = CLHEP;

std::optional<G4ClassificationOfNewTrack> CustomIsotopeFilter::StackingActionClassify(
    const G4Track* aTrack,
    int stage
) {

  bool temporary_switch = true;
  // we are only interested in stacking into stage 1 after stage 0 finished.
  if (stage != 0) return std::nullopt;
  if (!temporary_switch) return std::nullopt;
  // defer tracking of electrons.
  if (aTrack->GetDefinition() == G4Positron::PositronDefinition() ||
      aTrack->GetDefinition() == G4Electron::ElectronDefinition()) {
      
    // Check if the track is in the "tank" or "tank_water" volume
    auto* volume = aTrack->GetTouchableHandle()->GetVolume();
    if (volume) {
      auto volName = volume->GetName();
      if (volName == "tank" || volName == "tank_water" || volName == "outercryostat") {
        if(aTrack->GetKineticEnergy() < 250 * u::keV) {
          if (aTrack->GetDefinition() == G4Electron::ElectronDefinition()) {
            return fKill; // This might be an issue for positrons
          }
          else {
            return std::nullopt; // Either simulate directly or keep for later. Might make Cerenkov light.
          }
        }
        return fWaiting;
      }
    }
    // Don't defer high energy electrons, as they might cause inelastic scattering and therefore nCaptures
    if (aTrack->GetKineticEnergy() > 10 * u::MeV || aTrack->GetKineticEnergy() < 100 * u::keV) {
      return std::nullopt;
    }
    
    return fWaiting;
  }
  return std::nullopt;
}

// vim: tabstop=2 shiftwidth=2 expandtab
