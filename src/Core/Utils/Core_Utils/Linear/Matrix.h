#pragma once
#include "Core_Utils/Concepts.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Utils/Log.h"
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <type_traits>

/*
 * Provides useful types and associated functions with Matrices
 * TODO: Might be interesting to explore a design where we store it as an array of vectors,
 * which would help with things like returning a row as a vector without having to construct one,
 * and possibly transposing, however it brings up challenges when we need a pointer to the data. For example,
 * opengl requires a simple pointer which we could just contstruct, but how do we ensure that the pointer stays valid?
 *
 * TODO: Add more flexibility in creation of matricies (and possibly vectors too for that matter)
 * For example: Vecs of different types create a common matrix of some other type
 * For example: Matrix(fmat2, 1.f) creates a mat3 that looks like:
 *    x x 0  = fmat2 | 0
 *    x x 0  =   0   | 1.f
 *    0 0 1  =
 *
 *    TODO: (For Vector as well): Figure out how to do type deduction and dimension deduction and such
 */

namespace Core{
   namespace Linear{

      // Forward declarations
      template<typename T, std::size_t N> requires(Concepts::numeric<T>)
      class Vector;

      // Creates an M x N matrix of type T where M is the number of rows and N is the number of columns, and T is numeric
      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      class Matrix{
         public: 
            // creates an MxN matrix of type T with all elements initialized to 0
            Matrix(); 

            // creates an MxN matrix of type T with all elements initialized to val
            Matrix(const T val);

            // Creates an MxN matrix of type T from M (row) vectors of length N
            Matrix(std::initializer_list<Vector<T, N>> vals);

            // TODO: Probably not the best way to do this but works (?) for now
            // Creates an MxN matrix of type T from M (row) vectors of length N
            template<typename... Args>
            requires(sizeof...(Args) == M &&
                     (std::is_same_v<Args, Vector<T, N>> && ...))
            Matrix(Args... args);

            // TODO: Big 5 if needed. For now default
            Matrix(const Matrix& m) = default;
            Matrix(Matrix&& m) = default;
            Matrix& operator=(const Matrix& m) = default;
            Matrix& operator=(Matrix&& m) = default;
            ~Matrix() = default;

            // TODO: As with most of these I am not sure of their efficiency
            Matrix(const Vector<T, M>& vec) requires(N == 1);

            // Matrix Multiplication
            template<typename U, std::size_t R, std::size_t C>
            requires(Concepts::numeric<U> && N == R)
            Matrix<std::common_type_t<T, U>, M, C> operator*(const Matrix<U, R, C>& rhs) const;

            // Mat * Vector
            template<typename U>
            requires(Concepts::numeric<U>)
            Vector<std::common_type_t<T, U>, M> operator*(const Vector<U, M>& vec){
               // TODO: For now use our Vector constructor from matrix of size M x 1 ?
               /* 
               auto res = ((*this) * static_cast<Matrix<U, M, 1>>(vec));
               return res.col(0);
               */ 
               return (*this) * static_cast<Matrix<U, M, 1>>(vec); 
            }

            // Transpose
            Matrix<T, N, M> transpose() const;


            // (row, col) access to elements (0-indexed)
            T& at(const std::size_t row, const std::size_t col);
            // (row, col) access to elements (0-indexed)
            const T& at(const std::size_t row, const std::size_t col) const;

            Vector<T, N> row(std::size_t row) const;
            Vector<T, M> col(std::size_t col) const;

            // TODO: (might need to write a version of Vector for this) something like mat.row(0) = { 1, 2, 3, 4 }, and this can set that row 
            // Also: mat<T, 4, 4>.row(0) = { 1, 2 } will just set the first two vals

            void fill(const T val);
         private:
            T m_data[M * N];
      };

      /*
       *
       * COMMON MATRIX TYPEDEFS
       *
       */

      typedef Matrix<int, 3, 3> imat3;
      typedef Matrix<int, 4, 4> imat4;
      typedef Matrix<float, 3, 3> fmat3;
      typedef Matrix<float, 4, 4> fmat4;
      typedef Matrix<double, 3, 3> dmat3;
      typedef Matrix<double, 4, 4> dmat4;

      /*
       *
       * FREE METHOD DECLARATIONS
       *
       */
      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      std::ostream& operator<<(std::ostream& out, const Matrix<T, M, N>& m);

      /*
       *
       * MATRIX CLASS IMPLEMENTATION
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

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      Matrix<T, M, N>::Matrix()
         : Matrix(0)
      { }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      Matrix<T, M, N>::Matrix(const T val){
         fill(val);
      }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      Matrix<T, M, N>::Matrix(std::initializer_list<Vector<T, N>> list){
         FIG_ASSERT(list.size() == M , "Amount of Vectors supplied does not match the amount of rows needed")
         constexpr std::size_t ROW_SIZE = N * sizeof(T); 

         std::size_t cnt { 0 };
         for(const Vector<T, N>& row : list){
            std::memcpy(m_data + (cnt * N), &row.data()[0], ROW_SIZE);
            ++cnt;
         }
      }


      template<typename T, std::size_t M, std::size_t N> requires(Concepts::numeric<T>)
      template<typename... Args> requires(sizeof...(Args) == M && (std::is_same_v<Args, Vector<T, N>> && ...))
      Matrix<T, M, N>::Matrix(Args... args)
      : Matrix({ args... })
      { }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      Matrix<T, M, N>::Matrix(const Vector<T, M>& vec) requires(N == 1) {
         for(std::size_t i { 0 }; i < M; ++i){
            m_data[i] = vec[i];
         }
      }

      /*
       *
       * OPERATORS
       *
       */

      template<typename T, std::size_t M, std::size_t N> requires(Concepts::numeric<T>)
      template<typename U, std::size_t R, std::size_t C> requires(Concepts::numeric<U> && N == R)
      Matrix<std::common_type_t<T, U>, M, C> Matrix<T, M, N>::operator*(const Matrix<U, R, C>& rhs) const{
         // TODO: Is there a more efficient way? Could just create the underlying m_data structure and then initialize with that?
         Matrix<std::common_type_t<T, U>, M, C> res;
         // element ij is rowi dot colj
         for(std::size_t row { 0 }; row < M; ++row){
            for(std::size_t col{ 0 }; col < C; ++col){
               res.at(row, col) = dotProduct( (*this).row(row), rhs.col(col) );
            }
         }

         return res;
      }


      /*
       *
       * MEMBER METHODS
       * 
       */

      template<typename T, std::size_t M, std::size_t N> 
      requires(Concepts::numeric<T>)
      Matrix<T, N, M> Matrix<T, M, N>::transpose() const{
         // swap rows and cols
         // m row vectors of length n
         // get each col in a vector and use those as row vector 
         // TODO: can probably be more efficient with less copies
         Matrix<T, N, M> ret {};
         for(std::size_t newRow { 0 }; newRow < N; ++newRow){
            for(std::size_t newCol { 0 }; newCol < N; ++newCol){
               ret.at(newRow, newCol) = this->at(newCol, newRow);
            }
         }

         return ret;
      }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      T& Matrix<T, M, N>::at(const std::size_t row, std::size_t col){
         // TODO: throw exception?
         FIG_ASSERT(row < M , "Row out of bounds")
         FIG_ASSERT(col < N , "Col out of bounds")

         return m_data[(row * N) + col];
      }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      const T& Matrix<T, M, N>::at(const std::size_t row, std::size_t col) const{
         // TODO: throw exception?
         FIG_ASSERT(row < M , "Row out of bounds")
         FIG_ASSERT(col < N , "Col out of bounds")

         return m_data[(row * N) + col];
      }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      Vector<T, N> Matrix<T, M, N>::row(std::size_t row) const{
         // TODO: Is this efficient? Is there a way to avoid returning by value? Is it copy ellided?
         // TODO: Is there a way to just do some type of argument expansion instead of looping through? This might allow returning right away 
         // Same stuff for col func

         Vector<T, N> vec;
         for(std::size_t i { 0 }; i < N; ++i){
            vec[i] = (*this).at(row, i);
         }

         return vec;
      }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      Vector<T, M> Matrix<T, M, N>::col(std::size_t col) const{
         // TODO: See row func
         Vector<T, M> vec;
         for(std::size_t i { 0 }; i < M; ++i){
            vec[i] = (*this).at(i, col);
         }

         return vec;
      }

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      void Matrix<T, M, N>::fill(const T val){
         for(std::size_t i { 0 }; i < M * N; ++i){
            m_data[i] = val;
         }
      }

      /*
       *
       * FREE METHOD IMPLEMENTATIONS
       *
       */

      template<typename T, std::size_t M, std::size_t N>
      requires(Concepts::numeric<T>)
      std::ostream& operator<<(std::ostream& out, const Matrix<T, M, N>& m){
         // TODO: If replace [] with at function, update this 
         out << '\n';
         for(std::size_t row{ 0 }; row < M; ++row){
            out << "| ";
            for(std::size_t col { 0 }; col < N; ++col){
               out << m.at(row, col) << ' ';
            }
            out << "|\n";
         }
         return out;
      }


   } // namespace Linear
} // namespace Core
