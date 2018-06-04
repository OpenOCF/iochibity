/// BlueZ D-Bus service name.
#define BLUEZ_NAME "org.bluez"

/// BlueZ D-Bus adapter interface name.
static char const BLUEZ_ADAPTER_INTERFACE[] = BLUEZ_NAME ".Adapter1";

/// BlueZ D-Bus device interface name.
static char const BLUEZ_DEVICE_INTERFACE[] = BLUEZ_NAME ".Device1";

/// BlueZ D-Bus LE advertising manager interface.
static char const BLUEZ_ADVERTISING_MANAGER_INTERFACE[] =
    BLUEZ_NAME ".LEAdvertisingManager1";

/// BlueZ D-Bus GATT manager interface.
static char const BLUEZ_GATT_MANAGER_INTERFACE[] =
    BLUEZ_NAME ".GattManager1";

/// BlueZ D-Bus adapter GATT service interface name.
static char const BLUEZ_GATT_SERVICE_INTERFACE[] =
    BLUEZ_NAME ".GattService1";

/// BlueZ D-Bus adapter GATT characteristic interface name.
static char const BLUEZ_GATT_CHARACTERISTIC_INTERFACE[] =
    BLUEZ_NAME ".GattCharacteristic1";

/// BlueZ D-Bus adapter GATT service interface name.
static char const BLUEZ_GATT_DESCRIPTOR_INTERFACE[] =
    BLUEZ_NAME ".GattDescriptor1";
