#pragma once
#include "Core_UI/UniformVariableWidgetRegistry.h"
#include "Core_UI/UniformVariableWidgets.h"
#include "Core_Utils/Log.h"

namespace Core{
   UniformVariableWidgetRegistry& UniformVariableWidgetRegistry::instance(){
      static UniformVariableWidgetRegistry registry;
      return registry;
   }

   void UniformVariableWidgetRegistry::registerWidget(const std::string& name, std::shared_ptr<const IUniformWidget> widget){
      FIG_ASSERT(m_widgets.find(name) == m_widgets.end(), "This widget has already been registered") // perhaps we can check if the widgets are the same
         
      m_widgets.emplace(name, std::move(widget));
   }

   std::shared_ptr<const IUniformWidget> UniformVariableWidgetRegistry::get(const std::string& name) const {
      auto it = m_widgets.find(name);
      return it == m_widgets.end() ? nullptr : it->second;
   }

   UniformVariableWidgetRegistry::UniformVariableWidgetRegistry(){
      // TODO: Move the initial registration code outta here probably
      // TODO: Are we fine with RGB only being float or do we want double as well?

      std::string checkbox = "Checkbox";
      registerWidget(checkbox, std::make_shared<CheckboxWidget>());

      std::string rgbSelector = "RGBSelector";
      registerWidget(rgbSelector, std::make_shared<RGBSelectorWidget>());

      // sliders
      std::string i32Slider = "I32Slider";
      registerWidget(i32Slider, std::make_shared<SliderWidget<int32_t, PrimitiveType::I32>>());

      std::string floatSlider = "FSlider";
      registerWidget(floatSlider, std::make_shared<SliderWidget<float, PrimitiveType::FLOAT>>());

      std::string doubleSlider = "DSlider";
      registerWidget(doubleSlider, std::make_shared<SliderWidget<double, PrimitiveType::DOUBLE>>());

   }
} // namespace Core
