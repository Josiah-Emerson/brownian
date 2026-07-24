#pragma once
#include <array>
#include <memory>
#include "Core_Utils/Flags.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Utils/Types.h"

namespace Core{
   class GuiImpl;

   class Gui{
      public: 
         // TODO: Poor Flags decision means prefix needed for flags
         
         // TODO: Add WindowFlags
         enum WindowFlags_{
            WindowFlags_None = 0, 
         };

         // TODO: Add TextFlags
         enum TextFlags_{
            TextFlags_None = 0,
         };

         // TODO: Add WidgetFlags
         enum WidgetFlags_{
            WidgetFlags_None = 0, 
         };

         typedef Flags<WindowFlags_> WindowFlags;
         typedef Flags<TextFlags_> TextFlags;
         typedef Flags<WidgetFlags_> WidgetFlags;

         class Group;
         class Widgets{
            public:
               Group group(const char* label, bool beginExpanded = false);

               // 'Layout' options
               void separator(std::size_t count = 1);
               void text(const char* text, bool sameLine = false);

               // Value manipulators
               
               // return true if the button was pressed
               // id should only be used if you are in a loop with labels that are the same, in which case, 
               // you can pass a positive integer which is unique to this element to differentiate it
               bool button(const char* label, int id = -1, bool sameLine = false); // TODO: set size? I think ImGui Auto sets to size of label if size is not specified

               // return true if the value changed
               // label: label for checkbox 
               // val: The value which the gui will update to reflect whether the box is checked or not 
               // id should only be used if you are in a loop with labels that are the same, in which case, 
               // you can pass a positive integer which is unique to this element to differentiate it
               // sameLine: same line?
               bool checkbox(const char* label, bool& val, int id = -1, bool sameLine = false);
               
               /*
                * TODO: Figure out the color stuff behind the scenes (i.e. rn we have our Color3 component 
                * as a uint8 0-255, but most things expect color as a float between 0 and 1)
                * I think this expects 0-255, converts to 0-1, presents slider, returns between 0-255
                */ 
               bool rgbSelector(const char* label, Linear::fvec3& color, int id = -1, bool sameLine = false);
               bool rgbSelector(const char* label, Color3& color, int id = -1, bool sameLine = false);

               // TODO: Stuff for floating point vs vector ?
               // TODO: Min/Max checking? Or let ImGui do that
               // id should only be used if you are in a loop with labels that are the same, in which case, 
               // you can pass a positive integer which is unique to this element to differentiate it
               template<typename T>
               bool slider(
                     const char* label,
                     T& var,
                     T min = std::numeric_limits<T>::lowest(),
                     T max = std::numeric_limits<T>::max(),
                     int id = -1,
                     bool sameLine = false
                  );

               template<std::size_t N>
               static constexpr std::array<int, N> makeFilledArray(int value);

               // returns true if any var in vars is updated
               /*
               template<typename T, std::size_t N>
               bool slider(
                     std::array<std::string&, N> labels,
                     Linear::Vector<T, N>& vars, 
                     std::array<T, N> mins = std::array<T, N> { std::numeric_limits<T>::lowest() },
                     std::array<T, N> maxes = std::array<T, N> { std::numeric_limits<T>::max() },
                     std::array<int, N> ids = makeFilledArray<N>(-1),
                     std::array<bool, N> sameLines = std::array<T, N> { false }
                  );
                  */
               template<typename T, std::size_t N>
               bool slider(
                     std::array<const char*, N> labels,
                     Linear::Vector<T, N>& vars, 
                     std::array<T, N> mins = std::array<T, N> { std::numeric_limits<T>::lowest() },
                     std::array<T, N> maxes = std::array<T, N> { std::numeric_limits<T>::max() },
                     std::array<int, N> ids = makeFilledArray<N>(-1),
                     std::array<bool, N> sameLines = std::array<bool, N> { false }
                  );

            protected:
               Widgets() = default;
               Gui* m_gui = nullptr;
         };

         class Group : public Widgets{
            public:
               // TODO: Not really sure here what good a default constructor is, but Falcor had it, 
               // so we can play around with it later
               Group() = default;
               Group(Gui* gui, const char* label, bool beginExpanded = false);
               /* Falcor also had this constructor, but I am not sure if/why we would ever need it 
                * obv would need to add .gui() func
                * Group(const Widgets& w, rest of above...) : Group(w.gui(), label, beginExpanded) { }
                */

               // Return true if this group is open
               bool open() const;
               operator bool() const { return open(); }

               ~Group();

               // End Group block
               void release();
         };

         class Window : public Widgets{
            public:
            Window(Gui* gui, 
                  const char* name, 
                  bool& open,
                  Linear::ivec2 size, 
                  Linear::ivec2 pos = {0, 0}, 
                  WindowFlags flags = WindowFlags_None);
            ~Window();

            /*
             * End Window 
             */
            void release();

            // TODO: columns within 
            // TODO: Sizing 
         };

         Gui();
         ~Gui(); 

         /*
          * Begins the UI frame. Must be called at the start of each frame 
          */
         void beginFrame();

         /*
          * Renders the UI, and ends the ImGui frame 
          */
         void render();

      private: 
         std::unique_ptr<GuiImpl> m_impl;
   };

   template<std::size_t N>
   constexpr std::array<int, N> Gui::Widgets::makeFilledArray(int value){
      std::array<int, N> arr {};
      arr.fill(-1);
      return arr;
   }

   template<typename T, std::size_t N>
   bool Gui::Widgets::slider(std::array<const char*, N> labels, Linear::Vector<T, N>& vars, 
         std::array<T, N> mins, std::array<T, N> maxes, std::array<int, N> ids, std::array<bool, N> sameLines){

      bool b { false };
      for(std::size_t i { 0 }; i < N; ++i){
         if(slider(labels[i], vars[i], mins[i], maxes[i], ids[i],sameLines[i]))
            b = true;
      }

      return b;
   }

} // namespace Core
