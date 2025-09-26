#include "Pervasive_BWRY_Small.h"

// DEMO Image Set <Comment out when in User Mode>
// Screen Size: 154, 206, 213, 266, 417, 437
#define SCRN 206
#include "globalupdate_src/demoImageData.h"


Pervasive_BWRY_Small myDriver(eScreen_EPD_206_QS_06, boardRaspberryPiPico_RP2040);
// Pervasive_BWRY_Small myDriver(eScreen_EPD_266_QS_0F, boardRaspberryPiPico_RP2040);
// Pervasive_BWRY_Small myDriver(eScreen_EPD_417_QS_0A, boardRaspberryPiPico_RP2040);
// Pervasive_BWRY_Small myDriver(eScreen_EPD_437_QS_0B, boardRaspberryPiPico_RP2040);

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
