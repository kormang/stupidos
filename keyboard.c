#include "keyboard.h"
#include "pic.h"
#include "io.h"
#include "isr.h"

 // Reference: http://www.osdever.net/bkerndev/Docs/keyboard.htm
 /* KBDUS means US Keyboard Layout. This is a scancode table
 *  used to layout a standard US keyboard. I have left some
 *  comments in to give you an idea of what key is what, even
 *  though I set it's array index to 0. You can change that to
 *  whatever you want using a macro, if you wish! */
 unsigned char kbdus[128] =
 {
     0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
   '9', '0', '-', '=', '\b',	/* Backspace */
   '\t',			/* Tab */
   'q', 'w', 'e', 'r',	/* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
     0,			/* 29   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
  '\'', '`',   0,		/* Left shift */
  '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
   'm', ',', '.', '/',   0,				/* Right shift */
   '*',
     0,	/* Alt */
   ' ',	/* Space bar */
     0,	/* Caps lock */
     0,	/* 59 - F1 key ... > */
     0,   0,   0,   0,   0,   0,   0,   0,
     0,	/* < ... F10 */
     0,	/* 69 - Num lock*/
     0,	/* Scroll Lock */
     0,	/* Home key */
     0,	/* Up Arrow */
     0,	/* Page Up */
   '-',
     0,	/* Left Arrow */
     0,
     0,	/* Right Arrow */
   '+',
     0,	/* 79 - End key*/
     0,	/* Down Arrow */
     0,	/* Page Down */
     0,	/* Insert Key */
     0,	/* Delete Key */
     0,   0,   0,
     0,	/* F11 Key */
     0,	/* F12 Key */
     0,	/* All other keys are undefined */
 };

static void (*on_key_pressed)(char);

static void keyboard_handler(registers_t* regs)
{
    uint8_t scancode;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* This one can be used to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        on_key_pressed(kbdus[scancode]);
    }
}

void init_keyboard () {
  uint16_t mask;
  register_interrupt_handler(IRQ(1), &keyboard_handler);
  mask = inb(PIC1_DATA);
  mask = mask & 0xFD; // enable IRQ 1
  outb(PIC1_DATA, mask);
}

void set_on_keypressed(void (*f)(char)) {
  on_key_pressed = f;
}
