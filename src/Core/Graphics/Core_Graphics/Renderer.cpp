#pragma once
#include "RenderPass.h"
#include "Renderer.h"
#include "Core_Graphics/CommandList.h"
#include "Core_Utils/Linear/Matrix.h"
#include "Core_Utils/Linear/MatrixTransform.h"
#include "Core_Utils/Log.h"

namespace Core{

   Renderer::Renderer(RenderDevice* device, AssetManager& assetManager)
      : m_device(device)
      , m_assetManager { assetManager }
   { 
      FIG_ASSERT(m_device, "nullptr to render device")

      BufferDesc cameraDataDesc {
         .size = sizeof(UniformCameraData),
         .bUsage = BufferUsage::UNIFORM_BUFFER,
         .mUsage = MemoryUsage::CPU_TO_GPU
      };

      m_uniformBuffers.cameraData = m_device->createBuffer(cameraDataDesc);
   }

   void Renderer::RenderEntities(IRendererSortedRegistryView* view, Camera& camera){
      auto& positions = view->positionPool();
      auto& meshes = view->meshPool();
      auto& materials = view->materialPool();
      std::size_t entityCount = positions.size();

      Linear::fmat4 M { 0 };
      Linear::fmat4 V = camera.viewMatrix();
      Linear::fmat4 P = camera.projectionMatrix();
      Linear::fmat4 MVP { 0 };

      RenderPassDesc pass { };

      CommandList* cmd = m_device->beginCommandList();
      cmd->beginRenderPass(pass);

      // TODO: Assert/check on sizes
      for(std::size_t i { 0 }; i < entityCount; ++i){
         PositionComponent position = positions[i];
         MeshHandle h_mesh = meshes[i];
         MaterialHandle h_material = materials[i];

         M = Linear::modelMatrix(position.val, Linear::fvec3{0, 0, 0}, Linear::fvec3{1, 1, 1});
         MVP = P * V * M;
         UniformCameraData data { 
            .MVP = MVP,
            .color = Linear::fvec3 {1.f, 1.f, 1.f}
         };

         const Material& material = m_assetManager.getMaterial(h_material);
         const Mesh& mesh = m_assetManager.getMesh(h_mesh);

         cmd->bindVertexBuffer(mesh.vertexBuffer());
         cmd->bindShaderPipeline(material.shaderPipelineHandle);
         cmd->setUniformBuffer(m_uniformBuffers.cameraData, &data);

         cmd->drawElement();
      }

      cmd->endRenderPass();
      m_device->submitCommandList(cmd);
   }
} // namespace Core
