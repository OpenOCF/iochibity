/******************************************************************
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
#include "caipnwmonitor_posix.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <sys/select.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <errno.h>

#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include "utlist.h"

#define TAG "OIC_LINUX_CA_IP_MONITOR"

u_arraylist_t *CAFindInterfaceChange()
{
    u_arraylist_t *iflist = NULL;
#if defined(__linux__) || defined (__ANDROID__) /* GAR:  Darwin support */
    char buf[4096] = { 0 };
    struct nlmsghdr *nh = NULL;
    struct sockaddr_nl sa = { .nl_family = 0 };
    struct iovec iov = { .iov_base = buf,
                         .iov_len = sizeof (buf) };
    struct msghdr msg = { .msg_name = (void *)&sa,
                          .msg_namelen = sizeof (sa),
                          .msg_iov = &iov,
                          .msg_iovlen = 1 };

    ssize_t len = recvmsg(caglobals.ip.netlinkFd, &msg, 0);

    for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len))
    {
        if (nh != NULL && (nh->nlmsg_type != RTM_DELADDR && nh->nlmsg_type != RTM_NEWADDR))
        {
	    /* GAR: what about RTM_NEWLINK, RTM_DELLINK? */
            continue;
        }

        if (RTM_DELADDR == nh->nlmsg_type)
        {
            struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA (nh);
            if (ifa)
            {
                int ifiIndex = ifa->ifa_index;
                bool isFound = CACmpNetworkList(ifiIndex);
                if (isFound)
                {
                    CARemoveNetworkMonitorList(ifiIndex);
                    CAIPPassNetworkChangesToAdapter(CA_INTERFACE_DOWN);
                }
            }
            continue;
        }

        // Netlink message type is RTM_NEWADDR.
        struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA (nh);
        if (ifa)
        {
            int ifiIndex = ifa->ifa_index;
	    /* FIXME: BUG. what if > 1 new addrs? only last will be in iflist */
            iflist = CAIPGetInterfaceInformation(ifiIndex);
            if (!iflist)
            {
                OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
                return NULL;
            }
	    /* GAR: CAProcessNewInterfaceItem? (android) */
        }
    }
#endif
    return iflist;
}