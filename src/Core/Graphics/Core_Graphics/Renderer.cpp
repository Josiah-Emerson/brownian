#pragma once
#include "Renderer.h"
#include "Core_ECS/Components.h"
#include "RenderPass.h"
#include "Core_Graphics/CommandList.h"
#include "Core_Graphics/RenderDevice.h"
#include "Core_Graphics/ShaderPipeline.h"
#include "Core_Graphics/Mesh.h"
#include "Core_Graphics/Material.h"
#include "Core_Utils/Linear/Matrix.h"
#include "Core_Utils/Linear/MatrixTransform.h"
#include "Core_Utils/Log.h"

namespace Core{

   bool tempComperand::operator==(const tempComperand& other) const{
      static bool w { true };
      if(w){
         FIG_LOG_HIGH_WARNING("Use of temp function");
         w = false;
      }
      return true;
   }

   bool tempCompare::operator()(const tempComperand& lhs, const tempComperand& rhs) const {
      static bool w { true };
      if(w){
         FIG_LOG_HIGH_WARNING("Use of temp function");
         w = false;
      }
      return lhs == rhs;
   }

   Renderer::Renderer(RenderDevice* device, AssetManager& assetManager)
      : m_device(device)
      , m_assetManager { assetManager }
   { 
      FIG_ASSERT(m_device, "nullptr to render device");
   }

   void Renderer::renderEntities(IRendererSortedRegistryView* view, Camera& camera){
      auto& positions = view->positionPool();
      auto& meshes = view->meshPool();
      auto& materials = view->materialPool();
      auto& colors = view->colorPool();
      std::size_t entityCount = positions.size();

      Linear::fmat4 M {};
      Linear::fmat4 V = camera.viewMatrix();
      Linear::fmat4 P = camera.projectionMatrix();
      Linear::fmat4 MVP {};
      Linear::fmat4 MVP_T {};

      RenderPassDesc pass { };

      CommandList* cmd = m_device->beginCommandList();
      cmd->beginRenderPass(pass);

      // TODO: Assert/check on sizes
      for(std::size_t i { 0 }; i < entityCount; ++i){
         PositionComponent position = positions[i];
         MeshHandle h_mesh = meshes[i];
         MaterialHandle h_material = materials[i];

         M = Linear::modelMatrix(position.val, Linear::fvec3{1, 1, 1}, Linear::fvec3{1, 1, 1});
         MVP = P * V * M;
         MVP_T = MVP.transpose();
         static bool w { true };
         if(w){
            // TODO: Remove the color component on the 3 (?) view things in Renderer.h, 
            // get color out of the uniform camera data, 
            // and remove Color3Component from registry
            FIG_LOG_HIGH_WARNING("The view is updated to have a color pool component, when this is only something we are using temporarily")
            w = false;
         }
         const Color3Component& col = colors[i];
         Linear::fvec3 color = {
            col.val.R / 255.f,
            col.val.G / 255.f,
            col.val.B / 255.f,
         };

         UniformCameraData data { 
            .MVP = MVP_T,
            .color = color,
         };

         const Material& material = m_assetManager.getMaterial(h_material);
         const Mesh& mesh = m_assetManager.getMesh(h_mesh);

         cmd->bindShaderPipeline(material.shaderPipelineHandle);
         cmd->bindVertexBuffer(mesh.vBuf);
         cmd->bindIndexBuffer(mesh.eBuf);
         cmd->setUniformBufferData(StandardUniformBlock::CAMERA_DATA, &data);

         cmd->drawElement(mesh.elementCnt);
      }

      cmd->endRenderPass();
      m_device->submitCommandList(cmd);
   }

} // namespace Core
