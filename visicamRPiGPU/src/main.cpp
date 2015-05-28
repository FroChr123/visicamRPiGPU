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

#include "ofMain.h"
#include "ofGLProgrammableRenderer.h"
#include "visicamRPiGPU.h"

// Argument 0: (string) Execution command for this program
// Argument 1: (int) Width pixel
// Argument 2: (int) Height pixel
// Argument 3: (int) Refresh time in seconds (captured original output image, homography input matrix)
// Argument 4: (string) Homography matrix input path
// Argument 5: (string) Processed output image path
// Argument 6: (string) Captured output image path
int main(int argc, char *argv[])
{
    // Quit if argument count does not match
    if (argc != 7)
    {
        printf("\n####### USAGE: #######\n");
        printf("Argument 1: (int) Width pixel\n");
        printf("Argument 2: (int) Height pixel\n");
        printf("Argument 3: (int) Refresh time in seconds (captured original output image, homography input matrix)\n");
        printf("Argument 4: (string) Homography matrix input path\n");
        printf("Argument 5: (string) Processed output image path\n");
        printf("Argument 6: (string) Captured output image path\n\n");

        printf("Argument error: Incorrect amount of arguments - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Information for program
    printf("\n####### PROGRAM:#######\n");
    printf("This program is designed to use the Raspberry Pi 2 (OS: Raspbian) hardware acceleration APIs.\n");
    printf("OpenGL ES 2.0 and OpenMax IL are currently used for this purpose and it might be adaptable to other systems.\n");
    printf("For more details please visit the project page (https://github.com/FroChr123/visicamRPiGPU).\n");

    // Information for paths
    printf("\n####### PATHS:#######\n");
    printf("You should use absolute paths in the input parameters.\n");
    printf("It is advised to use paths to the shared memory /run/shm because there are a lot of file read / write operations in this program.\n");

    // Information for camera options
    printf("\n####### CAMERA OPTIONS:#######\n");
    printf("There are a lot of different camera options.\n");
    printf("You can change all used camera options in the defines of the header file.\n");

    // Information for memory split
    printf("\n####### MEMORY SPLIT:#######\n");
    printf("For the maximum allowed resolution of 1920 x 1072 you need to have a memory split in raspi-config of around 140 MB.\n");
    printf("The resolution 1280 x 720 is recommended because of processing speed and it runs with 128 MB memory split in raspi-config.\n");
    printf("Recommended image format is 16:9, otherwise you might get weird cropping effects or borders in your images.\n");
    printf("You can get any kind of strange behaviour if there is not enough GPU memory such as white screens or errors in buffer allocation.\n\n");

    // Initialize app
    visicamRPiGPU app;
    app.width = atoi(argv[1]);
    app.height = atoi(argv[2]);
    app.refreshTimeSeconds = atoi(argv[3]);
    app.homographyInputPath = std::string(argv[4]);
    app.processedOutputPath = std::string(argv[5]);
    app.capturedOutputPath = std::string(argv[6]);

    // Check refresh time: Must be more than 0 seconds
    if (app.refreshTimeSeconds <= 0)
    {
        printf("Argument error: Refresh time must be more than 0 seconds - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Check resolution values: Width must be between 640 and 1920 pixels (camera resolutions)
    if (app.width < 640 || app.width > 1920)
    {
        printf("Argument error: Width must be between 640 and 1920 pixels (camera resolutions) - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Check resolution values: Height must be between 480 and 1080 pixels (camera resolutions)
    if (app.height < 480 || app.height > 1080)
    {
        printf("Argument error: Height must be between 480 and 1080 pixels (camera resolutions) - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Check resolution values: Width must be multiple of 32 (OMX component requirements: format.image.nStride)
    if ((app.width % 32) != 0)
    {
        printf("Argument error: Width must be a multiple of 32  (OMX component requirements: format.image.nStride) - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Check resolution values: Height must be multiple of 16 (OMX component requirements: format.image.nSliceHeight)
    if ((app.height % 16) != 0)
    {
        printf("Argument error: Height must be multiple of 16 (OMX component requirements: format.image.nSliceHeight) - EXITING APPLICATION\n");
        std::exit(1);
    }

    // Load special renderer for OpenGL ES
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);

    // Setup OpenGL context
    ofSetupOpenGL(app.width, app.height, OF_WINDOW);

    // Run app
    ofRunApp(&app);
}
