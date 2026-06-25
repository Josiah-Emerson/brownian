#pragma once
#include "Core_Utils/Linear/Vector.h"
#include "Core_Utils/Linear/Matrix.h"
#include "Core_Utils/Linear/MatrixTransform.h"
#include "Core_Utils/Spherical.h"

// TODO: Should look be a direction or a point?
// TODO: Pile all camera variables into a common struct ?
// TODO: We could store proj and view matrices as members and only update them when something which depends on them is updated 
// For example only update view matrix when pos, look or up changes. This would allow us to not calc these every frame, and only 
// when an update is needed. We can return a ptr to these things to items which need it?

namespace Core{
   class Camera{
      public:
         // LOCKED means that the look vector is interpreted as a point in world space to be looked at 
         // DIRECTIONAL means that the look vector is interpreted as a direction to be looking down. 
         // the look vector becomes a direction where the camera is looking in spherical coordinates, with 
         // the axes the same as the world coordinates, but positioned at the camera's location
         // TODO: In my mind if it is a spherical coordinate, the radius wouldn't matter and can be kept 
         // at 1 or whatever as the 'ray' of where we are looking is along the same line regardless of its length
         enum LookMode{
            LOCKED,
            DIRECTIONAL,
         };

         // Constructors
         Camera() = delete;
         // TODO: Logic for if LookMode is DIRECTIONAL
         Camera(const Linear::fvec3& pos, 
               float FOV, 
               float width, 
               float height, 
               const Linear::fvec3& look, 
               LookMode mode = LOCKED);

         // Big 5
         Camera(const Camera&) = delete;
         Camera(Camera&&) = delete;
         Camera& operator=(const Camera&) = delete;
         Camera& operator=(Camera&&) = delete;
         ~Camera() = default;

         // MVP = P * V * M
         template<typename T = float>
         auto viewMatrix() const;

         Linear::fmat4 projectionMatrix() const;
         Linear::fvec3& pos() { return m_pos; }
         float& fov() { return m_FOV; }
         Linear::fvec3& look() { return m_look; }
         // Updates the look vector with helpful bounds checking, especially when in DIRECTIONAL mode
         void updateLookWithOffset(const Linear::fvec3&);
         void updateLookMode(LookMode mode); 
         LookMode lookMode() const { return m_mode; }


      private:
         LookMode m_mode;
         Linear::fvec3 m_pos; // Position of camera in world coordinates
         Linear::fvec3 m_look; // Where the camera is looking in world coordinates, or forward direction
         float m_FOV; // vertical FOV in degrees
         float m_height;
         float m_width;
   };

   // NOTE: T is a default arg
   template<typename T>
   auto Camera::viewMatrix() const{
      if(m_mode == LOCKED)
         return Linear::viewMatrix(m_pos, m_look, Linear::Vector(0, 1, 0));

      // convert m_look vector from a direction in camera space to world space coordinates
      auto camToWorldMat = Linear::modelMatrix(m_pos, Linear::Vector{0, 0, 0}, Linear::Vector{1, 1, 1});
      auto look = Spherical::sphericalToCartesian(m_look);
      Linear::fvec4 tempLook4 = {look[0], look[1], look[2], 1};
      tempLook4 = camToWorldMat * tempLook4;
      auto tempLook = Linear::Vector { tempLook4[0], tempLook4[1], tempLook4[2] };

      return Linear::viewMatrix(m_pos, tempLook, Linear::Vector(0, 1, 0));
   }

} // namespace Core
