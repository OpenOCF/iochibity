#if EXPORT_INTERFACE
/**
 * Device properties persistent store name.
 */
 /* src: ocresourcehandler.h */
#define OC_DEVICE_PROPS_FILE_NAME  "device_properties.dat"

/**
 * Device properties name
 */
 /* src: ocresourcehandler.h */
#define OC_JSON_DEVICE_PROPS_NAME "DeviceProperties"

/**
 * This structure is expected as input for device properties.
 * device name is mandatory and expected from the application.
 * device id of type UUID will be generated by the stack.
 * @deprecated: Use OCSetPropertyValue  to set device value.
 */
typedef struct
{
    /** Pointer to the device name.*/
    char *deviceName;
    /** Pointer to the types.*/
    OCStringLL *types;
    /** Pointer to the device specification version.*/
    char *specVersion;
    /** Pointer to the device data model versions (in CSV format).*/
    OCStringLL *dataModelVersions;
} OCDeviceInfo;

/**
 * Data structure for holding enhanced device information
 */
typedef struct _OCDeviceProperties
{
    /** Protocol Independent Id.*/
    char protocolIndependentId[UUID_STRING_SIZE];
} OCDeviceProperties;
#endif

/**
 * Device's power on/off state.
 */
typedef enum {
    DEV_STATUS_ON = (1 << 0),
    DEV_STATUS_OFF = (1 << 1)
}DeviceStatus;
