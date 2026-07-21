#pragma once
#include <memory>
#include <unordered_map>

namespace Core{
   class IUniformWidget;

   class UniformVariableWidgetRegistry{
      public:
         static UniformVariableWidgetRegistry& instance();

         void registerWidget(const std::string& name, std::shared_ptr<const IUniformWidget> widget);
         std::shared_ptr<const IUniformWidget> get(const std::string& name) const;

      private: 
         UniformVariableWidgetRegistry();
         std::unordered_map<std::string, std::shared_ptr<const IUniformWidget>> m_widgets;
   };
} // namespace Core
