#!/usr/bin/python
#
# Copyright (C) 2010, Nokia <jean-luc.lamadon@nokia.com>
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

"Constants describing Tracker D-Bus services"

import os

TRACKER_BUSNAME = 'org.freedesktop.Tracker1'
TRACKER_OBJ_PATH = '/org/freedesktop/Tracker1/Resources'
RESOURCES_IFACE = "org.freedesktop.Tracker1.Resources"

MINERFS_BUSNAME = "org.freedesktop.Tracker1.Miner.Files"
MINERFS_OBJ_PATH = "/org/freedesktop/Tracker1/Miner/Files"
MINER_IFACE = "org.freedesktop.Tracker1.Miner"

TRACKER_BACKUP_OBJ_PATH = "/org/freedesktop/Tracker1/Backup"                                            
BACKUP_IFACE = "org.freedesktop.Tracker1.Backup"

TRACKER_STATS_OBJ_PATH = "/org/freedesktop/Tracker1/Statistics"
STATS_IFACE = "org.freedesktop.Tracker1.Statistics"

TRACKER_STATUS_OBJ_PATH = "/org/freedesktop/Tracker1/Status"
STATUS_IFACE = "org.freedesktop.Tracker1.Status"

TRACKER_EXTRACT_BUSNAME = "org.freedesktop.Tracker1.Extract"
TRACKER_EXTRACT_OBJ_PATH = "/org/freedesktop/Tracker1/Extract"
TRACKER_EXTRACT_IFACE = "org.freedesktop.Tracker1.Extract"

WRITEBACK_BUSNAME = "org.freedesktop.Tracker1.Writeback"

def expandvars (variable):
    # Note: the order matters!
    result = variable
    for var, value in [("${datarootdir}", RAW_DATAROOT_DIR),
                       ("${exec_prefix}", RAW_EXEC_PREFIX),
                       ("${prefix}", PREFIX)]:
        result = result.replace (var, value)


    return result



PREFIX = "/usr/local"
#
# This raw variables are set by autotools without translating vars:
#   E.G. bindir='${exec_prefix}/bin
#
# So we do the translation by hand in the expandvars function
#
RAW_EXEC_PREFIX = "${prefix}"
RAW_EXEC_DIR = "${exec_prefix}/libexec"
RAW_DATA_DIR = "${datarootdir}"
RAW_DATAROOT_DIR = "${prefix}/share"
RAW_BINDIR = "${exec_prefix}/bin"

EXEC_PREFIX = os.path.normpath (expandvars (RAW_EXEC_DIR))
DATADIR = os.path.normpath (expandvars (RAW_DATA_DIR))
BINDIR = os.path.normpath (expandvars (RAW_BINDIR))
                            
haveMaemo = ("#" == "")

TEST_TMP_DIR = os.path.join (os.environ["HOME"], ".tracker-tests")
        
