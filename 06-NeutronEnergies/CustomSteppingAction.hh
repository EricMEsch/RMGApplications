

#ifndef _CUSTOM_STEPPING_ACTION_HH_
#define _CUSTOM_STEPPING_ACTION_HH_

#include <memory>

#include "G4UserSteppingAction.hh"

class G4Step;
class CustomSteppingAction : public G4UserSteppingAction {

  public:

    CustomSteppingAction();
    ~CustomSteppingAction() = default;

    void UserSteppingAction(const G4Step*) override;
};

#endif