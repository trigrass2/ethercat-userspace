#------------------------------------------------------------------------------
#
#  $Id: Makefile.am,v 070b4b420ab1 2010/01/08 12:10:12 fp $
#
#  Copyright (C) 2006-2008  Florian Pose, Ingenieurgemeinschaft IgH
#
#  This file is part of the IgH EtherCAT Master.
#
#  The IgH EtherCAT Master is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License version 2, as
#  published by the Free Software Foundation.
#
#  The IgH EtherCAT Master is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
#  Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with the IgH EtherCAT Master; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#  ---
#  
#  The license mentioned above concerns the source code only. Using the
#  EtherCAT technology and brand is only permitted in compliance with the
#  industrial property and similar rights of Beckhoff Automation GmbH.
#
#------------------------------------------------------------------------------

SUBDIRS =

if ENABLE_USERLIB
SUBDIRS += \
	dc_user \
	user
endif

if ENABLE_TTY
SUBDIRS += \
	tty
endif

DIST_SUBDIRS = \
	dc_rtai \
	dc_user \
	mini \
	rtai \
	tty \
	user

EXTRA_DIST = \
	Kbuild.in

BUILT_SOURCES = \
	Kbuild

#------------------------------------------------------------------------------
