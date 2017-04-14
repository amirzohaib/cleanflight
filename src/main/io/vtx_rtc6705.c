/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Created by jflyper */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "platform.h"

#if (defined(VTX_RTC6705) && defined(VTX_CONTROL)) || defined(VTX_RTC6705SOFTSPI)
#include "build/build_config.h"

#include "cms/cms.h"
#include "cms/cms_types.h"

#include "common/printf.h"
#include "common/utils.h"

#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "drivers/system.h"
#include "drivers/vtx_common.h"
#include "drivers/vtx_rtc6705.h"

#include "fc/rc_controls.h"
#include "fc/runtime_config.h"

#include "io/vtx_rtc6705.h"
#include "io/vtx_string.h"

#include "build/debug.h"

#if defined(VTX_RTC6705SOFTSPI) || defined(VTX_RTC6705)
PG_REGISTER_WITH_RESET_TEMPLATE(vtxRTC6705Config_t, vtxRTC6705Config, PG_VTX_RTC6705_CONFIG, 0);

PG_RESET_TEMPLATE(vtxRTC6705Config_t, vtxRTC6705Config,
    .band = 4,    //Fatshark/Airwaves
    .channel = 1, //CH1
    .rfPower = 0,
    .enabled = true
);
#endif


#if defined(CMS) || defined(VTX_COMMON)
static const char * const rtc6705PowerNames[] = {
    "---", "25 ", "200",
};
#endif

#ifdef VTX_COMMON
static vtxVTable_t rtc6705VTable;    // Forward
static vtxDevice_t vtxRTC6705 = {
    .vTable = &rtc6705VTable,
    .numBand = 5,
    .numChan = 8,
    .numPower = 3,
    .bandNames = (char **)vtx58BandNames,
    .chanNames = (char **)vtx58ChannelNames,
    .powerNames = (char **)rtc6705PowerNames,
};
#endif

typedef struct rtc6705Dev_s {
    uint8_t band;    // 1-5
    uint8_t channel; // 1-8
    uint8_t rfPower; // 0-1
    bool enabled;
} rtc6705Dev_t;

rtc6705Dev_t rtc6705Dev;


bool vtxRTC6705Init(void)
{
    memset(&rtc6705Dev, 0, sizeof(rtc6705Dev));

    vtxRTC6705.vTable = &rtc6705VTable;
    vtxCommonRegisterDevice(&vtxRTC6705);

    return true;
}

static void vtxRTC6705Disable(void)
{
    rtc6705Disable();
    rtc6705Dev.enabled = false;
}

static void vtxRTC6705Enable(void)
{
    rtc6705Enable();

    delay(RTC6705_BOOT_DELAY);

    rtc6705SetRFPower(rtc6705Dev.rfPower);
    rtc6705SetChannel(rtc6705Dev.band - 1, rtc6705Dev.channel - 1);

    rtc6705Dev.enabled = true;
}

void vtxRTC6705Process(uint32_t now)
{
    UNUSED(now);

    static bool configured = false;
    if (!configured) {
        rtc6705Dev.band = vtxRTC6705Config()->band;
        rtc6705Dev.channel = vtxRTC6705Config()->channel;
        rtc6705Dev.rfPower = vtxRTC6705Config()->rfPower;
        if (vtxRTC6705Config()->enabled) {
            vtxRTC6705Enable();
        } else {
            vtxRTC6705Disable();
        }

        configured = true;
    }
}

#ifdef VTX_COMMON
// Interface to common VTX API

vtxDevType_e vtxRTC6705GetDeviceType(void)
{
    return VTXDEV_RTC6705;
}

bool vtxRTC6705IsReady(void)
{
    return true;
}

void vtxRTC6705SetBandChan(uint8_t band, uint8_t chan)
{
    if (band && chan)
        rtc6705SetChannel(band - 1, chan - 1);
}

void vtxRTC6705SetPowerByIndex(uint8_t index)
{
#ifdef RTC6705_POWER_PIN
    if (index == 0) {
        // TODO check power isn't already off?
        rtc6705Disable();
        return;
    }
    // TODO ensure power is on.
#else
    if (index == 0) {
        return;
    }
#endif

    rtc6705SetRFPower(index - 1);
}

void vtxRTC6705SetPitmode(uint8_t onoff)
{
    UNUSED(onoff);
    return;
}

bool vtxRTC6705GetBandChan(uint8_t *pBand, uint8_t *pChan)
{
    *pBand = rtc6705Dev.band + 1;
    *pChan = rtc6705Dev.channel + 1;
    return true;
}

bool vtxRTC6705GetPowerIndex(uint8_t *pIndex)
{
    *pIndex = rtc6705Dev.enabled ? (rtc6705Dev.rfPower + 1) : (0);
    return true;
}

bool vtxRTC6705GetPitmode(uint8_t *pOnOff)
{
    UNUSED(pOnOff);
    return false;
}

static vtxVTable_t rtc6705VTable = {
    .process = vtxRTC6705Process,
    .getDeviceType = vtxRTC6705GetDeviceType,
    .isReady = vtxRTC6705IsReady,
    .setBandChan = vtxRTC6705SetBandChan,
    .setPowerByIndex = vtxRTC6705SetPowerByIndex,
    .setPitmode = vtxRTC6705SetPitmode,
    .getBandChan = vtxRTC6705GetBandChan,
    .getPowerIndex = vtxRTC6705GetPowerIndex,
    .getPitmode = vtxRTC6705GetPitmode,
};
#endif // VTX_COMMON

#endif // VTX_RTC6705
