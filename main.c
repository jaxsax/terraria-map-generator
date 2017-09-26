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

bool parseFileFormatHeader(uint8_t **buffer, FileFormatHeader *data) {
  uint8_t *temp_buffer = *buffer;
  temp_buffer = readUint32(temp_buffer, &data->currentRelease);
  temp_buffer = readUint64(temp_buffer, &data->magicNumber);
  temp_buffer = readUint32(temp_buffer, &data->revision);
  temp_buffer = readUint64(temp_buffer, &data->unknown_param1);

  if (data->magicNumber != 172097103742133618) {
    return false;
  }

  uint16_t sections = 0;
  temp_buffer = readUint16(temp_buffer, &sections);
  for (uint16_t i = 0; i < sections; i++) {
    uint32_t section_id = 0;
    temp_buffer = readUint32(temp_buffer, &section_id);

    printf("%u; id=%u\n", i, section_id);
  }
  uint16_t tile_types = 0;
  temp_buffer = readUint16(temp_buffer, &tile_types);

  uint16_t mask = 0x80;
  for (uint16_t i = 0; i < tile_types; i++) {
    uint8_t flags = 0;
    if (mask == 0x80) {
      temp_buffer = readUint8(temp_buffer, &flags);
      mask = 0x01;
    } else {
      mask <<= 1;
    }

    if ((flags & mask) == mask) {
      printf("Adding multitiles %u\n", i);
    }
  }

  data->sectionCount = sections;
  data->tileTypeCount = tile_types;

  *buffer = temp_buffer;

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

    fclose(fp);
    return false;
  }

  fclose(fp);
  return true;
}

static void printFileFormatHeader(FileFormatHeader header) {
  printf("currentRelease: %u\n", header.currentRelease);
  printf("magicNumber: %lu\n", header.magicNumber);
  printf("revision: %u\n", header.revision);
  printf("sections: %u\n", header.sectionCount);
  printf("tile_types: %u\n", header.tileTypeCount);
}

static void cleanup(uint8_t *buffer) {
  if (!buffer) {
    return;
  }

  free(buffer);
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
  buffer = (uint8_t *)calloc(sizeof(uint8_t), file_size);
  if (buffer == NULL) {
    printf("Failure to allocate %lu bytes\n", file_size);
    return 1;
  }

  /**
   * Using an intermediate buffer so that we can move the pointer position as we
   * read the headers
   */
  uint8_t *intermediate_buffer = buffer;
  FileFormatHeader file_format;

  readFileIntoMemory(buffer, filename, file_size);
  bool parseFileHeaderSuccess =
      parseFileFormatHeader(&intermediate_buffer, &file_format);

  if (!parseFileHeaderSuccess) {
    printf("Error parsing %s, invalid checksum\n", filename);

    cleanup(buffer);
    return 1;
  }

  printFileFormatHeader(file_format);

  cleanup(buffer);
  return 0;
}
