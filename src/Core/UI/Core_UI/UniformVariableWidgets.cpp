#include "UniformVariableWidgets.h"

namespace Core{

   bool RGBSelectorWidget::drawTyped(Core::Gui::Widgets& w, const std::string& label, float* base, int id) const{
      Linear::fvec3 data { base[0], base[1], base[2] };
      if(w.rgbSelector(label.c_str(), data, id)){
         std::memcpy(base, &data, sizeof(float) * 3);
         return true;
      }

      return false;
   }

   bool CheckboxWidget::drawTyped(Core::Gui::Widgets& w, const std::string& label, bool* base, int id) const{
      return w.checkbox(label.c_str(), *base, id);
   }
} // namespace Core
