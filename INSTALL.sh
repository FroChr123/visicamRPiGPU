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

# Store current directory
ORIGINALDIR=$( pwd )

# Really need to change directory here as it seems, otherwise it will not install correctly
command_success "cd of_v0.8.4_linuxarmv7l_release/scripts/linux/debian"
command_success "./install_dependencies.sh"

# Change back to original directory
command_success "cd $ORIGINALDIR"

echo ""
echo "#######################################################################"
echo "Finished install"
echo "Consider increasing your GPU memory split"
echo "You can compile now"
echo "#######################################################################"
echo ""