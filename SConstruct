#******************************************************************
#
# Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

# prevent accidentally building all. to intentionally build all:
# $ scons .
# Default(None)

print "BUILD_TARGETS is", map(str, BUILD_TARGETS)

##
# The main build script
#
##
import os

print "CL TARGETS: ", COMMAND_LINE_TARGETS
print "BUILD_TARGETS is", map(str, BUILD_TARGETS)

# Load common build config
SConscript('hosts/SConscript')

Import('env')

# print env.Dump()

# if env.get('VERBOSE'):
#     print "ENV:"
#     print env.Dump()
#     print "START"

if os.environ.get('TERM') != None:
	env['ENV']['TERM'] = os.environ['TERM']

target_os = env.get('TARGET_OS')
target_arch = env.get('TARGET_ARCH')
src_dir = env.get('SRC_DIR')

# GAR FIXME: only build arduino on demand
# if target_os == 'arduino':
# 	SConscript('arduino.scons')

build_dir = env.get('BUILD_DIR')

#GAR default: build the kernel
# Build 'resource' sub-project
SConscript(build_dir + '/resource/SConscript')

#GAR FIXME: only build services on demand
# # Build 'service' sub-project
#SConscript(build_dir + '/service/SConscript')

#GAR FIXME: only build cloud on demand
# # Build "cloud" sub-project
# SConscript(build_dir + '/cloud/SConscript')

#GAR FIXME: only build plugins on demand
# # Build "plugin interface" sub-project
# SConscript(build_dir + '/plugins/SConscript')

print "EVAL TEST"
SConscript(build_dir + '/test/SConscript')

# Append targets information to the help information, to see help info, execute command line:
#     $ scon [options] -h
env.PrintTargets()

# GAR FIXME: only build arduino on demand
# # Print bin upload command line (arduino only)
# if target_os == 'arduino':
# 	env.UploadHelp()

#GAR FIXME
# # to install the generated pc file into custome prefix location
# env.UserInstallTargetPCFile('iotivity.pc', 'iotivity.pc')

print "PRINTING TARGETS"
env.PrintTargets()