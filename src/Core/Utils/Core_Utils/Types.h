#pragma once
#include <cstdint>

namespace Core{
   // NOTE: Code using EntityID assumes ID type is unsigned and overflow is defined as wrap around
   using EntityID = unsigned long; // NOTE: Though machine dependent, unsigned long is at least 32 bits (64 on my machine though) which means 4,294,967,295 possible entities. Should be enough

   // NOTE: If the underlying types for this changes, need to update the following code: 
   //    - The 3 static casts in Gui::Widgets::rgbSelector
   struct Color3{
      std::uint8_t R;
      std::uint8_t G;
      std::uint8_t B;
   };

   // TODO: Create Color4 type, and allow for casting from color3 to color4
} // namespace Core
