
/** @file udp_status_manager_darwin.c
 *
 * @was: caipserver_darwin.c
 */
/* ****************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542 // for PKTINFO
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for in6_pktinfo
#endif

#include "udp_status_manager_ios.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif

#include <pthread.h>

#include <errno.h>

#include <inttypes.h>

#if INTERFACE
#include <SystemConfiguration/SystemConfiguration.h>
#endif

#define USE_IP_MREQN

/*
 * Logging tag for module name
 */
#define TAG "OIC_DARWIN_CA_IP_SERVER"

void CADeInitializeMonitorGlobals(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    /* Currently, same as for posix. that will change */
    if (udp_shutdownFds[0] != -1)
    {
        close(udp_shutdownFds[0]);
        udp_shutdownFds[0] = -1;
    }
    // no netlink on darwin
    /* if (udp_netlinkFd != OC_INVALID_SOCKET) */
    /* { */
    /*     close(udp_netlinkFd); */
    /*     udp_netlinkFd = OC_INVALID_SOCKET; */
    /* } */
}

void *ip_watcher(void *data)
{
    SCDynamicStoreRef storeRef = NULL;
    CFRunLoopSourceRef sourceRef = NULL;
    OSStatus result;

    /* result = CreateIPAddressListChangeCallbackSCF(NULL, /\* void* contextPtr *\/ */
    /*     					  &storeRef, */
    /*     					  &sourceRef); */
    /* if (result != noErr) { */
    /*     OIC_LOG_V(ERROR, TAG, "Register for addr change failed: %s", result); */
    /* } */
    /* CFRunLoopRef runLoop = CFRunLoopGetCurrent(); */
    /* // https://developer.apple.com/documentation/corefoundation/1543356-cfrunloopaddsource?language=objc */
    /* CFRunLoopAddSource(runLoop, sourceRef, kCFRunLoopDefaultMode); */
    /* while(true) // _threadKeepGoing)   // may be set to false by main thread at shutdown time */
    /*     { */
    /*         CFRunLoopRun(); */
    /*     } */

    /* // cleanup time:  release our resources */
    /* CFRunLoopRemoveSource(CFRunLoopGetCurrent(), sourceRef, kCFRunLoopDefaultMode); */
    /* CFRelease(storeRef); */
    /* CFRelease(sourceRef); */
}

void launch_ip_watcher(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);

   // Create the thread using POSIX routines.
    pthread_attr_t  attr;
    pthread_t       posixThreadID;
    int             returnVal;

    returnVal = pthread_attr_init(&attr);
    assert(!returnVal);
    returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    assert(!returnVal);

    int     threadError = pthread_create(&posixThreadID, &attr, &ip_watcher, NULL);

    returnVal = pthread_attr_destroy(&attr);
    assert(!returnVal);
    if (threadError != 0)
    {
         // Report an error.
    }
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

void CARegisterForAddressChanges(void)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    launch_ip_watcher();
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

/* void CAInitializeFastShutdownMechanism() */
/* { */
/*     OIC_LOG_V(DEBUG, TAG, "IN %s", __func__); */
/*     udp_selectTimeout = -1; // don't poll for shutdown */
/*     int ret = -1; */
/*     ret = pipe(udp_shutdownFds); */
/*     if (-1 != ret) */
/*     { */
/*         ret = fcntl(udp_shutdownFds[0], F_GETFD); */
/*         if (-1 != ret) */
/*         { */
/*             ret = fcntl(udp_shutdownFds[0], F_SETFD, ret|FD_CLOEXEC); */
/*         } */
/*         if (-1 != ret) */
/*         { */
/*             ret = fcntl(udp_shutdownFds[1], F_GETFD); */
/*         } */
/*         if (-1 != ret) */
/*         { */
/*             ret = fcntl(udp_shutdownFds[1], F_SETFD, ret|FD_CLOEXEC); */
/*         } */
/*         if (-1 == ret) */
/*         { */
/*             close(udp_shutdownFds[1]); */
/*             close(udp_shutdownFds[0]); */
/*             udp_shutdownFds[0] = -1; */
/*             udp_shutdownFds[1] = -1; */
/*         } */
/*     } */
/*     UDP_CHECKFD(udp_shutdownFds[0]); */
/*     UDP_CHECKFD(udp_shutdownFds[1]); */
/* /\* #endif *\/ */
/*     if (-1 == ret) */
/*     { */
/*         OIC_LOG_V(ERROR, TAG, "fast shutdown mechanism init failed: %s", CAIPS_GET_ERROR); */
/*         udp_selectTimeout = SELECT_TIMEOUT; //poll needed for shutdown */
/*     } */
/*     OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__); */
/* } */

/* https://stackoverflow.com/questions/3613521/udp-socket-network-disconnect-behavior-on-windows-linux-macq */

/* https://developer.apple.com/library/content/technotes/tn1145/_index.html#//apple_ref/doc/uid/DTS10002984-CH1-SECGETTINGIPLIST */


// @rewrite udp_if_change_handler_darwin @was CAFindInterfaceChange
// FIXME: this routine is transport independent? rename to ip_if_ch...
void udp_if_change_handler_darwin(SCDynamicStoreRef store, CFArrayRef changedKeys, void *info)
{
    OIC_LOG_V(DEBUG, TAG, "%s ENTRY", __func__);
    CFIndex nameCount = CFArrayGetCount( changedKeys );
    const char *strName;
    const char *strVal;
    /* for( int i = 0; i < nameCount ; ++i  ) { */
    /*     CFStringRef key = (CFStringRef)CFArrayGetValueAtIndex( changedKeys, i ); */
    /*     strName = CFStringGetCStringPtr( key , kCFStringEncodingMacRoman ); */
    /*     OIC_LOG_V(DEBUG, TAG, "Change key: %s", strName); */
    /*     CFPropertyListRef prop; */
    /*     prop = SCDynamicStoreCopyValue(store, key); */
    /*     CFStringRef val = CFCopyDescription(prop); */
    /*     strVal = CFStringGetCStringPtr( val , kCFStringEncodingMacRoman ); */
    /*     OIC_LOG_V(DEBUG, TAG, "Change val: %s", strVal); */
    /*     CFShow(prop); */
    /* } */
    // For now, process all IFs
    // u_arraylist_t *iflist = CAFindInterfaceChange();

    // NB: CAFindInterfaceChange calls
    // CAIPPassNetworkChangesToTransports for deletions,
    // udp_get_ifs_for_rtm_newaddr(ifiIndex) for additions

    u_arraylist_t *iflist = udp_get_ifs_for_rtm_newaddr(0);
    if (iflist) {
	size_t listLength = u_arraylist_length(iflist);
	for (size_t i = 0; i < listLength; i++)
	    {
		CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
		if (ifitem)
                    {
                        udp_add_if_to_multicast_groups(ifitem); // @was CAProcessNewInterface
                    }
	    }
	u_arraylist_destroy(iflist);
    } else {
	OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
    }

    // udp_if_change_handler(CA_INTERFACE_UP); // @was CAIPPassNetworkChangesToTransports
#ifndef DISABLE_UDP
    udp_status_change_handler(CA_ADAPTER_IP, CA_INTERFACE_UP); // @was CAIPAdapterHandler
#endif
#ifdef ENABLE_TCP
    tcp_interface_change_handler(CA_ADAPTER_IP, CA_INTERFACE_UP); // @was CATCPAdapterHandler
#endif

    // CFRelease(store);
    OIC_LOG_V(DEBUG, TAG, "%s EXIT", __func__);
}

// Error Handling
// --------------
// SCF returns errors in two ways:
//
// o The function result is usually set to something
//   generic (like NULL or false) to indicate an error.
//
// o There is a call, SCError, that returns the error
//   code for the most recent function. These error codes
//   are not in the OSStatus domain.
//
// We deal with this using two functions, MoreSCError
// and MoreSCErrorBoolean. Both of these take a generic
// failure indicator (a pointer or a Boolean) and, if
// that indicates an error, they call SCError to get the
// real error code. They also act as a bottleneck for
// mapping SC errors into the OSStatus domain, although
// I don't do that in this simple implementation.
//
// Note that I could have eliminated the failure indicator
// parameter and just called SCError but I'm worried
// about SCF returning an error indicator without setting
// the SCError. There's no justification for this worry
// other than general paranoia (I know of no examples where
// this happens),

static OSStatus MoreSCErrorBoolean(Boolean success)
{
    OSStatus err;
    int scErr;

    err = noErr;
    if ( ! success ) {
        scErr = SCError();
        if (scErr == kSCStatusOK) {
            scErr = kSCStatusFailed;
        }
        // Return an SCF error directly as an OSStatus.
        // That's a little cheesy. In a real program
        // you might want to do some mapping from SCF
        // errors to a range within the OSStatus range.
        err = scErr;
    }
    return err;
}

static OSStatus MoreSCError(const void *value)
{
    return MoreSCErrorBoolean(value != NULL);
}

static OSStatus CFQError(CFTypeRef cf)
    // Maps Core Foundation error indications (such as they
    // are) to the OSStatus domain.
{
    OSStatus err;

    err = noErr;
    if (cf == NULL) {
        // err = coreFoundationUnknownErr;
    }
    return err;
}

static void CFQRelease(CFTypeRef cf)
    // A version of CFRelease that's tolerant of NULL.
{
    if (cf != NULL) {
        CFRelease(cf);
    }
}

/* see https://github.com/erluko/netwatcher/blob/master/netwatcher.cpp */
/* https://public.msli.com/lcs/jaf/osx_ip_change_notify.cpp */
