#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include <termina.h>

#define DEBUG_BASE UINT32_C(0x80000000)

#define DSU_CTRL_OFFSET UINT32_C(0x0)

#define DSU_STATUS_OFFSET UINT32_C(0x4)

#define DSU_CTRL_RESET UINT32_C(0x1)

typedef struct {
    __termina_task_t __task;
    volatile uint32_t * register;
    __termina_sink_port_t timer_ev;
} WATCHDOG;

Result WATCHDOG__reboot_system(WATCHDOG * const self, TimeVal _current_time);

#endif
