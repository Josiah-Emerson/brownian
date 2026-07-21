#pragma once
#include "Gui.h"
#include "Core_Graphics/ShaderData.h"
#include <cstddef>
#include <string>

namespace Core{
   // interface class for the GUI Widget for controlling a uniform var
   class IUniformWidget{
      public:
         virtual ~IUniformWidget() = default;

         // Draws the gui widget 
         // widget: the gui widget to be drawn under 
         // label: A string label for what this widget represents
         // base: a pointer to the first byte of contiguous memory that the widget should operate on
         //       NOTE: base + (primitiveCount() + primitiveSize()) must be valid data
         // id: For any widgets in the same layer that will have the same label, pass a unique ID 
         //       EXAMPLE: Looping through widgets, and they all (or some) might have the same label, 
         //       pass the loop counter as id. This will not affect how it looks
         virtual bool draw(Core::Gui::Widgets& widget, 
                           const std::string& label, 
                           std::byte* base, 
                           int id = -1) const = 0; 

         // number of base primitives this widget covers
         virtual int primitiveCount() const = 0; 

         // the actual size of the primitive (PrimitiveType::FLOAT = sizeof(float))
         virtual std::size_t primitiveSize() const = 0; 

         virtual PrimitiveType primitiveType() const = 0;
   };

   // Type convenience layer for widgets to derive from 
   // The benefit for this is that we don't have to write the reinterpret cast stuff twice ?
   template<ShaderVisitors::IsPrimitiveType T, PrimitiveType PT>
   class TypedUniformWidget : public IUniformWidget{
      public: 
         bool draw(Core::Gui::Widgets& widget, const std::string& label, std::byte* base, int id = -1) const override{
            return drawTyped(widget, label, reinterpret_cast<T*>(base), id);
         }

         std::size_t primitiveSize() const final { return sizeof(T); }
         PrimitiveType primitiveType() const final { return PT; }
      protected:
         virtual bool drawTyped(Core::Gui::Widgets& widget, const std::string& label, T* base, int id) const = 0;
   };

   class RGBSelectorWidget final : public TypedUniformWidget<float, PrimitiveType::FLOAT>{
      public: 
         int primitiveCount() const override { return 3; }

      protected: 
         bool drawTyped(Core::Gui::Widgets& widget, const std::string& label, float* base, int id) const override;
   };

   template<ShaderVisitors::IsPrimitiveType T, PrimitiveType PT>
   class SliderWidget : public TypedUniformWidget<T, PT>{
      public: 
         static_assert(!std::same_as<T, bool>, "Slider widget not available for type bool");
         int primitiveCount() const override { return 1; }

      protected: 
         bool drawTyped(Core::Gui::Widgets& widget, const std::string& label, T* base, int id) const override{
            return widget.slider(label.c_str(), *base, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), id);
         }
   };

   class CheckboxWidget : public TypedUniformWidget<bool, PrimitiveType::BOOL>{
      public: 
         int primitiveCount() const override { return 1; }

      protected:
         bool drawTyped(Core::Gui::Widgets& widget, const std::string& label, bool* base, int id) const override;
   };
} // namespace Core
