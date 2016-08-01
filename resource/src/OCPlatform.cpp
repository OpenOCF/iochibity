//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//******************************************************************
// File name:
//     OCPlatform.cpp
//
// Description: Implementation of the OCPlatform.
//
//
//
//*********************************************************************
#include <OCPlatform.h>
namespace OC
{
    namespace OCPlatform
    {
        void Configure(const PlatformConfig& config)
        {
            OCPlatform_impl::Configure(config);
        }

        OCStackResult setDefaultDeviceEntityHandler(EntityHandler entityHandler)
        {
            return OCPlatform_impl::Instance().setDefaultDeviceEntityHandler(entityHandler);
        }

        OCStackResult notifyAllObservers(OCResourceHandle resourceHandle,
                                     QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().notifyAllObservers(resourceHandle, QoS);
        }

        OCStackResult notifyAllObservers(OCResourceHandle resourceHandle)
        {
            return OCPlatform_impl::Instance().notifyAllObservers(resourceHandle);
        }

        OCStackResult notifyListOfObservers(OCResourceHandle resourceHandle,
                                     ObservationIds& observationIds,
                                     const std::shared_ptr<OCResourceResponse> pResponse)
        {
            return OCPlatform_impl::Instance().notifyListOfObservers(resourceHandle,
                                     observationIds, pResponse);
        }

        OCStackResult notifyListOfObservers(OCResourceHandle resourceHandle,
                                     ObservationIds& observationIds,
                                     const std::shared_ptr<OCResourceResponse> pResponse,
                                     QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().notifyListOfObservers(resourceHandle,
                                     observationIds, pResponse, QoS);
        }

        OCResource::Ptr constructResourceObject(const std::string& host,
                                     const std::string& uri,
                                     OCConnectivityType connectivityType,
                                     bool isObservable,
                                     const std::vector<std::string>& resourceTypes,
                                     const std::vector<std::string>& interfaces)
        {
            return OCPlatform_impl::Instance().constructResourceObject(host,
                                     uri, connectivityType,
                                     isObservable,
                                     resourceTypes, interfaces);
        }

        OCStackResult findResource(const std::string& host,
                                 const std::string& resourceName,
                                 OCConnectivityType connectivityType,
                                 FindCallback resourceHandler)
        {
            return OCPlatform_impl::Instance().findResource(host, resourceName,
                                 connectivityType, resourceHandler);
        }

        OCStackResult findResource(const std::string& host,
                                 const std::string& resourceName,
                                 OCConnectivityType connectivityType,
                                 FindCallback resourceHandler,
                                 QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().findResource(host, resourceName,
                                 connectivityType, resourceHandler, QoS);
        }

        OCStackResult findResource(const std::string& host,
                                            const std::string& resourceName,
                                            OCConnectivityType connectivityType,
                                            FindCallback resourceHandler,
                                            FindErrorCallback errorHandler)
        {
            return OCPlatform_impl::Instance().findResource(host, resourceName,
                                    connectivityType, resourceHandler, errorHandler);
        }

        OCStackResult findResource(const std::string& host,
                                            const std::string& resourceName,
                                            OCConnectivityType connectivityType,
                                            FindCallback resourceHandler,
                                            FindErrorCallback errorHandler,
                                            QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().findResource(host, resourceName,
                                    connectivityType, resourceHandler, errorHandler, QoS);
        }

        OCStackResult getDeviceInfo(const std::string& host,
                                 const std::string& deviceURI,
                                 OCConnectivityType connectivityType,
                                 FindDeviceCallback deviceInfoHandler)
        {
            return OCPlatform_impl::Instance().getDeviceInfo(host, deviceURI,
                                 connectivityType, deviceInfoHandler);
        }

        OCStackResult getDeviceInfo(const std::string& host,
                                 const std::string& deviceURI,
                                 OCConnectivityType connectivityType,
                                 FindDeviceCallback deviceInfoHandler,
                                 QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().getDeviceInfo(host, deviceURI, connectivityType,
                                 deviceInfoHandler, QoS);
        }

        OCStackResult getPlatformInfo(const std::string& host,
                                 const std::string& platformURI,
                                 OCConnectivityType connectivityType,
                                 FindPlatformCallback platformInfoHandler)
        {
            return OCPlatform_impl::Instance().getPlatformInfo(host, platformURI,
                                 connectivityType, platformInfoHandler);
        }

        OCStackResult getPlatformInfo(const std::string& host,
                                 const std::string& platformURI,
                                 OCConnectivityType connectivityType,
                                 FindPlatformCallback platformInfoHandler,
                                 QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().getPlatformInfo(host, platformURI, connectivityType,
                                 platformInfoHandler, QoS);
        }

        OCStackResult registerResource(OCResourceHandle& resourceHandle,
                                 std::string& resourceURI,
                                 const std::string& resourceTypeName,
                                 const std::string& resourceInterface,
                                 EntityHandler entityHandler,
                                 uint8_t resourceProperty)
        {
            return OCPlatform_impl::Instance().registerResource(resourceHandle, resourceURI,
                                 resourceTypeName, resourceInterface,
                                 entityHandler, resourceProperty);
        }

        OCStackResult registerResource(OCResourceHandle& resourceHandle,
                                 const std::shared_ptr< OCResource > resource)
        {
            return OCPlatform_impl::Instance().registerResource(resourceHandle, resource);
        }

        OCStackResult registerDeviceInfo(const OCDeviceInfo deviceInfo)
        {
            return OCPlatform_impl::Instance().registerDeviceInfo(deviceInfo);
        }

        OCStackResult registerPlatformInfo(const OCPlatformInfo platformInfo)
        {
            return OCPlatform_impl::Instance().registerPlatformInfo(platformInfo);
        }

        OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
        {
            return OCPlatform_impl::Instance().unregisterResource(resourceHandle);
        }

        OCStackResult unbindResource(OCResourceHandle collectionHandle,
                                 OCResourceHandle resourceHandle)
        {
            return OCPlatform_impl::Instance().unbindResource(collectionHandle, resourceHandle);
        }

        OCStackResult unbindResources(const OCResourceHandle collectionHandle,
                                 const std::vector<OCResourceHandle>& resourceHandles)
        {
            return OCPlatform_impl::Instance().unbindResources(collectionHandle, resourceHandles);
        }

        OCStackResult bindResource(const OCResourceHandle collectionHandle,
                                 const OCResourceHandle resourceHandle)
        {
            return OCPlatform_impl::Instance().bindResource(collectionHandle, resourceHandle);
        }

        OCStackResult bindResources(const OCResourceHandle collectionHandle,
                                 const std::vector<OCResourceHandle>& resourceHandles
                                 )
        {
            return OCPlatform_impl::Instance().bindResources(collectionHandle, resourceHandles);
        }

        OCStackResult bindTypeToResource(const OCResourceHandle& resourceHandle,
                                 const std::string& resourceTypeName)
        {
            return OCPlatform_impl::Instance().bindTypeToResource(resourceHandle,resourceTypeName);
        }

        OCStackResult bindInterfaceToResource(const OCResourceHandle& resourceHandle,
                                 const std::string& resourceInterfaceName)
        {
            return OCPlatform_impl::Instance().bindInterfaceToResource(resourceHandle,
                                                             resourceInterfaceName);
        }

        OCStackResult startPresence(const unsigned int announceDurationSeconds)
        {
            return OCPlatform_impl::Instance().startPresence(announceDurationSeconds);
        }

        OCStackResult stopPresence()
        {
            return OCPlatform_impl::Instance().stopPresence();
        }

        OCStackResult subscribePresence(OCPresenceHandle& presenceHandle,
                                     const std::string& host,
                                     OCConnectivityType connectivityType,
                                     SubscribeCallback presenceHandler)
        {
            return OCPlatform_impl::Instance().subscribePresence(presenceHandle, host,
                                                             connectivityType, presenceHandler);
        }

        OCStackResult subscribePresence(OCPresenceHandle& presenceHandle,
                                     const std::string& host,
                                     const std::string& resourceType,
                                     OCConnectivityType connectivityType,
                                     SubscribeCallback presenceHandler)
        {
            return OCPlatform_impl::Instance().subscribePresence(presenceHandle, host,
                                             resourceType, connectivityType, presenceHandler);
        }

        OCStackResult unsubscribePresence(OCPresenceHandle presenceHandle)
        {
            return OCPlatform_impl::Instance().unsubscribePresence(presenceHandle);
        }

#ifdef WITH_CLOUD
        OCStackResult subscribeDevicePresence(OCPresenceHandle& presenceHandle,
                                              const std::string& host,
                                              const QueryParamsList& queryParams,
                                              OCConnectivityType connectivityType,
                                              ObserveCallback callback)
        {
            return OCPlatform_impl::Instance().subscribeDevicePresence(presenceHandle,
                                                                       host,
                                                                       queryParams,
                                                                       connectivityType,
                                                                       callback);
        }
#endif

        OCStackResult sendResponse(const std::shared_ptr<OCResourceResponse> pResponse)
        {
            return OCPlatform_impl::Instance().sendResponse(pResponse);
        }

        OCStackResult findDirectPairingDevices(unsigned short waittime,
                                         GetDirectPairedCallback directPairingHandler)
        {
            return OCPlatform_impl::Instance().findDirectPairingDevices(waittime,
                                         directPairingHandler);
        }

        OCStackResult getDirectPairedDevices(GetDirectPairedCallback directPairingHandler)
        {
            return OCPlatform_impl::Instance().getDirectPairedDevices(directPairingHandler);
        }

        OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel,
                                 const std::string& pinNumber,
                                 DirectPairingCallback resultCallback)
        {
            return OCPlatform_impl::Instance().doDirectPairing(peer, pmSel,
                                             pinNumber, resultCallback);
        }
#ifdef WITH_CLOUD
        OCStackResult signUp(const std::string& host,
                             const std::string& authProvider,
                             const std::string& authCode,
                             OCConnectivityType connectivityType,
                             PostCallback cloudConnectHandler)
        {
            return OCPlatform_impl::Instance().signUp(host, authProvider, authCode,
                                                      connectivityType, cloudConnectHandler);
        }

        OCStackResult signIn(const std::string& host,
                             const std::string& accessToken,
                             OCConnectivityType connectivityType,
                             PostCallback cloudConnectHandler)
        {
            return OCPlatform_impl::Instance().signIn(host, accessToken,
                                                      connectivityType, cloudConnectHandler);
        }

        OCStackResult signOut(const std::string& host,
                              const std::string& accessToken,
                              OCConnectivityType connectivityType,
                              PostCallback cloudConnectHandler)
        {
            return OCPlatform_impl::Instance().signOut(host, accessToken,
                                                       connectivityType, cloudConnectHandler);
        }

        OCStackResult refreshAccessToken(const std::string& host,
                                         const std::string& refreshToken,
                                         OCConnectivityType connectivityType,
                                         PostCallback cloudConnectHandler)
        {
            return OCPlatform_impl::Instance().refreshAccessToken(host, refreshToken,
                                                       connectivityType, cloudConnectHandler);
        }
#endif // WITH_CLOUD
#ifdef RD_CLIENT
        OCStackResult publishResourceToRD(const std::string& host,
                                          OCConnectivityType connectivityType,
                                          PublishResourceCallback callback)
        {
            ResourceHandles resourceHandles;
            return OCPlatform_impl::Instance().publishResourceToRD(host, connectivityType,
                                                                   resourceHandles,
                                                                   callback);
        }

        OCStackResult publishResourceToRD(const std::string& host,
                                          OCConnectivityType connectivityType,
                                          ResourceHandles& resourceHandles,
                                          PublishResourceCallback callback)
        {
            return OCPlatform_impl::Instance().publishResourceToRD(host, connectivityType,
                                                                   resourceHandles,
                                                                   callback);
        }

        OCStackResult publishResourceToRD(const std::string& host,
                                          OCConnectivityType connectivityType,
                                          ResourceHandles& resourceHandles,
                                          PublishResourceCallback callback, QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().publishResourceToRD(host, connectivityType,
                                                                   resourceHandles,
                                                                   callback, QoS);
        }

        OCStackResult deleteResourceFromRD(const std::string& host,
                                           OCConnectivityType connectivityType,
                                           DeleteResourceCallback callback)
        {
            ResourceHandles resourceHandles;
            return OCPlatform_impl::Instance().deleteResourceFromRD(host, connectivityType,
                                                                    resourceHandles, callback);
        }

        OCStackResult deleteResourceFromRD(const std::string& host,
                                           OCConnectivityType connectivityType,
                                           ResourceHandles& resourceHandles,
                                           DeleteResourceCallback callback)
        {
            return OCPlatform_impl::Instance().deleteResourceFromRD(host, connectivityType,
                                                                    resourceHandles, callback);
        }

        OCStackResult deleteResourceFromRD(const std::string& host,
                                           OCConnectivityType connectivityType,
                                           ResourceHandles& resourceHandles,
                                           DeleteResourceCallback callback, QualityOfService QoS)
        {
            return OCPlatform_impl::Instance().deleteResourceFromRD(host, connectivityType,
                                                                    resourceHandles, callback,
                                                                    QoS);
        }
#endif
    } // namespace OCPlatform
} //namespace OC

