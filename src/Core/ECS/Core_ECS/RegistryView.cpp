#include "RegistryView.h"

namespace Core{
   void func(){
      using FullPack = ComponentPack<PositionComponent, DirectionComponent, ScaleComponent>;
      using ValidSubPack = ComponentPack<PositionComponent, DirectionComponent>;
      using InvalidSubPack = ComponentPack<MeshComponent>;

      struct U { bool operator==(const U& other) const; };
      struct C { bool operator()(const U& lhs, const U& rhs) const; };
      SortedRegistry<U, C, FullPack> reg;

      RegistryView<ValidSubPack> view {reg};
   }
} // namespace Core
