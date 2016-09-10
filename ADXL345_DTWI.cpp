#include <ADXL345_DTWI.h>

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

    writeRegister(0x32, format.val);

    power.link = 0;
    power.autosleep = 0;
    power.measure = 1;
    power.sleep = 0;
    power.wakeup = 3;

    writeRegister(0x2D, power.val);

}

int16_t ADXL345::readRawX() { return readRaw(0x33, 0x32); }
int16_t ADXL345::readRawY() { return readRaw(0x35, 0x34); }
int16_t ADXL345::readRawZ() { return readRaw(0x37, 0x36); }

float ADXL345::readX() { return readRawX() * 0.004; }
float ADXL345::readY() { return readRawY() * 0.004; }
float ADXL345::readZ() { return readRawZ() * 0.004; }

void ADXL345::setRange(uint8_t r) {
    format.range = r;
    writeRegister(0x32, format.val);
}
