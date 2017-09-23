#include "main.h"

uint8_t *readUint8(uint8_t *buffer, uint8_t *data) {
  *data = *buffer;
  buffer = buffer + 1;
  return buffer;
}

uint8_t *readUint16(uint8_t *buffer, uint16_t *data) {
  *data = (uint16_t)(*(buffer + 1) << 8) | *buffer;
  buffer = buffer + 2;
  return buffer;
}

uint8_t *readUint32(uint8_t *buffer, uint32_t *data) {
  *data = ((uint32_t) * (buffer + 3) << 24) |
          ((uint32_t) * (buffer + 2) << 16) | ((uint32_t) * (buffer + 1) << 8) |
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

  uint16_t sections = 0;
  buffer = readUint16(buffer, &sections);
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
                        size_t file_size) {
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

  size_t file_size = (size_t)file_info.st_size;
  uint8_t *buffer = NULL;
  buffer = (uint8_t *)malloc(file_size);
  if (buffer == NULL) {
    printf("Failure to allocate %lu bytes\n", file_size);
    return 1;
  }

  FileFormatHeader file_format;

  readFileIntoMemory(buffer, filename, file_size);
  parseFileFormatHeader(buffer, &file_format);
  free(buffer);

  return 0;
}
