#include "screen.h"
#include "io.h"

static volatile uint8_t currentRow = 0;
static volatile uint8_t currentColumn = 0;
static char* const video_memory = (char*)0xb8000;

void screen_set_cursor (uint8_t row, uint8_t column) {

  size_t pos = row * 80 + column;
  outb(0x3D4, 0xe); // tell VGA board to receive higher cursor byte
  outb(0x3D5, (uint8_t)(pos >> 8)); // send higher cursor byte
  outb(0x3D4, 0xf); // tell VGA board to receive lower cursor byte
  outb(0x3D5, (uint8_t)pos); // send lower cursor byte

  currentRow = row;
  currentColumn = column;
}

void screen_clear () {
	uint32_t i = 0;

	while (i < 80 * 25 * 2) {
		video_memory[i] = ' ';
		video_memory[i+1] = COLOR_CODE;
		i += 2;
	}

  screen_set_cursor(0, 0);
}

static void scroll(uint8_t row) {
  if (row < 25) {
    return;
  }

  size_t i = 0;

  for (; i < 24*80*2; ++i) {
    video_memory[i] = video_memory[i+160];
  }

  for (i = 24*80*2; i < 25*80*2; i += 2) {
    video_memory[i] = ' ';
    video_memory[i+1] = COLOR_CODE;
  }
}

void screen_put_char(const char c) {
  __asm__ volatile ("pushf"); // save eflags
  __asm__ volatile ("cli"); // clear interrupts flag to make printing char atomic

  if (c == '\r') {
    currentColumn = 0;
  } else if (c == '\n') {
    currentRow++;
    currentColumn = 0;
  } else if (c == 0x8 && currentColumn > 0) { // backspace
    currentColumn--;
  } else if (c == '\t') {
    currentColumn = (currentColumn + 8) & ~7;
  } else if (c >= ' ') {
    const size_t pos = (currentRow*80 + currentColumn) * 2;
    video_memory[pos] = c;
    video_memory[pos+1] = COLOR_CODE;
    currentColumn++;
  }

  if (currentColumn >= 80) {
    currentColumn = 0;
    currentRow++;
  }

  scroll(currentRow);

  if (currentRow >= 25) {
    currentRow = 24;
  }

  screen_set_cursor(currentRow, currentColumn);
  __asm__ volatile ("popf"); // restore eflags
}

void screen_print (const char* const str) {
	size_t i = 0;

	while(str[i]) {
    screen_put_char(str[i]);
		++i;
	}
}

void screen_print_hex (uint32_t number) {
  screen_put_char('0');
  screen_put_char('x');
  uint32_t offset = 32;
  do {
    offset -= 4;
    uint32_t part = (number >> offset) & 0xF;
    char c;
    if (part < 10) {
      c = ((char)('0' + part));
    } else {
      c = ((char)('A' + (part - 10)));
    }
    screen_put_char(c);
  } while (offset);
}
