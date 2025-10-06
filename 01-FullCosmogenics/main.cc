#include "RMGHardware.hh"
#include "RMGManager.hh"

#include "CosmogenicPhysics.hh"
#include "CustomIsotopeFilter.hh"
#include "CustomMUSUNGenerator.hh"
#include "RNGTrackingAction.hh"
#include "IsotopeOutputScheme.hh"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "CLI11.hpp"

int main(int argc, char **argv) {
  CLI::App app{"Cosmogenic Simulations"};
  int nthreads = 16;
  std::string macroName;
  int rngFlag = 0;
  std::string filename;

  std::string outputfilename = "build/RestoredOutput.hdf5";

  app.add_option("-m,--macro", macroName,
                 "<Geant4 macro filename> Default: None");
  app.add_option("-g,--gdml", filename,
                  "<GDML filename> Default: None");
  app.add_option("-t, --nthreads", nthreads,
                 "<number of threads to use> Default: 16");
  app.add_option("-r,--rng", rngFlag, "RNG restoration mode: 0 deactivated, 1 for prerun, 2 for restoration run");
  app.add_option("-o,--output", outputfilename,
                 "<output filename> Default: build/RestoredOutput.hdf5>");

  CLI11_PARSE(app, argc, argv);

  RMGManager manager("FullCosmogenics", argc, argv);
  // Overwrite RMGPhysics to use own Optical Processes
  std::cout << "current gdml file: " << filename << std::endl;
  manager.GetDetectorConstruction()->IncludeGDMLFile(filename);

  // Custom User init
  auto user_init = manager.GetUserInit();
  
  if (rngFlag != 0) {
    if(rngFlag == 1) {
      user_init->AddOptionalOutputScheme<CustomIsotopeFilter>(
        "CustomIsotopeFilter");
      outputfilename = "build/output.csv";
    }
    else {
      user_init->SetUserGenerator<CustomMUSUNGenerator>();
      user_init->AddTrackingAction<RNGTrackingAction>();
    }
  }
  // Dont ask why but this is here to avoid a segfault
  auto *RunManager = manager.GetG4RunManager();
  RunManager->SetNumberOfThreads(nthreads);
  manager.SetUserInit(new CosmogenicPhysics());

  user_init->AddOptionalOutputScheme<IsotopeOutputScheme>(
    "IsotopeOutputScheme");

  
  // Interactive or batch mode?
  if (!macroName.empty())
    manager.IncludeMacroFile(macroName);
  else
    manager.SetInteractive(true);

  // Outputfilename and Threads. Then run
  
  manager.GetOutputManager()->SetOutputFileName(outputfilename);
  manager.SetNumberOfThreads(nthreads);
  manager.GetOutputManager()->SetOutputOverwriteFiles(true);
  manager.Initialize();
  manager.Run();

  return 0;
}
// vim: tabstop=2 shiftwidth=2 expandtab
