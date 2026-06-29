#ifndef ACTION_INITIALIZATION_HH
#define ACTION_INITIALIZATION_HH
#include <G4VUserActionInitialization.hh>
class ActionInitialization : public G4VUserActionInitialization {
 public:
  explicit ActionInitialization(int mode = 2) : fMode(mode) {}
  void Build() const override;
  void BuildForMaster() const override;
  int GetMode() const { return fMode; }
 private:
  int fMode;
};
#endif
