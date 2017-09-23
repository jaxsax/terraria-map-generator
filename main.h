#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _FileFormatHeader {
  uint32_t currentRelease;
  uint64_t magicNumber;
  uint32_t revision;
  uint64_t unknown_param1;
  uint32_t sectionCount;
} FileFormatHeader;

uint8_t *readUint8(uint8_t *buffer, uint8_t *data);
uint8_t *readUint16(uint8_t *buffer, uint16_t *data);
uint8_t *readUint32(uint8_t *buffer, uint32_t *data);
uint8_t *readUint64(uint8_t *buffer, uint64_t *data);

bool parseFileFormatHeader(uint8_t *buffer, FileFormatHeader *data);
bool readFileIntoMemory(uint8_t *buffer, const char *filename,
                        size_t file_size);