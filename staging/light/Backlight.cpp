/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define LOG_TAG "android.hardware.biometrics.fingerprint@2.1-service.xiaomi_sdm660"

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <android/log.h>
#include "Backlight.h"

#include "LED.h"

namespace aidl {
namespace android {
namespace hardware {
namespace light {

class BacklightBrightness : public BacklightDevice {
public:
    BacklightBrightness(std::string name) : mBasePath(mkBacklightBasePath + name + "/") {
        if (!readFromFile(mBasePath + "max_brightness", &mMaxBrightness)) {
            mMaxBrightness = kDefaultMaxBrightness;
        }
    };

    void setBacklight(uint8_t value) {
        writeToFile(mBasePath + "brightness", value * mMaxBrightness / 0xFF);
        int pid = ::android::IPCThreadState::self()->getCallingPid();
	    int uid = ::android::IPCThreadState::self()->getCallingUid();
        ALOGE("[Alc]setbrightness HAL, pid=%d, uid=%d, value=%u, max=%u", pid,uid,value,mMaxBrightness);
    }

    bool exists() {
        return fileWriteable(mBasePath + "brightness");
    }
private:
    std::string mBasePath;
    uint32_t mMaxBrightness;

    inline static const std::string mkBacklightBasePath = "/sys/class/backlight/";
    inline static const uint32_t kDefaultMaxBrightness = 255;
};

static const std::string kBacklightDevices[] = {
    "backlight",
};

BacklightDevice *getBacklightDevice() {
    for (auto &device : kBacklightDevices) {
        auto backlight = new BacklightBrightness(device);
        if (backlight->exists()) {
            return backlight;
        }
        delete backlight;
    }
    return nullptr;
}

} // namespace light
} // namespace hardware
} // namespace android
} // namespace aidl
