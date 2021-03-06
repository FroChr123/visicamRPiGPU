#!/bin/bash

#    This file is part of visicamRPiGPU. (https://github.com/FroChr123/visicamRPiGPU)
#    Please note the additional licenses and references to other projects in the file LICENSE-ADDITIONAL.
#
#    visicamRPiGPU is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    visicamRPiGPU is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with visicamRPiGPU.  If not, see <http://www.gnu.org/licenses/>.

#
# This script is based on the installation routine described in:
# http://forum.openframeworks.cc/t/raspberry-pi-2-setup-guide/18690
#
# Thanks to the original authors!
#

# Function to check if a command exited correctly
# Argument 1: Command string
function command_success
{
    if ! ( eval "$1" )
    then
        echo ""
        echo "#######################################################################"
        echo "Error in command '$1'"
        echo "Quit with errors"
        echo "#######################################################################"
        echo ""
        exit 1
    fi

    return 0
}

if [[ $EUID -ne 0 ]]
then
    echo ""
    echo "#######################################################################"
    echo "Installer must be started as root, try sudo command"
    echo "#######################################################################"
    echo ""
    exit 1
fi

echo ""
echo "#######################################################################"
echo "Clear directories if they already exist"
echo "#######################################################################"
echo ""

if [ -d "of_v0.8.4_linuxarmv7l_release" ]
then
    command_success "rm -R of_v0.8.4_linuxarmv7l_release"
    echo "Removed directory of_v0.8.4_linuxarmv7l_release"
fi

if [ -d "visicamRPiGPU/bin" ]
then
    command_success "rm -R visicamRPiGPU/bin"
    echo "Removed directory visicamRPiGPU/bin"
fi

if [ -d "visicamRPiGPU/obj" ]
then
    command_success "rm -R visicamRPiGPU/obj"
    echo "Removed directory visicamRPiGPU/obj"
fi


echo ""
echo "#######################################################################"
echo "Unpack openFrameworks"
echo "#######################################################################"
echo ""
command_success "tar xvf of_v0.8.4_linuxarmv7l_release.tar.gz"

echo ""
echo "#######################################################################"
echo "Copy makefiles for Raspberry Pi 2 to openFrameworks"
echo "#######################################################################"
echo ""
command_success "cp config.linuxarmv7l.rpi2.mk of_v0.8.4_linuxarmv7l_release/libs/openFrameworksCompiled/project/linuxarmv7l/config.linuxarmv7l.rpi2.mk"

echo ""
echo "#######################################################################"
echo "Install dependencies of openFrameworks"
echo "#######################################################################"
echo ""
command_success "of_v0.8.4_linuxarmv7l_release/scripts/linux/debian/install_dependencies.sh"

echo ""
echo "#######################################################################"
echo "Finished install"
echo "Consider increasing your GPU memory split in raspi-config"
echo "128 MB for 1280 x 720 (recommended), 140 MB for maximum resolution"
echo "You can compile now"
echo "#######################################################################"
echo ""