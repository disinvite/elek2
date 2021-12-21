#ifndef INPUT_TIMER_H
#define INPUT_TIMER_H

#include "common/types.h"

extern int game_tics;
extern int game_seconds;

void PIT_Setup(void);
void PIT_Close(void);

#endif
