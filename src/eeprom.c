/**
 * @file eeprom.c
 *
 * @author Braedon Giblin <bgiblin@iastate.edu>
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "eeprom.h"

/*************************************
 *    STATIC FUNCTION DECLARATIONS   *
 *************************************/

/**
 * @brief Writes one Half Word to EEPROM.
 *
 * @param addr Address to write to
 * @param halfWord Half word to write
 */
void eeprom_writeHalfWord(uint8_t addr, uint32_t halfWord);

/**
 * @brief Reads one half word from EEPROM.
 *
 * @param addr Address to read from
 *
 * @return half word read
 */
uint32_t eeprom_readHalfWord(uint8_t addr);

/**
 * @brief Writes multiple half words to EEPROM.
 *
 * @param addr Address to write to
 * @param halfWords Half Words to write
 * @param number of half words to write
 */
void eeprom_writeHalfWords(uint8_t addr, uint32_t* halfWords, size_t n);

/**
 * Reads multiple halfWords from EEPROM
 *
 * @param addr Address to read from
 * @param n Number of halfWords to read
 *
 * @return Array of half words of length n
 */
uint32_t* eeprom_readHalfWords(uint8_t addr, size_t n);

/*************************************
 *  END STATIC FUNCTION DECLARATION  *
 *************************************/

static uint8_t _PAGE;

void eeprom_init(){
    static bool init = false;
    if (init) return;

    SYSCTL_RCGCEEPROM_R |= SYSCTL_RCGCEEPROM_R0;
    while(!(SYSCTL_PREEPROM_R & SYSCTL_RCGCEEPROM_R0));  // Must wait a minimum of 6 clock cycles
    while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);      // Wait for EEPROM to finish power on
    if (EEPROM_EESUPP_R & (EEPROM_EESUPP_PRETRY | EEPROM_EESUPP_ERETRY)) {
        // TODO: Raise error (write to errno or LCD).
        return;
    }

    SYSCTL_SREEPROM_R |= SYSCTL_SREEPROM_R0;             // Set the software reset bit
    SYSCTL_SREEPROM_R &= ~SYSCTL_SREEPROM_R0;            // Clear the software reset bit
    while (!(SYSCTL_PREEPROM_R & SYSCTL_SREEPROM_R0));   // Must wait minimum of 6 clock cycles
    while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);      // Wait for EEPROM to finish resetting
    if (EEPROM_EESUPP_R & (EEPROM_EESUPP_PRETRY | EEPROM_EESUPP_ERETRY)) {
        // TODO: Raise error (write to errno or LCD).
        return;
    }
    _PAGE = 0;
    init = true;
}


void eeprom_writeHalfWord(uint8_t addr, uint32_t halfWord) {
    uint8_t blockAddr = (addr >> 4) | (_PAGE << 4);
    uint8_t offset = addr & 0xF;

    // Set address
    EEPROM_EEBLOCK_R = (EEPROM_EEBLOCK_R & 0xFFFF0000) | 1 << blockAddr;
    EEPROM_EEOFFSET_R = (EEPROM_EEOFFSET_R & 0xFFFFFFF0) | offset;
    // Must wait 4 clock cycles before accessing data
    asm(" NOP"
        "\n"
        " NOP"
        "\n"
        " NOP"
        "\n"
        " NOP"
        "\n"
        " NOP"); // Extra NOP because per ARM, NOP cannot be used for timing as the pipeline
                 // is not deterministic. An extra NOP ensures that timings are correct.
    // Write data
    EEPROM_EERDWR_R = halfWord;

    while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);     // Wait for EEPROM to finish write
}


uint32_t eeprom_readHalfWord(uint8_t addr) {
    uint8_t blockAddr = (addr >> 4) | (_PAGE << 4);
    uint8_t offset = addr & 0xF;

    // Set address
    EEPROM_EEBLOCK_R = (EEPROM_EEBLOCK_R & 0xFFFF0000) | 1 << blockAddr;
    EEPROM_EEOFFSET_R = (EEPROM_EEOFFSET_R & 0xFFFFFFF0) | offset;
    // Must wait 4 clock cycles before accessing data
    asm(" NOP"
        "\n"
        " NOP"
        "\n"
        " NOP"
        "\n"
        " NOP"
        "\n"
        " NOP"); // Extra NOP because per ARM, NOP cannot be used for timing as the pipeline
                 // is not deterministic. An extra NOP ensures that timings are correct.
    // read data
    uint32_t ret = EEPROM_EERDWR_R;

    while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);     // Wait for EEPROM to finish write

    return ret;
}

// calculate the blocks need to store / read a value
static inline int eeprom_getBlocksNeeded(size_t n, uint8_t offset) {
    int blocksNeeded = 0;
    if (n > (16 - offset)) {
        blocksNeeded++;
        n -= (16 - offset);
    }
    blocksNeeded += (n / 16) + (n % 16 > 0);
    return blocksNeeded;
}


void eeprom_writeHalfWords(uint8_t addr, uint32_t* halfWords, size_t n) {
    uint8_t blockAddr = (addr >> 4) | (_PAGE << 4);
    uint8_t offset = addr & 0xF;
    size_t dataWritten = 0;
    int i, j;

    // Calculate the required amount of words and blocks
    int blocksNeeded = eeprom_getBlocksNeeded(n, offset);
    if (blockAddr + blocksNeeded > 31) {
        // TODO: Throw error
        blocksNeeded = 31 - blocksNeeded;                     // Write as much data as we can
        n = (blocksNeeded - 1) * 16 + (16 - offset);      // Calculate how much to write
    }

    for (i = 0; i < blocksNeeded; i++) {
        // Set address
        EEPROM_EEBLOCK_R = (EEPROM_EEBLOCK_R & 0xFFFF0000) | 1 << (blockAddr + i);
        EEPROM_EEOFFSET_R = (EEPROM_EEOFFSET_R & 0xFFFFFFF0) | offset;
        // Must wait 4 clock cycles before accessing data
        asm(" NOP"
            "\n"
            " NOP"
            "\n"
            " NOP"
            "\n"
            " NOP"
            "\n"
            " NOP"); // Extra NOP because per ARM, NOP cannot be used for timing as the pipeline
                     // is not deterministic. An extra NOP ensures that timings are correct.

        for (j = offset; j < 16 && dataWritten < n; j++) {
            // Write data
            EEPROM_EERDWRINC_R = halfWords[dataWritten++];      // Write data and increment offset
            while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);     // Wait for EEPROM to finish write
        }
    }
}


uint32_t* eeprom_readHalfWords(uint8_t addr, size_t n) {
    uint8_t blockAddr = (addr >> 4) | (_PAGE << 4);
    uint8_t offset = addr & 0xF;
    size_t dataRead = 0;
    uint32_t* data = NULL;
    int i, j;

    // Calculate the required amount of words and blocks
    int blocksNeeded = eeprom_getBlocksNeeded(n, offset);
    if (blockAddr + blocksNeeded > 31) {
        // TODO: Throw error
        blocksNeeded = 31 - blocksNeeded;                     // read as much data as we can
        n = (blocksNeeded - 1) * 16 + (16 - offset);      // Calculate how much to read
    }

    data = malloc(sizeof(uint32_t) * n);

    for (i = 0; i < blocksNeeded; i++) {
        // Set address
        EEPROM_EEBLOCK_R = (EEPROM_EEBLOCK_R & 0xFFFF0000) | 1 << (blockAddr + i);
        EEPROM_EEOFFSET_R = (EEPROM_EEOFFSET_R & 0xFFFFFFF0) | offset;
        // Must wait 4 clock cycles before accessing data
        asm(" NOP"
            "\n"
            " NOP"
            "\n"
            " NOP"
            "\n"
            " NOP"
            "\n"
            " NOP"); // Extra NOP because per ARM, NOP cannot be used for timing as the pipeline
                     // is not deterministic. An extra NOP ensures that timings are correct.
        for (j = offset; j < 16 && dataRead < n; j++) {
            data[dataRead++] = EEPROM_EERDWRINC_R;
            while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);     // Wait for EEPROM to finish read
        }
    }
    return data;
}


void eeprom_writeChar(uint8_t addr, char data) {
    union {
        char c;
        uint32_t u;
    } d;
    d.c = data;
    eeprom_writeHalfWord(addr, d.u);
}


void eeprom_writeShort(uint8_t addr, short data) {
    union {
        short s;
        uint32_t u;
    } d;
    d.s = data;
    eeprom_writeHalfWord(addr, d.u);
}


void eeprom_writeInt(uint8_t addr, int data) {
    union {
        int i;
        uint32_t u;
    } d;
    d.i = data;
    eeprom_writeHalfWord(addr, d.u);
}


void eeprom_writeLong(uint8_t addr, long data) {
    union {
        long l;
        uint32_t u;
    } d;
    d.l = data;
    eeprom_writeHalfWord(addr, d.u);
}


void eeprom_writeLongLong(uint8_t addr, long long data) {
    uint32_t d[2];
    memcpy(d, &data, 8);
    eeprom_writeHalfWords(addr, d, 2);
}


void eeprom_writeFloat(uint8_t addr, float data){
    union {
        float f;
        uint32_t u;
    } d;
    d.f = data;
    eeprom_writeHalfWord(addr, d.u);
}


void eeprom_writeDouble(uint8_t addr, double data){
    uint32_t d[2];
    memcpy(d, &data, 8);
    eeprom_writeHalfWords(addr, d, 2);
}


void eeprom_writeData(uint8_t addr, void* data, size_t n) {
    uint32_t* dataToWrite = (uint32_t*) data;
    int halfWords = (n / 4) + (n % 4 > 0);
    if (n % 4 != 0) {
        n += 4 - (n % 4);
        dataToWrite = malloc(halfWords);
        memcpy(dataToWrite, data, n);
    }

    eeprom_writeHalfWords(addr, data, halfWords);
}


char eeprom_readChar(uint8_t addr) {
    union {
        char c;
        uint32_t u;
    } d;
    d.u = eeprom_readHalfWord(addr);
    return d.c;
}


short eeprom_readShort(uint8_t addr) {
    union {
        short s;
        uint32_t u;
    } d;
    d.u = eeprom_readHalfWord(addr);
    return d.s;
}


int eeprom_readInt(uint8_t addr) {
    union {
        int i;
        uint32_t u;
    } d;
    d.u = eeprom_readHalfWord(addr);
    return d.i;
}


long eeprom_readLong(uint8_t addr) {
    union {
        long l;
        uint32_t u;
    } d;
    d.u = eeprom_readHalfWord(addr);
    return d.l;
}


long long eeprom_readLongLong(uint8_t addr) {
    uint32_t* data = eeprom_readHalfWords(addr, 2);

    long long* ret = malloc(sizeof(long long));
    memcpy(ret, data, sizeof(long long));
    return *ret;
}


float eeprom_readFloat(uint8_t addr) {
    union {
        float f;
        uint32_t u;
    } d;
    d.u = eeprom_readHalfWord(addr);
    return d.f;
}


double eeprom_readDouble(uint8_t addr){
   uint32_t* data = eeprom_readHalfWords(addr, 2);

   double* ret = malloc(sizeof(double));
   memcpy(ret, data, sizeof(double));
   return *ret;
}


void* eeprom_readData(uint8_t addr, size_t n){
    int numWords = (n / 4) + (4 - n % 4);
    void* ret = NULL;

    uint32_t* data = eeprom_readHalfWords(addr, numWords);

    ret = malloc(sizeof(void) * n);
    memcpy(ret, data, n);
    return ret;
}


void setPage(uint8_t page) {
    _PAGE = page;
}


uint8_t getPage() {
    return _PAGE;
}
