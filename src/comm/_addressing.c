/*
 * MAX_ADDR_STR_SIZE_CA - fromm octypes.h and cacommon.h, both of
 * which contained copies of this code.
 */
#if INTERFACE
#ifdef RA_ADAPTER
#define MAX_ADDR_STR_SIZE (256)
#else
/*
 * Max Address could be "coaps+tcp://[xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:yyy.yyy.yyy.yyy]:xxxxx"
 * Which is 65, +1 for null terminator => 66
 * OCDevAddr (defined in OCTypes.h) must be the same
 * as CAEndpoint_t (defined here)
 */
#define MAX_ADDR_STR_SIZE (66)
#endif
#define MAX_ADDR_STR_SIZE_CA MAX_ADDR_STR_SIZE
#endif	/* INTERFACE */
