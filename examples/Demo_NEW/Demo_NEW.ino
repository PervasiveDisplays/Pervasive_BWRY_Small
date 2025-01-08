

#include "Pervasive_BWRY_Small.h"

// DEMO Image Set <Comment out when in User Mode>
// Screen Size: 154, 213, 266
#define SCRN 154
#include "globalupdate_src/demoImageData.h"


Pervasive_BWRY_Small myDriver(eScreen_EPD_154_QS_0F, boardRaspberryPiPico_RP2040);
// Pervasive_BWRY_Small myDriver(eScreen_EPD_213_QS_0F, boardRaspberryPiPico_RP2040);
// Pervasive_BWRY_Small myDriver(eScreen_EPD_266_QS_0F, boardRaspberryPiPico_RP2040);

void setup()
{
    hV_HAL_begin();

    myDriver.begin();

    myDriver.updateNormal(BW_monoBuffer, frameSize);

    hV_HAL_exit();
}

void loop()
{
    hV_HAL_delayMilliseconds(1000);
}
