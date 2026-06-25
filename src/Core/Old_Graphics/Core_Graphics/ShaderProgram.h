#pragma once
#include "GraphicsComponents.h"
#include "Shader.h"
#include "Core_Utils/Flags.h"
#include <memory>
#include <string>
#include <vector>

namespace Core{
   // Data types that must be supported in some way by a graphics implementation
   // Adding on to this list will require updating all existing graphics implementations
   // NOTE: if a type gets added here, likely need to update every function that either takes 
   // a parameter of ShaderDataType, or returns a parameter of ShaderDataType
   enum ShaderDataType{ 
      INT,
      FLOAT,
      DOUBLE,
      F_VEC3,
      F_MAT4,
      UNKNOWN,
   };

   // This function type takes in a const void* which the writer of the function is expected to 
   // properly set (or point towards) as correct data type for this uniform variable 
   // the pairs are component bool value where the bool represents if the component existed for 
   // this particular U
   // TODO: This seems veryyyy dangerous but for now oh well
   using UniformVariableSetterFunction = typename CreateComponentFunction<GraphicsComponentList>::type;

   struct ShaderInputVariableInfo{
      ShaderDataType type;
      std::string name;
      std::size_t numElements; // usually 1, but if it is an array of things it might be more
      UniformVariableSetterFunction callback = nullptr;
   };


   class ShaderProgram{
      public:
         // Shader Program validity is tracked via stages. Each stage can have any number of fail reasons within it which have kept it from advanving to the next state
         enum ShaderProgramStage{
            NOT_CREATED, 
            CREATED,
            LINKED
         };

         // NOTE: The valid in does not mean that the fail bit cannot be used in other stages, just that 
         // those fail bits only come from an 'actual error' in those states as opposed to breaking the flow
         // Example: in NOT_CREATED stage, a fail bit of ATTACHMENT_NOT_ATTEMPTED means that a shader was attempted 
         // to be added to a program which was not yet created yet. Wheras ATTACHMENT_NOT_ATTEMPTED in CREATED or LINKED, 
         // indicates something was wrong with the shader itself which casued the program to decide not to even attempt to attach it
         // TODO: Do we want to add a fail stage for if a callback is incorrectly added?
         enum StageFailFlags_{
            NONE                       = 0,      // Valid in: All stages
            CREATION_FAILURE           = 1 << 0, // Valid in: NOT_CREATED
            ATTACHMENT_NOT_ATTEMPTED   = 1 << 1, // Valid in: CREATED, LINKED (?)
            ATTACHMENT_FAILED          = 1 << 2, // Valid in: CREATED, LINKED (?)
            LINK_FAILED                = 1 << 3, // Valid in: CREATED, LINKED (?)
            STALE                      = 1 << 4, // Valid in: LINKED
            INVALID_FOR_USE            = 1 << 5, // Valid in: LINKED
         };
         typedef Flags<StageFailFlags_> StageFailFlags;


         struct ProgramState{
            ShaderProgramStage stage;
            StageFailFlags status;
         };

         ShaderProgram();
         virtual ~ShaderProgram() = default;

         virtual bool initializeProgram() = 0;

         // returns true if the shader already is attached
         virtual bool addShader(std::shared_ptr<Shader> shader) = 0;

         // get list of shaders
         const std::vector<std::shared_ptr<Shader>>& getAttachedShadersList() const { return m_shaders; }
         std::vector<std::shared_ptr<Shader>>& getAttachedShadersList() { return m_shaders; }

         // links all added shaders together
         virtual bool link() = 0;

         // TODO: here or let render device do that? or return some type of ID?
         // TODO: Add logic for checkign if all the uniform callbacks are set
         virtual bool useProgram() = 0;

         // This should only be used to retrieve an internal graphics implementation log
         // Returns an empty string if there is no log information
         virtual std::string getInfoLog() const = 0;

         // TODO: Do we need these?
         // Returns a list of ShaderInputVariableInfo structs which are graphics implementation specific and represent necessary info for Uniform variables
         virtual const std::vector<ShaderInputVariableInfo>& getUniformVariables() const = 0;
         // Returns a list of ShaderInputVariableInfo structs which are graphics implementation specific and represent necessary info for per-vertex variables
         virtual const std::vector<ShaderInputVariableInfo>& getAttributeVariables() const = 0;

         virtual bool setUniformCallback(std::string_view name, UniformVariableSetterFunction callback) = 0;

         // NOTE: not string_view because we need to pass as char* to some functions 
         virtual bool setUniform(std::string name, void* value) = 0;
         // TODO: Currently all of our attributes are handles by binding a VAO which matches the layout specified 
         // in Core/Graphics/TODO.txt and so there is not really a reason to use setAttribute() currently
         virtual bool setAttribute(std::string_view name, void* value, ShaderDataType type) = 0;

         bool isFailBitSet(const StageFailFlags bits, StageFailFlags_ flag) const;

         // For now if it is linked and no failbits are set but likely to changed based on some things
         // pulled into its own function so that if multiple areas need to know if it is 'valid' and our def 
         // of valid changes we only have to change this.
         bool isValid() const;


      protected:
         void setState(ShaderProgramStage newStage, StageFailFlags fail = NONE);

         ProgramState m_state;
         std::vector<std::shared_ptr<Shader>> m_shaders;
         std::vector<ShaderInputVariableInfo> m_attributeVariables; 
         std::vector<ShaderInputVariableInfo> m_uniformVariables;
   };

} // namespace Core

// NOTE: SEE glGetProgramiv function!!
