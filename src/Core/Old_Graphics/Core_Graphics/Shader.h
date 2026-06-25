#pragma once
#include <string>

// TODO: Add a way for ShaderPrograms to be notified about a change in subShader compilations. Call back?

namespace Core{
   enum ShaderType : int{
      VERTEX_SHADER,
      FRAGMENT_SHADER,
      COMPUTE_SHADER,
   };

   // Represents an agnostic compiled shader
   class Shader{
      public: 
         enum ShaderCompilationStage{
            NO_COMPILATION_ATTEMPTED,  
            FILE_PARSE_FAILURE, // NOTE: We don't really do any parsing but it works for a possible failure point in the Shader process
            COMPILATION_FAILED,
            RECOMPILATION_FAILED,
            SUCCESSFUL_COMPILATION,
            STALE_COMPILATION
         };

         Shader(ShaderType type, ShaderCompilationStage stage) 
            : m_type { type } 
            , m_stage { stage }
         { }
         virtual ~Shader() = default;


         ShaderType getShaderType() const { return m_type; }
         ShaderCompilationStage getStatus() const{ return m_stage; }
         virtual bool compile() = 0;
         virtual bool isCompiled() const = 0;
         virtual bool reload() = 0; // Hot reloading
         virtual std::string getLogInfo() const = 0;
         virtual bool operator==(const Shader&) const = 0;

         // TODO: Is this helpful? If this is true, there are lots of invariants we need to ensure
         // For openGL we need to remember to delete the old shader object and compile the new one (if it works successfully)
         // virtual void updatePath(const char* newPath) = 0;

      protected: 
         ShaderCompilationStage m_stage;

      private: 
         ShaderType m_type;
   };
} // namespace Core
