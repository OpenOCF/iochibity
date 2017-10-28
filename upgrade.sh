#!/bin/sh -e

IOTIVITY=$HOME/iotivity/master-git
# run this from an empty temp directory

echo $IOTIVITY

mkdir -pv src/util

# CONNECTIVITY
COMM="$IOTIVITY/resource/csdk/connectivity"
echo "COMM=$COMM"
mkdir -pv src/comm/api
mkdir -pv src/comm/common
mkdir -pv src/comm/adapter_util
mkdir -pv src/comm/interface
mkdir -pv src/comm/security
mkdir -pv src/comm/transport/ble
mkdir -pv src/comm/transport/bredr
mkdir -pv src/comm/transport/nfc
mkdir -pv src/comm/transport/tcpip
mkdir -pv src/comm/transport/udpip
mkdir -vp src/comm/transport/udpip/android
mkdir -vp src/comm/transport/udpip/linux
# mkdir -vp src/comm/transport/udpip/tizen TODO
mkdir -vp src/comm/transport/udpip/windows

# OMIT cp -v $COMM/src/ip_adapter/arduino

mkdir -pv src/comm/transport/util
mkdir -pv src/comm/transport/xmpp
mkdir -vp src/comm/transport/udpip/linux

mkdir -pv src/comm/util

cp -v $COMM/api/cacommon.h           src/comm/api
cp -v $COMM/api/cainterface.h         src/comm/api
cp -v $COMM/api/casecurityinterface.h src/comm/api
cp -v $COMM/api/cautilinterface.h     src/comm/util

# OMIT: $COMM/build/*

cp -v $COMM/common/inc/cacommonutil.h                src/comm/common
cp -v $COMM/common/inc/caremotehandler.h             src/comm/common
cp -v $COMM/common/inc/cathreadpool.h                src/comm/common
cp -v $COMM/common/inc/uarraylist.h                  src/util
cp -v $COMM/common/inc/ulinklist.h                   src/comm/common
cp -v $COMM/common/inc/uqueue.h                      src/comm/common

cp -v $COMM/common/src/caremotehandler.c             src/comm/common
cp -v $COMM/common/src/cathreadpool_pthreads.c       src/comm/common
cp -v $COMM/common/src/uarraylist.c                  src/util
cp -v $COMM/common/src/ulinklist.c                   src/comm/common
cp -v $COMM/common/src/uqueue.c                      src/comm/common

cp -v $COMM/inc/ca_adapter_net_ssl.h                 src/comm/security
cp -v $COMM/inc/caadapterinterface.h                 src/comm/interface
cp -v $COMM/inc/caadapternetdtls.h                   src/comm
cp -v $COMM/inc/caadapterutils.h                     src/comm/transport/util
cp -v $COMM/inc/cablockwisetransfer.h                src/comm
cp -v $COMM/inc/caedradapter.h                       src/comm/transport/bredr
cp -v $COMM/inc/caedradapter_singlethread.h          src/comm/transport/bredr
cp -v $COMM/inc/caedrinterface.h                     src/comm/transport/bredr
cp -v $COMM/inc/cafragmentation.h                    src/comm/transport/ble
cp -v $COMM/inc/cagattservice.h                      src/comm/transport/ble
cp -v $COMM/inc/cainterfacecontroller.h              src/comm
cp -v $COMM/inc/cainterfacecontroller_singlethread.h src/comm
cp -v $COMM/inc/caipadapter.h                        src/comm/transport/udpip
cp -v $COMM/inc/caipinterface.h                      src/comm/transport/udpip
cp -v $COMM/inc/caipnwmonitor.h                      src/comm/transport/udpip
cp -v $COMM/inc/caleadapter.h                        src/comm/transport/ble
cp -v $COMM/inc/caleinterface.h                      src/comm/transport/ble
cp -v $COMM/inc/camessagehandler.h                   src/comm
cp -v $COMM/inc/canetworkconfigurator.h              src/comm
cp -v $COMM/inc/canfcadapter.h                       src/comm/transport/nfc
cp -v $COMM/inc/canfcinterface.h                     src/comm/transport/nfc
cp -v $COMM/inc/caprotocolmessage.h                  src/comm/interface
cp -v $COMM/inc/caqueueingthread.h                   src/comm
cp -v $COMM/inc/caraadapter.h                        src/comm/transport/xmpp
cp -v $COMM/inc/caretransmission.h                   src/comm
cp -v $COMM/inc/catcpadapter.h                       src/comm/transport/tcpip
cp -v $COMM/inc/catcpinterface.h                     src/comm/transport/tcpip

# OMIT $COMM/lib  - moved to third_party

cp -v $COMM/src/adapter_util/ca_adapter_net_ssl.c src/comm/security
cp -v $COMM/src/adapter_util/caadapterutils.c src/comm/transport/util
cp -v $COMM/src/adapter_util/cafragmentation.c src/comm/transport/ble

cp -v $COMM/src/bt_edr_adapter/android src/comm/transport/bredr

mkdir -pv src/comm/bt_edr_adapter
cp -v $COMM/src/bt_edr_adapter/caedradapter.c src/comm/transport/bredr

# cp -v $COMM/src/bt_edr_adapter/tizen TODO

cp -v $COMM/src/bt_le_adapter/android/* src/comm/transport/ble/ble/android
# OMIT $COMM/src/src/bt_le_adapter/arduino
cp -v $COMM/src/bt_le_adapter/caleadapter.c src/comm/transport/ble
cp -v $COMM/src/bt_le_adapter/linux/* src/comm/transport/ble/linux
rm -v src/comm/bt_le_adapter/linux/SConscript
#TODO cp -v $COMM/src/bt_le_adapter/tizen src/comm/transport/ble/tizen

cp -v $COMM/src/cablockwisetransfer.c src/comm
cp -v $COMM/src/caconnectivitymanager.c src/comm
cp -v $COMM/src/cainterfacecontroller.c src/comm
cp -v $COMM/src/camessagehandler.c src/comm
cp -v $COMM/src/canetworkconfigurator.c src/comm
cp -v $COMM/src/caprotocolmessage.c src/comm
cp -v $COMM/src/caqueueingthread.c src/comm
cp -v $COMM/src/caretransmission.c src/comm

cp -v $COMM/src/ip_adapter/android src/comm/transport/udpip/android
cp -v $COMM/src/ip_adapter/caipadapter.c src/comm/transport/udpip
cp -v $COMM/src/ip_adapter/caipserver.c src/comm/transport/udpip
cp -v $COMM/src/ip_adapter/linux/caipnwmonitor.c  src/comm/transport/udpip/linux
# cp -v $COMM/src/ip_adapter/tizen/caipnwmonitor.c  src/comm/network/ip_adapter/tizen TODO
cp -v $COMM/src/ip_adapter/windows/caipnwmonitor.c  src/comm/transport/udpip/windows

cp -v $COMM/src/nfc_adapter/android src/comm/nfc_adapter/android/* src/comm/transport/nfc/android
cp -v $COMM/src/nfc_adapter/canfcadapter.c src/comm/transport/nfc

cp -v $COMM/src/ra_adapter/caraadapter.c src/comm/transport/xmpp

# OMIT cp -v $COMM/src/tcp_adapter/arduino
cp -v $COMM/src/tcp_adapter/catcpadapter.c src/comm/transport/tcpip
cp -v $COMM/src/tcp_adapter/catcpserver.c src/comm/transport/tcpip

mkdir -pv test/connectivity
cp -vr $COMM/test/* test/connectivity
rm -v test/connectivity/SConscript

cp -v $COMM/util/inc/cabtpairinginterface.h src/comm/util
cp -v $COMM/util/inc/caconnectionmanager.h  src/comm/util
cp -v $COMM/util/inc/camanagerleinterface.h src/comm/util
cp -v $COMM/util/src/cautilinterface.c  src/comm/util

# cp -v $COMM/util/src/btpairing/android  TODO
cp -rv $COMM/util/src/camanager src/comm/util


#### INCLUDE (API)
mkdir -pv src/ocf
cp -v $IOTIVITY/resource/csdk/include/octypes.h src/ocf

#### LOGGER
LOGGER="$IOTIVITY/resource/csdk/logger"
cp -v $LOGGER/include/experimental/logger.h src/logger
cp -v $LOGGER/include/experimental/logger_types.h src/logger
cp -v $LOGGER/include/trace.h src/logger
cp -v $LOGGER/src/logger.c src/logger
cp -v $LOGGER/src/trace.c src/logger
mkdir -pv test/logger
cp -vr $LOGGER/test test/logger

#### RESOURCE DIRECTORY  - TODO

# cp -v $IOTIVITY/resource-directory  TODO

#### ROUTING  -  TODO


#### PORTABILITY
COMMON="$IOTIVITY/resource/c_common"

mkdir -pv src/portability/noop
mkdir -pv src/portability/posix
mkdir -pv src/portability/windows
mkdir -pv test/portability

cp -v $COMMON/experimental/byte_array.h	src/util
cp -v $COMMON/iotivity_commontypes.h src/portability
cp -v $COMMON/iotivity_debug.h src

cp -v $COMMON/ocatomic/include/ocatomic.h src/portability
cp -v $COMMON/ocatomic/src/others/ocatomic.c src/portability/posix
cp -v $COMMON/ocatomic/src/windows/ocatomic.c src/portability/windows

cp -v $COMMON/ocevent/include/ocevent.h src/portability
cp -v $COMMON/ocevent/src/others/ocevent.c src/portability/posix
cp -v $COMMON/ocevent/src/windows/ocevent.c src/portability/windows
cp -v $COMMON/ocevent/test/eventtest.cpp test/portability

cp -v $COMMON/ocrandom/include/experimental/ocrandom.h src/portability
cp -v $COMMON/ocrandom/src/ocrandom.c src/portability
cp -v $COMMON/ocrandom/test/randomtest.cpp test/portability
mkdir -pv test/portability/android
cp -v $COMMON/ocrandom/test/android/randomtest.cpp test/portability/android
# OMIT cp -v $COMMON/ocrandom/test/arduino/randomtest.cpp

cp -v $COMMON/octhread/include/octhread.h src/portability
cp -v $COMMON/octhread/src/noop/octhread.c src/portability/noop
cp -v $COMMON/octhread/src/posix/octhread.c src/portability/posix
cp -v $COMMON/octhread/src/windows/octhread.c src/portability/windows

cp -v $COMMON/octimer/include/octimer.h src/portability
cp -v $COMMON/octimer/src/octimer.c src/portability

cp -v $COMMON/oic_malloc/include/oic_malloc.h src/portability
cp -v $COMMON/oic_malloc/src/oic_malloc.c src/portability
mkdir -pv test/portability/linux
cp -v $COMMON/oic_malloc/test/linux/oic_malloc_tests.cpp test/portability/linux

cp -v $COMMON/oic_platform/include/oic_platform.h src/portability
cp -v $COMMON/oic_platform/src/oic_platform.c src/portability
cp -v $COMMON/oic_platform/src/others/oic_otherplatforms.c src/portability/noop
cp -v $COMMON/oic_platform/src/windows/oic_winplatform.cpp src/portability/windows

cp -v $COMMON/oic_string/include/oic_string.h src/portability
cp -v $COMMON/oic_string/src/oic_string.c src/portability
cp -rv $COMMON/oic_string/test/linux/oic_string_tests.cpp test/portability/linux

cp -v $COMMON/oic_time/include/oic_time.h src/portability
cp -v $COMMON/oic_time/src/oic_time.c src/portability
cp -rv $COMMON/oic_time/test/linux/oic_time_tests.cpp test/portability/linux

cp -v $COMMON/platform_features.h src/system.h   # INSPECTION REQUIRED

# OMIT $COMMON/unittests
# OMIT $IOTIVITY/resource/csdk/unittests

cp -v $COMMON/utlist.h src/util

cp -v $COMMON/windows/include/getopt.h src/portability/windows
cp -v $COMMON/windows/include/memmem.h src/portability/windows
cp -v $COMMON/windows/include/pthread_create.h src/portability/windows
cp -v $COMMON/windows/include/vs12_snprintf.h src/portability/windows
cp -v $COMMON/windows/include/win_sleep.h src/portability/windows
cp -v $COMMON/windows/src/getopt.c src/portability/windows
cp -v $COMMON/windows/src/memmem.c src/portability/windows
cp -v $COMMON/windows/src/pthread_create.c src/portability/windows
cp -v $COMMON/windows/src/snprintf.c src/portability/windows
cp -v $COMMON/windows/src/win_sleep.c src/portability/windows
mkdir -pv test/portability/windows
cp -rv $COMMON/windows/test/snprintf_test.cpp test/portability/windows

#### PROVISIONING
PROVISIONING="$IOTIVITY/resource/csdk/security/provisioning"
mkdir -pv src/provisioning/multiowner

#OMIT cp -v $PROVISIONING/include/cloud

cp -v $PROVISIONING/include/ocprovisioningmanager.h src/provisioning
cp -v $PROVISIONING/include/pmtypes.h src/provisioning
cp -v $PROVISIONING/include/pmutility.h src/provisioning
cp -v $PROVISIONING/include/internal/credentialgenerator.h src/provisioning
cp -v $PROVISIONING/include/internal/multipleownershiptransfermanager.h src/provisioning/multiowner
cp -v $PROVISIONING/include/internal/otmcontextlist.h src/provisioning
cp -v $PROVISIONING/include/internal/ownershiptransfermanager.h src/provisioning
cp -v $PROVISIONING/include/internal/pmutilityinternal.h src/provisioning
cp -v $PROVISIONING/include/internal/provisioningdatabasemanager.h src/provisioning
cp -v $PROVISIONING/include/internal/secureresourceprovider.h src/provisioning
cp -v $PROVISIONING/include/oxm/oxmjustworks.h src/provisioning
cp -v $PROVISIONING/include/oxm/oxmmanufacturercert.h src/provisioning
cp -v $PROVISIONING/include/oxm/oxmpreconfpin.h src/provisioning/multiowner
cp -v $PROVISIONING/include/oxm/oxmrandompin.h src/provisioning

# OMIT $PROVISIONING/src/cloud
cp -v $PROVISIONING/src/credentialgenerator.c src/provisioning
cp -v $PROVISIONING/src/multipleownershiptransfermanager.c src/provisioning/multiowner
cp -v $PROVISIONING/src/ocprovisioningmanager.c src/provisioning
cp -v $PROVISIONING/src/otmcontextlist.c src/provisioning
cp -v $PROVISIONING/src/ownershiptransfermanager.c src/provisioning
cp -v $PROVISIONING/src/oxmjustworks.c src/provisioning
cp -v $PROVISIONING/src/oxmmanufacturercert.c src/provisioning
cp -v $PROVISIONING/src/oxmpreconfpin.c src/provisioning/multiowner
cp -v $PROVISIONING/src/oxmrandompin.c src/provisioning
cp -v $PROVISIONING/src/pmutility.c src/provisioning
cp -v $PROVISIONING/src/provisioningdatabasemanager.c src/provisioning
cp -v $PROVISIONING/src/secureresourceprovider.c src/provisioning

mkdir -pv examples-bazel/provisioning
cp -vr $PROVISIONING/sample examples-bazel/provisioning

mkdir -pv test/provisioning
cp -vr $PROVISIONING/unittest/* test/provisioning
rm -v test/provisioning/SConscript

#### SECURITY
SEC="$IOTIVITY/resource/csdk/security"
mkdir -pv src/sec

cp -v $SEC/include/base64.h src/util
cp -v $SEC/include/experimental/securevirtualresourcetypes.h src/sec
cp -v $SEC/include/internal/acl_logging.h src/sec
cp -v $SEC/include/internal/aclresource.h src/sec
cp -v $SEC/include/internal/amaclresource.h src/sec
cp -v $SEC/include/internal/certhelpers.h src/sec
cp -v $SEC/include/internal/credresource.h src/sec
cp -v $SEC/include/internal/crl_logging.h src/sec
cp -v $SEC/include/internal/crlresource.h src/sec
cp -v $SEC/include/internal/csrresource.h src/sec
cp -v $SEC/include/internal/deviceonboardingstate.h src/sec
cp -v $SEC/include/internal/doxmresource.h src/sec
cp -v $SEC/include/internal/policyengine.h src/sec
cp -v $SEC/include/internal/psinterface.h src/sec
cp -v $SEC/include/internal/pstatresource.h src/sec
cp -v $SEC/include/internal/resourcemanager.h src/sec
cp -v $SEC/include/internal/rolesresource.h src/sec
cp -v $SEC/include/internal/secureresourcemanager.h src/sec
cp -v $SEC/include/internal/security_internals.h src/sec
cp -v $SEC/include/internal/srmresourcestrings.h src/sec
cp -v $SEC/include/iotvticalendar.h src/sec
cp -v $SEC/include/occertutility.h src/sec
cp -v $SEC/include/ocsecurity.h src/sec
cp -v $SEC/include/oxmverifycommon.h src/sec
cp -v $SEC/include/pbkdf2.h src/sec
cp -v $SEC/include/pinoxmcommon.h src/sec
cp -v $SEC/include/pkix_interface.h src/sec
cp -v $SEC/include/srmutility.h src/sec

cp -v $SEC/src/aclresource.c src/sec
cp -v $SEC/src/amaclresource.c src/sec
cp -v $SEC/src/base64.c src/util
cp -v $SEC/src/certhelpers.c src/sec
cp -v $SEC/src/credresource.c src/sec
cp -v $SEC/src/crlresource.c src/sec
cp -v $SEC/src/csrresource.c src/sec
cp -v $SEC/src/deviceonboardingstate.c src/sec
cp -v $SEC/src/doxmresource.c src/sec
cp -v $SEC/src/iotvticalendar.c src/sec
cp -v $SEC/src/occertutility.c src/sec
cp -v $SEC/src/ocsecurity.c src/sec
cp -v $SEC/src/oxmpincommon.c src/sec
cp -v $SEC/src/oxmverifycommon.c src/sec
cp -v $SEC/src/pbkdf2.c src/sec
cp -v $SEC/src/pkix_interface.c src/sec
cp -v $SEC/src/policyengine.c src/sec
cp -v $SEC/src/psinterface.c src/sec
cp -v $SEC/src/pstatresource.c src/sec
cp -v $SEC/src/resourcemanager.c src/sec
cp -v $SEC/src/rolesresource.c src/sec
cp -v $SEC/src/secureresourcemanager.c src/sec
cp -v $SEC/src/srmresourcestrings.c src/sec
cp -v $SEC/src/srmutility.c src/sec
cp -v $SEC/src/strptime.c src/portability/windows

cp -vr $SEC/tool src/sec

mkdir -pv test/security
cp -vr $SEC/unittest/* test/security
rm -v test/security/SConscript
# OMIT cp -vr $SEC/unittests


#### STACK

STACK="$IOTIVITY/resource/csdk/stack"

mkdir -pv src/ocf
cp -v $STACK/include/ocpayload.h src/ocf
cp -v $STACK/include/ocpresence.h src/ocf
cp -v $STACK/include/ocstack.h src/ocf
cp -v $STACK/include/ocstackconfig.h src/ocf
cp -v $STACK/include/experimental/payload_logging.h src/ocf
cp -v $STACK/include/internal/occlientcb.h src/ocf
cp -v $STACK/include/internal/occollection.h src/ocf
cp -v $STACK/include/internal/occonnectionmanager.h src/ocf
cp -v $STACK/include/internal/ocendpoint.h src/ocf
cp -v $STACK/include/internal/ocobserve.h src/ocf
cp -v $STACK/include/internal/ocpayloadcbor.h src/ocf
cp -v $STACK/include/internal/ocresource.h src/ocf
cp -v $STACK/include/internal/ocresourcehandler.h src/ocf
cp -v $STACK/include/internal/ocserverrequest.h src/ocf
cp -v $STACK/include/internal/ocsqlite3helper.h src/ocf
cp -v $STACK/include/internal/ocstackinternal.h src/ocf
cp -v $STACK/include/internal/oicgroup.h src/ocf
cp -v $STACK/include/internal/oickeepalive.h src/ocf
cp -v $STACK/include/internal/tree.h src/ocf

# WINDOWS stuff - ???  TODO
# cp -v $STACK/octbstack_product.def TODO
# cp -v $STACK/octbstack_product_secured.def TODO
# cp -v $STACK/octbstack_product_secured_mot.def TODO
# cp -v $STACK/octbstack_rd_client.def TODO
# cp -v $STACK/octbstack_rd_server.def TODO
# OMIT cp -v $STACK/samples/arduino
mkdir -pv examples-bazel
cp -vr $STACK/samples/linux examples-bazel
# cp -vr $STACK/samples/tizen TODO
cp -v $STACK/src/occlientcb.c src/ocf
cp -v $STACK/src/occollection.c src/ocf
cp -v $STACK/src/occonnectionmanager.c src/ocf
cp -v $STACK/src/ocendpoint.c src/ocf
cp -v $STACK/src/ocobserve.c src/ocf
cp -v $STACK/src/ocpayload.c src/ocf
cp -v $STACK/src/ocpayloadconvert.c src/ocf
cp -v $STACK/src/ocpayloadparse.c src/ocf
cp -v $STACK/src/ocresource.c src/ocf
cp -v $STACK/src/ocserverrequest.c src/ocf
cp -v $STACK/src/ocsqlite3helper.c src/ocf
cp -v $STACK/src/ocstack.c src/ocf
cp -v $STACK/src/oicgroup.c src/ocf
cp -v $STACK/src/oickeepalive.c src/ocf
cp -v $STACK/src/oicresourcedirectory.c src/ocf

mkdir -pv test/stack
cp -vr $STACK/test/* test/stack

