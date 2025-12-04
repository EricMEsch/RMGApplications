import subprocess
from pathlib import Path

macro_file = Path("optical.mac")
macro_content = """\
/RMG/Manager/Logging/LogLevel summary

#/tracking/verbose 2

/RMG/Processes/HadronicPhysics Shielding
/RMG/Processes/OpticalPhysics true

/RMG/Output/ActivateOutputScheme CustomIsotopeFilter
/RMG/Output/ActivateOutputScheme IsotopeOutputScheme

/RMG/Geometry/RegisterDetectorsFromGDML Germanium
/RMG/Geometry/RegisterDetectorsFromGDML Optical
/RMG/Geometry/RegisterDetector Scintillator atmosphericlar 12000
#/RMG/Geometry/RegisterDetector Scintillator undergroundlar 12001

/run/initialize

/RMG/Output/IsotopeFilter/AddIsotope 77 32
/RMG/Output/IsotopeFilter/DiscardPhotonsIfIsotopeNotProduced true
/RMG/Output/NtuplePerDetector false

/RMG/Processes/Stepping/DaughterNucleusMaxLifetime 1 hour

/RMG/Generator/Confine UnConfined

/RMG/Generator/Select MUSUNCosmicMuons
/RMG/Generator/MUSUNCosmicMuons/MUSUNFile musun/combined_file.dat

#/process/optical/cerenkov/setTrackSecondariesFirst

/RMG/Output/Scintillator/Cluster/PreClusterOutputs true
/RMG/Output/Scintillator/Cluster/CombineLowEnergyElectronTracks true
/RMG/Output/Scintillator/Cluster/RedistributeGammaEnergy true
/RMG/Output/Scintillator/Cluster/PreClusterDistance 1 mm
/RMG/Output/Scintillator/Cluster/PreClusterTimeThreshold 10 us

/run/beamOn 1000000
"""

macro_file.write_text(macro_content)

try:
    # Run Geant4 simulation
    result = subprocess.run(
        [
            "./build/FullCosmogenics",
            "-m", str(macro_file),
            "-g", "gdml/l1000_cosm_giga_reduced_new.gdml",
            "-o", "build/out.lh5",
            "-t", "16"
        ],
        check=True,          # Raises CalledProcessError if nonzero exit
        capture_output=False # Ensures live forwarding to terminal
    )
except subprocess.CalledProcessError as e:
    print(f"❌ Simulation failed with exit code {e.returncode}")
    # Optionally, you can also print stderr if you used capture_output=True
finally:
    # Cleanup: delete macro file
    if macro_file.exists():
        macro_file.unlink()

