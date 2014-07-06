// *** Hardwarespecific defines ***
#define cbi(reg, mask) GPIOPinWrite(reg, mask, 0)
#define sbi(reg, mask) GPIOPinWrite(reg, mask, mask)
#define pulse_high(reg, bitmask) { sbi(reg, bitmask); cbi(reg, bitmask); }
#define pulse_low(reg, bitmask) { cbi(reg, bitmask); sbi(reg, bitmask); }

#define swap(type, i, j) {type t = i; i = j; j = t;}

#define fontbyte(x) cfont.font[x]  

#define pgm_read_word(data) *(data)
#define pgm_read_byte(data) *(data)
#define PROGMEM
typedef volatile uint32_t regtype;
typedef uint8_t regsize;
typedef unsigned short* bitmapdatatype;

