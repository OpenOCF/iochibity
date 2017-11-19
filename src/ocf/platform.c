/**
 * This structure describes the platform properties. All non-Null properties will be
 * included in a platform discovery request.
 * @deprecated: Use OCSetPropertyValue  to set platform value.
 */
#if EXPORT_INTERFACE
typedef struct
{
    /** Platform ID.*/
    char *platformID;

    /** Manufacturer name.*/
    char *manufacturerName;

    /** Manufacturer URL for platform property.*/
    char *manufacturerUrl;

    /** Model number.*/
    char *modelNumber;

    /** Manufacturer date.*/
    char *dateOfManufacture;

    /** Platform version.*/
    char *platformVersion;

    /** Operating system version.*/
    char *operatingSystemVersion;

    /** HW version.*/
    char *hardwareVersion;

    /** FW version.*/
    char *firmwareVersion;

    /** Platform support URL.*/
    char *supportUrl;

    /** System time.*/
    char *systemTime;

} OCPlatformInfo;
#endif	/* INTERFACE */
