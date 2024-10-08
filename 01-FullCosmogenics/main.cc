#include "RMGHardware.hh"
#include "RMGLog.hh"
#include "RMGManager.hh"

#include "CustomIsotopeFilter.hh"
#include "HardwareQEOverride.hh"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// The names can also be hardcoded when following a strict name convention
// But as the number of rows and columns can change in the future this is better
// Still the PMT name needs to start with "PMT"!
std::vector<std::string> getPMTNames(std::string filename) {
  std::vector<std::string> PMTnames;
  std::ifstream gdmlfile;
  gdmlfile.open(filename);
  std::string key = "physvol name=\"PMT"; // The physical volume names have this
                                          // as indicator before them
  if (!gdmlfile) {
    throw std::runtime_error("Error opening file: " + filename);
  }
  // Search the file for a physical volume that starts with "PMT"
  std::string line;
  while (std::getline(gdmlfile, line)) {
    size_t pos = line.find(key);
    if (pos != std::string::npos) {
      line.erase(0, pos + key.length());
      pos = line.find("0x"); // Start of the hexadecimal pointer that will be
                             // ignored by geant4
      std::string name =
          "PMT" + line.substr(0, pos); // Deleted the "PMT" out of the name
                                       // previously so add it again
      PMTnames.push_back(name);
    }
  }
  return PMTnames;
}

int main(int argc, char **argv) {

  // RMGLog::SetLogLevel(RMGLog::debug);

  std::string filename = "gdml/WLGDOptical.gdml";

  RMGManager manager("FullCosmogenics", argc, argv);
  // Overwrite the standard Hardware with one that reads
  // in the PMT QE from datasheet
  manager.SetUserInit(new HardwareQEOverride());
  manager.GetDetectorConstruction()->IncludeGDMLFile(filename);

  // Get the physical volume names of the PMTs to register them
  std::vector<std::string> PMTnames = getPMTNames(filename);
  int id = 0;
  // Register all of the PMTs
  for (const auto &name : PMTnames) {
    manager.GetDetectorConstruction()->RegisterDetector(RMGHardware::kOptical,
                                                        name, id);
    id++;
  }
  // Register the germanium volume as germanium detector.
  manager.GetDetectorConstruction()->RegisterDetector(RMGHardware::kGermanium,
                                                      "Ge_phys", id + 1000);

  auto user_init = manager.GetUserInit();
  user_init->AddOptionalOutputScheme<CustomIsotopeFilter>(
      "CustomIsotopeFilter");

  // Interactive or batch mode?
  std::string macro = argc > 1 ? argv[1] : "";
  if (!macro.empty())
    manager.IncludeMacroFile(macro);
  else
    manager.SetInteractive(true);

  // Outputfilename and Threads. Then run
  std::string outputfilename = "build/output.csv";
  manager.SetOutputFileName(outputfilename);
  manager.SetNumberOfThreads(16);
  manager.Initialize();
  manager.Run();

  return 0;
}
