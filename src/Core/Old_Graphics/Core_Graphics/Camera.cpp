#include "Camera.h"
#include "Core_Utils/Linear/MatrixTransform.h"
#include "Core_Utils/Spherical.h"

namespace Core{

   // NOTE: mode is a default value
   Camera::Camera(const Linear::fvec3& pos, float FOV, float width, float height, const Linear::fvec3& look, LookMode mode)
      : m_pos { pos }
      , m_FOV { FOV }
      , m_height { height }
      , m_width { width }
      , m_look { look }
      , m_mode { mode }
   { 
      // TODO: If mode is DIRECTIONAL, do we; 
      // A) take look as a point and then convert to spherical in here 
      // B) take look as a direction in spherical coordinates and bounds check
      // For now, just take as a point and convert to spherical
      if(m_mode == DIRECTIONAL){
         m_look = Spherical::cartesianToSpherical(m_look);
      }
   }

   void Camera::updateLookMode(LookMode mode){
      if(m_mode == LOCKED){
         if(mode == LOCKED) 
            return;

         m_look = Spherical::cartesianToSpherical(m_look);
      } else{
         if(mode == DIRECTIONAL) 
            return;

         m_look = Spherical::sphericalToCartesian(m_look);
      }

      m_mode = mode;
   }

   // NOTE: when in DIRECTIONAL, offset[1] is theta (angle from y), 
   // and offset[2] is phi (offset from z) 
   // furthermore, increasing offset[1] will look down and increasing offset[2] will look left usually
   void Camera::updateLookWithOffset(const Linear::fvec3& offset){
      m_look = m_look + offset;
      if(m_mode == DIRECTIONAL){
         // force R to be 1?
         // m_look[0] = 1;

         if(m_look[1] < 0 || m_look[1] > M_PI){ // theta
            bool flipped { false };
            if(m_look[1] < 0 ){
               m_look[1] *= -1;
               flipped = !flipped;
            }

            while(m_look[1] > 2 * M_PI)
               m_look[1] -= 2*M_PI;

            // theta is now in 0 - 2pi but not in 0 - pi
            if(m_look[1] > M_PI){
               m_look[1] = (2*M_PI) - m_look[1];
               flipped = !flipped;
            }

            if(flipped)
               m_look[2] += M_PI;
         }

         if(m_look[2] < 0 || m_look[2] >= 2 * M_PI){ // phi
            if(m_look[2] < 0)
               while(m_look[2] < 0 )
                  m_look[2] += 2 * M_PI;
            if(m_look[2] >= 2 * M_PI)
               m_look[2] -= 2 * M_PI;
         }
      }
      FIG_ASSERT(m_look[1] >= 0 && m_look[1] <= M_PI , "Theta offset not updated properly")
      FIG_ASSERT(m_look[2] >= 0 && m_look[2] < 2 * M_PI , "Phi offset not updated properly")
   }

   Linear::fmat4 Camera::projectionMatrix() const{
      return Linear::projectionMatrix(m_width / m_height, m_FOV, 0.1f, 100.f);
   }
} // namespace Core
