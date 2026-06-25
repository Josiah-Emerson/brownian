#include "Gui.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Utils/Log.h"
#include "imgui.h"
#include "Application.h"

/*
 * OS and Graphics ifdefs 
 */
#ifdef LINUX 
#include <backends/imgui_impl_x11.h>
#include "Core_Window/LinuxWindow.h"
#endif 

#ifdef OPENGL
#include <backends/imgui_impl_opengl3.h>
#endif


namespace Core{
   class GuiImpl{
      public: 
         GuiImpl(); // TODO: Why public if Gui is whats gonna create it anyway? 
                    // Is it because of unique_ptr/make_unique?

      private: 
         friend class Gui;

         void addSeparator();
         void addText(const char* text, bool sameline);

         bool beginWindow(const char* name, bool& open, Linear::ivec2 size, Linear::ivec2 pos);
         void endWindow();

         bool addButton(const char* label, bool sameLine);
         bool addRGBSelector(const char* label, Linear::fvec3& color, bool sameLine);

         bool addSliderHelper(const char* label, void* data, ImGuiDataType imguiType, void* min, void* max, bool sameLine);

         template<typename T>
         bool addSlider(const char* label, T& var, T min, T max, bool sameLine);

         bool beginGroup(const char* label, bool beginExpanded);
         void endGroup();

         void beginFrame();
         void renderDrawData();

         /*
          * MEMBER VARS
          */
         ImGuiContext* m_imGuiContext { nullptr };
         void (*m_renderDrawDataFunc)(ImDrawData*) { nullptr };
         void (*m_graphicImplNewFrame)() { nullptr };
         void (*m_windowImplNewFrame)() { nullptr };
         int m_groupStackSize { 0 };
   };

   GuiImpl::GuiImpl(){
      IMGUI_CHECKVERSION();
      m_imGuiContext = ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      ImGui::StyleColorsDark();

      // Window system init
#ifdef LINUX
      // TODO: What if we mix X11 with not OpenGL
      LinuxWindow::LinuxImplementationDetails* winDetails = 
         (LinuxWindow::LinuxImplementationDetails*)
         Application::get().getWindow()->getImplementationSpecificDetails();

      ImGui_ImplX11_InitForOpenGL(winDetails->m_XDisplay, 
                                 &winDetails->m_XWindow);
      m_windowImplNewFrame = ImGui_ImplX11_NewFrame;
#endif

      // Graphics init
#ifdef OPENGL
      ImGui_ImplOpenGL3_Init();
      m_renderDrawDataFunc = ImGui_ImplOpenGL3_RenderDrawData;
      m_graphicImplNewFrame = ImGui_ImplOpenGL3_NewFrame;
#endif 
      FIG_ASSERT(m_imGuiContext && 
            m_renderDrawDataFunc && 
            m_graphicImplNewFrame && 
            m_windowImplNewFrame , 
            "nullptrs")
   }

   void GuiImpl::addSeparator(){
      ImGui::Separator();
   }

   void GuiImpl::addText(const char* text, bool sameLine){
      if(sameLine)
         ImGui::SameLine();

      ImGui::TextUnformatted(text);
   }

   bool GuiImpl::beginWindow(const char* name, bool& open, Linear::ivec2 size, Linear::ivec2 pos){
      // TODO: update to guiconfig macros instead of ImGuiVec
      ImGui::SetNextWindowSize(ImVec2(size[0], size[1]));
      ImGui::SetNextWindowPos(ImVec2(pos[0], pos[1]));
      ImGui::Begin(name, &open);

      if(!open)
         ImGui::End();


      return open;
   }

   void GuiImpl::endWindow(){
      ImGui::End();
   }

   bool GuiImpl::addButton(const char* label, bool sameLine){
      if(sameLine)
         ImGui::SameLine();

      return ImGui::Button(label);
   }

   bool GuiImpl::addRGBSelector(const char* label, Linear::fvec3& color, bool sameLine){
      if(sameLine)
         ImGui::SameLine();

      // TODO: ColorEdit vs ColorPicker
      return ImGui::ColorEdit3(label, &color[0]);
   }

   bool GuiImpl::addSliderHelper(const char* label, void* data, ImGuiDataType imguiType, void* min, void* max, bool sameLine){
      if(sameLine)
         ImGui::SameLine();

      return ImGui::DragScalar(label, imguiType, data, 1.f, min, max);
   }

   template<typename T>
   bool GuiImpl::addSlider(const char* label, T& var, T min, T max, bool sameLine){
      if constexpr (std::is_same_v<T, std::int32_t>){
         return addSliderHelper(label, &var, ImGuiDataType_S32, &min, &max, sameLine);
      } else if constexpr (std::is_same_v<T, float>){
         return addSliderHelper(label, &var, ImGuiDataType_Float, &min, &max, sameLine);
      } else {
         static_assert(!sizeof(T), "Data type not supported");
      }
   }

   bool GuiImpl::beginGroup(const char* label, bool beginExpanded){
      bool visible = ImGui::TreeNodeEx(label, beginExpanded ? 
            ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);

      if(visible)
         ++m_groupStackSize;

      return visible; 
   }

   void GuiImpl::endGroup(){
      FIG_ASSERT(m_groupStackSize > 0, "m_groupStackSize not > 0")
      ImGui::TreePop();
      --m_groupStackSize;
   }

   void GuiImpl::beginFrame(){
      // TODO: Do we need to set the current context ?
      ImGui::SetCurrentContext(m_imGuiContext);
      m_graphicImplNewFrame();
      m_windowImplNewFrame();
      ImGui::NewFrame();
   }

   void GuiImpl::renderDrawData(){
      // TODO: Like the other one, do we need to ?
      ImGui::SetCurrentContext(m_imGuiContext);
      ImGui::Render();
      ImDrawData* drawData = ImGui::GetDrawData();
      m_renderDrawDataFunc(drawData);
   }

   Gui::Group Gui::Widgets::group(const char* label, bool beginExpanded){
      return {m_gui, label, beginExpanded};
   }

   void Gui::Widgets::separator(std::size_t count){
      if(!m_gui)
         return;

      for(std::size_t i { 0 }; i < count; ++i)
         m_gui->m_impl->addSeparator();
   }

   void Gui::Widgets::text(const char* text, bool sameLine){
      if(!m_gui)
         return;
      m_gui->m_impl->addText(text, sameLine);
   }

   bool Gui::Widgets::button(const char* label, bool sameLine){
      return m_gui && m_gui->m_impl->addButton(label, sameLine);
   }

   bool Gui::Widgets::rgbSelector(const char* label, Linear::fvec3& color, bool sameLine){
      return m_gui && m_gui->m_impl->addRGBSelector(label, color, sameLine);
   }

   bool Gui::Widgets::rgbSelector(const char* label, Color3& color, bool sameLine){
      Linear::fvec3 floatVec { color.R / 255.f, color.G / 255.f, color.B / 255.f };
      if(!rgbSelector(label, floatVec, sameLine))
         return false;
      floatVec = floatVec * 255;
      // TODO: Assert if > 255 ?
      // NOTE: If we remove these casts, update comment for Color3 Types.h about updating here 
      color.R = static_cast<std::uint8_t>(floatVec[0]);
      color.G = static_cast<std::uint8_t>(floatVec[1]);
      color.B = static_cast<std::uint8_t>(floatVec[2]);
      return true;
   }

   template<typename T>
   bool Gui::Widgets::slider(const char* label, T& var, T min, T max, bool sameLine){
      return m_gui && m_gui->m_impl->addSlider(label, var, min, max, sameLine);
   }

#define ADD_SLIDER_TYPE(Type) \
   template bool Gui::Widgets::slider<Type>(const char*, Type&, Type, Type, bool);

   ADD_SLIDER_TYPE(std::int32_t);
   ADD_SLIDER_TYPE(float);
#undef ADD_SLIDER_TYPE

   Gui::Group::Group(Gui* gui, const char* label, bool beginExpanded){
      if(gui && gui->m_impl->beginGroup(label, beginExpanded))
         m_gui = gui;
   }

   bool Gui::Group::open() const{
      return m_gui != nullptr;
   }

   Gui::Group::~Group(){
      release();
   }

   void Gui::Group::release(){
      if(m_gui)
         m_gui->m_impl->endGroup();

      m_gui = nullptr;
   }

   Gui::Window::Window(Gui* gui, const char* name, bool& open, Linear::ivec2 size, Linear::ivec2 pos, WindowFlags flags){
      if(gui->m_impl->beginWindow(name, open, size, pos))
         m_gui = gui;
   }

   Gui::Window::~Window(){
      release();
   }

   void Gui::Window::release(){
      if(m_gui)
         m_gui->m_impl->endWindow();

      m_gui = nullptr;
   }

   Gui::Gui(){
      m_impl = std::make_unique<GuiImpl>();
   }

   // TODO
   Gui::~Gui() = default;

   void Gui::beginFrame(){
      m_impl->beginFrame();
   }

   void Gui::render(){
      m_impl->renderDrawData();
   }
};
