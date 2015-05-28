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

echo ""
echo "#######################################################################"
echo "Setting special compile flags for Raspberry Pi 2"
echo "#######################################################################"
echo ""

# Note: This seems to be session dependant, do not eval it with command_success
export MAKEFLAGS=-j4 PLATFORM_VARIANT=rpi2

echo ""
echo "#######################################################################"
echo "Start compile"
echo "#######################################################################"
echo ""
command_success "make -C visicamRPiGPU"

echo ""
echo "#######################################################################"
echo "Finished compile"
echo "Binary is located in: visicamRPiGPU/bin/visicamRPiGPU"
echo "#######################################################################"
echo ""