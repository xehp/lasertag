/*
log.h

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

History

2021-06-30 Created by Henrik Bjorkman
*/


#ifndef AVR_LOG_H
#define AVR_LOG_H

#include <stdint.h>




#define ERRLOG_P(str, n) {errlog_P(PSTR(str), n);}

// To save flash memory the __FILE__ is not given here for now. Add it if needed.
#define ASSERT(c) {if (!(c)) {ERRLOG_P("ASSERT ", __LINE__);}}


void errlog_P(const char *pgm_addr, uint16_t errorCode);


#endif
