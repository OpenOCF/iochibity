import sys
import os
import platform
# from sys import platform as _platform
from SCons.Script import *

path = 'log'
try:
    os.mkdir(path)
except  OSError as e:
    if not os.path.isdir(path):
        raise

# print "ARGS: ", sys.argv

# argarray = sys.argv[1:]
# if len(sys.argv) > 1:
#     args = dict( (n,v) for n,v in (a.split('=') for a in argarray) )
# else:
#     args = {}

# print "RELEASE? ", args['RELEASE']
# print "SECURED? ", args['SECURED']
# print "VERBOSE?",  args['VERBOSE']
# #print "DBUG?",    args['DBUG']
# print "TARGET OS:", args['TARGET_OS']

logfile = path + "/build"
# if args.get('RELEASE','true') == 'true':
#     logfile = logfile + ".release"
# else:
#     logfile = logfile + ".debug"
# if args.get('SECURED',1) == 1 :
#     logfile = logfile + ".secured"
# else:
#     logfile = logfile + ".unsecured"

logfile = logfile + ".log"

if sys.platform.startswith('linux'):
    sys_platform = 'linux'
elif sys.platform.startswith('darwin'):
    sys_platform = 'darwin'
else:
    sys_platform = sys.platform

print "sys.platform:     " + sys.platform
print "sys.platform normalized:     " + sys_platform
print "platform.architecture:  ", platform.architecture()
print "platform.machine:  " + platform.machine()
print "platform.processor:  " + platform.processor()
print "platform.system:  " + platform.system()
print "platform.release: " + platform.release()
print "os.name:          " + os.name
sys.stdout.flush()

if sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
    sys.stdout = os.popen("tee " + logfile, "w")
    sys.stderr = sys.stdout
# elif sys.platform == "win32":
#     #  tee?

print "SCons Initializing"
# print "Target:", sys.platform, "release:", args.get('RELEASE','true'), " secured:", args.get('SECURED',1)

# put this in gerrit?
import subprocess
if sys.platform.startswith('darwin'):
    # Linux-specific code here...
    print  subprocess.Popen(["sw_vers"], stdout=subprocess.PIPE).communicate()[0]
    print "uname: " + subprocess.Popen(["uname", "-a"], stdout=subprocess.PIPE).communicate()[0]
    print "clang:"
    print subprocess.Popen(["clang", "--version"], stdout=subprocess.PIPE).communicate()[0]
    print subprocess.Popen(["clang", "--print-search-dirs"], stdout=subprocess.PIPE).communicate()[0]
    print "gcc:"
    print subprocess.Popen(["gcc", "--version"], stdout=subprocess.PIPE).communicate()[0]

if sys.platform.startswith('linux'):
    print "uname: " + subprocess.Popen(["uname", "-a"], stdout=subprocess.PIPE).communicate()[0]
    print "gcc:"
    print subprocess.Popen(["gcc", "--version"], stdout=subprocess.PIPE).communicate()[0]
    print "cpp:"
    print subprocess.Popen(["gcc", "-dM", "-E", "site_scons/foo.h"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]
elif sys.platform.startswith('freebsd'):
    pass
elif sys.platform.startswith('win32'):
    pass



# System	platform value
# Linux (2.x and 3.x)	'linux2'
# Windows	'win32'
# Windows/Cygwin	'cygwin'
# Mac OS X	'darwin'
# OS/2	'os2'
# OS/2 EMX	'os2emx'
# RiscOS	'riscos'
# AtheOS	'atheos'

