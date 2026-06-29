#pragma once
#include "AssetManager.h"
#include "Core_Graphics/Buffer.h"
#include "Core_Utils/Log.h"

// TODO: Before implementing a whole loader for files, lets just test with the mesh data 

namespace Core{
   std::vector<float> tmpGetVertices();

   AssetManager::AssetManager(RenderDevice* renderDevice)
      : m_device { renderDevice }
   {
      FIG_ASSERT(m_device, "Render Device ptr null")
   }

   MeshHandle AssetManager::createMesh(const std::string& path){
      return createMesh(path.c_str());
   }

   MeshHandle AssetManager::createMesh(const char* path){
      std::vector<float> vertices = tmpGetVertices();

      BufferUsageFlags bFlags = BufferUsage::VERTEX_BUFFER;
      bFlags |= BufferUsage::TRANSFER_DST;

      BufferDesc vertexBufferDesc{
         .size = sizeof(float) * vertices.size(),
         .bUsage =  bFlags,
         .mUsage = MemoryUsage::GPU_ONLY
      };

      BufferHandle vBuffer = m_device->createBuffer(vertexBufferDesc, vertices.data());
      m_meshes.emplace_back(vBuffer, vertices.size());

      return MeshHandle { m_meshes.size() - 1 };
   }


   struct ShaderProgramHandle { };
   MaterialHandle AssetManager::createMaterial(ShaderProgramHandle shaderProgramHandle){
      FIG_UNCREACHABLE("Not implemented yet")
      return { };
   }

   // NOTE: This only takes the buffer handle so that we can glue to separate things together 
   // for a proof of concept
   MaterialHandle AssetManager::createMaterial(){
      static bool w { true };
      if(w){
         FIG_LOG_MEDIUM_WARNING("Use of a temporary func")
         w = false;
      }

      // IMPORTANT NOTE: Obviously we will need to separate shader creation from 
      // material creation
      ShaderPipelineDesc desc {
         .vertex = "Resources/Shaders/Vertex.vs",
         .fragment = "Resources/Shaders/Fragment.fs",
         .vLayout = VertexData::getVertexLayout(),
         .uLayout = { }, // TODO: use this for custom uniform values?
      };

      ShaderPipelineHandle h = m_device->createShaderPipeline(desc);
      m_materials.emplace_back(h);

      return MaterialHandle {m_materials.size() - 1};
   }

   const Mesh& AssetManager::getMesh(MeshHandle handle) const{
      FIG_ASSERT(handle.idx < m_meshes.size() , "handle references a mesh with an index outside the bounds of m_meshes")

      // TODO: either implement the generation counter like we talked about, and a dead/live marker 
      // or a map for mapping handles to indices. As an exercies, calculate which one would likely be more 
      // space and time efficient
      return m_meshes[handle.idx];
   }

   const Material& AssetManager::getMaterial(MaterialHandle handle) const {
      FIG_ASSERT(handle.idx < m_meshes.size() , "handle references a material with an index outside the bounds of m_materials")

      // TODO: either implement the generation counter like we talked about, and a dead/live marker 
      // or a map for mapping handles to indices. As an exercies, calculate which one would likely be more 
      // space and time efficient
      return m_materials[handle.idx];
   }

   std::vector<float> tmpGetVertices(){
      FIG_LOG_LOW_WARNING("use of a temporary function")

      return {
          -1.0f,-1.0f,-1.0f, // triangle 1 : begin
          -1.0f,-1.0f, 1.0f,
          -1.0f, 1.0f, 1.0f, // triangle 1 : end
          1.0f, 1.0f,-1.0f, // triangle 2 : begin
          -1.0f,-1.0f,-1.0f,
          -1.0f, 1.0f,-1.0f, // triangle 2 : end
          1.0f,-1.0f, 1.0f,
          -1.0f,-1.0f,-1.0f,
          1.0f,-1.0f,-1.0f,
          1.0f, 1.0f,-1.0f,
          1.0f,-1.0f,-1.0f,
          -1.0f,-1.0f,-1.0f,
          -1.0f,-1.0f,-1.0f,
          -1.0f, 1.0f, 1.0f,
          -1.0f, 1.0f,-1.0f,
          1.0f,-1.0f, 1.0f,
          -1.0f,-1.0f, 1.0f,
          -1.0f,-1.0f,-1.0f,
          -1.0f, 1.0f, 1.0f,
          -1.0f,-1.0f, 1.0f,
          1.0f,-1.0f, 1.0f,
          1.0f, 1.0f, 1.0f,
          1.0f,-1.0f,-1.0f,
          1.0f, 1.0f,-1.0f,
          1.0f,-1.0f,-1.0f,
          1.0f, 1.0f, 1.0f,
          1.0f,-1.0f, 1.0f,
          1.0f, 1.0f, 1.0f,
          1.0f, 1.0f,-1.0f,
          -1.0f, 1.0f,-1.0f,
          1.0f, 1.0f, 1.0f,
          -1.0f, 1.0f,-1.0f,
          -1.0f, 1.0f, 1.0f,
          1.0f, 1.0f, 1.0f,
          -1.0f, 1.0f, 1.0f,
          1.0f,-1.0f, 1.0f
      };
   }

} // namespace Core
