import os, urllib2
from sys import platform as _platform
import subprocess
from string import maketrans
from SCons.Script import *


host_target_map = {
		'linux': ['linux', 'android', 'arduino', 'yocto', 'tizen'],
		'windows': ['windows', 'android', 'arduino'],
		'darwin': ['darwin', 'ios', 'android', 'arduino'],
		'msys_nt' :['msys_nt'],
		}

# Map of os and allowed archs (os: allowed archs)
os_arch_map = {
		'linux': ['x86', 'x86_64', 'arm', 'arm-v7a', 'arm64'],
		'tizen': ['x86', 'x86_64', 'arm', 'arm-v7a', 'armeabi-v7a', 'arm64'],
		'android': ['x86', 'x86_64', 'armeabi', 'armeabi-v7a', 'armeabi-v7a-hard', 'arm64-v8a'],
		'windows': ['x86', 'amd64', 'arm'],
		'msys_nt':['x86', 'x86_64'],
		'darwin': ['i386', 'x86_64'],
		'ios': ['i386', 'x86_64', 'armv7', 'armv7s', 'arm64'],
		'arduino': ['avr', 'arm'],
		'yocto': ['i586', 'x86_64', 'arm', 'powerpc', 'powerpc64', 'mips', 'mipsel'],
		}

# from Android NDK build/core/toolchains
toolchain_arch_abi = {
        'arm':    ['armabi', 'armeabi-v7a'], # 32-bit
        'arm64':  ['arm64-v8a'],             # 64-bit
        'mips':   ['mips'],                  # 32-bit
        'mips64': ['mips64'],                # 64-bit
        'x86':    ['x86'],                   # 32-bit
        'x86_64': ['x86_64']                 # 64-bit
        }

# see ndk build/core/init.mk
def get_host(env):
        # auto-detect host os, arch, etc. if not provided in env var
# For all systems, we will have HOST_OS_BASE defined as
# $(HOST_OS), except on Cygwin where we will have:
#
#  HOST_OS      == cygwin
#  HOST_OS_BASE == windows
        return env

def config_transport(env):
# from connectivity/SConscript:
# 	if with_ra == True:
# 			env.AppendUnique(CPPDEFINES = ['RA_ADAPTER'])
# 	if with_tcp == True:
# 			env.AppendUnique(CPPDEFINES = ['TCP_ADAPTER'])
# if 'ALL' in transport:
# 	if with_ra == True:
# 			env.AppendUnique(CPPDEFINES = ['RA_ADAPTER'])
# 	if with_tcp == True:
# 			env.AppendUnique(CPPDEFINES = ['TCP_ADAPTER'])

# 	if target_os == 'linux':
# 	        gdbus = 0
# 		for item in env.get('CPPDEFINES'):
#         	        if 'HAVE_GDBUS' in item:
# 	        	        gdbus = 1
# 				break;
# 		if gdbus:
# 		        env.AppendUnique(CPPDEFINES = ['IP_ADAPTER','NO_EDR_ADAPTER','LE_ADAPTER'])
# 		else:  # Edison uses dbus-glib, gdbus-codegen unavail
# 		        print "**** WARNING: host uses dbus-glib, GDBus required (for gdbus-codegen)"
# 			env.AppendUnique(CPPDEFINES = ['IP_ADAPTER','NO_EDR_ADAPTER','NO_LE_ADAPTER'])

# 	elif target_os == 'tizen':
# 		env.AppendUnique(CPPDEFINES = ['IP_ADAPTER','EDR_ADAPTER','LE_ADAPTER'])
# 	elif target_os == 'android':
# 		env.AppendUnique(CPPDEFINES = ['IP_ADAPTER','EDR_ADAPTER','LE_ADAPTER', 'NFC_ADAPTER'])
# 	elif target_os in['darwin','ios']:
# 		env.AppendUnique(CPPDEFINES = ['IP_ADAPTER','NO_EDR_ADAPTER','NO_LE_ADAPTER'])
# 	elif target_os in ['msys_nt', 'windows']:
# 		env.AppendUnique(CPPDEFINES = ['IP_ADAPTER','NO_EDR_ADAPTER','NO_LE_ADAPTER'])
# 	else:
# 		env.AppendUnique(CPPDEFINES = ['IP_ADAPTER','EDR_ADAPTER','LE_ADAPTER'])
# else:
# 	if 'BT' in transport:
# 		if target_os in ['darwin', 'linux']:
# 			print "CA Transport BT is not supported on " + target_os
# 			Exit(1)
# 		else:
# 			env.AppendUnique(CPPDEFINES = ['EDR_ADAPTER'])
# 	else:
# 		env.AppendUnique(CPPDEFINES = ['NO_EDR_ADAPTER'])

# 	if 'BLE' in transport:
# 	        if target_os == 'darwin':
# 		    print "CA Transport BLE not supported on Darwin"
# 		    Exit(1)
# 		else:
# 		    env.AppendUnique(CPPDEFINES = ['LE_ADAPTER'])
# 	else:
# 		env.AppendUnique(CPPDEFINES = ['NO_LE_ADAPTER'])

# 	if 'IP' in transport:
# 		env.AppendUnique(CPPDEFINES = ['IP_ADAPTER'])
# 	else:
# 		env.AppendUnique(CPPDEFINES = ['NO_IP_ADAPTER'])

# 	if with_tcp == True:
# 		if target_os in ['darwin', 'linux', 'tizen', 'android', 'arduino']:
# 			env.AppendUnique(CPPDEFINES = ['TCP_ADAPTER', 'WITH_TCP'])
# 		else:
# 			print "CA Transport TCP is not supported "
# 			Exit(1)
# 	else:
# 		env.AppendUnique(CPPDEFINES = ['NO_TCP_ADAPTER'])

# 	if 'NFC' in transport:
# 		if target_os in['android']:
# 			env.AppendUnique(CPPDEFINES = ['NFC_ADAPTER'])
# 		else:
# 			print "CA Transport NFC is not supported "
# 			Exit(1)
# 	else:
# 		env.AppendUnique(CPPDEFINES = ['NO_NFC_ADAPTER'])

#         if 'SUB' in with_mq:
# 	env.AppendUnique(CPPDEFINES = ['MQ_SUBSCRIBER', 'WITH_MQ'])
# 	print "MQ SUB support"

# if 'PUB' in with_mq:
# 	env.AppendUnique(CPPDEFINES = ['MQ_PUBLISHER', 'WITH_MQ'])
# 	print "MQ PUB support"

# if 'BROKER' in with_mq:
# 	env.AppendUnique(CPPDEFINES = ['MQ_BROKER', 'WITH_MQ'])
# 	print "MQ Broker support"



        return env

def host(env) :
        print "Running feature tests for host system..."
        # env = Environment()
        # if env.GetOption('clean'):
        #         print "We are cleaning, skip the config"
        #         return;

        env.Replace(BUILD_SYSROOT   = env['ENV']['BUILD_SYSROOT'])
        env.Replace(INSTALL_SYSROOT = env['ENV']['INSTALL_SYSROOT'])

        #FIXME: put this in a fn:
        try:
                ip_api = env['ENV']['IOTIVITY_IP_API']
                if ip_api == 0 :
                        env.Append(CPPDEFINES = ['NO_IP_ADAPTER'])
                else:
                        env.Append(CPPDEFINES = ['IP_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['IP_ADAPTER'])

        try:
                bt_api = env['ENV']['IOTIVITY_BT_API']
                if bt_api:
                        env.Append(CPPDEFINES = ['EDR_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_EDR_ADAPTER'])

        try:
                ble_api = env['ENV']['IOTIVITY_BLE_API']
                if ble_api:
                        env.Append(CPPDEFINES = ['LE_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_LE_ADAPTER'])

        try:
                nfc_api = env['ENV']['IOTIVITY_NFC_API']
                if nfc_api:
                        env.Append(CPPDEFINES = ['NFC_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_NFC_ADAPTER'])



        conf = Configure(env)

        if not conf.CheckCXX():
                print('!! Your compiler and/or environment is not correctly configured.')
                Exit(0)

        if not conf.CheckFunc('printf'):
                print('!! Your compiler and/or environment is not correctly configured.')
                Exit(0)

        # WARNING: CheckLib has the side-effect of adding found libs to LIBS env var
        # only check for libs you want included in every compile
        #GAR FIXME: check for boost libs UNLESS build target is c kernel

        if conf.CheckLib('uuid'):
                env.AppendUnique(CPPDEFINES = ['HAVE_UUID'])

        if conf.CheckLibWithHeader('pthread', 'pthread.h', 'c'):
                env.AppendUnique(CPPDEFINES = ['HAVE_PTHREAD_H'])
                env.AppendUnique(PTHREAD_CFLAGS = '-pthread')
                env.AppendUnique(PTHREAD_LIB = 'pthread')

        if conf.CheckCHeader('curl/curl.h'):
                env.AppendUnique(CPPDEFINES = ['HAVE_CURL_CURL_H'])

        if conf.CheckProg('gdbus-codegen'):
                env.AppendUnique(CPPDEFINES = ['HAVE_GDBUS'])
                #WARNING: having gdbus-codegen does not guarantee bluez stack
                # it could be installed on e.g. os x by some lib other than bluez

        if conf.CheckProg('bluetoothd'):
                #TODO: make sure we have a bluez stack
                sys.stdout.write('Checking bluetooth version... ')
                sys.stdout.flush()
                #try
                bltv = subprocess.Popen(["bluetoothd", "-v"], stdout=subprocess.PIPE).communicate()[0]
                sys.stdout.write(bltv)
                bltv = bltv.replace('.', '')
                bldef = 'BLUEZ=' + bltv.rstrip()
                conf.env.PrependUnique(CPPDEFINES = [bldef])

        # funcs
        if conf.CheckFunc('clock_gettime'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_CLOCK_GETTIME'])
        if conf.CheckFunc('gettimeofday'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETTIMEOFDAY'])
        if conf.CheckFunc('localtime_r'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_LOCALTIME_R'])
        if conf.CheckFunc('pthread_condattr_setclock'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_PTHREAD_CONDATTR_SETCLOCK'])
        if conf.CheckFunc('QueryPerformanceFrequency'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_QUERYPERFORMANCEFREQUENCY'])
        if conf.CheckFunc('strptime'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_STRPTIME'])

        # in alpha order
        if conf.CheckCHeader('arpa/inet.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_ARPA_INET_H'])
        if conf.CheckCHeader('fcntl.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_FCNTL_H'])
        if conf.CheckCHeader('grp.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_GRP_H'])
        if conf.CheckCHeader('i6addr.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_I6ADDR_H'])
        if conf.CheckCHeader('limits.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_LIMITS_H'])
        else:
                print "limits.h is required but not found; abending build"
                Exit(2)
        if conf.CheckCHeader('linux/limits.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_LINUX_LIMITS_H'])
        if conf.CheckCHeader('memory.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_MEMORY_H'])
        if conf.CheckCHeader('netdb.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_NETDB_H'])
        if conf.CheckCHeader('netinet/in.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_NETINET_IN_H'])
        if conf.CheckCHeader('pthread.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_PTHREAD_H'])
        if conf.CheckCHeader('stdlib.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_STDLIB_H'])
        if conf.CheckCHeader('string.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_STRING_H'])
        if conf.CheckCHeader('strings.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_STRINGS_H'])
        if conf.CheckCHeader('sys/socket.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_SYS_SOCKET_H'])
        if conf.CheckCHeader('sys/stat.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_SYS_STAT_H'])
        if conf.CheckCHeader('sys/time.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_SYS_TIME_H'])
        if conf.CheckCHeader('sys/timeb.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_SYS_TIMEB_H'])
        if conf.CheckCHeader('sys/types.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_SYS_TYPES_H'])
        if conf.CheckCHeader('sys/unistd.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_SYS_UNISTD_H'])
        if conf.CheckCHeader('syslog.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_SYSLOG_H'])
        if conf.CheckCHeader('time.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_TIME_H'])
        if conf.CheckCHeader('unistd.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_UNISTD_H'])
        if conf.CheckCHeader('uuid/uuid.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_UUID_UUID_H'])
        if conf.CheckCHeader('windows.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_WINDOWS_H'])
        if conf.CheckCHeader('winsock2.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_WINSOCK2_H'])
        if conf.CheckCHeader('ws2tcpip.h'):
                conf.env.Append(CPPDEFINES = ['HAVE_WS2TCPIP_H'])


        try:
                target_os = os.environ['TARGET_OS']
        except KeyError:
                target_os = os.environ['HOST_OS']

        if conf.CheckFunc('GetSystemTimeAsFileTime') or target_os == 'windows':
	# TODO: Remove target_os check.
	# We currently check for 'windows' as well, because the environment can
	# sometimes get so polluted that CheckFunc ceases to work!
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETSYSTEMTIMEASFILETIME'])

        env = conf.Finish()
        print "... feature testing complete"

        # Then: transports
        try:
                ip_api = env['ENV']['IOTIVITY_IP_API']
                if ip_api:
                        env.Append(CPPDEFINES = ['IP_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_IP_ADAPTER'])

        try:
                bt_api = env['ENV']['IOTIVITY_BT_API']
                if bt_api:
                        env.Append(CPPDEFINES = ['EDR_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_EDR_ADAPTER'])

        try:
                ble_api = env['ENV']['IOTIVITY_BLE_API']
                if ble_api:
                        env.Append(CPPDEFINES = ['LE_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_LE_ADAPTER'])

        try:
                nfc_api = env['ENV']['IOTIVITY_NFC_API']
                if nfc_api:
                        env.Append(CPPDEFINES = ['NFC_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_NFC_ADAPTER'])


        return env

def android(env):
        print "SETUP.TARGET.ANDROID()"
        #FIXME: currently this is just for POC.
        # TODO: elim hardcoded stuff, use sourced env vars to drive config
        # TODO: get the versioning of the tools, needed for linking to crystax boost libs

        # First: tools
        env.Replace(CC = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-gcc')
        env.Replace(CXX = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-g++')
        env.Replace(LINK = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-g++')
        env.Replace(LD = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-ld')
        env.Replace(AR = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-ar')
        env.Replace(RANLIB = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-ranlib')

        # Then: transports
        try:
                ip_api = env['ENV']['IOTIVITY_IP_API']
                if ip_api:
                        env.Append(CPPDEFINES = ['IP_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_IP_ADAPTER'])

        try:
                bt_api = env['ENV']['IOTIVITY_BT_API']
                if bt_api:
                        env.Append(CPPDEFINES = ['EDR_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_EDR_ADAPTER'])

        try:
                ble_api = env['ENV']['IOTIVITY_BLE_API']
                if ble_api:
                        env.Append(CPPDEFINES = ['LE_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_LE_ADAPTER'])

        try:
                nfc_api = env['ENV']['IOTIVITY_NFC_API']
                if nfc_api:
                        env.Append(CPPDEFINES = ['NFC_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_NFC_ADAPTER'])
        env.Append(CPPDEFINES = ['HAVE_ARPA_INET_H'])
        env.Append(CPPDEFINES = ['HAVE_ASSERT_H'])
        env.Append(CPPDEFINES = ['HAVE_FCNTL_H'])
        env.Append(CPPDEFINES = ['HAVE_LIMITS_H'])
        env.Append(CPPDEFINES = ['HAVE_NETDB_H'])
        env.Append(CPPDEFINES = ['HAVE_NETINET_IN_H'])
        #GAR FIXME: depends on the android version:
        # if version >= 21
        env.Append(CPPDEFINES = ['HAVE_PTHREAD_CONDATTR_SETCLOCK'])
        env.Append(CPPDEFINES = ['HAVE_PTHREAD_H'])
        env.Append(CPPDEFINES = ['HAVE_SYS_SOCKET_H'])
        env.Append(CPPDEFINES = ['HAVE_SYS_TIME_H'])
        env.Append(CPPDEFINES = ['HAVE_SYS_TYPES_H'])
        env.Append(CPPDEFINES = ['HAVE_TIME_H'])
        env.Append(CPPDEFINES = ['WITH_POSIX'])
        env.Append(CPPDEFINES = ['__ANDROID__'])
        env.Append(CPPDEFINES = ['__JAVA__'])
        env.Append(CPPDEFINES = ['WITH_POSIX'])
        #GAR FIXME
        # env.Append(LIBPATH = ['/Users/gar/android/crystax-ndk-10.3.2/sources/boost/1.59/libs/armeabi-v7a/llvm-3.6/'])
        env.Append(CPPPATH = ['/Users/gar/android/crystax-ndk-10.3.2/sources/boost/1.59.0/include'])
        env.Append(LIBPATH = ['/Users/gar/android/crystax-ndk-10.3.2/sources/boost/1.59.0/libs/armeabi-v7a/gnu-4.9/'])
        env.Append(LIBS = ['boost_date_time'])
        env.Append(LIBS = ['boost_system'])
        env.Append(LIBS = ['boost_thread'])
        env.AppendUnique(CXXFLAGS = ['-std=c++11'])
        env.AppendUnique(CFLAGS = ['-std=c11'])
        env.AppendUnique(LINKFLAGS = ['-shared'])
        env.Replace(SHLIBSUFFIX = '.so')

        return env

def edison(env):
        print "SETUP.TARGET.EDISON()"
        #FIXME: currently this is just for POC.
        # TODO: elim hardcoded stuff, use sourced env vars to drive config
        # TODO: get the versioning of the tools, needed for linking to crystax boost libs

        # env.Replace(IOTIVITY_HOST_OS = env['ENV']['IOTIVITY_HOST_OS'])
        env.Replace(HOST_OS = env['ENV']['HOST_OS'])
        env.Replace(HOST_ARCH = env['ENV']['HOST_ARCH'])
        env.Replace(TARGET_OS = env['ENV']['TARGET_OS'])
        env.Replace(TARGET_OS_VERSION = env['ENV']['TARGET_OS_VERSION'])
        env.Replace(TARGET_ARCH = env['ENV']['TARGET_ARCH'])

        # tools
        try:
                env.Replace(ADDR2LINE = env['ENV']['ADDR2LINE'])
        except:
                pass
        try:
                env.Replace(AR        = env['ENV']['AR'])
        except:
                pass

        try:
                env.Replace(AS        = env['ENV']['AS'])
        except:
                pass

        try:
                env.Replace(CC        = env['ENV']['CC'])
        except:
                pass

        try:
                env.Replace(CPP       = env['ENV']['CPP'])
        except:
                pass

        try:
                env.Replace(CXX       = env['ENV']['CXX'])
        except:
                pass

        try:
                env.Replace(CXXFILE   = env['ENV']['CXXFILT'])
        except:
                pass

        try:
                env.Replace(ELFEDIT   = env['ENV']['ELFEDIT'])
        except:
                pass

        try:
                env.Replace(GCOV      = env['ENV']['GCOV'])
        except:
                pass

        try:
                env.Replace(GDB       = env['ENV']['GDB'])
        except:
                pass

        try:
                env.Replace(GPROF     = env['ENV']['GPROF'])
        except:
                pass

        try:
                env.Replace(LD        = env['ENV']['LD'])
        except:
                pass

        try:
                env.Replace(LINK      = env['ENV']['CXX'])
        except:
                pass
        try:
                env.Replace(NM        = env['ENV']['NM'])
        except:
                pass

        try:
                env.Replace(OBJCOPY   = env['ENV']['OBJCOPY'])
        except:
                pass

        try:
                env.Replace(OBJDUMP   = env['ENV']['OBJDUMP'])
        except:
                pass

        try:
                env.Replace(RANLIB    = env['ENV']['RANLIB'])
        except:
                pass

        try:
                env.Replace(READELF   = env['ENV']['READELF'])
        except:
                pass

        try:
                env.Replace(SIZE      = env['ENV']['SIZE'])
        except:
                pass

        try:
                env.Replace(STRIP     = env['ENV']['STRIP'])
        except:
                pass

        try:
                env.Replace(STRINGS   = env['ENV']['STRINGS'])
        except:
                pass

        # env.Replace(CPPPATH   = env['ENV']['CPPPATH'])
        s = env['ENV']['CPPPATH']
        items = s.split()
        for item in items:
                env.AppendUnique(CPPPATH = [item])

        # env.Replace(CPPFLAGS  = env['ENV']['CPPFLAGS'])
        s = env['ENV']['CPPFLAGS']
        items = s.split()
        for item in items:
                env.AppendUnique(CPPFLAGS = [item])

        # env.Replace(CFLAGS    = env['ENV']['CFLAGS'])
        s = env['ENV']['CFLAGS']
        items = s.split()
        for item in items:
                env.AppendUnique(CFLAGS = [item])

        # env.Replace(CXXFLAGS  = env['ENV']['CXXFLAGS'])
        s = env['ENV']['CXXFLAGS']
        items = s.split()
        for item in items:
                env.AppendUnique(CXXFLAGS = [item])

        # env.Replace(LDFLAGS   = env['ENV']['LDFLAGS'])
        s = env['ENV']['LDFLAGS']
        items = s.split()
        for item in items:
                env.AppendUnique(LDFLAGS = [item])

        s = env['ENV']['LINKFLAGS']
        items = s.split()
        for item in items:
                env.AppendUnique(LINKFLAGS = [item])

        # env.Replace(HOST_SYSROOT = env['ENV']['HOST_SYSROOT'])
        env.Replace(TARGET_SYSROOT = env['ENV']['TARGET_SYSROOT'])

        env.Replace(PREFIX         = env['ENV']['PREFIX'])
        # env.Replace(BUILD_HOME     = env['ENV']['BUILD_HOME'])

        env.Replace(BUILD_SYSROOT   = env['ENV']['BUILD_SYSROOT'])
        env.Replace(INSTALL_SYSROOT = env['ENV']['INSTALL_SYSROOT'])

        env.Replace(SHLIBSUFFIX = env['ENV']['SHLIBSUFFIX'])
        env.Replace(SHLINKFLAGS = ['$LINKFLAGS', env['ENV']['SHLINKFLAG']])

        s = env['ENV']['CPPDEFINES']
        items = s.split()
        for item in items:
                env.AppendUnique(CPPDEFINES = [item])

        # target_sysroot
        # sysroot = env['ENV']['TARGET_SYSROOT']
        # env.AppendUnique(CPPFLAGS = ['-E',
        #                              '-shared',
        #                              '-m32', '-march=core2', '-mtune=core2',
        #                              '-msse3', '-mfpmath=sse', '-mstackrealign',
        #                              '-fno-omit-frame-pointer',
        #                              '--sysroot=' + sysroot])

        # env.AppendUnique(CFLAGS = ['-m32', '-march=core2', '-mtune=core2',
        #                            '-msse3', '-mfpmath=sse', '-mstackrealign',
        #                            '-fno-omit-frame-pointer',
        #                            '-Wl,--no-undefined',
        #                            '--sysroot=' + sysroot])

        # env.AppendUnique(CXXFLAGS = ['-m32', '-march=core2', '-mtune=core2',
        #                              '-msse3', '-mfpmath=sse', '-mstackrealign',
        #                              '-fno-omit-frame-pointer',
        #                              '--sysroot=' + sysroot])

        # env.AppendUnique(LINKFLAGS = ['--sysroot=' + sysroot])

        # CONFIGURE_FLAGS="--target=i586-poky-linux --host=i586-poky-linux --build=i386-linux --with-libtool-sysroot=$SDKTARGETSYSROOT"
        # env.AppendUnique(CPPFLAGS = ['--target=i586-poky-linux',
        #                               '--host=i586-poky-linux',
        #                               '--build=i386-linux',
        #                               '--with-libtool-sysroot=' + sysroot])

        # CFLAGS=" -O2 -pipe -g -feliminate-unused-debug-types"
        # env.AppendUnique(CFLAGS = ['-O2', '-pipe', '-g', '-feliminate-unused-debug-types', '-pie'])

        # export CXXFLAGS=" -O2 -pipe -g -feliminate-unused-debug-types"
        # env.AppendUnique(CXXFLAGS = ['-O2', '-pipe', '-g', '-feliminate-unused-debug-types'])
        # env.AppendUnique(CPPPATH = [sysroot + '/usr/include/c++/4.9.1',
        #                             sysroot + '/usr/include/c++/4.9.1/i586-poky-linux'])

        # export LDFLAGS="-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed"
        #env.AppendUnique(LINKFLAGS = ['-Wl,-O1', '-Wl,--hash-style=gnu' '-Wl,--as-needed'])
        # env.AppendUnique(LINKFLAGS = ['-O1', '--hash-style=gnu', '--as-needed'])

        # env.AppendUnique(STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME = 1)

        # export CPPFLAGS=""
        # export KCFLAGS="--sysroot=$SDKTARGETSYSROOT"

        env.AppendUnique(CPPPATH = [env['ENV']['INSTALL_OUT'] + '/include'])
        env.Append(LIBPATH = [env['ENV']['INSTALL_OUT'] + '/lib'])

        # Then: transports
        try:
                ip_api = env['ENV']['IOTIVITY_IP_API']
                if ip_api == 0 :
                        env.Append(CPPDEFINES = ['NO_IP_ADAPTER'])
                else:
                        env.Append(CPPDEFINES = ['IP_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['IP_ADAPTER'])

        try:
                bt_api = env['ENV']['IOTIVITY_BT_API']
                if bt_api:
                        env.Append(CPPDEFINES = ['EDR_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_EDR_ADAPTER'])

        try:
                ble_api = env['ENV']['IOTIVITY_BLE_API']
                if ble_api:
                        env.Append(CPPDEFINES = ['LE_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_LE_ADAPTER'])

        try:
                nfc_api = env['ENV']['IOTIVITY_NFC_API']
                if nfc_api:
                        env.Append(CPPDEFINES = ['NFC_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_NFC_ADAPTER'])

        #FIXME: put this stuff in CPPDEFINES env var?
        # env.Append(CPPDEFINES = ['HAVE_ARPA_INET_H'])
        # env.Append(CPPDEFINES = ['HAVE_ASSERT_H'])
        # env.Append(CPPDEFINES = ['HAVE_FCNTL_H'])
        # env.Append(CPPDEFINES = ['HAVE_LIMITS_H'])
        # env.Append(CPPDEFINES = ['HAVE_NETDB_H'])
        # env.Append(CPPDEFINES = ['HAVE_NETINET_IN_H'])
        # env.Append(CPPDEFINES = ['HAVE_PTHREAD_CONDATTR_SETCLOCK'])
        # env.Append(CPPDEFINES = ['HAVE_PTHREAD_H'])
        # env.Append(CPPDEFINES = ['HAVE_STDLIB_H'])
        # env.Append(CPPDEFINES = ['HAVE_STRING_H'])
        # env.Append(CPPDEFINES = ['HAVE_SYS_SOCKET_H'])
        # env.Append(CPPDEFINES = ['HAVE_SYS_TIME_H'])
        # env.Append(CPPDEFINES = ['HAVE_SYS_TYPES_H'])
        # env.Append(CPPDEFINES = ['HAVE_TIME_H'])
        # env.Append(CPPDEFINES = ['HAVE_UNISTD_H'])
        # env.Append(CPPDEFINES = ['WITH_POSIX'])

        env.Append(LIBS = ['pthread'])
        env.Append(LIBS = ['boost_date_time'])
        env.Append(LIBS = ['boost_system'])
        env.Append(LIBS = ['boost_thread'])
        # env.AppendUnique(CXXFLAGS = ['-std=c++11'])
        # env.AppendUnique(CFLAGS = ['-std=c11'])
        # env.AppendUnique(LINKFLAGS = ['-shared'])
        # env.Replace(SHLIBSUFFIX = '.so')

        return env

def generic(env):
        print "SETUP.TARGET.GENERIC()"
        #FIXME: currently this is just for POC.
        # TODO: elim hardcoded stuff, use sourced env vars to drive config
        # TODO: get the versioning of the tools, needed for linking to crystax boost libs

        # First: tools
        # env.Replace(CC = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-gcc')
        # env.Replace(CXX = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-g++')
        # env.Replace(LINK = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-g++')
        # env.Replace(LD = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-ld')
        # env.Replace(AR = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-ar')
        # env.Replace(RANLIB = env['ENV']['TOOLCHAIN_DIR'] + '/bin/arm-linux-androideabi-ranlib')

        # Then: transports
        try:
                ip_api = env['ENV']['IOTIVITY_IP_API']
                if ip_api:
                        env.Append(CPPDEFINES = ['IP_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_IP_ADAPTER'])

        try:
                bt_api = env['ENV']['IOTIVITY_BT_API']
                if bt_api:
                        env.Append(CPPDEFINES = ['EDR_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_EDR_ADAPTER'])

        try:
                ble_api = env['ENV']['IOTIVITY_BLE_API']
                if ble_api:
                        env.Append(CPPDEFINES = ['LE_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_LE_ADAPTER'])

        try:
                nfc_api = env['ENV']['IOTIVITY_NFC_API']
                if nfc_api:
                        env.Append(CPPDEFINES = ['NFC_ADAPTER'])
        except KeyError:
                env.Append(CPPDEFINES = ['NO_NFC_ADAPTER'])
        env.Append(CPPDEFINES = ['HAVE_ARPA_INET_H'])
        env.Append(CPPDEFINES = ['HAVE_ASSERT_H'])
        env.Append(CPPDEFINES = ['HAVE_FCNTL_H'])
        env.Append(CPPDEFINES = ['HAVE_LIMITS_H'])
        env.Append(CPPDEFINES = ['HAVE_NETDB_H'])
        env.Append(CPPDEFINES = ['HAVE_NETINET_IN_H'])
        env.Append(CPPDEFINES = ['HAVE_PTHREAD_H'])
        env.Append(CPPDEFINES = ['HAVE_SYS_SOCKET_H'])
        env.Append(CPPDEFINES = ['HAVE_SYS_TIME_H'])
        env.Append(CPPDEFINES = ['HAVE_SYS_TYPES_H'])
        env.Append(CPPDEFINES = ['HAVE_TIME_H'])
        env.Append(CPPDEFINES = ['WITH_POSIX'])
        env.Append(CPPDEFINES = ['__ANDROID__'])
        env.Append(CPPDEFINES = ['__JAVA__'])
        env.Append(CPPDEFINES = ['WITH_POSIX'])
        #GAR FIXME
        # env.Append(LIBPATH = ['/Users/gar/android/crystax-ndk-10.3.2/sources/boost/1.59/libs/armeabi-v7a/llvm-3.6/'])
        env.Append(CPPPATH = ['/Users/gar/android/crystax-ndk-10.3.2/sources/boost/1.59.0/include'])
        env.Append(LIBPATH = ['/Users/gar/android/crystax-ndk-10.3.2/sources/boost/1.59.0/libs/armeabi-v7a/gnu-4.9/'])
        env.Append(LIBS = ['boost_date_time'])
        env.Append(LIBS = ['boost_system'])
        env.Append(LIBS = ['boost_thread'])
        env.AppendUnique(CXXFLAGS = ['-std=c++11'])
        env.AppendUnique(CFLAGS = ['-std=c11'])
        env.AppendUnique(LINKFLAGS = ['-shared'])
        env.Replace(SHLIBSUFFIX = '.so')

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

        # home        = os.environ['HOME']

# cppdefs   = {'CPPDEFINES': features_env['CPPDEFINES']}
# #cppflags  = {'CPPFLAGS': features_env['CPPFLAGS']}
# pthreadcflags  = {'PTHREAD_CFLAGS': features_env['PTHREAD_CFLAGS']}
# pthreadlibs   = {'PTHREAD_LIBS': features_env['PTHREAD_LIBS']}

# env.MergeFlags(cppdefs)
# #env.MergeFlags(cppflags)
# env.MergeFlags(pthreadcflags)
# env.MergeFlags(pthreadlibs)

# print "CPPDEFINES", env.get('CPPDEFINES')
# print "CPPFLAGS", env.get('CPPFLAGS')


# try:
# 	env.AppendUnique(HAVE_LIBCURL = features_env['HAVE_LIBCURL'])
# except:
# 	pass
