#ifndef _COSMOGENIC_OUTPUT_SCHEME_HH_
#define _COSMOGENIC_OUTPUT_SCHEME_HH_

#include <optional>
#include <set>

#include "G4AnalysisManager.hh"
#include "G4GenericMessenger.hh"
#include "G4ThreeVector.hh"

#include "RMGGermaniumDetector.hh"
#include "RMGOpticalDetector.hh"
#include "RMGVOutputScheme.hh"

class G4Event;
class CosmogenicOutputScheme : public RMGVOutputScheme {

  public:

    CosmogenicOutputScheme();

    void ClearBeforeEvent() override;
    void AssignOutputNames(G4AnalysisManager* ana_man) override;
    void StoreEvent(const G4Event*) override;
    bool ShouldDiscardEvent(const G4Event*) override;
    void TrackingActionPre(const G4Track* aTrack) override;

  protected:

    [[nodiscard]] inline std::string GetNtuplenameFlat() const override { return "cosmogenic"; }

  private:

    RMGGermaniumDetectorHitsCollection* GetGeHitColl(const G4Event*);
    RMGOpticalDetectorHitsCollection* GetOptHitColl(const G4Event*);
    int GetGeFlag(const RMGGermaniumDetectorHitsCollection*);
    int GetWaterFlag(const RMGOpticalDetectorHitsCollection*);

    std::unique_ptr<G4GenericMessenger> fMessenger;
    void DefineCommands();

    double fEdepCutLow = -1;
    double fEdepCutHigh = -1;
    std::set<int> fEdepCutDetectors;

    int OutputRegisterID = -101; // Just set an ID that has not yet been used

    std::vector<G4ThreeVector> Capture_Positions;

    double gerad          = 4.0;                    // in cm
    double gehheight      = 5.0;                    // full height 10 cm
    double gegap          = 3.0;                    // gap between Ge 3cm
    double dist_to_next_string = 16.75; // in cm
};

#endif

// vim: tabstop=2 shiftwidth=2 expandtab
