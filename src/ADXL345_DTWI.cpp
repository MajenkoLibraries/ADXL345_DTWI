#include <ADXL345_DTWI.h>

#define REG_DEVID          0x00    // R,     11100101,   Device ID
#define REG_THRESH_TAP     0x1D    // R/W,   00000000,   Tap threshold
#define REG_OFSX           0x1E    // R/W,   00000000,   X-axis offset
#define REG_OFSY           0x1F    // R/W,   00000000,   Y-axis offset
#define REG_OFSZ           0x20    // R/W,   00000000,   Z-axis offset
#define REG_DUR            0x21    // R/W,   00000000,   Tap duration
#define REG_LATENT         0x22    // R/W,   00000000,   Tap latency 
#define REG_WINDOW         0x23    // R/W,   00000000,   Tap window
#define REG_THRESH_ACT     0x24    // R/W,   00000000,   Activity threshold
#define REG_THRESH_INACT   0x25    // R/W,   00000000,   Inactivity threshold
#define REG_TIME_INACT     0x26    // R/W,   00000000,   Inactivity time
#define REG_ACT_INACT_CTL  0x27    // R/W,   00000000,   Axis enable control for activity and inactiv ity detection
#define REG_THRESH_FF      0x28    // R/W,   00000000,   Free-fall threshold
#define REG_TIME_FF        0x29    // R/W,   00000000,   Free-fall time
#define REG_TAP_AXES       0x2A    // R/W,   00000000,   Axis control for single tap/double tap
#define REG_ACT_TAP_STATUS 0x2B    // R,     00000000,   Source of single tap/double tap
#define REG_BW_RATE        0x2C    // R/W,   00001010,   Data rate and power mode control
#define REG_POWER_CTL      0x2D    // R/W,   00000000,   Power-saving features control
#define REG_INT_ENABLE     0x2E    // R/W,   00000000,   Interrupt enable control
#define REG_INT_MAP        0x2F    // R/W,   00000000,   Interrupt mapping control
#define REG_INT_SOURCE     0x30    // R,     00000010,   Source of interrupts
#define REG_DATA_FORMAT    0x31    // R/W,   00000000,   Data format control
#define REG_DATAX0         0x32    // R,     00000000,   X-Axis Data 0
#define REG_DATAX1         0x33    // R,     00000000,   X-Axis Data 1
#define REG_DATAY0         0x34    // R,     00000000,   Y-Axis Data 0
#define REG_DATAY1         0x35    // R,     00000000,   Y-Axis Data 1
#define REG_DATAZ0         0x36    // R,     00000000,   Z-Axis Data 0
#define REG_DATAZ1         0x37    // R,     00000000,   Z-Axis Data 1
#define REG_FIFO_CTL       0x38    // R/W,   00000000,   FIFO control
#define REG_FIFO_STATUS    0x39    // R,     00000000,   FIFO status

uint8_t ADXL345::readRegister(uint8_t reg) {
    uint8_t state = 0;
    uint8_t val = 0;
    while (1) {
        switch (state) {
            case 0: // begin write 
                if (dtwi->startMasterWrite(addr)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (dtwi->write(&reg, 1) == 1) {
                    state = 2;
                }
                break;
            case 2: // Stop Master
                if (dtwi->stopMaster()) {
            state = 3;
                }
                break;
            case 3:
                if (dtwi->startMasterRead(addr, 1)) {
                    state = 4;
                }
                break;
            case 4:
                if (dtwi->available()) {
                    dtwi->read(&val, 1);
                    state = 5;
                }
                break;
            case 5:
                if (dtwi->stopMaster()) {
                    return val;
                }
                break;
        }
    }
}

void ADXL345::writeRegister(uint8_t reg, uint8_t val) {
    uint8_t state = 0;
    while (1) {
        switch (state) {
            case 0: // begin write 
                if (dtwi->startMasterWrite(addr)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (dtwi->write(&reg, 1) == 1) {
                    state = 2;
                }
                break;
            case 2: // Send register address
                if (dtwi->write(&val, 1) == 1) {
                    state = 3;
                }
                break;
            case 3: // Stop Master
                if (dtwi->stopMaster()) {
                    return;
                }
                break;
        }
    }
}

int16_t ADXL345::readRaw(uint8_t h, uint8_t l) {
    uint8_t msb = readRegister(h);
    uint8_t lsb = readRegister(l);
    return (msb << 8) | lsb;
}

void ADXL345::begin() {
    dtwi->beginMaster();
    format.selftest = 0;
    format.spi = 0;
    format.invert = 0;
    format.fullres = 1;
    format.justify = 0;
    format.range = 0;

    writeRegister(REG_DATA_FORMAT, format.val);

    power.link = 0;
    power.autosleep = 0;
    power.measure = 1;
    power.sleep = 0;
    power.wakeup = 3;

    writeRegister(REG_POWER_CTL, power.val);

    writeRegister(REG_INT_ENABLE, 0);
    writeRegister(REG_INT_MAP, 0);

}

int16_t ADXL345::readRawX() { return readRaw(REG_DATAX1, REG_DATAX0); }
int16_t ADXL345::readRawY() { return readRaw(REG_DATAY1, REG_DATAY0); }
int16_t ADXL345::readRawZ() { return readRaw(REG_DATAZ1, REG_DATAZ0); }

float ADXL345::readX() { return readRawX() * 0.004; }
float ADXL345::readY() { return readRawY() * 0.004; }
float ADXL345::readZ() { return readRawZ() * 0.004; }

void ADXL345::setRange(uint8_t r) {
    format.range = r;
    writeRegister(REG_DATA_FORMAT, format.val);
}

void ADXL345::enableSingleTap(int i, uint8_t t, uint8_t d, uint8_t l) {
    writeRegister(REG_THRESH_TAP, t);
    writeRegister(REG_DUR, d);
    writeRegister(REG_LATENT, l);
    uint8_t r = readRegister(REG_INT_ENABLE);
    r |= 0b01000000;
    writeRegister(REG_INT_ENABLE, r);

    writeRegister(REG_TAP_AXES, 0b111);

    r = readRegister(REG_INT_MAP);
    if (i == 1) {
        r |= 0b01000000;
    } else {
        r &= 0b10111111;
    }
    writeRegister(REG_INT_MAP, r);
}

void ADXL345::disableSingleTap() {
    uint8_t r = readRegister(REG_INT_ENABLE);
    r &= 0b10111111;
    writeRegister(REG_INT_ENABLE, r);
}

void ADXL345::enableDoubleTap(int i) {
    uint8_t r = readRegister(REG_INT_ENABLE);
    r |= 0b00100000;
    writeRegister(REG_INT_ENABLE, r);

    r = readRegister(REG_INT_MAP);
    if (i == 1) {
        r |= 0b00100000;
    } else {
        r &= 0b11011111;
    }
    writeRegister(REG_INT_MAP, r);
}

void ADXL345::disableDoubleTap() {
    uint8_t r = readRegister(REG_INT_ENABLE);
    r &= 0b11011111;
    writeRegister(REG_INT_ENABLE, r);
}

uint8_t ADXL345::getInterrupts() {
    uint8_t r = readRegister(REG_INT_SOURCE);
    return r;
}
