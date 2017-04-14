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

#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include "platform.h"

#ifdef CMS

#if defined(VTX_RTC6705)

#include "build/version.h"

#include "cms/cms.h"
#include "cms/cms_types.h"
#include "cms/cms_menu_vtx.h"

#include "common/utils.h"

#include "config/feature.h"
#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "fc/config.h"

#include "io/vtx_rtc6705.h"

static bool featureRead = false;
static uint8_t cmsx_featureVtx = 0;
static bool cmsx_vtxEnabled;
static uint8_t cmsx_vtxBand;
static uint8_t cmsx_vtxChannel;
static uint8_t cmsx_vtxRfPower;

static long cmsx_Vtx_FeatureRead(void)
{
    if (!featureRead) {
        cmsx_featureVtx = feature(FEATURE_VTX) ? 1 : 0;
        featureRead = true;
    }

    return 0;
}

static long cmsx_Vtx_FeatureWriteback(void)
{
    if (featureRead) {
        if (cmsx_featureVtx)
            featureSet(FEATURE_VTX);
        else
            featureClear(FEATURE_VTX);
    }

    return 0;
}

static const char * const vtxBandNames[] = {
    "BOSCAM A",
    "BOSCAM B",
    "BOSCAM E",
    "FATSHARK",
    "RACEBAND",
};

static OSD_TAB_t entryVtxBand = {&cmsx_vtxBand, 4, &vtxBandNames[0]};
static OSD_UINT8_t entryVtxChannel =  {&cmsx_vtxChannel, 1, 8, 1};
static OSD_UINT8_t entryVtxRfPower =  {&cmsx_vtxRfPower, 0, 1, 1};

static void cmsx_Vtx_ConfigRead(void)
{
    cmsx_vtxEnabled = vtxRTC6705Config()->enabled;
    cmsx_vtxBand = vtxRTC6705Config()->band - 1;
    cmsx_vtxChannel = vtxRTC6705Config()->channel;
    cmsx_vtxRfPower = vtxRTC6705Config()->rfPower;
}

static void cmsx_Vtx_ConfigWriteback(void)
{
    vtxRTC6705ConfigMutable()->enabled = cmsx_vtxEnabled;
    vtxRTC6705ConfigMutable()->band = cmsx_vtxBand + 1;
    vtxRTC6705ConfigMutable()->channel = cmsx_vtxChannel;
    vtxRTC6705ConfigMutable()->rfPower = cmsx_vtxRfPower;
}

static long cmsx_Vtx_onEnter(void)
{
    cmsx_Vtx_FeatureRead();
    cmsx_Vtx_ConfigRead();

    return 0;
}

static long cmsx_Vtx_onExit(const OSD_Entry *self)
{
    UNUSED(self);

    cmsx_Vtx_ConfigWriteback();

    return 0;
}


static OSD_Entry cmsx_menuVtxEntries[] =
{
    {"--- VTX ---", OME_Label, NULL, NULL, 0},
    {"FEATURE", OME_Bool, NULL, &cmsx_featureVtx, 0},
    {"ENABLED", OME_Bool, NULL, &cmsx_vtxEnabled, 0},
    {"BAND", OME_TAB, NULL, &entryVtxBand, 0},
    {"CHANNEL", OME_UINT8, NULL, &entryVtxChannel, 0},
    {"RF POWER", OME_UINT8, NULL, &entryVtxRfPower, 0},
    {"BACK", OME_Back, NULL, NULL, 0},
    {NULL, OME_END, NULL, NULL, 0}
};

CMS_Menu cmsx_menuVtx = {
    .GUARD_text = "MENUVTX",
    .GUARD_type = OME_MENU,
    .onEnter = cmsx_Vtx_onEnter,
    .onExit= cmsx_Vtx_onExit,
    .onGlobalExit = cmsx_Vtx_FeatureWriteback,
    .entries = cmsx_menuVtxEntries
};

#endif // VTX || USE_RTC6705
#endif // CMS
