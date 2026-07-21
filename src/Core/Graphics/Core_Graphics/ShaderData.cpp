#include "ShaderData.h"

namespace Core{
   namespace ShaderVisitors{
      ShaderData createShaderData(const Core::ShaderTypeDescription& typeDesc, const void* data){
         switch(typeDesc.primType){
            case(Core::PrimitiveType::BOOL):
               return createShaderDataTyped<bool>(typeDesc.primCnt, data);
            case(Core::PrimitiveType::I32):
               return createShaderDataTyped<int32_t>(typeDesc.primCnt, data);
            case(Core::PrimitiveType::FLOAT):
               return createShaderDataTyped<float>(typeDesc.primCnt, data);
            case(Core::PrimitiveType::DOUBLE):
               return createShaderDataTyped<double>(typeDesc.primCnt, data);
            default:
               break;
         }

         FIG_UNCREACHABLE("Type was either unhandled or we don't handle all PrimitiveTypes")
         return {};
      }

      template<>
      ShaderData createShaderDataTyped<bool>(std::size_t cnt, const void* data){
         FIG_ASSERT(cnt == 1, "Only have support for bool of cnt 1");
         return ShaderData { *static_cast<const bool*>(data) };
      }
   } // namespace ShaderVisitors
} // namespace Core
