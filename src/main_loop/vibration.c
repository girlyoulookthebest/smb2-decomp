/* Controller rumble. g_rumble_controller queues a command for a pad and
 * the vibration event hands queued commands to the motors each frame.
 * Commands: 1 and 2 run the motor for dur frames (stopping softly or
 * hard), 3 stops hard now, 4 stops now. */
#pragma peephole off
#include "types.h"
#include "event.h"
#include "mode.h"
#include "os.h"

struct PadMotor
{
    /*0x00*/ u8 cmd;
    /*0x02*/ u16 dur;
}; /* size = 0x4 */

void PADControlMotor(int chan, u32 command);
void PADControlAllMotors(const u32 *commands);

u8 rumble_enabled_bitflag = 0xF;
struct PadMotor g_some_pad_motor_array1[4];
struct PadMotor g_some_pad_motor_array2[4];

void event_vibration_init(void)
{
    memset(g_some_pad_motor_array1, 0, sizeof(g_some_pad_motor_array1));
    memset(g_some_pad_motor_array2, 0, sizeof(g_some_pad_motor_array2));
}

void event_vibration_tick(void)
{
    int i;
    struct PadMotor *active;
    struct PadMotor *queued;

    if (main_mode == 0)
        return;

    active = g_some_pad_motor_array2;
    queued = g_some_pad_motor_array1;
    for (i = 0; i < 4; i++, active++, queued++)
    {
        if (queued->cmd != 0)
        {
            active->cmd = queued->cmd;
            if (queued->cmd == 4 || queued->cmd == 3)
                active->dur = 0;
            else if (queued->cmd == 1 || queued->cmd == 2)
            {
                PADControlMotor(i, 1);
                active->dur = queued->dur;
            }
            queued->cmd = 0;
        }
    }

    active = g_some_pad_motor_array2;
    for (i = 0; i < 4; i++, active++)
    {
        if (active->cmd == 4)
        {
            PADControlMotor(i, 0);
            active->cmd = 0;
        }
        else if (active->cmd == 3)
        {
            PADControlMotor(i, 2);
            active->cmd = 0;
        }
        else
        {
            if (active->dur != 0)
                active->dur--;
            if (active->dur == 1)
            {
                PADControlMotor(i, (active->cmd == 1) ? 2 : 0);
                active->cmd = 0;
                active->dur = 0;
            }
        }
    }
}

void event_vibration_dest(void)
{
    u32 commands[4];
    int i;

    for (i = 0; i < 4; i++)
        commands[i] = 2;
    PADControlAllMotors(commands);
    memset(g_some_pad_motor_array1, 0, sizeof(g_some_pad_motor_array1));
    memset(g_some_pad_motor_array2, 0, sizeof(g_some_pad_motor_array2));
}

void g_rumble_controller(int pad, int cmd, int dur)
{
    struct PadMotor *motor;

    if (events[EVENT_VIBRATION].state != EV_STATE_RUNNING)
        return;
    if (cmd == 0)
        return;
    if (pad < 0 || pad >= 4)
        return;
    if (dur > 180)
        return;
    if ((rumble_enabled_bitflag & (1 << pad)) == 0)
        return;
    if (cmd == 4 || cmd == 3)
        dur = 0;

    motor = &g_some_pad_motor_array1[pad];
    switch (motor->cmd)
    {
    case 4:
        if (cmd == 3)
            motor->cmd = cmd;
        break;
    case 1:
        switch (cmd)
        {
        case 3:
        case 4:
            motor->cmd = cmd;
            break;
        case 1:
            if (motor->dur < dur)
            {
                motor->cmd = cmd;
                motor->dur = dur;
            }
            break;
        }
        break;
    case 2:
        switch (cmd)
        {
        case 3:
        case 4:
            motor->cmd = cmd;
            break;
        case 2:
            if (motor->dur < dur)
                break;
            /* fallthrough */
        case 1:
            motor->cmd = cmd;
            motor->dur = dur;
            break;
        }
        break;
    case 0:
        motor->cmd = cmd;
        motor->dur = dur;
        break;
    }
}

void fn_1_FD62C(u8 flags)
{
    rumble_enabled_bitflag = flags;
}

u8 fn_1_FD63C(void)
{
    return rumble_enabled_bitflag;
}
