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

/* #####################################
MISC DEFINES
##################################### */
#define FIRST_FORCED_REFRESH_SECONDS            3

/* #####################################
OMX
##################################### */

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