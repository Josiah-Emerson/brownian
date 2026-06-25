#include "Model.h"
#include "Core_Utils/Log.h"

namespace Core{
   bool Model::operator==(const Model& other) const{
      return (m_vertexData == other.m_vertexData) && (m_colorData == other.m_colorData);
   }

   void Model::calcTriangles(){
      // TODO: For now no EBO (?) stuff. Maybe we won't need it since we probably wont have high res models for a while 
      FIG_ASSERT((m_vertexData.size() % 3 == 0) && !m_vertexData.empty() , "Number of vertices is either 0 or not a multiple of three")
      m_triangles = m_vertexData.size() / 3;
   }
} // namespace Core

