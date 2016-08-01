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

#ifndef OC_IN_PROC_CLIENT_WRAPPER_H_
#define OC_IN_PROC_CLIENT_WRAPPER_H_

#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>

#include <OCApi.h>
#include <IClientWrapper.h>
#include <InitializeException.h>
#include <ResourceInitException.h>

namespace OC
{
    namespace ClientCallbackContext
    {
        struct GetContext
        {
            GetCallback callback;
            GetContext(GetCallback cb) : callback(cb){}
        };

        struct SetContext
        {
            PutCallback callback;
            SetContext(PutCallback cb) : callback(cb){}
        };

        struct ListenContext
        {
            FindCallback callback;
            std::weak_ptr<IClientWrapper> clientWrapper;

            ListenContext(FindCallback cb, std::weak_ptr<IClientWrapper> cw)
                : callback(cb), clientWrapper(cw){}
        };

        struct ListenErrorContext
        {
            FindCallback callback;
            FindErrorCallback errorCallback;
            std::weak_ptr<IClientWrapper> clientWrapper;

            ListenErrorContext(FindCallback cb1, FindErrorCallback cb2,
                               std::weak_ptr<IClientWrapper> cw)
                : callback(cb1), errorCallback(cb2), clientWrapper(cw){}
        };

        struct DeviceListenContext
        {
            FindDeviceCallback callback;
            IClientWrapper::Ptr clientWrapper;
            DeviceListenContext(FindDeviceCallback cb, IClientWrapper::Ptr cw)
                    : callback(cb), clientWrapper(cw){}
        };

        struct SubscribePresenceContext
        {
            SubscribeCallback callback;
            SubscribePresenceContext(SubscribeCallback cb) : callback(cb){}
        };

        struct DeleteContext
        {
            DeleteCallback callback;
            DeleteContext(DeleteCallback cb) : callback(cb){}
        };

        struct ObserveContext
        {
            ObserveCallback callback;
            ObserveContext(ObserveCallback cb) : callback(cb){}
        };

        struct DirectPairingContext
        {
            DirectPairingCallback callback;
            DirectPairingContext(DirectPairingCallback cb) : callback(cb){}

        };

#ifdef WITH_MQ
        struct CreateMQTopicContext
        {
            MQCreateTopicCallback callback;
            std::weak_ptr<IClientWrapper> clientWrapper;
            CreateMQTopicContext(MQCreateTopicCallback cb, std::weak_ptr<IClientWrapper> cw)
                : callback(cb), clientWrapper(cw){}
        };
#endif
    }

    class InProcClientWrapper : public IClientWrapper
    {

    public:

        InProcClientWrapper(std::weak_ptr<std::recursive_mutex> csdkLock,
                            PlatformConfig cfg);
        virtual ~InProcClientWrapper();

        virtual OCStackResult ListenForResource(const std::string& serviceUrl,
            const std::string& resourceType, OCConnectivityType transportFlags,
            FindCallback& callback, QualityOfService QoS);

        virtual OCStackResult ListenErrorForResource(const std::string& serviceUrl,
            const std::string& resourceType, OCConnectivityType transportFlags,
            FindCallback& callback, FindErrorCallback& errorCallback, QualityOfService QoS);

        virtual OCStackResult ListenForDevice(const std::string& serviceUrl,
            const std::string& deviceURI, OCConnectivityType transportFlags,
            FindDeviceCallback& callback, QualityOfService QoS);

        virtual OCStackResult GetResourceRepresentation(
            const OCDevAddr& devAddr,
            const std::string& uri,
            const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
            GetCallback& callback, QualityOfService QoS);

        virtual OCStackResult PutResourceRepresentation(
            const OCDevAddr& devAddr,
            const std::string& uri,
            const OCRepresentation& attributes, const QueryParamsMap& queryParams,
            const HeaderOptions& headerOptions, PutCallback& callback, QualityOfService QoS);

        virtual OCStackResult PostResourceRepresentation(
            const OCDevAddr& devAddr,
            const std::string& uri,
            const OCRepresentation& attributes, const QueryParamsMap& queryParams,
            const HeaderOptions& headerOptions, OCConnectivityType connectivityType,
            PostCallback& callback, QualityOfService QoS);

        virtual OCStackResult DeleteResource(
            const OCDevAddr& devAddr,
            const std::string& uri,
            const HeaderOptions& headerOptions,
            DeleteCallback& callback, QualityOfService QoS);

        virtual OCStackResult ObserveResource(
            ObserveType observeType, OCDoHandle* handle,
            const OCDevAddr& devAddr,
            const std::string& uri,
            const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
            ObserveCallback& callback, QualityOfService QoS);

        virtual OCStackResult CancelObserveResource(
            OCDoHandle handle,
            const std::string& host,
            const std::string& uri,
            const HeaderOptions& headerOptions, QualityOfService QoS);

        virtual OCStackResult SubscribePresence(
            OCDoHandle *handle,
            const std::string& host,
            const std::string& resourceType,
            OCConnectivityType transportFlags,
            SubscribeCallback& presenceHandler);

        virtual OCStackResult UnsubscribePresence(OCDoHandle handle);

#ifdef WITH_CLOUD
        virtual OCStackResult SubscribeDevicePresence(OCDoHandle* handle,
                                                      const std::string& host,
                                                      const QueryParamsList& queryParams,
                                                      OCConnectivityType connectivityType,
                                                      ObserveCallback& callback);
#endif

        OCStackResult GetDefaultQos(QualityOfService& QoS);

        virtual OCStackResult FindDirectPairingDevices(unsigned short waittime,
                       GetDirectPairedCallback& callback);

        virtual OCStackResult GetDirectPairedDevices(GetDirectPairedCallback& callback);

        virtual OCStackResult DoDirectPairing(std::shared_ptr<OCDirectPairing> peer, const OCPrm_t& pmSel,
                const std::string& pinNumber, DirectPairingCallback& resultCallback);

#ifdef WITH_MQ
        virtual OCStackResult ListenForMQTopic(
            const OCDevAddr& devAddr,
            const std::string& resourceUri,
            const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
            FindCallback& callback, QualityOfService QoS);

        virtual OCStackResult PutMQTopicRepresentation(
            const OCDevAddr& devAddr,
            const std::string& uri,
            const OCRepresentation& rep,
            const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
            MQCreateTopicCallback& callback, QualityOfService QoS);
#endif

    private:
        void listeningFunc();
        std::string assembleSetResourceUri(std::string uri, const QueryParamsMap& queryParams);
        std::string assembleSetResourceUri(std::string uri, const QueryParamsList& queryParams);
        OCPayload* assembleSetResourcePayload(const OCRepresentation& attributes);
        OCHeaderOption* assembleHeaderOptions(OCHeaderOption options[],
           const HeaderOptions& headerOptions);
        void convert(const OCDPDev_t *list, PairedDevices& dpList);
        std::thread m_listeningThread;
        bool m_threadRun;
        std::weak_ptr<std::recursive_mutex> m_csdkLock;

    private:
        PlatformConfig  m_cfg;
    };
}

#endif

