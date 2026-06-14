#include "Application.h"
// #include "CameraRotateLayer.h"
#include "GuiTestLayer.h"
#include "Log.h"
#include "OverlayLayer.h"

int main(){
   FIG_INITIALIZE_LOGGING
   Core::ApplicationSpec appSpec;
   appSpec.title = "Josiah's Fig";

   Core::Application app { appSpec };
   app.pushLayer<OverlayLayer>();
   app.pushLayer<GuiTestLayer>();
   // app.pushLayer<CameraRotateLayer>();
   app.run();
   FIG_LOG_OUTPUT_LOG_INFO
}
