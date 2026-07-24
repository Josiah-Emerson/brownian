#pragma once
#include "OBJParser.h"
#include "Core_Utils/Log.h"
#include <algorithm>
#include <sstream>
#include <string>

namespace Core{
   const std::set<std::string> OBJParser::m_unsupportedKeywords = {
      "vt", "vn", "vp", "cstype", "det", "bmat", "step",                                                       // vertex data
      "p", "l", "curv", "curv2", "surf",                                                                       // elements
      "parm", "trim", "hole", "scrv", "sp", "end",                                                             // free-form statements
      "con",                                                                                                   // connectivity
      "g", "s", "mg", "o",                                                                                     // grouping
      "bevel", "c_interp", "d_interp", "lod", "usemtl", "mtllib", "shadow_obj", "trace_obj", "ctech", "stech", // display/render
   };

   OBJParser::OBJObject OBJParser::parse(std::ifstream& strm){
      if(!strm.is_open()){
         FIG_UNREACHABLE("stream not open")
      }

      std::string line;
      while(std::getline(strm, line)){
         std::stringstream sstr {line};
         std::string keyword;
         sstr >> keyword;
         if(keyword == "") continue;

         KeywordToken kt = parseKeyword(keyword);

         sstr >> std::ws;
         parseLine(sstr, kt);
      }

      outputWarnings();

      validityCheck();
      return m_obj; // TODO: Make this efficient so we don't copy return a huge object
   }

   OBJParser::KeywordToken OBJParser::parseKeyword(const std::string& keyword){
      if(m_unsupportedKeywords.contains(keyword)){
         m_encounteredUnsupportedKeywords.insert(keyword);
         return UNSUPPORTED;
      }else if(keyword == "#"){
         return COMMENT;
      }else if(keyword == "v"){
         return V;
      }else if(keyword == "f"){
         return F;
      }

      std::string str = "Unknown keyword token encountered: ";
      str += keyword;
      FIG_UNREACHABLE(str.c_str());

      return UNKNOWN;
   }

   void OBJParser::parseLine(std::stringstream& sstrm, KeywordToken kt){
      switch(kt){
         // UNKNOWN / UNSUPPORTED
         case(UNKNOWN):
         case(UNSUPPORTED):
            // FIG_UNREACHABLE("Have not decided how to handle this case yet")
            break;

         case(COMMENT):
            break;

         // vertex data
         case(V):
            vPos(sstrm);
            break;

         // elements
         case(F):
            face(sstrm);
            break;
      }
   }

   void OBJParser::vPos(std::stringstream& sstrm){
      // TODO: Handle exception thrown by std::stof
      std::string token;
      float posData[4];
      posData[3] = 1.f; // if not specified, w is 1.0
      std::size_t idx = 0;

      while(sstrm >> token){
         if(idx > 3){
            FIG_LOG_MEDIUM_WARNING("More than 4 vertex positions (x, y, z, w) encountered");
            break;
         }

         posData[idx++] = std::stof(token);
      }

      m_obj.vertices.emplace_back
         (Vertex{posData[0], posData[1], posData[2], posData[3]});
   }

   void OBJParser::face(std::stringstream& sstrm){
      std::string token;
      Face face {};

      while(sstrm >> token){
         std::size_t slashPos = token.find('/');
         int vertex;
         if(slashPos == std::string::npos){
            vertex = std::stoi(token);
         }else{
            vertex = std::stoi(token.substr(0, slashPos));
         }

         if(vertex <= 0){
            FIG_UNREACHABLE("Vertex reference is either negative (not supported yet) or 0 which makes no sense");
         }
         std::size_t vIdx = vertex;
         m_largestVertexRef = std::max(m_largestVertexRef, vIdx);
         face.vertices.push_back(vIdx);
      }

      FIG_ASSERT(face.vertices.size() >= 3, "Face does not contain the minimum number of vertices (3)");

      m_obj.faces.push_back(face);
   }

   void OBJParser::validityCheck(){
      if(m_largestVertexRef > m_obj.vertices.size()){
         std::string str {"The .obj has a face which references a vertex index which is larger than the number of vertices. Number of vertices: "};
         str += m_obj.vertices.size();
         str += ". The largest vertex referenced was: ";
         str += m_largestVertexRef;
         FIG_UNREACHABLE(str.c_str());
      }
   }

   void OBJParser::outputWarnings(){
      if(m_encounteredUnsupportedKeywords.empty())
         return;

      std::string str { "The following keywords were encountered but are not supported: " };
      for(const std::string& word : m_encounteredUnsupportedKeywords){
         str += word;
         str += ", ";
      }
      FIG_LOG_LOW_WARNING(str.c_str());
   }
} // namespace Core
