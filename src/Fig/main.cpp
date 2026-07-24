#include "Application.h"
// #include "CameraRotateLayer.h"
// #include "GuiTestLayer.h"
#include "Core_Utils/Log.h"
#include "NewGraphicsTestLayer.h"
#include "OverlayLayer.h"

int main(){
   FIG_INITIALIZE_LOGGING
   Core::ApplicationSpec appSpec;
   appSpec.title = "Josiah's Fig";

   Core::Application app { appSpec };
   app.pushLayer<OverlayLayer>();
   // FOR SOME REASON, THE PUSHLAYER CALLS AN ASSETMANAGER DESTRUCTOR 
   // WHICH INVLAIDATES EVERYTHING
   app.pushLayer<NewGraphicsTestLayer>(app.getRenderDevice(), app.getAssetManager());
   // app.pushLayer<GuiTestLayer>();
   // app.pushLayer<CameraRotateLayer>();
   app.run();
   FIG_LOG_OUTPUT_LOG_INFO
}
