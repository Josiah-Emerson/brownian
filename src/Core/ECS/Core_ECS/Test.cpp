#include "Core_Utils/Log.h"
#include "SortedComponentPool.h"

namespace Core{
   void func(){
      struct Comperand  {
         int x; 

         bool operator==(const Comperand& other) const {return x == other.x; }
      };

      struct Compare{
         bool operator()(const Comperand& lhs, const Comperand& rhs) const { return lhs == rhs; }
      };

      struct MyComponentTag {};
      using MyComponent = Component<int, MyComponentTag>;

      SortedComponentPool<MyComponent, Compare, Comperand> pool;
      for(int i = 0; i < 10; ++i){
         pool.insert(i, MyComponent{i}, Comperand{i});
      }

      for(const auto& c : pool){
         std::string str = "Element: ";
         str += std::to_string(c.val);
         FIG_LOG_MESSAGE(str.c_str());
      }
   }
} // namespace Core
