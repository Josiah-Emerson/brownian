#include "GLRenderDevice.h"
#include "Core_Graphics/GraphicsComponents.h"
#include "Core_Graphics/RenderDevice.h"
#include "Core_Graphics/ShaderProgram.h"
#include "GLShader.h"
#include "GLShaderProgram.h"
#include <memory>
#include <tuple>

namespace Core{
   bool GLRenderDevice::registerModel(Model& model){
      // TODO: identify fail points

      GLuint VAO;
      m_openGL.glGenVertexArrays(1, &VAO);
      m_openGL.glBindVertexArray(VAO);

      GLuint VBOs[2];
      m_openGL.glGenBuffers(2, VBOs);

      // vertex position data
      m_openGL.glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
      m_openGL.glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * model.getVertexData().size()),
                           model.getVertexData().data(), GL_STATIC_DRAW);
      m_openGL.glEnableVertexAttribArray(0);
      m_openGL.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      // color data
      m_openGL.glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
      m_openGL.glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * model.getColorData().size()),
                           model.getColorData().data(), GL_STATIC_DRAW);
      m_openGL.glEnableVertexAttribArray(1);
      m_openGL.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      m_openGL.glBindVertexArray(0);
      m_modelVAOList.emplace_back(model, VAO);

      return true;
   }

   bool GLRenderDevice::registerEntity(EntityID id, Model& model, std::shared_ptr<ShaderProgram> shaderProgram){
      return false;
   }

   std::shared_ptr<Shader> GLRenderDevice::createShader(const char* filepath, ShaderType type) {
      return std::make_shared<GLShader>(filepath, type, m_openGL);
   }

   std::shared_ptr<ShaderProgram> GLRenderDevice::createShaderProgram() {
      return std::make_shared<GLShaderProgram>(m_openGL);
   }

   bool GLRenderDevice::isValidGraphicsComperand(const GraphicsComperand& cmp) const{
      if(!cmp.prog->isValid())
         return false;

      auto search = std::find_if(m_modelVAOList.begin(), m_modelVAOList.end(), 
            [&cmp](const std::pair<const Model&, GLuint>& v){
               return v.first == *cmp.model;
            });

      return !(search == m_modelVAOList.end());
   }

   void GLRenderDevice::drawRegisteredEntities() {
      std::vector<GraphicsComperand> gComps = m_registry.getAllComperands();
      std::shared_ptr<ShaderProgram> currBoundProgram { nullptr };
      std::shared_ptr<Model> currBoundModel { nullptr };
      for(const GraphicsComperand& u : gComps){
         if(u.prog != currBoundProgram){
            FIG_ASSERT(u.prog->useProgram() , "Error using program")
            currBoundProgram = u.prog;
         }

         if(u.model != currBoundModel){
            auto search = std::find_if(m_modelVAOList.begin(), m_modelVAOList.end(), 
                  [&u](const std::pair<const Model&, GLuint>& v){
                     return v.first == *(u.model);
                  });
            FIG_ASSERT(search != m_modelVAOList.end() , "Model in pool not found in RenderDevice list")
            m_openGL.glBindVertexArray(search->second);
            currBoundModel = u.model;
         }

         using ArgHelperType = typename CreateComponentFunction<GraphicsComponentList>::ArgType;
         ArgHelperType components {};
         
         // Gross for now but oh well
         // Loops through all component types and sets the pointer to either the first valid 
         // value or a nullptr if that pool does not contain the component
         // range becomes the number of entities for this comperand to draw
         std::size_t range { 0 };
         std::apply([&u, &range, this](auto&... args){
                  std::size_t ranges[sizeof...(args)];
                  std::size_t idx { 0 };
                  ((ranges[idx++] = getPtrAndRangeFromComponentTypeAndComperand(
                                 u, &args)), ...);
                  // ensure num of elements is correct 
                  for(std::size_t i { 0 }; i < sizeof...(args); ++i){
                     if(ranges[i] != 0){ // this component contains entries
                        if(range == 0) // first encounter of a component pool containing entries
                           range = ranges[i];

                        FIG_ASSERT(range == ranges[i] , "Component pool has a different number of entries than the rest")
                     }
                  }
               }, components);

         // loop through and draw all entities with this comperand
         for(std::size_t i { 0 }; i < range; ++i){
            const auto& uniformVars = currBoundProgram->getUniformVariables();
            for(const auto& uVar : uniformVars){
               void* data = getDataPtr(uVar.type);
               void* oldData = data; // if callback re-points elsewhere we need to 
                                     // know what the old ptr was to not leak memory
                                     // if callback copies into this location, then we are fine 
                                     // to delete it anyways as setUniform will send the data where we need
                                     // NOTE: if the pointer gets re-pointed, it should be to 
                                     // data which is managed on a larger scope than the callback, 
                                     // and will thus persist long enough to call setUniform 
                                     // basically uVar.callback() should not point it towards some memory which 
                                     // the callback itself then deletes
               uVar.callback(&data, components, i);
               currBoundProgram->setUniform(uVar.name, data);
               deleteDataPtr(oldData, uVar.type); // delete the allocated bit from getDataPtr
            }

            // TODO: Make this depend on the model
            m_openGL.glDrawArrays(GL_TRIANGLES, 0, currBoundModel->triangles() * 3);
         }
      }
   }

   void* GLRenderDevice::getDataPtr(ShaderDataType type){
      void* data = nullptr;
      switch(type){
         case(INT):
            {
               data = new int;
               break;
            }
         case(FLOAT): 
            {
               data = new float;
               break;
            }
         case(DOUBLE): 
            {
               data = new double;
               break;
            }
         case(F_VEC3): 
            {
               data = new float[3];
               break;
            }
         case(F_MAT4): 
            {
               data = new float[16];
               break;
            }
         case(UNKNOWN):
         default: 
            FIG_ASSERT(false , "Unknown ShaderDataType encounted in getDataPtr func")
      }

      return data;
   }

   void GLRenderDevice::deleteDataPtr(void* ptr, ShaderDataType type){
      FIG_ASSERT(ptr , "nullptr passed to GLRenderDevice::deleteDataPtr")
      switch(type){
         case(INT):
            {
               int* data = static_cast<int*>(ptr);
               delete data;
               break;
            }
         case(FLOAT): 
            {
               float* data = static_cast<float*>(ptr);
               delete data;
               break;
            }
         case(DOUBLE): 
            {
               double* data = static_cast<double*>(ptr);
               delete data;
               break;
            }
         case(F_VEC3): 
            {
               float* data = static_cast<float*>(ptr);
               delete[] data;
               break;
            }
         case(F_MAT4): 
            {
               float* data = static_cast<float*>(ptr);
               delete[] data;
               break;
            }
         case(UNKNOWN):
         default: 
            FIG_ASSERT(false , "Unknown ShaderDataType encounted in deleteDataPtr func")
      }
   }


} // namespace Core
