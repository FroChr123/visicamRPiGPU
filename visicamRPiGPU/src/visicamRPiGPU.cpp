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

#include "visicamRPiGPU.h"

/* #####################################
OMX
##################################### */

// OMX function event handler for all components
// Only include required events for this application
OMX_ERRORTYPE OMXEventHandler(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_EVENTTYPE eEvent, OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2, OMX_IN OMX_PTR pEventData)
{
    OMXComponent* component = (OMXComponent*)(pAppData);

    switch (eEvent)
    {
        case OMX_EventCmdComplete:
        {
            switch (nData1)
            {
                case OMX_CommandStateSet:
                    VCOSsendEvent(component, VCOS_EVENT_STATE_SET);
                    break;
                case OMX_CommandPortEnable:
                    VCOSsendEvent(component, VCOS_EVENT_PORT_ENABLE);
                    break;
                case OMX_CommandPortDisable:
                    VCOSsendEvent(component, VCOS_EVENT_PORT_DISABLE);
                    break;
                case OMX_CommandFlush:
                    VCOSsendEvent(component, VCOS_EVENT_COMMAND_FLUSH);
                    break;
                default:
                    break;
            }
            break;
        }
        case OMX_EventError:
            VCOSsendEvent(component, VCOS_EVENT_ERROR);
            break;
        case OMX_EventParamOrConfigChanged:
            VCOSsendEvent(component, VCOS_EVENT_PARAM_OR_CONFIG_CHANGED);
            break;
        case OMX_EventBufferFlag:
            VCOSsendEvent(component, VCOS_EVENT_BUFFER_FLAG_SET);
            break;
        default:
            break;
    }

    return OMX_ErrorNone;
}

// OMX function empty buffer handler for all components
OMX_ERRORTYPE OMXEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMXComponent* component = (OMXComponent*)(pAppData);
    VCOSsendEvent(component, VCOS_EVENT_EMPTY_BUFFER_DONE);
    return OMX_ErrorNone;
}

// OMX function fill buffer handler for all components
OMX_ERRORTYPE OMXFillBufferDone(OMX_OUT OMX_HANDLETYPE hComponent, OMX_OUT OMX_PTR pAppData, OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMXComponent* component = (OMXComponent*)(pAppData);
    VCOSsendEvent(component, VCOS_EVENT_FILL_BUFFER_DONE);
    return OMX_ErrorNone;
}

// OMX function which uses VCOS to send events for components
void VCOSsendEvent(OMXComponent* component, VCOS_UNSIGNED sendEvents)
{
    vcos_event_flags_set(&component->vcos_flags, sendEvents, VCOS_OR);
}

// OMX function which uses VCOS to wait for events for components
void VCOSwaitEvent(OMXComponent* component, VCOS_UNSIGNED waitEvents)
{
    VCOS_UNSIGNED VCOSresult;

    // Wait until any of waitEvents or VCOS_EVENT_ERROR is contained in vcos_flag
    if (vcos_event_flags_get(&component->vcos_flags, waitEvents | VCOS_EVENT_ERROR, VCOS_OR_CONSUME, VCOS_SUSPEND, &VCOSresult))
    {
        printf("OMX Error: VCOS wait event - EXITING APPLICATION\n");
        std::exit(1);
    }

    // If result contains error, exit application
    if (VCOSresult & VCOS_EVENT_ERROR)
    {
        printf("OMX Error: VCOS wait event error - EXITING APPLICATION\n");
        std::exit(1);
    }
}

// OMX function to initialize OMX structs correctly
template<typename T> void OMXinitializeStruct(T* OMXstruct)
{
    memset(OMXstruct, 0, sizeof(T));
    OMXstruct->nSize = sizeof(T);
    OMXstruct->nVersion.nVersion = OMX_VERSION;
    OMXstruct->nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXstruct->nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXstruct->nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXstruct->nVersion.s.nStep = OMX_VERSION_STEP;
}

// OMX function to initialize components correctly
void OMXInitializeComponent(OMXComponent* component, OMX_U32 id, const char* name)
{
    // Setup component: Set id and name
    component->id = id;
    component->name = (OMX_STRING)(name);

    // Setup component: VCOS flags
    if (vcos_event_flags_create(&component->vcos_flags, name))
    {
        printf("OMX Error: VCOS flags component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }

    // Setup component: Callbacks
    OMX_CALLBACKTYPE OMXcallbacks;
    OMXcallbacks.EventHandler = &OMXEventHandler;
    OMXcallbacks.FillBufferDone = &OMXFillBufferDone;
    OMXcallbacks.EmptyBufferDone = &OMXEmptyBufferDone;

    // Setup component: Register handle in OMX
    if (OMX_GetHandle(&component->handle, component->name, component, &OMXcallbacks))
    {
        printf("OMX Error: OMX get component %s handle - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }
}

// OMX function to set component state and optionally wait
void OMXSetStateComponent(OMXComponent* component, OMX_STATETYPE state)
{
    // Send command to change state
    if (OMX_SendCommand(component->handle, OMX_CommandStateSet, state, NULL))
    {
        printf("OMX Error: OMX set state on component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }
}

// OMX function to enable or disable component port and optionally wait
void OMXPortEnableDisableComponent(OMXComponent* component, OMX_U32 port, bool enable)
{
    // Send command to enable or disable component port
    if (OMX_SendCommand(component->handle, (enable ? OMX_CommandPortEnable : OMX_CommandPortDisable), port, NULL))
    {
        printf("OMX Error: OMX enable or disable port on component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }
}

// OMX function to setup camera correctly
// Component in state loaded and ports disabled
void OMXSetupCamera(OMXComponent* component, int cameraWidth, int cameraHeight)
{
    // Setup camera component: Check for correct component
    if (component->id != OMX_COMPONENT_CAMERA_ID)
    {
        printf("OMX Error: Setup camera called on wrong component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }

    // Setup camera component: Enable callback on configuration change OMX_IndexParamCameraDeviceNumber
    OMX_CONFIG_REQUESTCALLBACKTYPE OMXcameraCallbackConfigParamEnable;
    OMXinitializeStruct<OMX_CONFIG_REQUESTCALLBACKTYPE>(&OMXcameraCallbackConfigParamEnable);
    OMXcameraCallbackConfigParamEnable.nPortIndex = OMX_ALL;
    OMXcameraCallbackConfigParamEnable.bEnable = OMX_TRUE;
    OMXcameraCallbackConfigParamEnable.nIndex = OMX_IndexParamCameraDeviceNumber;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigRequestCallback, &OMXcameraCallbackConfigParamEnable))
    {
        printf("OMX Error: OMX enable config and parameter callback camera - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Set device id 0
    OMX_PARAM_U32TYPE OMXcameraParameterDevice;
    OMXinitializeStruct<OMX_PARAM_U32TYPE>(&OMXcameraParameterDevice);
    OMXcameraParameterDevice.nPortIndex = OMX_ALL;
    OMXcameraParameterDevice.nU32 = 0;

    if (OMX_SetParameter(component->handle, OMX_IndexParamCameraDeviceNumber, &OMXcameraParameterDevice))
    {
        printf("OMX Error: OMX set camera device id - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Blocking wait for setting camera device id
    VCOSwaitEvent(component, VCOS_EVENT_PARAM_OR_CONFIG_CHANGED);

    // Setup camera component: Sensor settings
    OMX_PARAM_SENSORMODETYPE OMXcameraSensor;
    OMXinitializeStruct<OMX_PARAM_SENSORMODETYPE>(&OMXcameraSensor);
    OMXcameraSensor.nPortIndex = OMX_ALL;
    OMXinitializeStruct<OMX_FRAMESIZETYPE>(&OMXcameraSensor.sFrameSize);
    OMXcameraSensor.sFrameSize.nPortIndex = OMX_ALL;

    if (OMX_GetParameter(component->handle, OMX_IndexParamCommonSensorMode, &OMXcameraSensor))
    {
        printf("OMX Error: OMX get camera sensor settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    OMXcameraSensor.bOneShot = OMX_FALSE;
    OMXcameraSensor.sFrameSize.nWidth = cameraWidth;
    OMXcameraSensor.sFrameSize.nHeight = cameraHeight;

    if (OMX_SetParameter(component->handle, OMX_IndexParamCommonSensorMode, &OMXcameraSensor))
    {
        printf("OMX Error: OMX set camera sensor settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Get port settings, real video port
    OMX_PARAM_PORTDEFINITIONTYPE OMXcameraPortRealVideo;
    OMXinitializeStruct<OMX_PARAM_PORTDEFINITIONTYPE>(&OMXcameraPortRealVideo);
    OMXcameraPortRealVideo.nPortIndex = OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT;

    if (OMX_GetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXcameraPortRealVideo))
    {
        printf("OMX Error: OMX get camera real video port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Set port settings, real video port
    OMXcameraPortRealVideo.format.video.nFrameWidth = cameraWidth;
    OMXcameraPortRealVideo.format.video.nFrameHeight = cameraHeight;
    OMXcameraPortRealVideo.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
    OMXcameraPortRealVideo.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    OMXcameraPortRealVideo.format.video.xFramerate = OMX_CAM_FRAMERATE << 16;
    OMXcameraPortRealVideo.format.video.nStride = cameraWidth;
    OMXcameraPortRealVideo.format.video.nStride = cameraWidth;

    if (OMX_SetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXcameraPortRealVideo))
    {
        printf("OMX Error: OMX set camera real video port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Get port settings, preview video port
    OMX_PARAM_PORTDEFINITIONTYPE OMXcameraPortPreview;
    OMXinitializeStruct<OMX_PARAM_PORTDEFINITIONTYPE>(&OMXcameraPortPreview);
    OMXcameraPortPreview.nPortIndex = OMX_PORT_CAMERA_PREVIEW_VIDEO_OUTPUT;

    if (OMX_GetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXcameraPortPreview))
    {
        printf("OMX Error: OMX get camera preview video port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Set port settings, preview video port
    // Preview video port needs to be enabled because it is used for automatic camera adjustments
    // Output is just tunneled to null_sink
    OMXcameraPortPreview.format.video.nFrameWidth = OMX_CAM_PREVIEW_WIDTH;
    OMXcameraPortPreview.format.video.nFrameHeight = OMX_CAM_PREVIEW_HEIGHT;
    OMXcameraPortPreview.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
    OMXcameraPortPreview.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    OMXcameraPortPreview.format.video.xFramerate = OMX_CAM_FRAMERATE << 16;
    OMXcameraPortPreview.format.video.nStride = OMX_CAM_PREVIEW_WIDTH;

    if (OMX_SetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXcameraPortPreview))
    {
        printf("OMX Error: OMX set camera preview video port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Sharpness
    OMX_CONFIG_SHARPNESSTYPE OMXcameraSharpness;
    OMXinitializeStruct<OMX_CONFIG_SHARPNESSTYPE>(&OMXcameraSharpness);
    OMXcameraSharpness.nPortIndex = OMX_ALL;
    OMXcameraSharpness.nSharpness = OMX_CAM_SHARPNESS;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonSharpness, &OMXcameraSharpness))
    {
        printf("OMX Error: OMX set camera setting Sharpness - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Contrast
    OMX_CONFIG_CONTRASTTYPE OMXcameraContrast;
    OMXinitializeStruct<OMX_CONFIG_CONTRASTTYPE>(&OMXcameraContrast);
    OMXcameraContrast.nPortIndex = OMX_ALL;
    OMXcameraContrast.nContrast = OMX_CAM_CONTRAST;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonContrast, &OMXcameraContrast))
    {
        printf("OMX Error: OMX set camera setting Contrast - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Saturation
    OMX_CONFIG_SATURATIONTYPE OMXcameraSaturation;
    OMXinitializeStruct<OMX_CONFIG_SATURATIONTYPE>(&OMXcameraSaturation);
    OMXcameraSaturation.nPortIndex = OMX_ALL;
    OMXcameraSaturation.nSaturation = OMX_CAM_SATURATION;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonSaturation, &OMXcameraSaturation))
    {
        printf("OMX Error: OMX set camera setting Saturation - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Brightness
    OMX_CONFIG_BRIGHTNESSTYPE OMXcameraBrightness;
    OMXinitializeStruct<OMX_CONFIG_BRIGHTNESSTYPE>(&OMXcameraBrightness);
    OMXcameraBrightness.nPortIndex = OMX_ALL;
    OMXcameraBrightness.nBrightness = OMX_CAM_BRIGHTNESS;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonBrightness, &OMXcameraBrightness))
    {
        printf("OMX Error: OMX set camera setting Brightness - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Exposure value
    OMX_CONFIG_EXPOSUREVALUETYPE OMXcameraExposureValue;
    OMXinitializeStruct<OMX_CONFIG_EXPOSUREVALUETYPE>(&OMXcameraExposureValue);
    OMXcameraExposureValue.nPortIndex = OMX_ALL;
    OMXcameraExposureValue.eMetering = OMX_CAM_METERING;
    OMXcameraExposureValue.xEVCompensation = (OMX_CAM_EXPOSURE_COMPENSATION << 16) / 6;
    OMXcameraExposureValue.nShutterSpeedMsec = OMX_CAM_SHUTTER_SPEED;
    OMXcameraExposureValue.bAutoShutterSpeed = OMX_CAM_SHUTTER_SPEED_AUTO;
    OMXcameraExposureValue.nSensitivity = OMX_CAM_ISO;
    OMXcameraExposureValue.bAutoSensitivity = OMX_CAM_ISO_AUTO;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonExposureValue, &OMXcameraExposureValue))
    {
        printf("OMX Error: OMX set camera setting Exposure value - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Exposure control
    OMX_CONFIG_EXPOSURECONTROLTYPE OMXcameraExposureControl;
    OMXinitializeStruct<OMX_CONFIG_EXPOSURECONTROLTYPE>(&OMXcameraExposureControl);
    OMXcameraExposureControl.nPortIndex = OMX_ALL;
    OMXcameraExposureControl.eExposureControl = OMX_CAM_EXPOSURE;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonExposure, &OMXcameraExposureControl))
    {
        printf("OMX Error: OMX set camera setting Exposure control - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Frame stabilisation
    OMX_CONFIG_FRAMESTABTYPE OMXcameraFrameStabilisation;
    OMXinitializeStruct<OMX_CONFIG_FRAMESTABTYPE>(&OMXcameraFrameStabilisation);
    OMXcameraFrameStabilisation.nPortIndex = OMX_ALL;
    OMXcameraFrameStabilisation.bStab = OMX_CAM_FRAME_STABILIZATION;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonFrameStabilisation, &OMXcameraFrameStabilisation))
    {
        printf("OMX Error: OMX set camera setting Frame stabilisation - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: White balance
    OMX_CONFIG_WHITEBALCONTROLTYPE OMXcameraWhiteBalance;
    OMXinitializeStruct<OMX_CONFIG_WHITEBALCONTROLTYPE>(&OMXcameraWhiteBalance);
    OMXcameraWhiteBalance.nPortIndex = OMX_ALL;
    OMXcameraWhiteBalance.eWhiteBalControl = OMX_CAM_WHITE_BALANCE;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonWhiteBalance, &OMXcameraWhiteBalance))
    {
        printf("OMX Error: OMX set camera setting White balance - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: White balance gains (if white balance is set to off)
    if (!OMX_CAM_WHITE_BALANCE)
    {
        OMX_CONFIG_CUSTOMAWBGAINSTYPE OMXcameraWhiteBalanceGains;
        OMXinitializeStruct<OMX_CONFIG_CUSTOMAWBGAINSTYPE>(&OMXcameraWhiteBalanceGains);
        OMXcameraWhiteBalanceGains.xGainR = (OMX_CAM_WHITE_BALANCE_RED_GAIN << 16) / 1000;
        OMXcameraWhiteBalanceGains.xGainB = (OMX_CAM_WHITE_BALANCE_BLUE_GAIN << 16) / 1000;

        if (OMX_SetConfig(component->handle, OMX_IndexConfigCustomAwbGains, &OMXcameraWhiteBalanceGains))
        {
            printf("OMX Error: OMX set camera setting White balance gains - EXITING APPLICATION\n");
            std::exit(1);
        }
    }

    // Setup camera component: Image filter
    OMX_CONFIG_IMAGEFILTERTYPE OMXcameraImageFilter;
    OMXinitializeStruct<OMX_CONFIG_IMAGEFILTERTYPE>(&OMXcameraImageFilter);
    OMXcameraImageFilter.nPortIndex = OMX_ALL;
    OMXcameraImageFilter.eImageFilter = OMX_CAM_IMAGE_FILTER;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonImageFilter, &OMXcameraImageFilter))
    {
        printf("OMX Error: OMX set camera setting Image filter - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Mirror
    OMX_CONFIG_MIRRORTYPE OMXcameraMirror;
    OMXinitializeStruct<OMX_CONFIG_MIRRORTYPE>(&OMXcameraMirror);
    OMXcameraMirror.nPortIndex = OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT;
    OMXcameraMirror.eMirror = OMX_CAM_MIRROR;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonMirror, &OMXcameraMirror))
    {
        printf("OMX Error: OMX set camera setting Mirror - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Rotation
    OMX_CONFIG_ROTATIONTYPE OMXcameraRotation;
    OMXinitializeStruct<OMX_CONFIG_ROTATIONTYPE>(&OMXcameraRotation);
    OMXcameraRotation.nPortIndex = OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT;
    OMXcameraRotation.nRotation = OMX_CAM_ROTATION;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonRotate, &OMXcameraRotation))
    {
        printf("OMX Error: OMX set camera setting Rotation - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Color enhancement
    OMX_CONFIG_COLORENHANCEMENTTYPE OMXcameraColorEnhancement;
    OMXinitializeStruct<OMX_CONFIG_COLORENHANCEMENTTYPE>(&OMXcameraColorEnhancement);
    OMXcameraColorEnhancement.nPortIndex = OMX_ALL;
    OMXcameraColorEnhancement.bColorEnhancement = OMX_CAM_COLOR_ENABLE;
    OMXcameraColorEnhancement.nCustomizedU = OMX_CAM_COLOR_U;
    OMXcameraColorEnhancement.nCustomizedV = OMX_CAM_COLOR_V;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigCommonColorEnhancement, &OMXcameraColorEnhancement))
    {
        printf("OMX Error: OMX set camera setting Color enhancement - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: Denoise
    OMX_CONFIG_BOOLEANTYPE OMXcameraDenoise;
    OMXinitializeStruct<OMX_CONFIG_BOOLEANTYPE>(&OMXcameraDenoise);
    OMXcameraDenoise.bEnabled = OMX_CAM_NOISE_REDUCTION;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigStillColourDenoiseEnable, &OMXcameraDenoise))
    {
        printf("OMX Error: OMX set camera setting Denoise - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: ROI
    OMX_CONFIG_INPUTCROPTYPE OMXcameraRoi;
    OMXinitializeStruct<OMX_CONFIG_INPUTCROPTYPE>(&OMXcameraRoi);
    OMXcameraRoi.nPortIndex = OMX_ALL;
    OMXcameraRoi.xLeft = (OMX_CAM_ROI_LEFT << 16) / 100;
    OMXcameraRoi.xTop = (OMX_CAM_ROI_TOP << 16) / 100;
    OMXcameraRoi.xWidth = (OMX_CAM_ROI_WIDTH << 16) / 100;
    OMXcameraRoi.xHeight = (OMX_CAM_ROI_HEIGHT << 16) / 100;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigInputCropPercentages, &OMXcameraRoi))
    {
        printf("OMX Error: OMX set camera setting ROI - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup camera component: DRC
    OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE OMXcameraDrc;
    OMXinitializeStruct<OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE>(&OMXcameraDrc);
    OMXcameraDrc.eMode = OMX_CAM_DRC;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigDynamicRangeExpansion, &OMXcameraDrc))
    {
        printf("OMX Error: OMX set camera setting DRC - EXITING APPLICATION\n");
        std::exit(1);
    }
}

// OMX function to start camera capturing
// Component in state executing and ports enabled
void OMXStartCameraCapturing(OMXComponent* component, int port)
{
    // Setup camera component: Check for correct component
    if (component->id != OMX_COMPONENT_CAMERA_ID)
    {
        printf("OMX Error: Start camera called on wrong component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }

    // Setup camera component: DRC
    OMX_CONFIG_PORTBOOLEANTYPE OMXcameraCapturePort;
    OMXinitializeStruct<OMX_CONFIG_PORTBOOLEANTYPE>(&OMXcameraCapturePort);
    OMXcameraCapturePort.nPortIndex = port;
    OMXcameraCapturePort.bEnabled = OMX_TRUE;

    if (OMX_SetConfig(component->handle, OMX_IndexConfigPortCapturing, &OMXcameraCapturePort))
    {
        printf("OMX Error: OMX start camera capturing - EXITING APPLICATION\n");
        std::exit(1);
    }
}

// OMX function to setup egl render correctly
// Component in state idle and ports enabled
void OMXSetupEGLRender(OMXComponent* component, EGLImageKHR* eglImage, OMX_BUFFERHEADERTYPE** outputBufferHeader)
{
    // Setup egl render component: Check for correct component
    if (component->id != OMX_COMPONENT_EGL_RENDER_ID)
    {
        printf("OMX Error: Setup egl render called on wrong component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }

    // Setup egl render component: Set output buffer and output eglImage
    if (OMX_UseEGLImage(component->handle, outputBufferHeader, OMX_PORT_EGL_RENDER_VIDEO_OUTPUT, NULL, (*eglImage)))
    {
        printf("OMX Error: OMX setup egl render image - EXITING APPLICATION\n");
        std::exit(1);
    }
}

// OMX function to setup egl render correctly
// Component in state loading and ports disabled
void OMXSetupImageEncodeSettings(OMXComponent* component, int cameraWidth, int cameraHeight)
{
    // Setup image encode component settings: Check for correct component
    if (component->id != OMX_COMPONENT_IMAGE_ENCODE_ID)
    {
        printf("OMX Error: Setup image encode settings called on wrong component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }

    // Setup image encode component settings: Prepare structure for input port
    OMX_PARAM_PORTDEFINITIONTYPE OMXimageEncodeInputPort;
    OMXinitializeStruct<OMX_PARAM_PORTDEFINITIONTYPE>(&OMXimageEncodeInputPort);
    OMXimageEncodeInputPort.nPortIndex = OMX_PORT_IMAGE_ENCODE_IMAGE_INPUT;

    // Setup image encode component settings: Get current information for input port
    if (OMX_GetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXimageEncodeInputPort))
    {
        printf("OMX Error: OMX get image encode input port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component settings: Change settings for image encoding
    // Note: Only some image formats are supported, most of them seem to have strange implementations
    // resulting in wrong images. Here OMX_COLOR_Format32bitABGR8888 is used, which is not an official part of
    // OMX but a vendor specific addition. It does not act like ABGR8888 but instead it works like RGBA8888.
    // It does not match the input colors perfectly, this seems to be related to an implementation specific bug (closed source?)
    OMXimageEncodeInputPort.format.image.nFrameWidth = cameraWidth;
    OMXimageEncodeInputPort.format.image.nFrameHeight = cameraHeight;
    OMXimageEncodeInputPort.format.image.nStride = 0;
    OMXimageEncodeInputPort.format.image.nSliceHeight = cameraHeight;
    OMXimageEncodeInputPort.format.image.eCompressionFormat = OMX_IMAGE_CodingUnused;
    OMXimageEncodeInputPort.format.image.eColorFormat = OMX_COLOR_Format32bitABGR8888;

    // Setup image encode component settings: Send changed settings for input port
    if (OMX_SetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXimageEncodeInputPort))
    {
        printf("OMX Error: OMX set image encode input port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component settings: Prepare structure for output port
    OMX_PARAM_PORTDEFINITIONTYPE OMXimageEncodeOutputPort;
    OMXinitializeStruct<OMX_PARAM_PORTDEFINITIONTYPE>(&OMXimageEncodeOutputPort);
    OMXimageEncodeOutputPort.nPortIndex = OMX_PORT_IMAGE_ENCODE_IMAGE_OUTPUT;

    // Setup image encode component settings: Get current information for output port
    if (OMX_GetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXimageEncodeOutputPort))
    {
        printf("OMX Error: OMX get image encode output port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component settings: Change settings for image encoding
    OMXimageEncodeOutputPort.format.image.nFrameWidth = cameraWidth;
    OMXimageEncodeOutputPort.format.image.nFrameHeight = cameraHeight;
    OMXimageEncodeOutputPort.format.image.nStride = cameraWidth;
    OMXimageEncodeOutputPort.format.image.nSliceHeight = cameraHeight;
    OMXimageEncodeOutputPort.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
    OMXimageEncodeOutputPort.format.image.eColorFormat = OMX_COLOR_FormatUnused;

    // Setup image encode component settings: Send changed settings for output port
    if (OMX_SetParameter(component->handle, OMX_IndexParamPortDefinition, &OMXimageEncodeOutputPort))
    {
        printf("OMX Error: OMX set image encode output port settings - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component settings: JPEG quality
    OMX_IMAGE_PARAM_QFACTORTYPE OMXimageEncodeQuality;
    OMXinitializeStruct<OMX_IMAGE_PARAM_QFACTORTYPE>(&OMXimageEncodeQuality);
    OMXimageEncodeQuality.nPortIndex = OMX_PORT_IMAGE_ENCODE_IMAGE_OUTPUT;
    OMXimageEncodeQuality.nQFactor = OMX_JPEG_QUALITY;

    if (OMX_SetParameter(component->handle, OMX_IndexParamQFactor, &OMXimageEncodeQuality))
    {
        printf("OMX Error: OMX set image encode JPEG quality - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component settings: JPEG EXIF
    OMX_CONFIG_BOOLEANTYPE OMXimageEncodeExif;
    OMXinitializeStruct<OMX_CONFIG_BOOLEANTYPE>(&OMXimageEncodeExif);
    OMXimageEncodeExif.bEnabled = OMX_JPEG_EXIF_ENABLE;

    if (OMX_SetParameter(component->handle, OMX_IndexParamBrcmDisableEXIF, &OMXimageEncodeExif))
    {
        printf("OMX Error: OMX set image encode JPEG EXIF - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component settings: JPEG IJG
    OMX_PARAM_IJGSCALINGTYPE OMXimageEncodeIjg;
    OMXinitializeStruct<OMX_PARAM_IJGSCALINGTYPE>(&OMXimageEncodeIjg);
    OMXimageEncodeIjg.nPortIndex = OMX_PORT_IMAGE_ENCODE_IMAGE_OUTPUT;
    OMXimageEncodeIjg.bEnabled = OMX_JPEG_IJG_ENABLE;

    if (OMX_SetParameter(component->handle, OMX_IndexParamBrcmEnableIJGTableScaling, &OMXimageEncodeIjg))
    {
        printf("OMX Error: OMX set image encode JPEG IJG - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component settings: JPEG Thumbnail
    OMX_PARAM_BRCMTHUMBNAILTYPE OMXimageEncodeThumbnail;
    OMXinitializeStruct<OMX_PARAM_BRCMTHUMBNAILTYPE>(&OMXimageEncodeThumbnail);
    OMXimageEncodeThumbnail.bEnable = OMX_JPEG_THUMBNAIL_ENABLE;
    OMXimageEncodeThumbnail.bUsePreview = OMX_JPEG_THUMBNAIL_PREVIEW_ENABLE;
    OMXimageEncodeThumbnail.nWidth = OMX_JPEG_THUMBNAIL_WIDTH;
    OMXimageEncodeThumbnail.nHeight = OMX_JPEG_THUMBNAIL_HEIGHT;

    if (OMX_SetParameter(component->handle, OMX_IndexParamBrcmThumbnail, &OMXimageEncodeThumbnail))
    {
        printf("OMX Error: OMX set image encode JPEG thumbnail - EXITING APPLICATION\n");
        std::exit(1);
    }
}

// OMX function to setup egl render correctly
// Component in state idle and ports enabled
void OMXSetupImageEncodeAllocate(OMXComponent* component, GLubyte* inputBuffer, OMX_BUFFERHEADERTYPE** inputBufferHeader, OMX_BUFFERHEADERTYPE** outputBufferHeader, int cameraWidth, int cameraHeight)
{
    // Setup image encode component allocate: Check for correct component
    if (component->id != OMX_COMPONENT_IMAGE_ENCODE_ID)
    {
        printf("OMX Error: Setup image encode allocate called on wrong component %s - EXITING APPLICATION\n", component->name);
        std::exit(1);
    }

    // Setup image encode component allocate: Set allocated buffer for input port
    if (OMX_UseBuffer(component->handle, inputBufferHeader, OMX_PORT_IMAGE_ENCODE_IMAGE_INPUT, NULL, 4 * cameraWidth * cameraHeight, inputBuffer))
    {
        printf("OMX Error: OMX allocate input buffer image encode - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup image encode component allocate: Allocate output buffer for output port
    // Just allocate 2 * cameraWidth * cameraHeight bytes for output, JPEG performs compression of raw input bytes
    if (OMX_AllocateBuffer(component->handle, outputBufferHeader, OMX_PORT_IMAGE_ENCODE_IMAGE_OUTPUT, NULL, 2 * cameraWidth * cameraHeight))
    {
        printf("OMX Error: OMX allocate output buffer image encode - EXITING APPLICATION\n");
        std::exit(1);
    }
}

/* #####################################
CUSTOM FUNCTIONS
##################################### */

// Check if file exists
bool fileExists(std::string path)
{
    return (access(path.c_str(), F_OK) != -1);
}

/* #####################################
MAIN APP
##################################### */

void visicamRPiGPU::setup()
{
    // Settings
    ofSetFrameRate(30);
    ofBackground(0, 0, 0);
    ofSetColor(255);
    ofDisableAlphaBlending();
    ofDisableAntiAliasing();
    ofDisableBlendMode();
    ofDisableDepthTest();
    ofDisablePointSprites();
    ofDisableSmoothing();
    ofHideCursor();

    // Initialize last refresh timespec
    lastRefreshTimespec.tv_sec = 0;
    lastRefreshTimespec.tv_nsec = 0;

    // Initialize current timespec
    currentTimespec.tv_sec = 0;
    currentTimespec.tv_sec = 0;

    // Initialize output captured original image with false, will be done in each refresh
    outputCapturedOriginalImage = false;

    // Initialize with identity matrix
    homographyInputMatrixValues[0] = 1.0f; // Row 1
    homographyInputMatrixValues[3] = 0.0f;
    homographyInputMatrixValues[6] = 0.0f;
    homographyInputMatrixValues[1] = 0.0f; // Row 2
    homographyInputMatrixValues[4] = 1.0f;
    homographyInputMatrixValues[7] = 0.0f;
    homographyInputMatrixValues[2] = 0.0f; // Row 3
    homographyInputMatrixValues[5] = 0.0f;
    homographyInputMatrixValues[8] = 1.0f;

    // Need to covert homography matrix in openCV format to openGL format
    // Can ignore z-coordinate here, so just fill up with empty values for z
    homographyInputMatrix.set(homographyInputMatrixValues[0], homographyInputMatrixValues[3], 0.0f, homographyInputMatrixValues[6],
                              homographyInputMatrixValues[1], homographyInputMatrixValues[4], 0.0f, homographyInputMatrixValues[7],
                              0.0f,                           0.0f,                           0.0f,                           0.0f,
                              homographyInputMatrixValues[2], homographyInputMatrixValues[5], 0.0f, homographyInputMatrixValues[8]);

    // Allocate default render FBO
    defaultRenderOutputFbo.allocate(width, height, GL_RGBA);

    // Allocate buffer for screen pixels and empty buffer
    OMXscreenPixelBuffer = (GLubyte*)(malloc(4 * width * height));
    memset(OMXscreenPixelBuffer, 0, 4 * width * height * sizeof(GLubyte));

    // Initialize OMX main components
    bcm_host_init();

    if (OMX_Init())
    {
        printf("OMX Error: OMX init - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Initialize OMXcameraComponent: Initialize, set component id and name, set VCOS flags, register OMX handle
    // Disable all ports, wait for port disable
    OMXInitializeComponent(&OMXcameraComponent, OMX_COMPONENT_CAMERA_ID, OMX_COMPONENT_CAMERA_NAME);

    OMXPortEnableDisableComponent(&OMXcameraComponent, OMX_PORT_CAMERA_PREVIEW_VIDEO_OUTPUT, false);
    VCOSwaitEvent(&OMXcameraComponent, VCOS_EVENT_PORT_DISABLE);

    OMXPortEnableDisableComponent(&OMXcameraComponent, OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT, false);
    VCOSwaitEvent(&OMXcameraComponent, VCOS_EVENT_PORT_DISABLE);

    OMXPortEnableDisableComponent(&OMXcameraComponent, OMX_PORT_CAMERA_STILL_IMAGE_OUTPUT, false);
    VCOSwaitEvent(&OMXcameraComponent, VCOS_EVENT_PORT_DISABLE);

    OMXPortEnableDisableComponent(&OMXcameraComponent, OMX_PORT_CAMERA_CLOCK_INPUT, false);
    VCOSwaitEvent(&OMXcameraComponent, VCOS_EVENT_PORT_DISABLE);

    // Initialize OMXnullSinkComponent: Initialize, set component id and name, set VCOS flags, register OMX handle
    // Disable all ports, wait for port disable
    OMXInitializeComponent(&OMXnullSinkComponent, OMX_COMPONENT_NULL_SINK_ID, OMX_COMPONENT_NULL_SINK_NAME);

    OMXPortEnableDisableComponent(&OMXnullSinkComponent, OMX_PORT_NULL_SINK_VIDEO_INPUT, false);
    VCOSwaitEvent(&OMXnullSinkComponent, VCOS_EVENT_PORT_DISABLE);

    OMXPortEnableDisableComponent(&OMXnullSinkComponent, OMX_PORT_NULL_SINK_IMAGE_INPUT, false);
    VCOSwaitEvent(&OMXnullSinkComponent, VCOS_EVENT_PORT_DISABLE);

    OMXPortEnableDisableComponent(&OMXnullSinkComponent, OMX_PORT_NULL_SINK_AUDIO_INPUT, false);
    VCOSwaitEvent(&OMXnullSinkComponent, VCOS_EVENT_PORT_DISABLE);

    // Initialize OMXeglRenderComponent: Initialize, set component id and name, set VCOS flags, register OMX handle
    // Disable all ports, wait for port disable
    OMXInitializeComponent(&OMXeglRenderComponent, OMX_COMPONENT_EGL_RENDER_ID, OMX_COMPONENT_EGL_RENDER_NAME);

    OMXPortEnableDisableComponent(&OMXeglRenderComponent, OMX_PORT_EGL_RENDER_VIDEO_INPUT, false);
    VCOSwaitEvent(&OMXeglRenderComponent, VCOS_EVENT_PORT_DISABLE);

    OMXPortEnableDisableComponent(&OMXeglRenderComponent, OMX_PORT_EGL_RENDER_VIDEO_OUTPUT, false);
    VCOSwaitEvent(&OMXeglRenderComponent, VCOS_EVENT_PORT_DISABLE);

    // Initialize OMXimageEncodeComponent: Initialize, set component id and name, set VCOS flags, register OMX handle
    // Disable all ports, wait for port disable
    OMXInitializeComponent(&OMXimageEncodeComponent, OMX_COMPONENT_IMAGE_ENCODE_ID, OMX_COMPONENT_IMAGE_ENCODE_NAME);

    OMXPortEnableDisableComponent(&OMXimageEncodeComponent, OMX_PORT_IMAGE_ENCODE_IMAGE_INPUT, false);
    VCOSwaitEvent(&OMXimageEncodeComponent, VCOS_EVENT_PORT_DISABLE);

    OMXPortEnableDisableComponent(&OMXimageEncodeComponent, OMX_PORT_IMAGE_ENCODE_IMAGE_OUTPUT, false);
    VCOSwaitEvent(&OMXimageEncodeComponent, VCOS_EVENT_PORT_DISABLE);

    // Setup OMXcameraComponent: Set camera device id, wait for device id set, configure sensor and port width and height, set encoding, brightness, sharpness, ...
    // Component in state loaded and ports disabled
    OMXSetupCamera(&OMXcameraComponent, width, height);

    // Setup OMXimageEncodeComponent: Set buffer sizes, port width and height, color format, jpeg settings
    // Component in state loaded and ports disabled
    OMXSetupImageEncodeSettings(&OMXimageEncodeComponent, width, height);

    // Setup tunnel: OMXcameraComponent (preview video output) => OMXnullSinkComponent (video input)
    if (OMX_SetupTunnel(OMXcameraComponent.handle, OMX_PORT_CAMERA_PREVIEW_VIDEO_OUTPUT, OMXnullSinkComponent.handle, OMX_PORT_NULL_SINK_VIDEO_INPUT))
    {
        printf("OMX Error: OMX tunnel OMXcameraComponent (preview video out) => OMXnullSinkComponent (video in) - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup tunnel: OMXcameraComponent (real video output) => OMXeglRenderComponent (video input)
    if (OMX_SetupTunnel(OMXcameraComponent.handle, OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT, OMXeglRenderComponent.handle, OMX_PORT_EGL_RENDER_VIDEO_INPUT))
    {
        printf("OMX Error: OMX tunnel OMXcameraComponent (real video out) => OMXeglRenderComponent (video in) - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup state: Set all components to state idle
    OMXSetStateComponent(&OMXcameraComponent, OMX_StateIdle);
    VCOSwaitEvent(&OMXcameraComponent, VCOS_EVENT_STATE_SET);
    OMXSetStateComponent(&OMXeglRenderComponent, OMX_StateIdle);
    VCOSwaitEvent(&OMXeglRenderComponent, VCOS_EVENT_STATE_SET);

    OMXSetStateComponent(&OMXnullSinkComponent, OMX_StateIdle);
    VCOSwaitEvent(&OMXnullSinkComponent, VCOS_EVENT_STATE_SET);

    OMXSetStateComponent(&OMXimageEncodeComponent, OMX_StateIdle);
    VCOSwaitEvent(&OMXimageEncodeComponent, VCOS_EVENT_STATE_SET);

    // Setup ports: Enable all required ports of components
    // Inconsistent behaviour on port enable, do not send port enabled event?
    // Therefore, no waiting for port enable events here
    OMXPortEnableDisableComponent(&OMXcameraComponent, OMX_PORT_CAMERA_PREVIEW_VIDEO_OUTPUT, true);
    OMXPortEnableDisableComponent(&OMXcameraComponent, OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT, true);
    OMXPortEnableDisableComponent(&OMXeglRenderComponent, OMX_PORT_EGL_RENDER_VIDEO_INPUT, true);
    OMXPortEnableDisableComponent(&OMXeglRenderComponent, OMX_PORT_EGL_RENDER_VIDEO_OUTPUT, true);
    OMXPortEnableDisableComponent(&OMXnullSinkComponent, OMX_PORT_NULL_SINK_VIDEO_INPUT, true);
    OMXPortEnableDisableComponent(&OMXimageEncodeComponent, OMX_PORT_IMAGE_ENCODE_IMAGE_INPUT, true);
    OMXPortEnableDisableComponent(&OMXimageEncodeComponent, OMX_PORT_IMAGE_ENCODE_IMAGE_OUTPUT, true);

    // Setup EGLImage: EGLImage needed for setting up OMXeglRenderComponent
    eglRenderOutputFbo.allocate(width, height, GL_RGBA);
    GLuint eglTextureID = eglRenderOutputFbo.getTextureReference().getTextureData().textureID;
    ofAppEGLWindow* eglWindow = (ofAppEGLWindow*)(ofGetWindowPtr());
    EGLDisplay eglDisplay = eglWindow->getEglDisplay();
    EGLContext eglContext = eglWindow->getEglContext();
    eglImage = eglCreateImageKHR(eglDisplay, eglContext, EGL_GL_TEXTURE_2D_KHR, (EGLClientBuffer)(eglTextureID), NULL);

    if (!eglImage)
    {
        printf("OMX Error: OMX create egl image - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Setup OMXeglRenderComponent: Setup output buffer and output eglImage object
    // Component in state idle and ports enabled
    OMXSetupEGLRender(&OMXeglRenderComponent, &eglImage, &OMXeglRenderOutputBufferHeader);

    // Setup OMXimageEncodeComponent: Allocate input and output buffers
    // Component in state idle and ports enabled
    OMXSetupImageEncodeAllocate(&OMXimageEncodeComponent, OMXscreenPixelBuffer, &OMXimageEncodeInputBufferHeader, &OMXimageEncodeOutputBufferHeader, width, height);

    // Setup state: Set all components to state executing
    OMXSetStateComponent(&OMXcameraComponent, OMX_StateExecuting);
    VCOSwaitEvent(&OMXcameraComponent, VCOS_EVENT_STATE_SET);

    OMXSetStateComponent(&OMXeglRenderComponent, OMX_StateExecuting);
    VCOSwaitEvent(&OMXeglRenderComponent, VCOS_EVENT_STATE_SET);

    OMXSetStateComponent(&OMXnullSinkComponent, OMX_StateExecuting);
    VCOSwaitEvent(&OMXnullSinkComponent, VCOS_EVENT_STATE_SET);

    OMXSetStateComponent(&OMXimageEncodeComponent, OMX_StateExecuting);
    VCOSwaitEvent(&OMXimageEncodeComponent, VCOS_EVENT_STATE_SET);

    // Start camera capturing
    // Component in state executing and ports enabled
    OMXStartCameraCapturing(&OMXcameraComponent, OMX_PORT_CAMERA_REAL_VIDEO_OUTPUT);
}

// Note: update is always called before draw in infinite loop
void visicamRPiGPU::update()
{
    // Set new current timer
    clock_gettime(CLOCK_MONOTONIC, &currentTimespec);

    // Check against last refresh timer, if we need to refresh. 0 values => was just initialized, need to refresh aswell
    if ((currentTimespec.tv_sec - lastRefreshTimespec.tv_sec >= refreshTimeSeconds) || (lastRefreshTimespec.tv_sec == 0 && lastRefreshTimespec.tv_nsec == 0))
    {
        // Set new last refresh timer
        clock_gettime(CLOCK_MONOTONIC, &lastRefreshTimespec);

        // Set flag for original captured image output
        outputCapturedOriginalImage = true;

        // Check if homography input path file exists
        if (fileExists(homographyInputPath))
        {
            // Open file
            int homographyInputFile = open(homographyInputPath.c_str(), O_RDONLY);
            if (homographyInputFile != -1)
            {
                // Lock file
                if (flock(homographyInputFile, LOCK_EX) != -1)
                {
                    // Open input filestream
                    std::ifstream homographyInputStream(homographyInputPath.c_str());

                    // Opening was successful
                    if (homographyInputStream)
                    {
                        // Counter which indicates if how many values were read
                        int valuesCounter = 0;

                        // Read matrix values from file, seperator is newline \n
                        std::string inputLine;
                        while (std::getline(homographyInputStream, inputLine))
                        {
                            // Increase counter
                            valuesCounter++;

                            // Something went wrong, file has more lines than expected
                            if (valuesCounter > 9)
                            {
                                break;
                            }

                            // Set values
                            homographyInputMatrixValues[valuesCounter-1] = atof(inputLine.c_str());
                        }

                        // If exactly 9 values were read write them to final matrix and set flag for original captured image output
                        if (valuesCounter == 9)
                        {
                            // Need to covert homography matrix in openCV format to openGL format
                            // Can ignore z-coordinate here, so just fill up with empty values for z
                            homographyInputMatrix.set(homographyInputMatrixValues[0], homographyInputMatrixValues[3], 0.0f, homographyInputMatrixValues[6],
                                                      homographyInputMatrixValues[1], homographyInputMatrixValues[4], 0.0f, homographyInputMatrixValues[7],
                                                      0.0f,                           0.0f,                           0.0f,                           0.0f,
                                                      homographyInputMatrixValues[2], homographyInputMatrixValues[5], 0.0f, homographyInputMatrixValues[8]);
                        }

                        // Always close stream if opened successfully
                        homographyInputStream.close();
                    }

                    // Unlock file
                    flock(homographyInputFile, LOCK_UN);
                }

                // Always close file if opened successfully
                close(homographyInputFile);
            }
        }
    }

    // Input image (for next iteration)
    // OMXcameraComponent: Tunnel preview data to OMXnullSinkComponent and real video to OMXeglRenderComponent
    // OMXeglRenderComponent: Hand back the output buffer to the component, will write to texture of eglRenderOutputFbo
    if (OMX_FillThisBuffer(OMXeglRenderComponent.handle, OMXeglRenderOutputBufferHeader))
    {
        printf("OMX Error: OMX egl render component fill buffer failed - EXITING APPLICATION\n");
        std::exit(1);
    }

    // OMXeglRenderComponent: Wait until output buffer is completely ready, component has processed input and hands output buffer back to application
    // Output data is written to texture of eglRenderOutputFbo
    VCOSwaitEvent(&OMXeglRenderComponent, VCOS_EVENT_FILL_BUFFER_DONE);

    // Prepare output image (from previous iteration)
    // Check if we should output rendered image or original captured image, choose correct FBO
    // Bind eglRenderOutputFbo by using FBO id
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, (outputCapturedOriginalImage ? eglRenderOutputFbo.getFbo() : defaultRenderOutputFbo.getFbo()));

    // Read pixels from eglRenderOutputFbo into memory buffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, OMXscreenPixelBuffer);

    // Reset to default FBO by using 0 for default FBO id
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);

    // OMXimageEncodeComponent: Hand back the output buffer to the component
    if (OMX_FillThisBuffer(OMXimageEncodeComponent.handle, OMXimageEncodeOutputBufferHeader))
    {
        printf("OMX Error: OMX image encode component fill buffer failed - EXITING APPLICATION\n");
        std::exit(1);
    }

    // OMXimageEncodeComponent: Set filled length of input buffer to full length, hand back input buffer to the component and start reading
    OMXimageEncodeInputBufferHeader->nFilledLen = OMXimageEncodeInputBufferHeader->nAllocLen;
    if (OMX_EmptyThisBuffer(OMXimageEncodeComponent.handle, OMXimageEncodeInputBufferHeader))
    {
        printf("OMX Error: OMX image encode component empty buffer failed - EXITING APPLICATION\n");
        std::exit(1);
    }

    // OMXimageEncodeComponent: Wait until input buffer is completely read, component processes input and hands input buffer back to application
    VCOSwaitEvent(&OMXimageEncodeComponent, VCOS_EVENT_EMPTY_BUFFER_DONE);

    // OMXimageEncodeComponent: Wait until output buffer is completely ready, component has processed input and hands output buffer back to application
    VCOSwaitEvent(&OMXimageEncodeComponent, VCOS_EVENT_FILL_BUFFER_DONE);

    // Write output image (from previous iteration)
    // Check if there is data to write
    if (OMXimageEncodeOutputBufferHeader->nFilledLen > 0)
    {
        // Open file
        int imageOutputFile = open((outputCapturedOriginalImage ? capturedOutputPath.c_str() : processedOutputPath.c_str()), O_WRONLY | O_CREAT | O_TRUNC, 0644);

        // Reset flag for output captured original image
        outputCapturedOriginalImage = false;

        // File open successful
        if (imageOutputFile != -1)
        {
            // Lock file
            if (flock(imageOutputFile, LOCK_EX) != -1)
            {
                // Valid bytes begin at OMXimageEncodeOutputBufferHeader->pBuffer + OMXimageEncodeOutputBufferHeader->nOffset
                // Length of valid bytes is stored in OMXimageEncodeOutputBufferHeader->nFilledLen
                // Check result to avoid compiler warning, no need to check it at all
                if (pwrite(imageOutputFile, OMXimageEncodeOutputBufferHeader->pBuffer + OMXimageEncodeOutputBufferHeader->nOffset, OMXimageEncodeOutputBufferHeader->nFilledLen, 0) == -1)
                {
                    // Error in file writing, but we can not do anything about it anyways
                }

                // Unlock file
                flock(imageOutputFile, LOCK_UN);
            }

            // Always close file if opened successfully
            close(imageOutputFile);
        }
    }
}

// Note: draw is always called after update in infinite loop
void visicamRPiGPU::draw()
{
    // Draw into default render FBO
    defaultRenderOutputFbo.begin();

    // Projection mode: Mirror displayed image on y axis
    // OpenGL writes images from bottom to top, but top to buttom is needed for output image
    ofSetMatrixMode(OF_MATRIX_PROJECTION);
    ofPushMatrix();
    ofScale(1.0f, -1.0f, 1.0f);

    // Perform homography matrix multiplication in modelview mode to texture of eglRenderOutputFbo
    ofSetMatrixMode(OF_MATRIX_MODELVIEW);
    ofPushMatrix();

    // Matrix multiplication with homography matrix in output image
    ofMultMatrix(homographyInputMatrix);

    // Draw image to modified modelview and restore previous matrix
    eglRenderOutputFbo.draw(0, 0);
    ofPopMatrix();

    // Restore previous state of projection matrix
    ofSetMatrixMode(OF_MATRIX_PROJECTION);
    ofPopMatrix();

    // Stop draw into default render FBO
    defaultRenderOutputFbo.end();
}