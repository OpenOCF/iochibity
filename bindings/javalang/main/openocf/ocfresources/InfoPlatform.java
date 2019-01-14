package openocf.ocfresources;
// package org.iotivity.base;

/**
 * This class describes the platform properties. All non-Null properties will be
 * included in a platform discovery request.
 */
public class InfoPlatform {
    private String mPlatformId;
    private String mManufacturerName;
    private String mManufacturerUrl;
    private String mModelNumber;
    private String mDateOfManufacture;
    private String mPlatformVersion;
    private String mOperatingSystemVersion;
    private String mHardwareVersion;
    private String mFirmwareVersion;
    private String mSupportUrl;
    private String mSystemTime;

    /**
     * construct InfoPlatform with mandatory fields which cannot be null
     * manufacturerName cannot be &gt; 16 chars
     * manufacturerUrl cannot be &gt; 32 chars
     */
    public InfoPlatform(String platformId, String manufacturerName,
                          String manufacturerUrl) {
        this.mPlatformId = platformId;
        this.mManufacturerName = manufacturerName;
        this.mManufacturerUrl = manufacturerUrl;
    }

    public InfoPlatform(String platformId,
                          String manufacturerName,
                          String manufacturerUrl,
                          String modelNumber,
                          String dateOfManufacture,
                          String platformVersion,
                          String operatingSystemVersion,
                          String hardwareVersion,
                          String firmwareVersion,
                          String supportUrl,
                          String systemTime) {
        this(platformId, manufacturerName, manufacturerUrl);
        this.mModelNumber = modelNumber;
        this.mDateOfManufacture = dateOfManufacture;
        this.mPlatformVersion = platformVersion;
        this.mOperatingSystemVersion = operatingSystemVersion;
        this.mHardwareVersion = hardwareVersion;
        this.mFirmwareVersion = firmwareVersion;
        this.mSupportUrl = supportUrl;
        this.mSystemTime = systemTime;
    }

    public String getPlatformId() {
        return mPlatformId;
    }

    public void setPlatformId(String platformId) {
        this.mPlatformId = platformId;
    }

    public String getManufacturerName() {
        return mManufacturerName;
    }

    public void setManufacturerName(String manufacturerName) {
        this.mManufacturerName = manufacturerName;
    }

    public String getManufacturerUrl() {
        return mManufacturerUrl;
    }

    public void setManufacturerUrl(String manufacturerUrl) {
        this.mManufacturerUrl = manufacturerUrl;
    }

    public String getModelNumber() {
        return mModelNumber;
    }

    public void setModelNumber(String modelNumber) {
        this.mModelNumber = modelNumber;
    }

    public String getDateOfManufacture() {
        return mDateOfManufacture;
    }

    public void setDateOfManufacture(String dateOfManufacture) {
        this.mDateOfManufacture = dateOfManufacture;
    }

    public String getPlatformVersion() {
        return mPlatformVersion;
    }

    public void setPlatformVersion(String platformVersion) {
        this.mPlatformVersion = platformVersion;
    }

    public String getOperatingSystemVersion() {
        return mOperatingSystemVersion;
    }

    public void setOperatingSystemVersion(String operatingSystemVersion) {
        this.mOperatingSystemVersion = operatingSystemVersion;
    }

    public String getHardwareVersion() {
        return mHardwareVersion;
    }

    public void setHardwareVersion(String hardwareVersion) {
        this.mHardwareVersion = hardwareVersion;
    }

    public String getFirmwareVersion() {
        return mFirmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.mFirmwareVersion = firmwareVersion;
    }

    public String getSupportUrl() {
        return mSupportUrl;
    }

    public void setSupportUrl(String supportUrl) {
        this.mSupportUrl = supportUrl;
    }

    public String getSystemTime() {
        return mSystemTime;
    }

    public void setSystemTime(String systemTime) {
        this.mSystemTime = systemTime;
    }
}
