import os, urllib2
import sys
from SCons.Script import *

# Download from URL 'url', will save as 'target'
def download(target, url) :
	if os.path.exists(target) :
		return target
	try :
		print "Download %s from %s" % (target, url)
		print "Downloading ..."
		stream = urllib2.urlopen(url)
		file = open(target, 'wb')
		file.write(stream.read())
		file.close()
		print "... Download complete"
		return target
	except Exception, e :
		raise SCons.Errors.StopError( '%s [%s]' % (e, url) )

def print_config(env):
        print "HOST OS:  ", env.get('IOTIVITY_HOST_OS')
        print "HOST ARCH:", env.get('IOTIVITY_HOST_ARCH')
        print "TARGET OS:", env.get('TARGET_OS')
        print "TARGET OS Version:", env.get('TARGET_OS_VERSION')
        print "TARGET ARCH:", env.get('TARGET_ARCH')
        return env

def get_help_vars():
        help_vars = Variables()
        help_vars.Add(BoolVariable('VERBOSE', 'Show compilation', False))
        help_vars.Add(BoolVariable('RELEASE', 'Build for release?', True)) # set to 'no', 'false' or 0 for debug
        # help_vars.Add(EnumVariable('TARGET_OS', 'Target platform', host, host_target_map[host]))

        help_vars.Add(BoolVariable('WITH_RA', 'Build with Remote Access module', False))
        help_vars.Add(BoolVariable('WITH_TCP', 'Build with TCP adapter', False))
        help_vars.Add(ListVariable('WITH_MQ', 'Build with MQ publisher/broker', 'OFF', ['OFF', 'SUB', 'PUB', 'BROKER']))
        help_vars.Add(BoolVariable('WITH_CLOUD', 'Build including Cloud Connector and Cloud Client sample', False))
        help_vars.Add(ListVariable('RD_MODE', 'Resource Directory build mode', 'CLIENT', ['CLIENT', 'SERVER']))

        help_vars.Add(BoolVariable('SIMULATOR', 'Build with simulator module', False))

        help_vars.Add(BoolVariable('WITH_RA_IBB', 'Build with Remote Access module(workssys)', False))

        # if target_os in targets_disallow_multitransport:
	help_vars.Add(ListVariable('TARGET_TRANSPORT', 'Target transport', 'IP', ['BT', 'BLE', 'IP', 'NFC']))
        # else:
        # help_vars.Add(ListVariable('TARGET_TRANSPORT', 'Target transport', 'ALL', ['ALL', 'BT', 'BLE', 'IP', 'NFC']))

        # help_vars.Add(EnumVariable('TARGET_ARCH', 'Target architecture', default_arch, os_arch_map[target_os]))
        help_vars.Add(EnumVariable('SECURED', 'Build with DTLS', '0', allowed_values=('0', '1')))
        help_vars.Add(EnumVariable('DTLS_WITH_X509', 'DTLS with X.509 support', '0', allowed_values=('0', '1')))
        help_vars.Add(EnumVariable('TEST', 'Run unit tests', '0', allowed_values=('0', '1')))
        help_vars.Add(BoolVariable('LOGGING', 'Enable stack logging', False)) # logging_default))
        # help_vars.Add(BoolVariable('UPLOAD', 'Upload binary ? (For Arduino)', require_upload))
        help_vars.Add(EnumVariable('ROUTING', 'Enable routing', 'EP', allowed_values=('GW', 'EP')))
        help_vars.Add(EnumVariable('BUILD_SAMPLE', 'Build with sample', 'ON', allowed_values=('ON', 'OFF')))
        # help_vars.AddVariables(('DEVICE_NAME', 'Network display name for device (For Arduino)', device_name, None, None),)
        help_vars.Add(PathVariable('ANDROID_NDK', 'Android NDK path', None, PathVariable.PathAccept))
        help_vars.Add(PathVariable('ANDROID_HOME', 'Android SDK path', None, PathVariable.PathAccept))
        #ES_TARGET_ENROLLEE is specifying what is our target enrollee (Arduino or rest of platforms which support Multicast)
        help_vars.Add(EnumVariable('ES_TARGET_ENROLLEE', 'Target Enrollee', 'arduino', allowed_values=('arduino', 'tizen', 'linux', 'darwin')))
        #ES_ROLE is for specifying the role (Enrollee or Mediator) for which scons is being executed
        help_vars.Add(EnumVariable('ES_ROLE', 'Target build mode', 'mediator', allowed_values=('mediator', 'enrollee')))
        #ES_SOFT_MODE is for specifying MODE (Mode 1 : Enrollee with  Soft AP or Mode 2  : Mediator with Soft AP)
        help_vars.Add(EnumVariable('ES_SOFTAP_MODE', 'Target build mode', 'ENROLLEE_SOFTAP', allowed_values=('ENROLLEE_SOFTAP', 'MEDIATOR_SOFTAP')))
 	help_vars.Add('TC_PREFIX', "Toolchain prefix (Generally only be required for cross-compiling)", os.environ.get('TC_PREFIX'))
 	help_vars.Add(PathVariable('TC_PATH',
 			'Toolchain path (Generally only be required for cross-compiling)',
 			os.environ.get('TC_PATH')))

        return help_vars

# migrated from build_common/SConscript:
def SetDir(env):
        dir = env.GetLaunchDir()

	if not os.path.exists(dir + '/SConstruct'):
		print '''
*************************************** Error *********************************
* The directory(%s) seems isn't a source code directory, no SConstruct file is
* found. *
*******************************************************************************
''' % dir
		Exit(1)

        target_os = env.get('TARGET_OS')
        target_arch = env.get('TARGET_ARCH')

        print "SetDir TARGET OS:", target_os
        print "SetDir TARGET ARCH:", target_arch
        print "SetDir BUILD SYSROOT:", env.get('BUILD_SYSROOT')

	env.Replace(SRC_DIR = dir)
	print "build.utils.SetDir SRC_DIR: ", env.Dump('SRC_DIR')

        build_dir = env.get('BUILD_SYSROOT')
	# if env.get('RELEASE'):
	# 	build_dir = dir + '/out/' + target_os + '/' + target_arch + '/release/'
	# else:
	# 	build_dir = dir + '/out/' + target_os + '/' + target_arch + '/debug/'

	# the following cmd will add the build_dir tree to env?
	# env.VariantDir(build_dir, dir, duplicate=0)

	env.Replace(BUILD_DIR = build_dir)
	print "build.utils.SetDir BUILD_DIR: ", env.Dump('BUILD_DIR')
	# NB: the build_dir tree has NOT yet been written to disk!
        return env

def src_to_obj(ienv, src, home = ''):
	obj = ienv.get('BUILD_DIR') + src.replace(home, '')
	if ienv.get('OBJSUFFIX'):
		obj += ienv.get('OBJSUFFIX')
	return ienv.Object(obj, src)

# def install_target(ienv, targets, name):
#         idir = ienv.get('INSTALL_SYSROOT') + '/lib'
#         print "INSTALL_LIB", targets, " to ", idir
# 	i_n = ienv.Install(idir, targets)
# 	Alias(name, i_n)
# 	ienv.AppendUnique(TS = [name])

def install_bin(ienv, targets, name):
	i_n = ienv.Install(ienv.get('INSTALL_SYSROOT') + '/bin', targets)
	Alias(name, i_n)
	ienv.AppendUnique(TS = [name])
	# user_prefix = ienv.get('PREFIX')
	# if user_prefix:
	# 	i_n = ienv.Install(user_prefix + '/bin', targets)
	# 	ienv.Alias("install", i_n)

def install_header(ienv, targets, dir):  # , name):
	# user_prefix = ienv.get('PREFIX')
	# if user_prefix:
	# 	i_n = ienv.Install(user_prefix + '/include/' + dir ,targets)
	# else:
	i_n = ienv.Install(os.path.join(ienv.get('INSTALL_SYSROOT'), 'include'), targets)
	ienv.Alias("install", i_n)

def install_shlib(ienv, targets, name):
        idir = ienv.get('INSTALL_SYSROOT') + '/lib'
        print "INSTALL_SHLIB", targets, " to ", idir
        sys.stdout.flush()
	i_n = ienv.Install(idir, targets)
	Alias(name, i_n)
	ienv.AppendUnique(TS = [name])

def install_lib(ienv, targets, name):
        idir = ienv.get('INSTALL_SYSROOT') + '/lib'
        print "INSTALL_LIB", targets, " to ", idir
        sys.stdout.flush()
	i_n = ienv.Install(idir, targets)
	Alias(name, i_n)
	ienv.AppendUnique(TS = [name])

	# user_prefix = ienv.get('PREFIX')
	# if user_prefix:
	# 	user_lib = ienv.get('LIB_INSTALL_DIR')
	# 	if user_lib:
	# 		i_n = ienv.Install(user_lib, targets)
	# 	else:
	# 		i_n = ienv.Install(user_prefix + '/lib', targets)
	# 	ienv.Alias("install", i_n)
	# else:
	# 	i_n = ienv.Install(ienv.get('INSTALL_SYSROOT') + '/lib', targets)
	# ienv.Alias("install", i_n)

def install_pcfile(ienv, targets, name):
	user_prefix = ienv.get('PREFIX')
	if user_prefix:
		user_lib = ienv.get('LIB_INSTALL_DIR')
		if user_lib:
			i_n = ienv.Install(user_lib + '/pkgconfig', targets)
		else:
			i_n = ienv.Install(user_prefix + '/lib/pkgconfig', targets)
	else:
		i_n = ienv.Install(ienv.get('BUILD_DIR') + 'lib/pkgconfig', targets)
	ienv.Alias("install", i_n)

def append_target(ienv, name, targets = None):
	if targets:
		ienv.Alias(name, targets)
	ienv.AppendUnique(TS = [name])

def print_targets(ienv):
	Help('''
===============================================================================
Targets:\n    ''')
	for t in ienv.get('TS'):
		Help(t + ' ')
	Help('''
\nDefault all targets will be built. You can specify the target to build:

    $ scons [options] [target]
===============================================================================
''')


# env.AddMethod(__print_targets, 'PrintTargets')
# env.AddMethod(__src_to_obj, 'SrcToObj')
# env.AddMethod(__append_target, 'AppendTarget')
# env.AddMethod(__install, 'InstallTarget')
# env.AddMethod(__installlib, 'UserInstallTargetLib')
# env.AddMethod(__installbin, 'UserInstallTargetBin')
# env.AddMethod(__installheader, 'UserInstallTargetHeader')
# env.AddMethod(__installpcfile, 'UserInstallTargetPCFile')
