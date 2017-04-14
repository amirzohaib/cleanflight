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


// Get target build configuration
#include "platform.h"

#include "common/maths.h"

#include "config/config_eeprom.h"
#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "fc/config.h"
#include "fc/runtime_config.h"

#include "io/beeper.h"
#include "io/osd.h"
#include "io/vtx_control.h"


#ifdef VTX_CONTROL

PG_REGISTER(vtxConfig_t, vtxConfig, PG_VTX_CONFIG, 1);

static uint8_t locked = 0;

void vtxControlInit(void)
{
    // NOTHING TO DO
}

static void setChannelSaveAndNotify(void)
{
    if (ARMING_FLAG(ARMED)) {
        locked = 1;
    }

    if (!locked) {
        // TODO use VTX api here.

        writeEEPROM();
        readEEPROM();
        //beeperConfirmationBeeps(band or channel);
    }
}

void vtxIncrementBand(void)
{
    // TODO use VTX api here.
}

void vtxDecrementBand(void)
{
    // TODO use VTX api here.
}

void vtxIncrementChannel(void)
{
    // TODO use VTX api here.
}

void vtxDecrementChannel(void)
{
    // TODO use VTX api here.
}

void vtxUpdateActivatedChannel(void)
{
    if (ARMING_FLAG(ARMED)) {
        locked = 1;
    }

    if (!locked) {
        static uint8_t lastIndex = -1;

        for (uint8_t index = 0; index < MAX_CHANNEL_ACTIVATION_CONDITION_COUNT; index++) {
            const vtxChannelActivationCondition_t *vtxChannelActivationCondition = &vtxConfig()->vtxChannelActivationConditions[index];

            if (isRangeActive(vtxChannelActivationCondition->auxChannelIndex, &vtxChannelActivationCondition->range)
                && index != lastIndex) {
                lastIndex = index;

                // TODO use VTX api here
                //vtxSetBandAndChannel(vtxChannelActivationCondition->band, vtxChannelActivationCondition->channel);
                break;
            }
        }
    }
}


#if 0
/**
 * Allow VTX channel/band/rf power/on-off and save via a single button.
 *
 * The LED1 flashes a set number of times, followed by a short pause, one per second.  The amount of flashes decreases over time while
 * the button is held to indicate the action that will be performed upon release.
 * The actions are ordered by most-frequently used action.  i.e. you change channel more frequently than band.
 *
 * The vtx settings can be changed while the VTX is OFF.
 * If the VTX is OFF when the settings are saved the VTX will be OFF on the next boot, likewise
 * If the VTX is ON when the settings are saved the VTX will be ON on the next boot.
 *
 * Future: It would be nice to re-use the code in statusindicator.c and blink-codes but target a different LED instead of the simple timed
 * behaviour of the LED1 here.
 *
 * Future: Blink out the state after changing it.
 */
void handleVTXControlButton(void)
{
#if defined(VTX) && defined(BUTTON_A_PIN)
    bool buttonHeld;
    bool buttonWasPressed = false;
    uint32_t start = millis();
    uint32_t ledToggleAt = start;
    bool ledEnabled = false;
    uint8_t flashesDone = 0;

    uint8_t actionCounter = 0;
    while ((buttonHeld = !digitalIn(BUTTON_A_PORT, BUTTON_A_PIN))) {
        uint32_t end = millis();

        int32_t diff = cmp32(end, start);
        if (diff > 25 && diff <= 1000) {
            actionCounter = 5;
        } else if (diff > 1000 && diff <= 3000) {
            actionCounter = 4;
        } else if (diff > 3000 && diff <= 5000) {
            actionCounter = 3;
        } else if (diff > 5000 && diff <= 10000) {
            actionCounter = 2;
        } else if (diff > 10000) {
            actionCounter = 1;
        }

        if (actionCounter) {

            diff = cmp32(ledToggleAt, end);

            if (diff < 0) {
                ledEnabled = !ledEnabled;

                const uint8_t updateDuration = 60;

                ledToggleAt = end + updateDuration;

                if (ledEnabled) {
                    LED1_ON;
                } else {
                    LED1_OFF;
                    flashesDone++;
                }

                if (flashesDone == actionCounter) {
                    ledToggleAt += (1000 - ((flashesDone * updateDuration) * 2));
                    flashesDone = 0;
                }
            }
            buttonWasPressed = true;
        }
    }

    if (!buttonWasPressed) {
        return;
    }

    LED1_OFF;

    switch(actionCounter) {
        case 5:
            vtxCycleChannel();
            break;
        case 4:
            vtxCycleBand();
            break;
        case 3:
            vtxCycleRFPower();
            break;
        case 2:
            vtxTogglePower();
            break;
        case 1:
            vtxSaveState();
            break;
    }
#endif
}
#endif

#endif

