#pragma once
#include "FMATReader.h"
#include "Core_Utils/Log.h"
#include <fstream>

namespace Core{
   FMATReader::FMATObject FMATReader::read(const std::filesystem::path& path){
      // .fmat stands for fig material
      FIG_ASSERT(path.extension() == ".fmat", "Trying to load a material using FMATReader, but the file extension is not '.fmat'")

      std::ifstream fstrm {path};
      if(!fstrm.is_open()){
         std::string str {"Error opening file: "};
         str += path;
         FIG_UNCREACHABLE(str.c_str());
      }

      std::string line;
      while(std::getline(fstrm, line)){
         std::stringstream sstr {line};
         std::string keyword;
         sstr >> keyword;

         KeywordToken kt = parseKeyword(keyword);

         sstr >> std::ws;
         parseLine(sstr, kt);
      }

      validityCheck();

      return m_fmat;
   }

   FMATReader::KeywordToken FMATReader::parseKeyword(std::string& keyword){
      if(keyword == "Frag"){
         return FRAG;
      }else if(keyword == "Vert"){
         return VERT;
      }

      return UNKNOWN;
   }

   void FMATReader::parseLine(std::stringstream& sstr, KeywordToken kt){
      FIG_ASSERT(kt != UNKNOWN, "Unknown keyword encountered");

      // NOTE: For now the fmat is literally 2 paths, so we can parse them the exact same
      std::filesystem::path path;
      sstr >> path;

      if(kt == FRAG){
         FIG_ASSERT(m_fmat.fsPath.empty(), "A fragment shader path has already been parsed from this .fmat file")
         m_fmat.fsPath = path;
      }else if(kt == VERT){
         FIG_ASSERT(m_fmat.vsPath.empty(), "A vertex shader path has already been parsed from this .fmat file")
         m_fmat.vsPath = path;
      }else{
         FIG_UNCREACHABLE("Unknown token encountered while parsing .fmat file")
      }
   }

   void FMATReader::validityCheck(){
      FIG_ASSERT(!m_fmat.vsPath.empty(), "No vertex shader path has been deduced from this .fmat file");
      FIG_ASSERT(!m_fmat.fsPath.empty(), "No fragment shader path has been deduced from this .fmat file");
   }

} // namespace Core
