#pragma once
#include <cstddef>
#include <fstream>
#include <set>
#include <vector>

// General TODO: Add better error/warning reporting. Example: what line of the file is this error encountered? 
// What was the string, etc...
// Also (especially when returning), what is the best way to return possibly large objects like Face or OBJObject
// Also maximum number of vertices and things like that which we probably won't need

namespace Core{
   class OBJParser{
      public:
         struct Vertex{
            float x;
            float y;
            float z;
            float w;
         };

         struct Face{
            std::vector<std::size_t> vertices;
         };

         struct OBJObject{
            std::vector<Vertex> vertices;
            std::vector<Face> faces;
         };

         // TODO: options?
         OBJParser() = default;

         OBJObject parse(std::ifstream& strm);
         
      private:
         enum KeywordToken{
            UNKNOWN,
            UNSUPPORTED,

            COMMENT,

            // Vertex data
            V,

            // Elements
            F,
         };

         KeywordToken parseKeyword(const std::string& keyword);

         // NOTE: assumes there is no leading white space 
         void parseLine(std::stringstream& sstrm, KeywordToken kt);

         // Vertex data parsing
         void vPos(std::stringstream& sstrm);

         // Element parsing
         void face(std::stringstream& sstrm);

         // final check for the validity of the object
         void validityCheck(); // TODO: make it more robust
                               // For example we check if the largest referenced vertex in a face 
                               // is greater than the amount of vertices. But if a face refernces say vertex 9
                               // when only verteices 1-7 have been defined, and then vertex 8 and 9 are defined,
                               // is that okay or not? The validity check would let it pass

         // For now a temp function for tracking/catching unsupported things and outputting all at once
         void outputWarnings(); 

         static const std::set<std::string> m_unsupportedKeywords;


         OBJObject m_obj;
         std::set<std::string> m_encounteredUnsupportedKeywords;
         std::size_t m_largestVertexRef { 0 };
   };
} // namespace Core
