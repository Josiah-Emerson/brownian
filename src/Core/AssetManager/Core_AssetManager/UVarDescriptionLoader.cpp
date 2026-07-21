#include "UVarDescriptionLoader.h"
#include "Core_Utils/Log.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace Core{
   UVarDescriptionLoader::DescriptionSet UVarDescriptionLoader::parse(const std::filesystem::path& path){
      std::ifstream fstrm { path };
      if(!fstrm.is_open()){
         std::string msg  = "Could not open file ";
         msg += path;
         FIG_UNCREACHABLE(msg.c_str())
      }

      using json = nlohmann::json;
      json data = json::parse(fstrm); // TODO/NOTE: This throws if file does not exist or if parsing fails 
                                      // due to being invalid json object
      if(data["uniforms"].size() == 0){
         return  { };
      }

      DescriptionSet set { };
      set.reserve(data["uniforms"].size());
      for(auto& uniform : data["uniforms"]){
         auto insert = set.insert({uniform["name"].get<std::string>(), parseUniformEntry(uniform)});
         FIG_ASSERT(insert.second, "Duplicate uniform variable found"); // TODO: probably just continue and document we use first instance of variable?
      }

      return std::move(set);
   }

   UVarDescriptionLoader::UVarDescriptionObject UVarDescriptionLoader::parseUniformEntry(nlohmann::json& uniform){
      std::vector<std::string> labels {};
      std::vector<std::string> widgets {};

      if(uniform.contains("labels")){
         labels = std::move(uniform["labels"].get<std::vector<std::string>>());
      }
      if(uniform.contains("widgets")){
         widgets = std::move(uniform["widgets"].get<std::vector<std::string>>());
      }

      std::optional<ShaderData> defaults {};
      using json_type = nlohmann::json::value_t;
      if(uniform.contains("defaults")){
         // TODO: handle below two asserts
         FIG_ASSERT(uniform["defaults"].type() == json_type::array, "defaults is not an array")
         FIG_ASSERT(!uniform["defaults"].empty(), "No elements in defaults array")

         // uniform["defaults"] is a non-empty array
         // check all elements have the same type
         const auto& jArr = uniform["defaults"];
         json_type t = jArr[0].type();
         for(const auto& e : jArr){
            // TODO: handle
            FIG_ASSERT(t == e.type(), "Not all elements in defaults are the same type")
         }
         PrimitiveType pType;
         std::size_t pCnt = jArr.size();
         switch(t){
            case(json_type::boolean):
               {
                  pType = PrimitiveType::BOOL;
                  defaults = getDefaultsFromArray<bool>(ShaderTypeDescription{ pType, pCnt }, jArr);
                  break;
               }
            case(json_type::number_integer):
               {
                  pType = PrimitiveType::I32;
                  defaults = getDefaultsFromArray<int32_t>(ShaderTypeDescription{ pType, pCnt }, jArr);
                  break;
               }
            case(json_type::number_float):
               {
                  // TODO: Figure out how to handle doubles (i.e. we cannot parse them out here since its only number_float)
                  // I believe that nlohmann stores it as a double anyways, but if the defaults are some crazy precision 
                  // casting as float will lose that. Furthermore whatever takes this UVarDescriptionObject will need to know
                  // that if the variable reflected back is type double, then it will find a type mismatch between float and double
                  pType = PrimitiveType::FLOAT;
                  defaults = getDefaultsFromArray<float>(ShaderTypeDescription{ pType, pCnt }, jArr);
                  break;
               }
            default:
               pType = PrimitiveType::INVALID;
               break;
         }
         FIG_ASSERT(pType != PrimitiveType::INVALID, "The type stored in defaults was not a valid uniform variable type");
         // TODO: The below one is just here because I believe that we should have either gotten a 
         // value or asserted by now, however, in the future when we don't want to crash on failure 
         // I think defaults should still be left as not having a value since it is a std::optional. 
         // If anything just leave a message that hey we detected an attempt at supplying defaults but 
         // they were for some reason not provided in a valid way
         FIG_ASSERT(defaults.has_value(), "I think that it should have been given a value here")
      }

      return UVarDescriptionObject{
         .labels     = std::move(labels),
         .widgets    = std::move(widgets),
         .defaults   = std::move(defaults)
      };
   }

   template<>
   ShaderData UVarDescriptionLoader::getDefaultsFromArray<bool>(const ShaderTypeDescription& typeDesc, const nlohmann::json& jArray){
      FIG_ASSERT(jArray[0].type() == nlohmann::json::value_t::boolean, "jArray does not have element of type boolean")
      FIG_ASSERT(typeDesc.primType == PrimitiveType::BOOL, "typeDesc has a type different from BOOL")

      bool* ptr = new bool[typeDesc.primCnt];
      for(std::size_t i { 0 }; i < typeDesc.primCnt; ++i){
         ptr[i] = jArray[i].get<bool>();
      }
      ShaderData sData = ShaderVisitors::createShaderData(typeDesc, ptr);
      delete[] ptr;

      return std::move(sData);
   }
} // namespace Core
