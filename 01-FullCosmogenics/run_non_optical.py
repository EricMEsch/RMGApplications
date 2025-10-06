import subprocess
from pathlib import Path

macro_file = Path("non_optical.mac")
macro_content = """\
/RMG/Manager/Logging/LogLevel summary

#/tracking/verbose 2

/RMG/Processes/HadronicPhysics Shielding

/RMG/Output/ActivateOutputScheme IsotopeFilter

/RMG/Geometry/RegisterDetectorsFromGDML Germanium

/run/initialize

/RMG/Output/IsotopeFilter/AddIsotope 77 32
/RMG/Output/NtuplePerDetector false

/RMG/Generator/Confine UnConfined

/RMG/Generator/Select MUSUNCosmicMuons
/RMG/Generator/MUSUNCosmicMuons/MUSUNFile musun/combined_file.dat

/run/beamOn 1000000
"""

macro_file.write_text(macro_content)

try:
    # Run Geant4 simulation
    result = subprocess.run(
        [
            "./build/FullCosmogenics",
            "-m", str(macro_file),
            "-g", "gdml/l1000_cosm_no_cavern.gdml",
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

