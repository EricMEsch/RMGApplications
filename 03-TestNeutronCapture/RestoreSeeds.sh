#!/bin/bash

# Step 1: Copy and rename files from /random directory into working directory (thanks GEANT4)
count=0
for file in random/run*evt*.rndm; do
    if [[ -f "$file" ]]; then
        new_name="run0evt${count}.rndm"
        cp "$file" "$new_name"
        ((count++))
    fi
done

# Step 2: Create new run file that keeps properties of run.mac
cp runRMGnCapture.mac RestoreSeedRun.mac

# Remove the lines of RestoreSeedRun.mac that we want different
sed -i '/^\/run\/beamOn/d' RestoreSeedRun.mac
sed -i '/^\/random\/setDirectoryName/d' RestoreSeedRun.mac
sed -i '/^\/random\/setSavingFlag/d' RestoreSeedRun.mac

# Add the command that reads in the seeds and only generate those events
echo "/random/resetEngineFromEachEvent true" >> RestoreSeedRun.mac
echo "/run/beamOn $count" >> RestoreSeedRun.mac

# Step 3: Start simulation
./build/TestNeutronCapture RestoreSeedRun.mac

# Step 4: Delete copied files to clean the working directory
# Leave RestoreSeedRun.mac to be able to check it for issues later
for (( i=0; i<$count; i++ )); do
    rm "run0evt${i}.rndm"
done