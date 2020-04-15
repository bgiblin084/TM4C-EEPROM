/**
 * @file calibration.h
 *
 * This file simply servers as a wrapper for the EEPROM library. This is to provide
 * an easy interface for students to consistently read from the same locations
 * for calibration values.
 *
 * @author Braedon Giblin <bgiblin@iastate.edu>
 */

#include <stdint.h>

/**
 * @brief Datatype for IR calibration.
 *
 * Calibration should be of the form distance a*x^b where "a" and "b" are
 * constants and x is the measured valued from the ADC
 */
typedef struct {
    float a;  ///< "a" coefficient value, stored in EEPROM 0x2
    float b;  ///< "b" exponential base value, stored in EEPROM 0x3
} cal_ir_t;

/**
 * @brief Datatype for Servo calibration.
 *
 * Calibration should be of the form match value = period - (a + b * (degrees / 180.0))
 * Where "a" and "b" are constants, period is the period in clock cycles (32000),
 * and degrees is the amount in degrees to turn.
 */
typedef struct {
    int a;  ///< "a" constant coefficient value, stored in EEPROM 0x0
    int b;  ///< "b" coefficient base value, stored in EEPROM 0x1
} cal_servo_t;

/**
 * @brief Retrieves the saved calibration values for the servo from EEPROM.
 */
cal_servo_t* calibration_getServoCalibration();

/**
 * @brief Retrieves the saved calibration values for the IR Sensor from EEPROM.
 */
cal_ir_t* calibration_getIRCalibration();

/**
 * @brief calibrates the IR Sensor.
 *
 * This will return a distance, in centimeters, given an ADC value.
 *
 * @param adcValue Raw reading from the ADC. This should be averaged if possible.
 *
 * @return distance in centimeters.
 */
float calibration_getDistance(float adcValue);

/**
 * @brief calibrates the servo motor.
 *
 * This will return a match value. This match value can then be written to
 * the MATCHR and TBMR registers.
 *
 * @param degrees Degrees to set the servo too. Must be between 0 and 180.
 *
 * @return Match value for timer
 */
uint32_t calibration_getMatchValue(float degrees);
