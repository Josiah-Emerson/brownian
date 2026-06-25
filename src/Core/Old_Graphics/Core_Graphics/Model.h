#pragma once
#include <vector>

namespace Core{
   class Model{
      public: 
         Model(const std::vector<float>& vertexData, const std::vector<float>& colorData) 
            : m_vertexData { vertexData }
            , m_colorData { colorData }
         { calcTriangles(); }
         Model(std::vector<float>&& vertexData, std::vector<float>&& colorData)
            : m_vertexData { std::move(vertexData) }
            , m_colorData { std::move(colorData) }
         { calcTriangles(); }

         const std::vector<float>& getVertexData() const { return m_vertexData; }
         const std::vector<float>& getColorData() const { return m_colorData; }

         std::vector<float>& getVertexData() { return m_vertexData; }
         std::vector<float>& getColorData() { return m_colorData; }

         int triangles() { return m_triangles; }

         bool operator==(const Model& other) const;
      private: 
         std::vector<float> m_vertexData;
         std::vector<float> m_colorData;
         int m_triangles; // num triangles

      private: 
         void calcTriangles();
   };
} // namespace Core
