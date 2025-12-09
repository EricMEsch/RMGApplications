#!/usr/bin/env python3
import os
import sys
import shutil
import glob
import subprocess
import re
import csv
from pathlib import Path

# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------

def ensure_directory(path):
    """Ensure directory exists. If file exists with same name → error."""
    if os.path.exists(path) and not os.path.isdir(path):
        print(f"Error: '{path}' exists but is not a directory.")
        sys.exit(1)
    if not os.path.isdir(path):
        print(f"Warning: '{path}/' directory not found. Creating it.")
        os.makedirs(path, exist_ok=True)

def run_command(cmd):
    """Run command and check return status."""
    result = subprocess.run(cmd)
    if result.returncode != 0:
        print(f"{cmd[0]} was killed or failed!", file=sys.stderr)
        sys.exit(1)

# ---------------------------------------------------------------------------
# Step 0: Initial directory checks
# ---------------------------------------------------------------------------

# Temporary folders
ensure_directory("random")
ensure_directory("temp")

if not os.path.isdir("musun"):
    print("Error: 'musun/' directory not found. Where are your primaries coming from?")
    sys.exit(1)

# Clear temporary folders
for folder in ("temp", "random"):
    for f in glob.glob(f"{folder}/*"):
        if os.path.isfile(f):
            os.remove(f)
        else:
            shutil.rmtree(f, ignore_errors=True)


# Create pre_run.mac file for first run

macro_file = Path("pre_run.mac")
macro_content = """\
/RMG/Manager/Logging/LogLevel summary

#/tracking/verbose 2

/RMG/Processes/HadronicPhysics Shielding
/RMG/Processes/UseGrabmayrsGammaCascades true

/RMG/GrabmayrGammaCascades/SetGammaCascadeFile 64 155 gammacascades/156Gd-5keV-cascades.dat
/RMG/GrabmayrGammaCascades/SetGammaCascadeFile 64 157 gammacascades/158Gd-5keV-cascades.dat
/RMG/GrabmayrGammaCascades/SetGammaCascadeFile 1 1 gammacascades/water_cascades.txt

#/RMG/Processes/OpticalPhysics true
/RMG/Output/ActivateOutputScheme CustomIsotopeFilter

#/RMG/Geometry/RegisterDetectorsFromGDML Germanium
#/RMG/Geometry/RegisterDetector Optical PMT.* 6000

/run/initialize

/RMG/Output/IsotopeFilter/AddIsotope 77 32
/RMG/Output/NtuplePerDetector false

/RMG/Generator/Confine UnConfined

/RMG/Generator/Select MUSUNCosmicMuons
/RMG/Generator/MUSUNCosmicMuons/MUSUNFile musun/combined_file.dat

/random/setDirectoryName random/
/random/setSavingFlag 1
/run/beamOn 10000000
"""

macro_file.write_text(macro_content)

# ---------------------------------------------------------------------------
# Run simulation without optical physics
# ---------------------------------------------------------------------------

run_command(["./build/FullCosmogenics", "-m", "pre_run.mac", "-r", "1"] + sys.argv[1:])

# ---------------------------------------------------------------------------
# Step 0b: Prepare MUSUN input filtering
# ---------------------------------------------------------------------------

output_files = glob.glob("temp/output_nt_musun_t*.csv")
input_files = glob.glob("build/output_nt_musun_t*.csv")
temp_file = "temp/temp_musun.dat"
concatenated_file = "temp/all_output_nt_musun.csv"

# Move MUSUN input files to temp
for f in input_files:
    shutil.move(f, "temp/")

# Empty the temp files
open(temp_file, "w").close()
open(concatenated_file, "w").close()

# Concatenate all CSVs (strip comment lines)
with open(concatenated_file, "w") as fout:
    for file in output_files:
        if not os.path.isfile(file):
            print(f"Warning: File {file} not found.")
            continue
        with open(file) as fin:
            for line in fin:
                if not line.startswith("#"):
                    fout.write(line)

print(f"Concatenated all files into {concatenated_file}")

# Load concatenated CSV into memory
concatenated_rows = []
with open(concatenated_file) as fin:
    reader = csv.reader(fin)
    for row in reader:
        if len(row) >= 9:
            concatenated_rows.append(row)

# ---------------------------------------------------------------------------
# Step 1: Copy and process random seeds + match MUSUN events
# ---------------------------------------------------------------------------

count = 0
random_files = glob.glob("random/run*evt*.rndm")

with open(temp_file, "a") as tf:
    for file in sorted(random_files):
        if not os.path.isfile(file):
            continue

        new_name = f"run0evt{count}.rndm"
        shutil.copy(file, new_name)

        # Extract event number
        evt_match = re.search(r"evt(\d+)", file)
        evt_number = int(evt_match.group(1)) if evt_match else None

        print(f"Processing file: {file} with evt_number: {evt_number}")

        # Find matching CSV row (first column == evt_number)
        match = next((row for row in concatenated_rows
                      if row[0].isdigit() and int(row[0]) == evt_number),
                     None)

        if match:
            print(f"Matching line found for evt_number {evt_number}: {match}")

            evtid = count
            type_, Ekin, x, y, z, px, py, pz = match[1:10]

            formatted_line = f"{evtid:8d} {int(type_):2d} {float(Ekin):8.1f} {float(x):8.1f} {float(y):8.1f} {float(z):8.1f} {float(px):8.6f} {float(py):8.6f} {float(pz):8.6f}\n"
            tf.write(formatted_line)
        else:
            print(f"No matching line found for evt_number {evt_number}")

        count += 1

# ---------------------------------------------------------------------------
# Step 2: Build re_run.mac
# ---------------------------------------------------------------------------

# Create re_run.mac file for second run

macro_file = Path("re_run.mac")
macro_content = """\
/RMG/Manager/Logging/LogLevel summary

#/tracking/verbose 2

/RMG/Processes/HadronicPhysics Shielding
/RMG/Processes/UseGrabmayrsGammaCascades true

/RMG/GrabmayrGammaCascades/SetGammaCascadeFile 64 155 gammacascades/156Gd-5keV-cascades.dat
/RMG/GrabmayrGammaCascades/SetGammaCascadeFile 64 157 gammacascades/158Gd-5keV-cascades.dat
/RMG/GrabmayrGammaCascades/SetGammaCascadeFile 1 1 gammacascades/water_cascades.txt

/RMG/Processes/OpticalPhysics true

/RMG/Geometry/RegisterDetectorsFromGDML Germanium
/RMG/Geometry/RegisterDetectorsFromGDML Optical

/RMG/Geometry/RegisterDetector Scintillator atmosphericlar 12000
/RMG/Output/ActivateOutputScheme Track
/RMG/Output/ActivateOutputScheme IsotopeFilter

/run/initialize

/RMG/Output/IsotopeFilter/AddIsotope 77 32
/RMG/Output/NtuplePerDetector false

/RMG/Generator/Confine UnConfined

/RMG/Generator/Select UserDefined
/Cosmogenics/Generator/SetMUSUNFile temp/temp_musun.dat
/random/resetEngineFromEachEvent true


"""

macro_file.append(f"/run/beamOn {count}\n")

macro_file.write_text(macro_content)


# Clear build folder
for f in glob.glob("build/*.csv"):
    if os.path.isfile(f):
        os.remove(f)
    else:
        shutil.rmtree(f, ignore_errors=True)


# ---------------------------------------------------------------------------
# Step 3: Run full simulation with optical physics
# ---------------------------------------------------------------------------

run_command(["./build/FullCosmogenics", "-m", "re_run.mac", "-r", "2"] + sys.argv[1:])

# ---------------------------------------------------------------------------
# Step 4: Cleanup
# ---------------------------------------------------------------------------

for i in range(count):
    fname = f"run0evt{i}.rndm"
    if os.path.exists(fname):
        os.remove(fname)

if os.path.exists(concatenated_file):
    os.remove(concatenated_file)

# print summary
print(f"Simulation completed. Processed {count} Ge77 producing events.")
