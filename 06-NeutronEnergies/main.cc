#include "RMGHardware.hh"
#include "RMGManager.hh"
#include "RMGOutputManager.hh"

#include "CustomSteppingAction.hh"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "CLI11.hpp"

int main(int argc, char **argv) {
  CLI::App app{"Neutron energies"};
  int nthreads = 16;
  std::string macroName;
  std::string gdmlfilename;

  std::string outputfilename;

  app.add_option("-m,--macro", macroName,
                 "<Geant4 macro filename> Default: None");
  app.add_option("-g,--gdml", gdmlfilename,
                  "<GDML filename> Default: None");
  app.add_option("-t, --nthreads", nthreads,
                 "<number of threads to use> Default: 16");
  app.add_option("-o,--output", outputfilename,
                 "<output filename> Default: build/RestoredOutput.hdf5>");

  CLI11_PARSE(app, argc, argv);

  RMGManager manager("NeutronEnergies", argc, argv);
  // Overwrite RMGPhysics to use own Optical Processes
  std::cout << "current gdml file: " << gdmlfilename << std::endl;
  manager.GetDetectorConstruction()->IncludeGDMLFile(gdmlfilename);

  // Custom User init
  auto user_init = manager.GetUserInit();

  user_init->AddSteppingAction<CustomSteppingAction>();
  

  
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
