#include "RMGHardware.hh"
#include "RMGLog.hh"
#include "RMGManager.hh"

#include "HPGeTestStand.hh"

int main(int argc, char** argv) {

  // RMGLog::SetLogLevel(RMGLog::debug);

  RMGManager manager("01-gdml", argc, argv);
  manager.GetDetectorConstruction()->IncludeGDMLFile("gdml/l200.gdml");
  //manager.SetUserInit(new HPGeTestStand());

  std::string macro;
  if (argc == 2) macro = argv[1];
  else if (argc == 3) macro = argv[2];
  else RMGLog::Out(RMGLog::fatal, "Bad command line options");

  if (argc > 1 and std::string(argv[1]) == "-i") { manager.SetInteractive(); }

  if (!macro.empty()) manager.IncludeMacroFile(macro);

  manager.Initialize();
  manager.Run();

  return 0;
}
