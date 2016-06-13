import sys
import os
from sys import platform as _platform
from SCons.Script import *

path = 'log'
try:
    os.mkdir(path)
except  OSError as e:
    if not os.path.isdir(path):
        raise

print "ARGS: ", sys.argv

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

if _platform == "linux" or _platform == "linux2" or _platform == "darwin":
    sys.stdout = os.popen("tee " + logfile, "w")
    sys.stderr = sys.stdout
# elif _platform == "win32":
#     #  tee?

print "SCons Initializing"
# print "Target:", _platform, "release:", args.get('RELEASE','true'), " secured:", args.get('SECURED',1)

print "Host:"

# put this in gerrit?
import subprocess
if _platform == "darwin":
    print  subprocess.Popen(["sw_vers"], stdout=subprocess.PIPE).communicate()[0]
    print "uname: " + subprocess.Popen(["uname", "-a"], stdout=subprocess.PIPE).communicate()[0]
    print "clang:"
    print subprocess.Popen(["clang", "--version"], stdout=subprocess.PIPE).communicate()[0]
    print subprocess.Popen(["clang", "--print-search-dirs"], stdout=subprocess.PIPE).communicate()[0]
    print "gcc:"
    print subprocess.Popen(["gcc", "--version"], stdout=subprocess.PIPE).communicate()[0]

if _platform == "linux" or _platform == "linux2":
    print "uname: " + subprocess.Popen(["uname", "-a"], stdout=subprocess.PIPE).communicate()[0]
    print "gcc:"
    print subprocess.Popen(["gcc", "--version"], stdout=subprocess.PIPE).communicate()[0]
    print "cpp:"
    print subprocess.Popen(["gcc", "-dM", "-E", "site_scons/foo.h"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]
