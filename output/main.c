
#include <rtems.h>

#include <termina.h>

#include "app.h"

#include "watchdog.h"

extern void __termina_app__init_globals();

#define __WATCHDOG__timer_ev 0

static rtems_task __rtems_task__WATCHDOG(rtems_task_argument arg) {
    
    WATCHDOG * self = (WATCHDOG *)arg;
    rtems_status_code status = RTEMS_SUCCESSFUL;
    uint32_t next_msg = 0;
    size_t size = 0;

    Result result;
    result.__variant = Result__Ok;

    TimeVal reboot_system__msg_data;

    for (;;) {
        
        status = rtems_message_queue_receive(self->__task.msgq_id, &next_msg,
                                             &size, RTEMS_WAIT,
                                             RTEMS_NO_TIMEOUT);

        if (RTEMS_SUCCESSFUL != status) {
            break;
        }

        switch (next_msg) {
            
            case __WATCHDOG__timer_ev:

                status = rtems_message_queue_receive(self->timer_ev,
                                                     &reboot_system__msg_data,
                                                     &size, RTEMS_NO_WAIT,
                                                     RTEMS_NO_TIMEOUT);

                if (RTEMS_SUCCESSFUL != status) {
                    rtems_shutdown_executive(1);
                }

                result = WATCHDOG__reboot_system(self, reboot_system__msg_data);

                if (result.__variant != Result__Ok) {
                    rtems_shutdown_executive(1);
                }

                break;

            default:

                rtems_shutdown_executive(1);

                break;

        }

    }

    rtems_shutdown_executive(1);

    return;

}

void __rtems_periodic_timer__timer(rtems_id _timer_id, void * _ignored) {
    
    rtems_status_code status = RTEMS_SUCCESSFUL;

    status = rtems_message_queue_send(timer.__timer.sink_msgq_id,
                                      &timer.__timer.current, sizeof(TimeVal));

    if (RTEMS_SUCCESSFUL == status) {
        
        status = rtems_message_queue_send(timer.__timer.task_msgq_id,
                                          &timer.__timer.task_port,
                                          sizeof(uint32_t));
    }

    if (RTEMS_SUCCESSFUL != status) {
        rtems_shutdown_executive(1);
    }

    __termina__add_timeval(&timer.__timer.current, &timer.period);

    status = __rtems__timer_delay_at(timer.__timer.timer_id,
                                     &timer.__timer.current,
                                     __rtems_periodic_timer__timer);

    if (RTEMS_SUCCESSFUL != status) {
        rtems_shutdown_executive(1);
    }

    return;

}

static void __rtems_app__enable_protection() {
    
    Result result;

    result.__variant = Result__Ok;

}

static void __rtems_app__init_globals() {
    
    Result result;

    result.__variant = Result__Ok;

    rtems_status_code status = RTEMS_SUCCESSFUL;

    status = __rtems__create_msg_queue(1, sizeof(uint32_t),
                                       &watchdog_instance.__task.msgq_id);

    if (RTEMS_SUCCESSFUL != status) {
        rtems_shutdown_executive(1);
    }

    status = __rtems__create_msg_queue(1, sizeof(TimeVal),
                                       &watchdog_instance.timer_ev);

    if (RTEMS_SUCCESSFUL != status) {
        rtems_shutdown_executive(1);
    }

    timer.__timer.task_msgq_id = watchdog_instance.__task.msgq_id;
    timer.__timer.sink_msgq_id = watchdog_instance.timer_ev;
    timer.__timer.task_port = __WATCHDOG__timer_ev;

    status = __rtems__create_timer(&timer.__timer.timer_id);

    if (RTEMS_SUCCESSFUL != status) {
        rtems_shutdown_executive(1);
    }

}

static void __rtems_app__install_emitters(TimeVal * current) {
    
    rtems_status_code status = RTEMS_SUCCESSFUL;

    if (RTEMS_SUCCESSFUL == status) {
        
        timer.__timer.current = *current;

        __termina__add_timeval(&timer.__timer.current, &timer.period);

        status = __rtems__timer_delay_at(timer.__timer.timer_id,
                                         &timer.__timer.current,
                                         __rtems_periodic_timer__timer);
    }

}

static void __rtems_app__create_tasks() {
    
    rtems_status_code status = RTEMS_SUCCESSFUL;

    status = __rtems__create_task(255, 4096, __rtems_task__WATCHDOG,
                                  (rtems_task_argument)&watchdog_instance,
                                  &watchdog_instance.__task.task_id);

    if (RTEMS_SUCCESSFUL != status) {
        rtems_shutdown_executive(1);
    }

}

rtems_task Init(rtems_task_argument _ignored) {
    
    TimeVal current;

    __termina__clock_get_uptime(&current);

    __termina_app__init_globals();

    __rtems_app__init_globals();

    __rtems_app__enable_protection();

    __rtems_app__install_emitters(&current);

    __rtems_app__create_tasks();

    rtems_task_delete(RTEMS_SELF);

}

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 2
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 0

#define CONFIGURE_MESSAGE_BUFFER_MEMORY (  \
        CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \
            1, \
            sizeof(uint32_t) \
        ) \
    + \
        CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \
            1, \
            sizeof(TimeVal) \
        ) \
    )

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
