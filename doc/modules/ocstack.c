
/* these resource handlese act as a resource db of sorts. at startup
  the resource is created with this handle. */

/* all resources are on this linked list, stack has an api for dealing
   with it, e.g. findResource, insertResource, deleteResourse, etc. */
OCResource *headResource = NULL;
static OCResource *tailResource = NULL;

/* these resources go on the master list, but a ref is retained (why? handles only used for creating r) */
static OCResourceHandle platformResource = {0};
static OCResourceHandle deviceResource = {0};
static OCResourceHandle introspectionResource = {0};
static OCResourceHandle introspectionPayloadResource = {0};
static OCResourceHandle wellKnownResource = {0};
#ifdef MQ_BROKER
static OCResourceHandle brokerResource = {0};
#endif



static OCDoHandle GenerateInvocationHandle(void);
static OCStackResult initResources(void);
static void insertResource(OCResource *resource);
static OCResource *findResource(OCResource *resource);
static void insertResourceType(OCResource *resource, OCResourceType *resourceType);
static OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle, uint8_t index);
static void insertResourceInterface(OCResource *resource, OCResourceInterface *resourceInterface);
static OCResourceInterface *findResourceInterfaceAtIndex(OCResourceHandle handle, uint8_t index);
static void deleteResourceType(OCResourceType *resourceType);
static void deleteResourceInterface(OCResourceInterface *resourceInterface);
static void deleteResourceElements(OCResource *resource);
static OCStackResult deleteResource(OCResource *resource);
static void deleteAllResources(void);
static void incrementSequenceNumber(OCResource * resPtr);
static CAResult_t OCSelectNetwork(OCTransportAdapter transportType);
static OCStackResult CAResponseToOCStackResult(CAResponseResult_t caCode);
static CATransportFlags_t OCToCATransportFlags(OCTransportFlags ocConType);
static OCTransportFlags CAToOCTransportFlags(CATransportFlags_t caConType);
static OCStackResult HandlePresenceResponse(const CAEndpoint_t *endPoint, const CAResponseInfo_t *responseInfo);
static void HandleCAResponses(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo);
static void HandleCARequests(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo);
static OCStackResult getQueryFromUri(const char * uri, char** resourceType, char ** newURI);
static OCResourceType *findResourceType(OCResourceType * resourceTypeList, const char * resourceTypeName);
static OCStackResult ResetPresenceTTL(ClientCB *cbNode, uint32_t maxAgeSeconds);
static OCStackResult SetHeaderOption(CAHeaderOption_t *caHdrOpt, size_t numOptions, uint16_t optionID, void* optionData, size_t optionDataLength);
static OCStackResult OCSendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo);
static void OCDefaultAdapterStateChangedHandler(CATransportAdapter_t adapter, bool enabled);
static void OCDefaultConnectionStateChangedHandler(const CAEndpoint_t *info, bool isConnected);
#if defined (IP_ADAPTER)
static OCStackResult OCMapZoneIdToLinkLocalEndpoint(OCDiscoveryPayload *payload, uint32_t ifindex);
#endif
static OCStackResult OCInitializeInternal(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags, OCTransportAdapter transportType);
static OCStackResult OCDeInitializeInternal(void);
static OCPayloadFormat CAToOCPayloadFormat(CAPayloadFormat_t caFormat)
static void OCEnterInitializer(void)
static void OCLeaveInitializer(void)
bool checkProxyUri(OCHeaderOption *options, uint8_t numOptions)
uint32_t GetTicks(uint32_t milliSeconds)
void CopyEndpointToDevAddr(const CAEndpoint_t *in, OCDevAddr *out)
void CopyDevAddrToEndpoint(const OCDevAddr *in, CAEndpoint_t *out)
void FixUpClientResponse(OCClientResponse *cr)
static OCStackResult OCSendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------
OCStackResult OCStackFeedBack(CAToken_t token, uint8_t tokenLength, uint8_t status)
OCStackResult CAResponseToOCStackResult(CAResponseResult_t caCode)
CAResponseResult_t OCToCAStackResult(OCStackResult ocCode, OCMethod method)
CATransportFlags_t OCToCATransportFlags(OCTransportFlags ocFlags)
OCTransportFlags CAToOCTransportFlags(CATransportFlags_t caFlags)
static OCStackResult ResetPresenceTTL(ClientCB *cbNode, uint32_t maxAgeSeconds)
const char *OC_CALL convertTriggerEnumToString(OCPresenceTrigger trigger)
OCPresenceTrigger OC_CALL convertTriggerStringToEnum(const char * triggerStr)
OCStackResult OC_CALL OCEncodeAddressForRFC6874(char *outputAddress, size_t outputSize, const char *inputAddress)
OCStackResult OC_CALL OCDecodeAddressForRFC6874(char *outputAddress,
                                                size_t outputSize,
                                                const char *inputAddress,
                                                const char *end)
static int FormCanonicalPresenceUri(const CAEndpoint_t *endpoint,
                                    char *presenceUri, bool isMulticast)
OCStackResult HandlePresenceResponse(const CAEndpoint_t *endpoint,
                            const CAResponseInfo_t *responseInfo)
OCStackResult HandleBatchResponse(char *requestUri, OCRepPayload **payload)
#if defined (IP_ADAPTER)
OCStackResult OCMapZoneIdToLinkLocalEndpoint(OCDiscoveryPayload *payload, uint32_t ifindex)
#endif
void OC_CALL OCHandleResponse(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
void HandleCAResponses(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
void HandleCAErrorResponse(const CAEndpoint_t *endPoint, const CAErrorInfo_t *errorInfo)
OCStackResult SendDirectStackResponse(const CAEndpoint_t* endPoint, const uint16_t coapID,
        const CAResponseResult_t responseResult, const CAMessageType_t type,
        const uint8_t numOptions, const CAHeaderOption_t *options,
        CAToken_t token, uint8_t tokenLength, const char *resourceUri,
        CADataType_t dataType)
OCStackResult HandleStackRequests(OCServerProtocolRequest * protocolRequest)
void OCHandleRequests(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
void HandleCARequests(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)

//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------
#ifdef RA_ADAPTER
OCStackResult OC_CALL OCSetRAInfo(const OCRAInfo_t *raInfo)
#endif
OCStackResult OC_CALL OCInit(const char *ipAddr, uint16_t port, OCMode mode)
OCStackResult OC_CALL OCInit1(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags)
OCStackResult OC_CALL OCInit2(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags,
                              OCTransportAdapter transportType)
OCStackResult OCInitializeInternal(OCMode mode, OCTransportFlags serverFlags,
                                   OCTransportFlags clientFlags, OCTransportAdapter transportType)
OCStackResult OC_CALL OCStop(void)
OCStackResult OCDeInitializeInternal(void)
OCStackResult OC_CALL OCStartMulticastServer(void)
OCStackResult OC_CALL OCStopMulticastServer(void)
CAMessageType_t qualityOfServiceToMessageType(OCQualityOfService qos)
static OCStackResult ParseRequestUri(const char *fullUri,
                                        OCTransportAdapter adapter,
                                        OCTransportFlags flags,
                                        OCDevAddr **devAddr,
                                        char **resourceUri,
                                        char **resourceType)
static OCStackResult OCPreparePresence(CAEndpoint_t *endpoint,
                                       char **requestUri,
                                       bool isMulticast)
OCStackResult OC_CALL OCDoResource(OCDoHandle *handle,
                                   OCMethod method,
                                   const char *requestUri,
                                   const OCDevAddr *destination,
                                   OCPayload* payload,
                                   OCConnectivityType connectivityType,
                                   OCQualityOfService qos,
                                   OCCallbackData *cbData,
                                   OCHeaderOption *options,
                                   uint8_t numOptions)
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
static const char* ASSERT_ROLES_CTX = "Asserting roles from OCDoRequest";
static void assertRolesCB(void* ctx, bool hasError)
#endif // __WITH_DTLS__ || __WITH_TLS__
OCStackResult OC_CALL OCDoRequest(OCDoHandle *handle,
                                  OCMethod method,
                                  const char *requestUri,
                                  const OCDevAddr *destination,
                                  OCPayload* payload,
                                  OCConnectivityType connectivityType,
                                  OCQualityOfService qos,
                                  OCCallbackData *cbData,
                                  OCHeaderOption *options,
                                  uint8_t numOptions)
OCStackResult OC_CALL OCCancel(OCDoHandle handle, OCQualityOfService qos, OCHeaderOption * options,
        uint8_t numOptions)
OCStackResult OC_CALL OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
OCPersistentStorage *OC_CALL OCGetPersistentStorageHandler(void)
#ifdef WITH_PRESENCE
OCStackResult OCProcessPresence(void)
#endif // WITH_PRESENCE
OCStackResult OC_CALL OCProcess(void)
#ifdef WITH_PRESENCE
OCStackResult OC_CALL OCStartPresence(const uint32_t ttl)
OCStackResult OC_CALL OCStopPresence(void)
#endif
OCStackResult OC_CALL OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandler entityHandler,
                                                      void* callbackParameter)
OCTpsSchemeFlags OC_CALL OCGetSupportedEndpointTpsFlags(void)
OCStackResult OC_CALL OCCreateResource(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri, OCEntityHandler entityHandler,
        void *callbackParam,
        uint8_t resourceProperties)
OCStackResult OC_CALL OCCreateResourceWithEp(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri, OCEntityHandler entityHandler,
        void *callbackParam,
        uint8_t resourceProperties,
        OCTpsSchemeFlags resourceTpsTypes)
OCStackResult OC_CALL OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
OCStackResult OC_CALL OCUnBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
static bool ValidateResourceTypeInterface(const char *resourceItemName)
OCStackResult BindResourceTypeToResource(OCResource* resource, const char *resourceTypeName)
OCStackResult BindResourceInterfaceToResource(OCResource* resource, const char *resourceInterfaceName)
OCStackResult BindTpsTypeToResource(OCResource* resource, OCTpsSchemeFlags resourceTpsTypes)
OCStackResult OC_CALL OCBindResourceTypeToResource(OCResourceHandle handle, const char *resourceTypeName)
OCStackResult OC_CALL OCBindResourceInterfaceToResource(OCResourceHandle handle, const char *resourceInterfaceName)
OCStackResult OC_CALL OCGetNumberOfResources(uint8_t *numResources)
OCResourceHandle OC_CALL OCGetResourceHandle(uint8_t index)
OCStackResult OC_CALL OCDeleteResource(OCResourceHandle handle)
const char *OC_CALL OCGetResourceUri(OCResourceHandle handle)
OCResourceProperty OC_CALL OCGetResourceProperties(OCResourceHandle handle)
OCStackResult OC_CALL OCSetResourceProperties(OCResourceHandle handle, uint8_t resourceProperties)
OCStackResult OC_CALL OCClearResourceProperties(OCResourceHandle handle, uint8_t resourceProperties)
OCStackResult OC_CALL OCGetNumberOfResourceTypes(OCResourceHandle handle, uint8_t *numResourceTypes)
const char *OC_CALL OCGetResourceTypeName(OCResourceHandle handle, uint8_t index)
OCStackResult OC_CALL OCGetNumberOfResourceInterfaces(OCResourceHandle handle, uint8_t *numResourceInterfaces)
const char *OC_CALL OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index)
OCResourceHandle OC_CALL OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle, uint8_t index)
OCStackResult OC_CALL OCBindResourceHandler(OCResourceHandle handle, OCEntityHandler entityHandler, void* callbackParam)
OCEntityHandler OC_CALL OCGetResourceHandler(OCResourceHandle handle)
void incrementSequenceNumber(OCResource * resPtr)
#ifdef WITH_PRESENCE
OCStackResult SendPresenceNotification(OCResourceType *resourceType, OCPresenceTrigger trigger)
OCStackResult SendStopNotification(void)
#endif // WITH_PRESENCE
OCStackResult OC_CALL OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos)
OCStackResult
OC_CALL OCNotifyListOfObservers (OCResourceHandle handle,
                                 OCObservationId  *obsIdList,
                                 uint8_t          numberOfIds,
                                 const OCRepPayload       *payload,
                                 OCQualityOfService qos)
OCStackResult OC_CALL OCDoResponse(OCEntityHandlerResponse *ehResponse)
//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------
static OCDoHandle GenerateInvocationHandle(void)
#ifdef WITH_PRESENCE
OCStackResult OCChangeResourceProperty(OCResourceProperty * inputProperty,
        OCResourceProperty resourceProperties, uint8_t enable)
#endif
OCStackResult initResources(void)
void insertResource(OCResource *resource)
void deleteAllResources(void)
OCStackResult deleteResource(OCResource *resource)
void deleteResourceElements(OCResource *resource)
void deleteResourceType(OCResourceType *resourceType)
void deleteResourceInterface(OCResourceInterface *resourceInterface)
void OCDeleteResourceAttributes(OCAttribute *rsrcAttributes)
void insertResourceType(OCResource *resource, OCResourceType *resourceType)
void insertResourceInterface(OCResource *resource, OCResourceInterface *newInterface)
OCResourceInterface *findResourceInterfaceAtIndex(OCResourceHandle handle, uint8_t index)
OCStackResult getQueryFromUri(const char * uri, char** query, char ** uriWithoutQuery)
static const OicUuid_t* OC_CALL OCGetServerInstanceID(void)
const char* OC_CALL OCGetServerInstanceIDString(void)
CAResult_t OCSelectNetwork(OCTransportAdapter transportType)
OCStackResult CAResultToOCResult(CAResult_t caResult)
bool OCResultToSuccess(OCStackResult ocResult)
OCStackResult OC_CALL OCSetProxyURI(const char *uri)
#if defined(RD_CLIENT) || defined(RD_SERVER)
OCStackResult OC_CALL OCBindResourceInsToResource(OCResourceHandle handle, int64_t ins)
OCStackResult OCUpdateResourceInsWithResponse(const char *requestUri, const OCClientResponse *response)
OCStackResult OC_CALL OCGetResourceIns(OCResourceHandle handle, int64_t* ins)
#endif // RD_CLIENT || RD_SERVER
OCResourceHandle OC_CALL OCGetResourceHandleAtUri(const char *uri)
static OCStackResult SetHeaderOption(CAHeaderOption_t *caHdrOpt, size_t numOptions,
        uint16_t optionID, void* optionData, size_t optionDataLength)
OCStackResult OC_CALL OCSetHeaderOption(OCHeaderOption* ocHdrOpt, size_t* numOptions, uint16_t optionID,
                                        void* optionData, size_t optionDataLength)
OCStackResult OC_CALL OCGetHeaderOption(OCHeaderOption* ocHdrOpt, size_t numOptions,
                                        uint16_t optionID, void* optionData, size_t optionDataLength,
                                        uint16_t* receivedDataLength)
void OCDefaultAdapterStateChangedHandler(CATransportAdapter_t adapter, bool enabled)
void OCDefaultConnectionStateChangedHandler(const CAEndpoint_t *info, bool isConnected)
OCStackResult OC_CALL OCGetDeviceId(OCUUIdentity *deviceId)
OCStackResult OC_CALL OCSetDeviceId(const OCUUIdentity *deviceId)
OCStackResult OC_CALL OCGetDeviceOwnedState(bool *isOwned)
#ifdef IP_ADAPTER
OCStackResult OC_CALL OCGetLinkLocalZoneId(uint32_t ifindex, char **zoneId)
#endif
OCStackResult OC_CALL OCSelectCipherSuite(uint16_t cipher, OCTransportAdapter adapterType)
OCStackResult OC_CALL OCGetIpv6AddrScope(const char *addr, OCTransportFlags *scope)
#ifdef TCP_ADAPTER
static void OCPongHandler(void *context, CAEndpoint_t endpoint, bool withCustody)
static void OCPongDeleter(void *context)
OCStackResult OC_CALL OCSendPingMessage(const OCDevAddr *devAddr, bool withCustody, OCCallbackData *cbData)
#endif // TCP_ADAPTER
