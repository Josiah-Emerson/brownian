#include "LinuxWindow.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_x11.h>

namespace Core{
   // NOTE: spec can be a default param (see header file), but cannot be redefined here
   LinuxWindow::LinuxWindow(const WindowSpec& spec )
      : Window { spec }
   {
      m_linuxImplementationDetails.m_XDisplay = XOpenDisplay(NULL);

      // TODO: More flexible way to choose options (i.e. if these aren't available)
      int glxfbAttributeList[] = {GLX_DOUBLEBUFFER, True,
                                 GLX_RED_SIZE, 8,
                                 GLX_GREEN_SIZE, 8,
                                 GLX_BLUE_SIZE, 8,
                                 GLX_ALPHA_SIZE, 8,
                                 GLX_X_RENDERABLE, True, // TODO: Does this matter?
                                 GLX_STENCIL_SIZE, 8,
                                 GLX_DEPTH_SIZE, 24,
                                 None};
      int numElements;
      GLXFBConfig* glxfbConfig = glXChooseFBConfig(m_linuxImplementationDetails.m_XDisplay, 
            XDefaultScreen(m_linuxImplementationDetails.m_XDisplay), 
            glxfbAttributeList, &numElements);
      // TODO: is throwin an error best way for this? init func returning bool better? idk
      if(!glxfbConfig)
         throw std::runtime_error("Error choosing framebuffer config for GLX in LinuxWindow constructor.");
      XVisualInfo* vInfo = glXGetVisualFromFBConfig(m_linuxImplementationDetails.m_XDisplay, 
            glxfbConfig[0]);

      XSetWindowAttributes setWindowAttributes { };
      // TODO: I think comment in IMGUI x11 implementation, but these are all needed for IMGUI to work, 
      // however, there might be a way for imgui impl to just subscribe to events it needs on its own,
      // and thus we can only define ones we want here? TBD
      setWindowAttributes.event_mask = ButtonPressMask | ButtonReleaseMask |
                                       KeyPressMask | KeyReleaseMask |
                                       StructureNotifyMask | PointerMotionMask | 
                                       EnterWindowMask | LeaveWindowMask;
      setWindowAttributes.colormap = XCreateColormap(m_linuxImplementationDetails.m_XDisplay, 
            RootWindow(m_linuxImplementationDetails.m_XDisplay, vInfo->screen), 
            vInfo->visual, AllocNone);

      m_linuxImplementationDetails.m_XWindow = XCreateWindow(m_linuxImplementationDetails.m_XDisplay, 
            RootWindow(m_linuxImplementationDetails.m_XDisplay, vInfo->screen), 
            spec.x, spec.y, spec.width, spec.height, 0, vInfo->depth, InputOutput, 
            vInfo->visual, CWEventMask | CWColormap, &setWindowAttributes);

      XMapWindow(m_linuxImplementationDetails.m_XDisplay, m_linuxImplementationDetails.m_XWindow);

      m_glxWindow = glXCreateWindow(m_linuxImplementationDetails.m_XDisplay, glxfbConfig[0], 
            m_linuxImplementationDetails.m_XWindow, NULL);
      m_glxContext = glXCreateNewContext(m_linuxImplementationDetails.m_XDisplay, 
            glxfbConfig[0], GLX_RGBA_TYPE, NULL, True);
      if(!glXMakeContextCurrent(m_linuxImplementationDetails.m_XDisplay, 
               m_glxWindow, m_glxWindow, m_glxContext))
         throw std::runtime_error("Error making m_glxContext current in LinuxWindow constructor");

      // TODO: Where should we put ImGui stuff?
   }

   LinuxWindow::~LinuxWindow(){
      if(m_linuxImplementationDetails.m_XDisplay)
         XCloseDisplay(m_linuxImplementationDetails.m_XDisplay);
      // TODO: Anything else? Window should auto close?
   }



   void LinuxWindow::update(){
      glXSwapBuffers(m_linuxImplementationDetails.m_XDisplay, m_glxWindow);
   }

   void LinuxWindow::prepareNewFrame(){
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void LinuxWindow::pollEvents(){
      // TODO: Decouple from ImGui
      while(XPending(m_linuxImplementationDetails.m_XDisplay) > 0){
         XEvent event {};
         XNextEvent(m_linuxImplementationDetails.m_XDisplay, &event);
         // TODO: The funky part here is that we need to be able to pass imgui the XEvent
         // Does it matter that we pass it here? Will layers be able to check if imgui wants it? What if imgui isn't init here?
         // Do we need to pass underlying XEvent as part of the Core::Events::Event? How would this work for other OS's which don't need it passed in automatically?
         // For now we will just pass the event here and the layer can either choose to listen if io wants it or not. 
         // NOTE: Possible issue: ImGui uses this event but a layer higher up which doesn't care about imgui handles the event. In this case both the layer 
         // and imgui 'handle' or use this event which is probably not what we want
         ImGui_ImplX11_ProcessEvent(&event);
         std::vector<Events::Event> events = translateXEventToFigEvents(&event);

         for(Events::Event& e : events){
            if(e.type == Events::Type::KEY_PRESS || e.type == Events::Type::KEY_RELEASE)
               updateKeyState(e.keyEvent);

            if(e.type == Events::Type::MOUSE_MOVEMENT){
               updatePointerPosition(e.mouseMotionEvent.x, e.mouseMotionEvent.y);
            }

            raiseEvent(e);
         }
      }
   }

   void LinuxWindow::raiseEvent(Core::Events::Event& event){
      if(m_windowSpec.eventCallback)
         m_windowSpec.eventCallback(event);
   }

   // PRIVATE METHODS
   std::vector<Events::Event> LinuxWindow::translateXEventToFigEvents(XEvent* event){
      std::vector<Events::Event> events;
      Events::Event figEvent;

      switch(event->type){
         case(KeyPress):
         case(KeyRelease):
            figEvent.type = event->type == KeyPress ? Events::KEY_PRESS : Events::KEY_RELEASE;
            figEvent.keyEvent.key = translateXKeyToFigKey(&event->xkey);
            events.push_back(figEvent);
            break;
         case(ButtonPress):
         case(ButtonRelease):
            figEvent.type = event->type == ButtonPress ? Events::BUTTON_PRESS : Events::BUTTON_RELEASE;
            events.push_back(figEvent);
            break;
         case(ConfigureNotify):
            if(event->xconfigure.x != m_windowSpec.x || event->xconfigure.y != m_windowSpec.y){
               figEvent.type = Events::WINDOW_MOVE;
               updatePosition( event->xconfigure.x, event->xconfigure.y ); // TODO: this should be here right?
               events.push_back(figEvent);
            }

            if(event->xconfigure.width != m_windowSpec.width || event->xconfigure.height != m_windowSpec.height){
               figEvent.type = Events::WINDOW_RESIZE;
               updateSize( event->xconfigure.width, event->xconfigure.height ); // TODO: this should be here right?
               events.push_back(figEvent);
            }

            if(events.size() == 0){ // TODO: Temp for if we geta ConfigureNotify event not handled by above two if statements
               figEvent.type = Events::WINDOW_OTHER;
               events.push_back(figEvent);
            }
            break;
         case(MotionNotify):
            figEvent.type = Events::MOUSE_MOVEMENT;
            // TODO: This was implemented in about 5 seconds, 
            // look at docs for X system to make sure that 
            // this right and only needs to go here
            figEvent.mouseMotionEvent.x = event->xmotion.x;
            figEvent.mouseMotionEvent.y = event->xmotion.y;
            events.push_back(figEvent);
            break;
         default: 
            figEvent.type = Events::NULL_EVENT;
            events.push_back(figEvent);
            break;
      }

      return events;
   }

   void (*LinuxWindow::getProcAddress(char* procName)) () {
      /*
      // TODO: Is this fine here in the LinuxWIndow? What if we don't use openGL?
      // Query extensions list to ensure it is supported
      const char* extensionsList = glXQueryExtensionsString(m_XDisplay, XDefaultScreen(m_XDisplay));
      char* extensionsCopy = new char[strlen(extensionsList) + 1]; // TODO: IMPORTANT!!! WHEN THIS FUNC GETS REFACTORED (and will probably best to store this as a member var of the window class?) move the creation and delete[] (if using c strings) into constructor and destructor!!!!
      strcpy(extensionsCopy, extensionsList);
      if(strcmp(extensionsCopy, "") == 0){ // empty string
         delete[] extensionsCopy;
         return nullptr;
      }
      char* token = strtok(extensionsCopy, " ");
      while(token){
         puts(token);
         if(strcmp(token, procName) == 0){ // procName is supported
            delete[] extensionsCopy;
            return glXGetProcAddress((GLubyte*) procName);
         }
         token = strtok(NULL, " ");
      }

      delete[] extensionsCopy;
      return nullptr;
      */ 
      // The above commented out code parses and extension list to see if the procName is supported in the available extensions. Could be useful in the future

      return glXGetProcAddress((GLubyte*) procName);
   }

   Events::Key LinuxWindow::translateXKeyToFigKey(XKeyEvent* keyEvent){
      KeySym keySym = XLookupKeysym(keyEvent, 0);

      using namespace Events;
      switch(keySym){
         case(XK_Escape): return Key::ESCAPE;
         case(XK_Shift_L): return Key::SHIFT_L;
         case(XK_space): return Key::SPACE;
         case(XK_Tab): return Key::TAB;
         case(XK_w):
         case(XK_W): return Key::W;
         case(XK_a):
         case(XK_A): return Key::A;
         case(XK_s):
         case(XK_S): return Key::S;
         case(XK_d):
         case(XK_D): return Key::D;
         default: return Key::OTHER;
      }
   }

   void LinuxWindow::updateKeyState(const Events::KeyEvent& keyEvent){
      if(keyEvent.type == Events::KEY_PRESS){
         m_keyState[static_cast<std::size_t>(keyEvent.key)] = true;
      } else{
         m_keyState[static_cast<std::size_t>(keyEvent.key)] = false;
      }
   }

   void LinuxWindow::updatePointerPosition(float x, float y){
      m_pointerPosition.x = x;
      m_pointerPosition.y = y;
   }

   // TODO: If more complicated and need multiple different contexts, may need to move imgui from being a window controlled thing

   void LinuxWindow::internalSetPointerPosition(Window::PointerPosition newPos){
      XWarpPointer(m_linuxImplementationDetails.m_XDisplay, None, 
                   m_linuxImplementationDetails.m_XWindow, 
                   0, 0, 0, 0,
                   newPos.x, newPos.y);
   }

} // namespace Core
