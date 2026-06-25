#include "ShaderProgram.h"

namespace Core{
   ShaderProgram::ShaderProgram() 
      : m_state {NOT_CREATED, NONE}
      , m_attributeVariables {}
      , m_uniformVariables {}
   { }
   bool ShaderProgram::isFailBitSet(const StageFailFlags bits, StageFailFlags_ flag) const{
      return bool(bits & flag);
   }

   bool ShaderProgram::isValid() const {
      return (m_state.stage == LINKED) && (!m_state.status);
   }

   // NOTE: fail is a default arg
   void ShaderProgram::setState(ShaderProgramStage newStage, StageFailFlags fail) { 
      m_state.stage = newStage;
      m_state.status = fail; 
   }
} // namespace Core
