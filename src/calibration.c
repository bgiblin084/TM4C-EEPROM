/**
 * @file calibration.c
 *
 * This file simply servers as a wrapper for the EEPROM library. This is to provide
 * an easy interface for students to consistently read from the same locations
 * for calibration values.
 *
 * @author Braedon Giblin <bgiblin@iastate.edu>
 */

#include <stdlib.h>
#include <math.h>
#include "calibration.h"
#include "eeprom.h"

cal_servo_t* calibration_getServoCalibration() {
    eeprom_init();
    cal_servo_t* ret = malloc(sizeof(cal_servo_t));
    ret->a = eeprom_readInt(0x0);
    ret->b = eeprom_readInt(0x1);

    return ret;
}

cal_ir_t* calibration_getIRCalibration() {
    eeprom_init();
    cal_ir_t* ret = (cal_ir_t*) malloc(sizeof(cal_ir_t));
    ret->a = eeprom_readFloat(0x2); // Read a from 0x2 of EEPROM
    ret->b = eeprom_readFloat(0x3); // Read b from 0x3 of EEPROM

    return ret;
}

float calibration_getDistance(float adcValue) {
    cal_ir_t* cal = calibration_getIRCalibration();
    return cal->a * pow(adcValue, cal->b);
}

uint32_t calibration_getMatchValue(float degrees) {
    degrees = degrees < 0 ? 0 : degrees > 180 ? 180 : degrees;  // Clamp between 0 and 180
    cal_servo_t* cal = calibration_getServoCalibration();
    return 320000 - (cal->a + cal->b * (degrees / 180.0f));
}
