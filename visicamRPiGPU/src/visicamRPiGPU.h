//    This file is part of visicamRPiGPU. (https://github.com/FroChr123/visicamRPiGPU)
//    Please note the additional licenses and references to other projects in the file LICENSE-ADDITIONAL.
//
//    visicamRPiGPU is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    visicamRPiGPU is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with visicamRPiGPU.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "visicamRPiGPU-settings.h"

#include <bcm_host.h>
#include <IL/OMX_Broadcom.h>
#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Types.h>
#include <interface/vcos/vcos.h>

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>

/* #####################################
OMX
##################################### */

// Custom ID numbers for easy identification of components
#define OMX_COMPONENT_CAMERA_ID                 1
#define OMX_COMPONENT_NULL_SINK_ID              2
#define OMX_COMPONENT_EGL_RENDER_ID             3
#define OMX_COMPONENT_IMAGE_ENCODE_ID           4

// Defines for component names
#define OMX_COMPONENT_CAMERA_NAME               "OMX.broadcom.camera"
#define OMX_COMPONENT_NULL_SINK_NAME            "OMX.broadcom.null_sink"
#define OMX_COMPONENT_EGL_RENDER_NAME           "OMX.broadcom.egl_render"
#define OMX_COMPONENT_IMAGE_ENCODE_NAME         "OMX.broadcom.image_encode"

// Custom flags for VCOS event identification
// Only define events, which are required by this application
#define VCOS_EVENT_ERROR                        0x001
#define VCOS_EVENT_PORT_ENABLE                  0x002
#define VCOS_EVENT_PORT_DISABLE                 0x004
#define VCOS_EVENT_STATE_SET                    0x008
#define VCOS_EVENT_PARAM_OR_CONFIG_CHANGED      0x010
#define VCOS_EVENT_BUFFER_FLAG_SET              0x020
#define VCOS_EVENT_COMMAND_FLUSH                0x040
#define VCOS_EVENT_FILL_BUFFER_DONE             0x080
#define VCOS_EVENT_EMPTY_BUFFER_DONE            0x100

// Port component definitions
#define OMX_PORT_CAMERA_PREVIEW_VIDEO_OUTPUT    70
#define OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT       71
#define OMX_PORT_CAMERA_STILL_IMAGE_OUTPUT      72
#define OMX_PORT_CAMERA_CLOCK_INPUT             73
#define OMX_PORT_EGL_RENDER_VIDEO_INPUT         220
#define OMX_PORT_EGL_RENDER_VIDEO_OUTPUT        221
#define OMX_PORT_NULL_SINK_VIDEO_INPUT          240
#define OMX_PORT_NULL_SINK_IMAGE_INPUT          241
#define OMX_PORT_NULL_SINK_AUDIO_INPUT          242
#define OMX_PORT_IMAGE_ENCODE_IMAGE_INPUT       340
#define OMX_PORT_IMAGE_ENCODE_IMAGE_OUTPUT      341

// OMX component struct definition
typedef struct
{
    OMX_U32             id;
    OMX_STRING          name;
    OMX_HANDLETYPE      handle;
    VCOS_EVENT_FLAGS_T  vcos_flags;
} OMXComponent;

// OMX functions
template<typename T> void OMXinitializeStruct(T* OMXstruct);

OMX_ERRORTYPE OMXEventHandler(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_EVENTTYPE eEvent, OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2, OMX_IN OMX_PTR pEventData);
OMX_ERRORTYPE OMXEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
OMX_ERRORTYPE OMXFillBufferDone(OMX_OUT OMX_HANDLETYPE hComponent, OMX_OUT OMX_PTR pAppData, OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);

void VCOSsendEvent(OMXComponent* OMXcomponent, VCOS_UNSIGNED sendEvents);
void VCOSwaitEvent(OMXComponent* OMXcomponent, VCOS_UNSIGNED waitEvents);

void OMXInitializeComponent(OMXComponent* component, OMX_U32 id, OMX_STRING name);
void OMXSetStateComponent(OMXComponent* component, OMX_STATETYPE state);
void OMXPortEnableDisableComponent(OMXComponent* component, OMX_U32 port, bool enable);

void OMXSetupCamera(OMXComponent* component, int cameraWidth, int cameraHeight);
void OMXStartCameraCapturing(OMXComponent* component, int port);
void OMXSetupEGLRender(OMXComponent* component, EGLImageKHR* eglImage, OMX_BUFFERHEADERTYPE** outputBufferHeader);
void OMXSetupImageEncodeSettings(OMXComponent* component, int cameraWidth, int cameraHeight);
void OMXSetupImageEncodeAllocate(OMXComponent* component, GLubyte* inputBuffer, OMX_BUFFERHEADERTYPE** inputBufferHeader, OMX_BUFFERHEADERTYPE** outputBufferHeader, int cameraWidth, int cameraHeight);

/* #####################################
CUSTOM FUNCTIONS
##################################### */

// Check if file exists
bool fileExists(std::string path);

// Catch kill signals, send SIGKILL to self (might not stop otherwise)
void signalHandler(int signal);

/* #####################################
MAIN APP
##################################### */

class visicamRPiGPU : public ofBaseApp
{
    public:
        // Functions used by OpenFramworks, all possible functions are not needed
        void setup();
        void update();
        void draw();

        // Input arguments for main
        int width;
        int height;
        int refreshTimeSeconds;
        int parentCheckPid;
        std::string homographyInputPath;
        std::string processedOutputPath;
        std::string capturedOutputPath;

        // OMX variables: Camera
        OMXComponent OMXcameraComponent;

        // OMX variables: Null sink
        OMXComponent OMXnullSinkComponent;

        // OMX variables: EGL render
        OMXComponent OMXeglRenderComponent;
        OMX_BUFFERHEADERTYPE* OMXeglRenderOutputBufferHeader;
        EGLImageKHR eglImage;
        ofFbo eglRenderOutputFbo;

        // OMX variables: Image encoder
        OMXComponent OMXimageEncodeComponent;
        GLubyte* OMXscreenPixelBuffer;
        OMX_BUFFERHEADERTYPE* OMXimageEncodeInputBufferHeader;
        OMX_BUFFERHEADERTYPE* OMXimageEncodeOutputBufferHeader;

        // Other variables
        struct timespec lastRefreshTimespec;
        struct timespec currentTimespec;
        bool firstForcedRefresh;
        bool outputCapturedOriginalImage;
        float homographyInputMatrixValues[9];
        ofMatrix4x4 homographyInputMatrix;
        ofFbo defaultRenderOutputFbo;
};