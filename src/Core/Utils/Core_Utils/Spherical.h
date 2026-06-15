#pragma once 
#include "Core_Utils/Concepts.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Utils/Log.h"
#include <cassert>
#include <cmath>

// helper functions for cartesion <-> spherical coordinates
namespace Core{
   namespace Spherical{
      // TODO: A) for cart to sphere, can we guarantee the bounds we give for theta and phi? 
      // B) for sphere to cart, how do we handle if we are given out of bounds values 

      // takes cartesian coordinates and converts them into their spherical counterparts 
      // (r, theta, phi) where theta is [0, pi] and phi is [0, 2pi), and r is >= 0
      template<typename T, typename U = Core::Concepts::float_promotion_t<T>>
      requires(Core::Concepts::numeric<T>)
      Core::Linear::Vector<U, 3> cartesianToSpherical(const Core::Linear::Vector<T, 3>& v);


      // takes spherical coordinates and converts them into their cartesian counterparts 
      // (x, y, z) 
      //  where theta is [0, pi] and phi is [0, 2pi)
      template<typename T, typename U = Core::Concepts::float_promotion_t<T>>
      requires(Core::Concepts::numeric<T>)
      Core::Linear::Vector<U, 3> sphericalToCartesian(const Core::Linear::Vector<T, 3>& v);

      /*
       *
       * Implementations 
       *
       */
      template<typename T, typename U> requires(Core::Concepts::numeric<T>)
      Core::Linear::Vector<U, 3> cartesianToSpherical(const Core::Linear::Vector<T, 3>& v){
         U r      = std::sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
         U theta  = std::acos(v[1] / r);
         U phi    = std::atan2(v[0], v[2]);
         if(phi < 0)
            phi += 2 * M_PI;

         FIG_ASSERT(theta >= 0 && theta <= M_PI, "Calculated theta out of expected range in cartesianToSpherical() function.");
         FIG_ASSERT(phi >= 0 && phi < (2 * M_PI) , "Calculated phi out of expected range in cartesianToSpherical() function.")

         return {r, theta, phi};
      }

      template<typename T, typename U> requires(Core::Concepts::numeric<T>)
      Core::Linear::Vector<U, 3> sphericalToCartesian(const Core::Linear::Vector<T, 3>& v){
         FIG_ASSERT(v[1] >= 0 && v[1] <= M_PI , "Supplied theta out of expected range in sphericalToCartesian() function.")
         FIG_ASSERT(v[2] >= 0 && v[2] < (2 * M_PI) , "Supplied phi out of expected range in sphericalToCartesian() function.")

         const U x = v[0] * std::sin(v[1]) * std::sin(v[2]);
         const U y = v[0] * std::cos(v[1]);
         const U z = v[0] * std::sin(v[1]) * std::cos(v[2]);

         return { x, y, z};
      }
   } // namespace Spherical
} // namespace Core
