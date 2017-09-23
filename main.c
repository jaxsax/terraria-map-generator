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

uint8_t *readUint8(uint8_t *buffer, uint8_t *data) {
  *data = *buffer;
  buffer = buffer + 1;
  return buffer;
}

uint8_t *readShort(uint8_t *buffer, short *data) {
  *data = (*(buffer + 1) << 8) | *buffer;
  buffer = buffer + 2;
  return buffer;
}

uint8_t *readUint32(uint8_t *buffer, uint32_t *data) {
  *data = (*(buffer + 3) << 24) | (*(buffer + 2) << 16) | (*(buffer + 1) << 8) |
          *buffer;
  buffer = buffer + 4;
  return buffer;
}

uint8_t *readUint64(uint8_t *buffer, uint64_t *data) {
  uint32_t a, b = 0;

  buffer = readUint32(buffer, &a);
  buffer = readUint32(buffer, &b);
  *data = (uint64_t)b << 32 | a;

  return buffer;
}

bool parseFileFormatHeader(uint8_t *buffer, FileFormatHeader *data) {
  buffer = readUint32(buffer, &data->currentRelease);
  buffer = readUint64(buffer, &data->magicNumber);
  buffer = readUint32(buffer, &data->revision);
  buffer = readUint64(buffer, &data->unknown_param1);

  short sections = 0;
  buffer = readShort(buffer, &sections);
  for (short i = 0; i < sections; i++) {
    uint32_t section_id = 0;
    buffer = readUint32(buffer, &section_id);

    printf("%u; id=%u\n", i, section_id);
  }

  printf("currentRelease: %u\n", data->currentRelease);
  printf("magicNumber: %lu\n", data->magicNumber);
  printf("revision: %u\n", data->revision);
  return true;
}

bool readFileIntoMemory(uint8_t *buffer, const char *filename,
                        uint32_t file_size) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    printf("Failed to read %s into memory\n", filename);
    return false;
  }

  size_t length_read = fread(buffer, sizeof(uint8_t), file_size, fp);
  if (length_read == 0) {
    printf("error in fread()");
    return false;
  }
  return true;
}

int main(int argc, const char **argv) {

  if (argc < 2) {
    printf("Usage: %s <wld file location>\n", argv[0]);
    return -1;
  }

  const char *filename = argv[1];

  struct stat file_info;
  if (stat(filename, &file_info) != 0) {
    printf("Error reading file (%s) info\n", filename);
    return 1;
  }

  uint8_t *buffer = NULL;
  buffer = (uint8_t *)malloc(file_info.st_size);
  if (buffer == NULL) {
    printf("Failure to allocate %lu bytes\n", file_info.st_size);
    return 1;
  }

  FileFormatHeader file_format;

  readFileIntoMemory(buffer, filename, file_info.st_size);
  parseFileFormatHeader(buffer, &file_format);
  free(buffer);

  return 0;
}
