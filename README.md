# visicamRPiGPU
visicamRPiGPU uses the hardware acceleration of the Raspberry Pi 2 (OS: Raspbian) to quickly create images for the project VisiCam (https://github.com/t-oster/VisiCam).
VisiCam and visicamRPiGPU need to work together to fulfil the task of providing perspective corrected camera images.
By using the hardware acceleration OpenMAX IL (https://www.khronos.org/openmax/il/) it is able to capture, correct, convert and save an image every 50 ms for a recommended resolution of 1280 x 720 pixels.
You can find more detailed information in the bash scripts and source files.

# Installation
visicamRPiGPU uses openFrameworks (http://openframeworks.cc/).
For compiling it needs to know the location of the openFrameworks directory and as a default setting it is currently configured to use relative paths for this purpose.
If you move around the directories and want to be able to compile the program again, you need to keep this relative directory structure or change the configuration.

Execute the following commands to install openFrameworks and compile visicamRPiGPU with the default settings:
```shell
sudo apt-get update
sudo apt-get install build-essential git
git clone https://github.com/FroChr123/visicamRPiGPU.git
cd visicamRPiGPU
sudo ./INSTALL.sh
./COMPILE.sh
```