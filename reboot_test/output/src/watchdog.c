
#include "watchdog.h"

Result WATCHDOG__reboot_system(WATCHDOG * const self, TimeVal _current_time) {
    
    Result ret;
    ret.__variant = Result__Ok;

    uint32_t ctrl_reg = *self->register;

    ctrl_reg = ctrl_reg | DSU_CTRL_RESET;

    *self->register;

    return ret;

}
