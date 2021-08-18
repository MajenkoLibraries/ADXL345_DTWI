#ifndef _ADXL345_DTWI_H
#define _ADXL345_DTWI_H

#include <Arduino.h>
#include <DTWI.h>

typedef union {
    uint8_t val;
    struct {
        unsigned range      :2;
        unsigned justify    :1;
        unsigned fullres    :1;
        unsigned            :1;
        unsigned invert     :1;
        unsigned spi        :1;
        unsigned selftest   :1;
    } __attribute__((packed));
} __attribute__((packed)) dataformat_t;

typedef union {
    uint8_t val;
    struct {
        unsigned wakeup     :2;
        unsigned sleep      :1;
        unsigned measure    :1;
        unsigned autosleep  :1;
        unsigned link       :1;
        unsigned            :2;
    } __attribute__((packed));
} __attribute__((packed)) powerctl_t;
    

class ADXL345 {
    private:
        DTWI *dtwi;

    public:
        uint8_t readRegister(uint8_t reg);
        void writeRegister(uint8_t reg, uint8_t val);
    private:
        int16_t readRaw(uint8_t h, uint8_t l);

        float declination;
        uint8_t addr;

        powerctl_t power;
        dataformat_t format;


    public:
        static const uint8_t INT1 = 1;
        static const uint8_t INT2 = 2;

        static const uint8_t RANGE_2G = 0;
        static const uint8_t RANGE_4G = 1;
        static const uint8_t RANGE_8G = 2;
        static const uint8_t RANGE_16G = 3;

        ADXL345(DTWI *d, uint8_t a = 0x53) : dtwi(d), addr(a) {}
        ADXL345(DTWI &d, uint8_t a = 0x53) : dtwi(&d), addr(a) {}
        
        void begin();
        int16_t readRawX();
        int16_t readRawY();
        int16_t readRawZ();
        float readX();
        float readY();
        float readZ();
        void setRange(uint8_t g);

        void enableSingleTap(int i, uint8_t t = 0x40, uint8_t d = 0x30, uint8_t l = 0x40);
        void disableSingleTap();
        void enableDoubleTap(int i);
        void disableDoubleTap();

        uint8_t getInterrupts();
};

#endif
