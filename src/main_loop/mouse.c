/* Debug mouse: a pad-driven cursor that can pick up and drag sprites. */
#pragma peephole off
#include "types.h"
#include "bitmap.h"
#include "dip.h"
#include "input.h"
#include "pool.h"
#include "sprite.h"

struct Mouse
{
    /*0x00*/ s16 x;
    /*0x02*/ s16 y;
    /*0x04*/ s16 prevX;
    /*0x06*/ s16 prevY;
    /*0x08*/ s16 deltaX;
    /*0x0A*/ s16 deltaY;
    /*0x0C*/ s16 spriteIdx;
    /*0x0E*/ s16 unkE;
};  /* size = 0x10 */

extern u32 lbl_1_bss_6C7E0;
extern struct Sprite sprites[];

void textdraw_reset(void);
void textdraw_set_pos(float x, float y);
void textdraw_print(char *str);

static struct Mouse s_mouse;

void event_mouse_init(void)
{
    s_mouse.x = 320;
    s_mouse.y = 240;
    s_mouse.spriteIdx = -1;
}

void event_mouse_tick(void)
{
    int step;

    if (lbl_1_bss_6C7E0 == 0U && (dip_switches & DIP_TEST_CAM) == 0)
    {
        s_mouse.prevX = s_mouse.x;
        s_mouse.prevY = s_mouse.y;
        step = 1;
        if (analog_inputs[0][0] & PAD_BUTTON_B)
            step = 8;
        if (pad_status_groups[0].unk0[4].button & PAD_BUTTON_UP)
            s_mouse.y -= step;
        if (pad_status_groups[0].unk0[4].button & PAD_BUTTON_DOWN)
            s_mouse.y += step;
        if (pad_status_groups[0].unk0[4].button & PAD_BUTTON_LEFT)
            s_mouse.x = s_mouse.x - step;
        if (pad_status_groups[0].unk0[4].button & PAD_BUTTON_RIGHT)
            s_mouse.x = s_mouse.x + step;
        s_mouse.x = s_mouse.x + (pad_status_groups[0].unk0[0].stickX * 0.1);
        s_mouse.y += (-pad_status_groups[0].unk0[0].stickY * 0.1);
        if (s_mouse.x < 0)
            s_mouse.x = 0;
        if (s_mouse.x > 624)
            s_mouse.x = 624;
        if (s_mouse.y < 0)
            s_mouse.y = 0;
        if (s_mouse.y > 464)
            s_mouse.y = 464;
        s_mouse.deltaX = s_mouse.x - s_mouse.prevX;
        s_mouse.deltaY = s_mouse.y - s_mouse.prevY;
        if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_A)
        {
            if (s_mouse.spriteIdx >= 0)
                s_mouse.spriteIdx = -1;
            else
            {
                struct Sprite *sprite;
                s8 *status;
                int i;

                s_mouse.spriteIdx = -1;
                sprite = sprites;
                status = g_poolInfo.spritePool.statusList;
                for (i = 0; i < g_poolInfo.spritePool.count; i++, sprite++, status++)
                {
                    if (*status != 0
                     && s_mouse.x >= sprite->left
                     && s_mouse.x <= sprite->right
                     && s_mouse.y >= sprite->top
                     && s_mouse.y <= sprite->bottom)
                    {
                        s_mouse.spriteIdx = i;
                        break;
                    }
                }
            }
        }
        if (s_mouse.spriteIdx >= 0)
        {
            struct Sprite *sprite = &sprites[s_mouse.spriteIdx];

            while (sprite->prev != NULL)
                sprite = sprite->prev;
            sprite->x = sprite->x + s_mouse.deltaX;
            sprite->y = sprite->y + s_mouse.deltaY;
        }
    }
}

void event_mouse_dest(void)
{
}

void g_smth_with_drawing_text_sprites(void)
{
    struct SpriteDrawRequest req;
    struct Sprite *sprite;

    if (lbl_1_bss_6C7E0 == 0U && (dip_switches & DIP_TEST_CAM) == 0)
    {
        req.bmpId = 1;
        req.x = s_mouse.x;
        req.y = s_mouse.y;
        req.z = 0.01f;
        req.scaleX = 1.0f;
        req.scaleY = 1.0f;
        req.u1 = 0.0f;
        req.v1 = 0.0f;
        req.u2 = 1.0f;
        req.v2 = 1.0f;
        req.rotation = 0;
        req.opacity = 1.0f;
        req.unk30 = -1;
        req.flags = 5;
        req.mulColor = -1;
        req.addColor = 0;
        draw_sprite_draw_request(&req);
        if (s_mouse.spriteIdx >= 0)
        {
            sprite = &sprites[s_mouse.spriteIdx];
            textdraw_reset();
            textdraw_set_pos(sprite->left - 8, sprite->top - 8);
            textdraw_print("+");
            textdraw_set_pos(sprite->left - 8, sprite->bottom);
            textdraw_print("+");
            textdraw_set_pos(sprite->right, sprite->top - 8);
            textdraw_print("+");
            textdraw_set_pos(sprite->right, sprite->bottom);
            textdraw_print("+");
        }
    }
}
