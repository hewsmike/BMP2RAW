#!/bin/bash

###########################################################################
#   Copyright (C) 2020 by Mike Hewson                                     #
#   hewsmike[AT]iinet.net.au                                              #
#                                                                         #
#   This is free software: you can redistribute it and/or modify          #
#   it under the terms of the GNU General Public License as published     #
#   by the Free Software Foundation, version 2 of the License.            #
#                                                                         #
#   BMP2RAW is distributed in the hope that it will be useful,            #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
#   GNU General Public License for more details.                          #
#                                                                         #
#   A copy of the GNU General Public License is available at              #
#   http://www.gnu.org/licenses/                                          #
#                                                                         #
###########################################################################
### globals ###############################################################

# Set paths.
ROOT=`pwd`

# Set the logfile.
LOGFILE=$ROOT/build.log

# Delete any prior build log
rm -f ./build.log

log() {
    echo $1 | tee -a $LOGFILE
    return 0
    }

log "Building BMP2RAW ( this will be ready in a jiffy )..."
make >> $LOGFILE 2>&1

exit 0
