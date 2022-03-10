#ifndef _POTENTIOMETER_LOCK_H_
#define _POTENTIOMETER_LOCK_H_

#include <stddef.h>
#include <stdint.h>

typedef uint8_t code_char;

extern int init_potentiometer_lock(code_char *code, size_t code_length);

// Setters
extern int potentiometer_lock_set_code(code_char *code, size_t code_length);
extern void potentiometer_lock_set_on_success_callback(void (*callback) (void));
extern void potentiometer_lock_set_on_failure_callback(void (*callback) (void));

#endif
