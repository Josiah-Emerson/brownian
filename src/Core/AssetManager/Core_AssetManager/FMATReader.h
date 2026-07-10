#pragma once
#include <filesystem>

namespace Core{

   class FMATReader{
      public:
         // .fmat format (fig material)
         struct FMATObject{
            std::filesystem::path vsPath;
            std::filesystem::path fsPath;
         };

         FMATObject read(const std::filesystem::path& path);

      private: 
         enum KeywordToken{
            UNKNOWN,

            // Shader paths
            VERT,
            FRAG,
         };

         KeywordToken parseKeyword(std::string& keyword);
         void parseLine(std::stringstream& sstr, KeywordToken kt);

         void validityCheck();

         FMATObject m_fmat;
   };
} // namespace Core
