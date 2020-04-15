/**
 * @file eeprom.h
 *
 * @author Braedon Giblin <bgiblin@iastate.edu>
 *
 * This defines a basic interface for the TM4C MCU's onboard EEPROM.
 * This supports basic read and write operations.
 *
 * **Note on addresses** EEPROM consists of 32 blocks of 16 (64 bit) words.
 * So, to make address 1 byte, EEPROM is paged into two pages (through software).
 * The upper four bytes of an address consist of the block, and the lower 4 bytes
 * represent the word.
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Initialize EEPROM.
 */
void eeprom_init();

/**
 * @brief Writes a char.
 *
 * @param addr Address to write to
 * @param data data to write
 */
void eeprom_writeChar(uint8_t addr, char data);

/**
 * @brief Writes a short.
 *
 * @param addr Address to write to
 * @param data data to write
 */
void eeprom_writeShort(uint8_t addr, short data);

/**
 * @brief Writes an int.
 *
 * @param addr Address to write to
 * @param data data to write
 */
void eeprom_writeInt(uint8_t addr, int data);

/**
 * @brief Writes a long.
 *
 * @param addr Address to write to
 * @param data data to write
 */
void eeprom_writeLong(uint8_t addr, long data);

/**
 * @brief Writes a long long.
 *
 * @param addr Address to write to
 * @param data data to write
 */
void eeprom_writeLongLong(uint8_t addr, long long data);

/**
 * @brief Writes a float.
 *
 * @param addr Address to write to
 * @param data data to write
 */
void eeprom_writeFloat(uint8_t addr, float data);

/**
 * @brief Writes a double.
 *
 * @param addr Address to write to
 * @param data data to write
 */
void eeprom_writeDouble(uint8_t addr, double data);

/**
 * @brief Writes a generic data type.
 *
 * Generic data type can be an array, struct, etc.
 *
 * @param addr Address to write to
 * @param data data to write
 * @param n size of dat
 */
void eeprom_writeData(uint8_t addr, void* data, size_t n);

/**
 * @brief Reads a char.
 *
 * @param addr Address to read from
 *
 * @return data read
 */
char eeprom_readChar(uint8_t addr);

/**
 * @brief Reads a short.
 *
 * @param addr Address to read from
 *
 * @return data read
 */
short eeprom_readShort(uint8_t addr);

/**
 * @brief Reads an int.
 *
 * @param addr Address to read from
 *
 * @return data read
 */
int eeprom_readInt(uint8_t addr);

/**
 * @brief Reads a long.
 *
 * @param addr Address to read from
 *
 * @return data read
 */
long eeprom_readLong(uint8_t addr);

/**
 * @brief Reads a long long.
 *
 * @param addr Address to read from
 *
 * @return data read
 */
long long eeprom_readLongLong(uint8_t addr);

/**
 * @brief Reads a float.
 *
 * @param addr Address to read from
 *
 * @return data read
 */
float eeprom_readFloat(uint8_t addr);

/**
 * @brief Reads a double.
 *
 * @param addr Address to read from
 *
 * @return data read
 */
double eeprom_readDouble(uint8_t addr);

/**
 * @brief Reads a generic data type.
 *
 * @param addr Address to read from
 * @param size Size of data to read
 *
 * @return data read
 *
 */
void* eeprom_readData(uint8_t addr, size_t size);

/**
 * @brief Sets the EEPROM page.
 *
 * Note, hardware has no concept of pages. This is entirely a software decision
 * to limit the size of an address to one byte.
 */
void setPage(uint8_t page);

/**
 * @brief Gets the EEPROM page.
 *
 * Note, hardware has no concept of pages. This is entirely a software decision
 * to limit the size of an address to one byte.
 */
uint8_t getPage();

#endif /* EEPROM_H_ */

