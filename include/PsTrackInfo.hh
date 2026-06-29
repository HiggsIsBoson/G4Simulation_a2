#ifndef PS_TRACK_INFO_HH
#define PS_TRACK_INFO_HH
#include <G4VUserTrackInformation.hh>

// Ps崩壊由来のトラックにタグ付けするための軽量クラス
// TrackingAction がPs崩壊ガンマをスタックに積む際にセット
class PsTrackInfo : public G4VUserTrackInformation {
 public:
  void Print() const override {}
};
#endif
