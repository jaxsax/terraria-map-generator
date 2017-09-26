#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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
  uint16_t sectionCount;
  uint16_t tileTypeCount;
} FileFormatHeader;

typedef struct _WorldHeader { char *worldName; } WorldHeader;

uint8_t *readUint8(uint8_t *buffer, uint8_t *data);
uint8_t *readUint16(uint8_t *buffer, uint16_t *data);
uint8_t *readUint32(uint8_t *buffer, uint32_t *data);
uint8_t *readUint64(uint8_t *buffer, uint64_t *data);
uint8_t *readString(uint8_t *buffer, char **data);

bool parseFileFormatHeader(uint8_t **buffer, FileFormatHeader *data);
bool parseWorldHeader(uint8_t **buffer, WorldHeader *data);
bool readFileIntoMemory(uint8_t *buffer, const char *filename,
                        size_t file_size);
