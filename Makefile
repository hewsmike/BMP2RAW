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

CXX=g++

CPPFLAGS = -std=c++11 -Wall
CPPFLAGS += -I/usr/include -I/usr/lib/x86_64-linux-gnu

LIBS = -Wl,-Bstatic
LIBS += -L/usr/lib -L/usr/lib/x86_64-linux-gnu

LDFLAGS += -static-libstdc++

DEPS=Makefile

bmp2raw: $(DEPS) bmp2raw.cpp
	$(CXX) -g $(CPPFLAGS) $(LDFLAGS) bmp2raw.cpp -o bmp2raw $(OBJS) $(LIBS)
