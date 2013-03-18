#
# The Qubes OS Project, http://www.qubes-os.org
#
# Copyright (C) 2010  Joanna Rutkowska <joanna@invisiblethingslab.com>
# Copyright (C) 2010  Rafal Wojtczuk  <rafal@invisiblethingslab.com>
# Copyright (C) 2013  Marek Marczykowski <marmarek@invisiblethingslab.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
#

%{!?version: %define version %(cat version)}


Name:		qubes-gui-common-devel
Version:	%{version}
Release:	1%{?dist}
Summary:	Common files for Qubes GUI - protocol headers

Group:		Qubes
License:	GPL
URL:		http://www.qubes-os.org

%define _builddir %(pwd)

%description
Protocol description headers for Qubes GUI

%prep

%build

%install
mkdir -p %{buildroot}/usr/include
cp include/*.h %{buildroot}/usr/include/

%files
/usr/include/qubes-gui-protocol.h
/usr/include/qubes-xorg-tray-defs.h

%changelog

