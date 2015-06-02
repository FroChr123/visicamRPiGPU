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
MISC DEFINES
##################################### */
#define FIRST_FORCED_REFRESH_SECONDS            3

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

// JPEG settings
#define OMX_JPEG_QUALITY                        100     // Allowed values: 0 to 100
#define OMX_JPEG_EXIF_ENABLE                    OMX_FALSE
#define OMX_JPEG_IJG_ENABLE                     OMX_TRUE
#define OMX_JPEG_THUMBNAIL_ENABLE               OMX_FALSE
#define OMX_JPEG_THUMBNAIL_PREVIEW_ENABLE       OMX_FALSE
#define OMX_JPEG_THUMBNAIL_WIDTH                0
#define OMX_JPEG_THUMBNAIL_HEIGHT               0

// Camera settings
#define OMX_CAM_SHARPNESS                       0       // Allowed values: -100 to 100
#define OMX_CAM_CONTRAST                        0       // Allowed values: -100 to 100
#define OMX_CAM_BRIGHTNESS                      50      // Allowed values: 0 to 100
#define OMX_CAM_SATURATION                      0       // Allowed values: -100 to 100
#define OMX_CAM_ISO                             100     // Allowed values: 100 to 800
#define OMX_CAM_EXPOSURE_COMPENSATION           0       // Allowed values: -24 to 24
#define OMX_CAM_ROTATION                        0       // Allowed values: 0, 90, 180, 270
#define OMX_CAM_COLOR_U                         128     // Allowed values: 0 to 255
#define OMX_CAM_COLOR_V                         128     // Allowed values: 0 to 255
#define OMX_CAM_ROI_TOP                         0       // Allowed values: 0 to 100
#define OMX_CAM_ROI_LEFT                        0       // Allowed values: 0 to 100
#define OMX_CAM_ROI_WIDTH                       100     // Allowed values: 0 to 100
#define OMX_CAM_ROI_HEIGHT                      100     // Allowed values: 0 to 100
#define OMX_CAM_FRAMERATE                       30
#define OMX_CAM_PREVIEW_WIDTH                   640
#define OMX_CAM_PREVIEW_HEIGHT                  480
#define OMX_CAM_SHUTTER_SPEED_AUTO              OMX_TRUE
#define OMX_CAM_SHUTTER_SPEED                   125000
#define OMX_CAM_ISO_AUTO                        OMX_TRUE
#define OMX_CAM_EXPOSURE                        OMX_ExposureControlAuto
#define OMX_CAM_MIRROR                          OMX_MirrorNone
#define OMX_CAM_COLOR_ENABLE                    OMX_FALSE
#define OMX_CAM_NOISE_REDUCTION                 OMX_TRUE
#define OMX_CAM_FRAME_STABILIZATION             OMX_FALSE
#define OMX_CAM_METERING                        OMX_MeteringModeAverage
#define OMX_CAM_WHITE_BALANCE                   OMX_WhiteBalControlAuto
#define OMX_CAM_WHITE_BALANCE_RED_GAIN          1000
#define OMX_CAM_WHITE_BALANCE_BLUE_GAIN         1000
#define OMX_CAM_IMAGE_FILTER                    OMX_ImageFilterNone
#define OMX_CAM_DRC                             OMX_DynRangeExpOff

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