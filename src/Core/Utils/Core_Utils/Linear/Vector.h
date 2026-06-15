#pragma once
#include "Core_Utils/Concepts.h"
#include "Core_Utils/Linear/Matrix.h"
#include "Core_Utils/Log.h"
#include <cmath>
#include <concepts>
#include <initializer_list>
#include <ostream>
#include <type_traits>

/*
 * Provides useful types and associated functions with vectors. This is meant in the Linear Algebra sense of the word, and not in the std::Vector sense
 * TODO: Look into handling math better (both more optimized and ensuring we don't lose data)
 * TODO: For functions: normalize() with type int should we error out? Unless the vector is a unit vector already it will likely return the vector { 0, 0, 0 }
 */

namespace Core{
   namespace Linear{
      // Forward declarations
      template<typename T, std::size_t M, std::size_t N> requires(Concepts::numeric<T>)
      class Matrix;

      // Creates an N x 1 column vector of type T, where T is numeric (treated as column for now because we like matrix vector multiplation) 
      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      class Vector{
         public: 

            // Creates a vector of size N with all elements uninitialized
            Vector();

            // Creates a vector of size N with all elements initialized to val
            Vector(T val);

            // TODO: For below two constructors: if size N is dictated by size of vals/args, then update the description to that
            // Otherwise, if this is not the case, update the description to reflect that any unspecified elements will be either 1 or 0 (TBD)

            // Creates a vector where vals specify the elements
            Vector(std::initializer_list<T> vals);

            // Creates a vector where args specify the elements
            template<typename... Args>
            requires(sizeof...(Args) <= N &&
                    (std::is_same_v<Args, T> && ...))
            Vector(Args... args);

            // TODO: Big 5 if needed
            template<typename U>
            requires(std::convertible_to<U, T>)
            Vector(const Vector<U, N>& v)
             : Vector()
            {
               for(std::size_t i { 0 }; i < N; ++i)
                  m_data[i] = v[i];
            }

            Vector(const Vector& v) = default; // copy constructor
            Vector(Vector&& v) = default; // move constructor
            Vector& operator=(const Vector& v) = default; // copy assignment
            Vector& operator=(Vector&& v)= default; // move assignment
            ~Vector() = default;

            Vector(const Matrix<T, N, 1>& mat);

            // Operators
            template<typename U, typename V = std::common_type_t<T, U>>
            requires(Concepts::numeric<U>)
            Vector<V, N> operator*(U scalar) const;

            // element-wise multiplation
            template<typename U, typename V = std::common_type_t<T, U>>
            requires(Concepts::numeric<U>)
            Vector<V, N> operator*(Vector<U, N> rhs) const;

            template<typename U, typename V = std::common_type_t<T, U>>
            requires(Concepts::numeric<U>)
            Vector<V, N> operator+(Vector<U, N> rhs) const;

            template<typename U, typename V = std::common_type_t<T, U>>
            requires(Concepts::numeric<U>)
            Vector<V, N> operator-(Vector<U, N> rhs) const;

            // accessors
            T& operator[](std::size_t i);
            const T& operator[](std::size_t i) const;

            // Methods
            double magnitude() const;
            void normalize();

            // returns a vector of length 1 in the direction of this vector
            Vector<Concepts::float_promotion_t<T>, N> unitVector() const;
            
            // TODO: Funcs below are helpful for now when working on the class, but probably don't actually want it public or at all?
            void fill(T val);
            T const * const data() const { return m_data; }
         private: 
            T m_data[N];
      };
      // Deduction guide
      template<typename... Args> requires(Concepts::numeric<Args> && ...)
      Vector(Args...) -> Vector<std::common_type_t<Args...>, sizeof...(Args)>;

      /*
       *
       * COMMON TYPEDEFS
       *
       */
      typedef Vector<int, 2> ivec2;
      typedef Vector<float, 2> fvec2;
      typedef Vector<double, 2> dvec2;
      typedef Vector<int, 3> ivec3;
      typedef Vector<float, 3> fvec3;
      typedef Vector<double, 3> dvec3;
      typedef Vector<int, 4> ivec4;
      typedef Vector<float, 4> fvec4;
      typedef Vector<double, 4> dvec4;

      /*
       *
       * FREE METHOD DECLARATIONS
       *
       */

      // TODO: could add a bool for right handedness and/or a vector which defines i j and k
      // Both could be default unless supplied
      // returns the cross product of 2 3D vectors using the right hand rule
      // NOTE: Assumes i, j, and k are 1
      template<typename T, typename U, typename V = Concepts::float_promotion_t<T, U>>
      requires(Concepts::numeric<T, U>)
      Vector<V, 3> crossProduct(const Vector<T, 3>& a, const Vector<U, 3>& b);

      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      std::ostream& operator<<(std::ostream& out, const Vector<T, N>& v);

      // Non-member function so we can do scalar * vector not just vector * scalar
      template<typename T, typename U, std::size_t N, typename V = std::common_type_t<T, U>>
      requires(Concepts::numeric<T, U>)
      Vector<V, N> operator*(U scalar, const Vector<T, N>& vector);

      template<typename T, typename U, std::size_t N, typename retType = std::common_type_t<T,U>>
      requires(Concepts::numeric<T, U>)
      retType dotProduct(const Vector<T, N>& a, const Vector<U, N>& b);

      // returns angle between two vectors. 
      // when rads is true, the angle is in radians, and degrees when false
      template<typename T, typename U, std::size_t N>
      requires(Concepts::numeric<T, U>)
      float angleBetweenVectors(const Vector<T, N>& a, const Vector<U, N>& b, bool rads = false);
      /*
       *
       * VECTOR CLASS IMPLEMENTATION
       *
       */


      /*
       *
       * PUBLIC METHODS
       *
       */

      /*
       *
       * CONSTRUCTORS, DESTRUCTORS and ASSIGNMENT
       *
       */

      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      Vector<T, N>::Vector()
         : Vector(0)
      { }

      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      Vector<T, N>::Vector(T val){
         fill(val);
      }

      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      Vector<T, N>::Vector(std::initializer_list<T> vals){
         // TODO: Should we just throw an exception here?
         FIG_ASSERT(vals.size() <= N , "Values given is larger than size of the vector")

         for(std::size_t i { 0 }; i < vals.size(); ++i){
            m_data[i] = vals.begin()[i];
         }

         if(vals.size() < N){
            for(std::size_t i { vals.size() }; i < N; ++i){
               m_data[i] = 0;
            }
         }
      }

      // Disgusting signature, but it just forwards args to the initializer list constructor above
      template<typename T, std::size_t N> requires(Concepts::numeric<T>)
      template<typename... Args> requires(sizeof...(Args) <= N && (std::is_same_v<Args, T> && ...))
      Vector<T, N>::Vector(Args... args)
         : Vector({ static_cast<T>(args)... }) // TODO: static cast is redundant for now as we ensure Args is T, but perhaps for future?
         { }


      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      Vector<T, N>::Vector(const Matrix<T, N, 1>& mat) {
         for(std::size_t i { 0 }; i < N; ++i){
            m_data[i] = mat.at(i, 0);
         }
      }

      /*
       *
       * OPERATORS
       *
       */

      // NOTE: V is a default arg 
      template<typename T, std::size_t N> requires(Concepts::numeric<T>)
      template<typename U, typename V> requires(Concepts::numeric<U>)
      Vector<V, N> Vector<T, N>::operator*(U scalar) const{
         Vector<V, N> res;
         for(std::size_t i { 0 }; i < N; ++i)
            res[i] = scalar * m_data[i];

         return res;
      }

      // NOTE: V is a default arg 
      template<typename T, std::size_t N> requires(Concepts::numeric<T>)
      template<typename U, typename V> requires(Concepts::numeric<U>)
      Vector<V, N> Vector<T, N>::operator*(Vector<U, N> rhs) const{
         Vector<V, N> temp;
         for(std::size_t i { 0 }; i < N; ++i)
            temp[i] = (*this)[i] * rhs[i];

         return temp;
      }

      // NOTE: V is a default arg 
      template<typename T, std::size_t N> requires(Concepts::numeric<T>)
      template<typename U, typename V> requires(Concepts::numeric<U>)
      Vector<V, N> Vector<T, N>::operator+(Vector<U, N> rhs) const{
         Vector<V, N> res;
         for(std::size_t i { 0 }; i < N; ++i)
            res[i] = (*this)[i] + rhs[i];

         return res;
      }

      // NOTE: V is a default arg 
      template<typename T, std::size_t N> requires(Concepts::numeric<T>)
      template<typename U, typename V> requires(Concepts::numeric<U>)
      Vector<V, N> Vector<T, N>::operator-(Vector<U, N> rhs) const{
         Vector<V, N> res;
         for(std::size_t i { 0 }; i < N; ++i)
            res[i] = (*this)[i] - rhs[i];

         return res;
      }


      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      T& Vector<T, N>::operator[](std::size_t i){
         // TODO: Should we just throw an exception here?
         FIG_ASSERT(i < N , "Out of bounds")
         return m_data[i];
      }


      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      const T& Vector<T, N>::operator[](std::size_t i) const {
         // TODO: Should we just throw an exception here?
         FIG_ASSERT(i < N , "Out of bounds")
         return m_data[i];
      }

      /*
       *
       * PUBLIC MEMBER OPERATIONS
       *
       */

      template<typename T, std::size_t N> 
      requires(Concepts::numeric<T>)
      double Vector<T, N>::magnitude() const{
         // TODO: At some point add a check to see if our values get to a point where we are losing precision
         double total { 0 };
         for(std::size_t i { 0 }; i < N; ++i)
            total += m_data[i] * m_data[i];

         return std::sqrt(total);
      }

      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      void Vector<T,N>::normalize(){
         // TODO: For now just assert out if T is an int I guess? At some point would be nice to either 
         // delete normalize() from Vector<int, N> but this might require some annoying inheritance hacking
         FIG_ASSERT((!std::is_same_v<T, int>) , "normalizing vector of type int will most likely result in an undesired vector. Try using unitVector() with a floating point type")
         FIG_ASSERT((std::is_floating_point_v<T>) , "vector type is not a floating point type, and normalization will likely result in undesired vector")
         double mag { magnitude() };
         for(std::size_t i { 0 }; i < N; ++i){
            m_data[i] /= mag;
         }
      }

      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      Vector<Concepts::float_promotion_t<T>, N> Vector<T, N>::unitVector() const{
         // TODO: is there a better way?
         typedef Concepts::float_promotion_t<T> Tp;
         Vector<Tp, N> temp;
         for(std::size_t i { 0 }; i < N; ++ i){
            temp[i] = static_cast<Tp>((*this)[i]);
         }

         temp.normalize();
         return temp;
      }

      template<typename T, std::size_t N> 
      requires(Concepts::numeric<T>)
      void Vector<T, N>::fill(T val){
         for(std::size_t i { 0 }; i < N; ++i){
            m_data[i] = val;
         }
      }


      /*
       *
       * END OF IMPLEMENTATION
       *
       */


      /*
       *
       * FREE VECTOR METHODS
       *
       */

      // NOTE: type V is a default arg
      template<typename T, typename U, typename V>
      requires(Concepts::numeric<T, U>)
      Vector<V,  3> crossProduct(const Vector<T, 3>& a, const Vector<U, 3>& b){
         // TODO: General, but check for precision loss!! (At least in debug mode if not in release)

         // NOTE: Maybe refactor to use a determinant function of matrix ?
         // Although this function doesn't need to be general since we can only really do it for 3D
         // Also may be slower
         return { static_cast<V>(  (a[1] * b[2]) - (a[2] * b[1]) ), 
                  static_cast<V>( -(a[0] * b[2]) + (a[2] * b[0]) ),
                  static_cast<V>(  (a[0] * b[1]) - (a[1] * b[0]) )};
      }
         
      // output overload: 
      template<typename T, std::size_t N>
      requires(Concepts::numeric<T>)
      std::ostream& operator<<(std::ostream& out, const Vector<T, N>& v){
         out << '(';
         for(std::size_t i { 0 }; i < N - 1; ++i){
            out << v[i] << ", ";
         }
         out << v[ N - 1] << ")";
         
         return out;
      }
      
      // NOTE: V is a default arg
      // Non-member function so we can do scalar * vector not just vector * scalar
      template<typename T, typename U, std::size_t N, typename V>
      requires(Concepts::numeric<T, U>)
      Vector<V, N> operator*(U scalar, const Vector<T, N>& vector){
         return vector * scalar;
      }

      // NOTE: retType is a default arg
      template<typename T, typename U, std::size_t N, typename retType>
      requires(Concepts::numeric<T, U>)
      retType dotProduct(const Vector<T, N>& a, const Vector<U, N>& b){
         auto vec = a * b;
         retType res { 0 };
         for(std::size_t i { 0 }; i < N; ++i){
            res += vec[i];
         }

         return res;
      }

      // forward declaration 
      template<typename T, typename U> requires(Concepts::numeric<T>)
      U radToDeg(T);

      // NOTE: rads is default arg
      template<typename T, typename U, std::size_t N>
      requires(Concepts::numeric<T, U>)
      float angleBetweenVectors(const Vector<T, N>& a, const Vector<U, N>& b, bool rads){
         float theta = std::acos( (dotProduct(a, b)) / (a.magnitude() * b.magnitude()) );
         return (rads ? theta : radToDeg<float, float>(theta));
      }

   } // namespace Linear
} // namespace Core
