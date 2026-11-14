#ifndef ITEM_H
#define ITEM_H

#include "types.h"
#include "ball.h"
#include "gx.h"
#include "mathutil.h"

struct GMAModel;
struct PhysicsBall;
struct StageCollHeader;

/* Banana placement in a stagedef collision header. */
struct StageBanana
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s32 type;  /* item subtype: 0 single, 1 bunch */
};  /* size = 0x10 */

/* Stage item (banana and so on). SMB1's struct Item plus the world position and the
 * downward raycast result used by the shadow renderer. */
struct Item
{
    /*0x00*/ s16 id;  /* index in the item pool */
    /*0x02*/ s16 unk2;  /* running spawn counter */
    /*0x04*/ s16 type;  /* IT_* */
    /*0x06*/ s16 subType;  /* single vs. bunch for bananas */
    /*0x08*/ u32 flags;  /* ITEM_FLAG_* */
    /*0x0C*/ s16 unkC;
    /*0x0E*/ s16 state;
    /*0x10*/ s16 unk10;
    /*0x12*/ s16 unk12;  /* remaining lifetime; 0 destroys the item */
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    /*0x1C*/ void * DPTR modelLODs;  /* DPTR fields: Item stays 0xB4 on x64 (literal 0xB4 memsets) */
    /*0x20*/ Vec pos;
    /*0x2C*/ Vec vel;
    /*0x38*/ s16 rotX;
    /*0x3A*/ s16 rotY;
    /*0x3C*/ s16 rotZ;
    /*0x3E*/ s16 rotVelX;
    /*0x40*/ s16 rotVelY;
    /*0x42*/ s16 rotVelZ;
    /*0x44*/ Vec prevPos;
    /*0x50*/ s16 prevRotX;
    /*0x52*/ s16 prevRotY;
    /*0x54*/ s16 prevRotZ;
    u8 filler56[2];
    /*0x58*/ void (* DPTR coliFunc)(struct Item *, struct PhysicsBall *);
    /*0x5C*/ s8 animGroupId;  /* item group this item is attached to */
    /*0x5E*/ s16 unk5E;
    /*0x60*/ struct StageBanana * DPTR stageBanana;
    /*0x64*/ s32 unk64;  /* shadow currently on the stage */
    /*0x68*/ struct GMAModel * DPTR shadowModel;
    /*0x6C*/ S16Vec unk6C;  /* shadow rotation */
    /*0x74*/ float unk74;  /* shadow height relative to the item */
    /*0x78*/ GXColor shadowColor;
    /*0x7C*/ Vec unk7C;  /* shadow scale */
    /*0x88*/ float unk88;  /* shadow fade */
    /*0x8C*/ Vec worldPos;  /* pos transformed by the item group */
    /*0x98*/ struct RaycastHit hit;  /* stage below the item */
};  /* size = 0xB4 */

/* struct Item::flags */
enum
{
    ITEM_FLAG_INVISIBLE = 1 << 0,
    ITEM_FLAG_4 = 1 << 2,
    ITEM_FLAG_SHADOW = 1 << 5,
    ITEM_FLAG_ON_STAGE = 1 << 6,  /* set by event_item_tick from the raycast */
    ITEM_FLAG_NO_RAYCAST = 1 << 7,
};

/* struct Item::type */
enum
{
    IT_COIN,
    IT_FGT_BANANA,
    IT_MINI_RACE,
    IT_FGT_POWER,
    IT_FGT_PUNCH,
    IT_PILOT,
    IT_DOGFIGHT,
    IT_TMP001,
    IT_TMP002,
    IT_TMP003,
    IT_TMP004,
    IT_TMP005,
    IT_TMP006,
    IT_TMP007,
    IT_TMP008,
    IT_TMP009,
    IT_TMP010,
    IT_TMP011,
    IT_TMP012,
    IT_TMP013,
    IT_TMP014,
    IT_TMP015,
    IT_TMP016,
    IT_COUNT,
};

struct ItemFuncs
{
    void (*init)(struct Item *);
    void (*tick)(struct Item *);
    void (*disp)(struct Item *);
    void (*coli)(struct Item *, struct PhysicsBall *);
    void (*dest)(struct Item *);
    void (*replayInit)(struct Item *);
    void (*debug)(struct Item *);
};  /* size = 0x1C */

extern s16 g_next_item_id;
extern struct Item items[256];
extern void (*g_item_tick_callback)(void);

extern char *item_names[];
extern void (*item_init_funcs[])(struct Item *);
extern void (*item_tick_funcs[])(struct Item *);
extern void (*item_disp_funcs[])(struct Item *);
extern void (*item_coli_funcs[])(struct Item *, struct PhysicsBall *);
extern void (*item_dest_funcs[])(struct Item *);
extern void (*item_replay_init_funcs[])(struct Item *);
extern void (*item_debug_funcs[])(struct Item *);
extern struct ItemFuncs item_default_funcs;

void event_item_init(void);
void event_item_tick(void);
void event_item_dest(void);
void draw_items(void);
int spawn_item(struct Item *src);
void draw_item_shadows(void);
void item_replay_init_all(int frame);
void init_coli_header_items(struct StageCollHeader *coliHeaders, int count);
void item_replace_type_funcs(int itemType, struct ItemFuncs *newFuncs);

void item_default_init(struct Item *item);
void item_default_tick(struct Item *item);
void item_default_disp(struct Item *item);
void item_default_coli(struct Item *item, struct PhysicsBall *ball);
void item_default_dest(struct Item *item);
void item_default_replay_init(struct Item *item);
void item_debug_default(struct Item *item);

/* item_coin.c */
struct ModelLOD
{
    s32 modelId;  /* init_common_gma model index, -1 ends the table */
    float distance;  /* use this model when the projected size is at least this */
};

struct GMAModel *g_something_with_coins(struct ModelLOD **modelLODs);
void item_coin_init(struct Item *item);
void item_coin_tick(struct Item *item);
void item_coin_disp(struct Item *item);
void item_coin_coli(struct Item *item, struct PhysicsBall *ball);
void item_coin_dest(struct Item *item);
void item_coin_replay_init(struct Item *item);
void item_debug_coin(struct Item *item);

/* pilot2.rel */
void item_pilot_init(struct Item *item);
void item_pilot_tick(struct Item *item);
void item_pilot_disp(struct Item *item);
void item_pilot_coli(struct Item *item, struct PhysicsBall *ball);
void item_pilot_dest(struct Item *item);
void item_pilot_replay_init(struct Item *item);
void item_debug_pilot(struct Item *item);

/* dogfight.rel */
void item_dogfight_init(struct Item *item);
void item_dogfight_tick(struct Item *item);
void item_dogfight_disp(struct Item *item);
void item_dogfight_coli(struct Item *item, struct PhysicsBall *ball);
void item_dogfight_dest(struct Item *item);
void item_dogfight_replay_init(struct Item *item);
void item_debug_dogfight(struct Item *item);

#endif
