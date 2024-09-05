
#include <termina.h>

#include "app.h"

#include "watchdog.h"

void __termina_app__init_globals() {
    
    timer.period.tv_sec = 10;
    timer.period.tv_usec = 0;
    watchdog_instance.register = (volatile uint32_t *)0x80000000;

    return;

}
