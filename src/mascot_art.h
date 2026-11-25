#ifndef MASCOT_ART_H
#define MASCOT_ART_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    MASCOT_CLAWD = 0, // 克劳德橙蟹
    MASCOT_CAT   = 1, // 小紫猫
    MASCOT_WHALE = 2, // 蓝鲸 Deepy (客串)
} MascotKind;

typedef enum {
    BODY_STAND = 0,
    BODY_TALL,
    BODY_SQUASH,
    BODY_LOAF,
    BODY_STRETCH,
    BODY_LEAN_L,
    BODY_LEAN_R,
    BODY_TURN_L,
    BODY_TURN_R,
    BODY_FLAT,
} MascotBodyId;

typedef enum {
    EYE_OPEN = 0,
    EYE_LEFT,
    EYE_RIGHT,
    EYE_UP,
    EYE_HALF,
    EYE_SHUT,
    EYE_HAPPY,
    EYE_SQUINT,
    EYE_WIDE,
} MascotEyeId;

typedef enum {
    MOUTH_NONE = 0,
    MOUTH_SMALL,
    MOUTH_YAWN,
} MascotMouthId;

typedef enum {
    LEG_NONE = 0,
    LEG_STAND,
    LEG_CROUCH,
    LEG_TUCK,
    LEG_WALK_A,
    LEG_WALK_B,
    LEG_FLAIL_A,
    LEG_FLAIL_B,
} MascotLegId;

typedef enum {
    CLAW_STUB = 0,
    CLAW_FRONT,
    CLAW_UP,
    CLAW_OPEN,
    CLAW_WAVE,
    CLAW_WRITE,
} MascotClawId;

typedef enum {
    EAR_STAND = 0,
    EAR_NOD,
    EAR_LOAF,
    EAR_AIRPLANE,
    EAR_TWITCH_L,
    EAR_TWITCH_R,
} MascotEarId;

typedef enum {
    PAW_NONE = 0,
    PAW_CHEER,
    PAW_UP,
    PAW_UP_R,
    PAW_GROOM_1,
    PAW_GROOM_2,
    PAW_WRITE,
} MascotPawId;

typedef enum {
    TAIL_UP = 0,
    TAIL_SWAY,
    TAIL_CURL,
    TAIL_LOAF,
    TAIL_PUFF,
} MascotTailId;

typedef struct {
    MascotBodyId body;
    MascotEyeId eyes;
    MascotMouthId mouth;
    MascotLegId legs;
    MascotClawId claw_l;
    MascotClawId claw_r;
    MascotEarId ears;
    MascotPawId paws;
    MascotTailId tail;
    int8_t sink;         // 垂直微沉格数 (-1..2)
    bool flip_x;         // 水平镜像转身
    bool rainbow;        // 彩虹极光皮肤模式
    uint8_t rainbow_phase;
} MascotPose;

typedef enum {
    MINI_COWBOY = 0,
    MINI_SHADES,
    MINI_WIZARD,
} MiniCrabKind;

typedef enum {
    PROP_COFFEE_0 = 0,
    PROP_COFFEE_1,
    PROP_DONUT_0,
    PROP_DONUT_1,
    PROP_BOWL,
    PROP_BOWL_EMPTY,
    PROP_CHOP_0,
    PROP_CHOP_1,
    PROP_FISH,
    PROP_FISH_BONE,
    PROP_STICK_L_G,
    PROP_STICK_UP_G,
    PROP_STICK_R_G,
    PROP_STICK_L_P,
    PROP_STICK_UP_P,
    PROP_STICK_R_P,
    PROP_MIC_STAND,
    PROP_SHEET,
    PROP_QUILL_0,
    PROP_QUILL_1,
    PROP_BOOK_0,
    PROP_BOOK_1,
    PROP_BULB_OFF,
    PROP_BULB_ON,
    PROP_SHADES,
    PROP_SURFBOARD,
    PROP_WAVE_0,
    PROP_WAVE_1,
    PROP_ICE_CRYSTAL,
    PROP_LASER_DOT,
    PROP_SEED,
    PROP_PLANT_1,
    PROP_PLANT_2,
    PROP_PLANT_3,
    PROP_PLANT_4,
    PROP_FLOWER_0,
    PROP_FLOWER_1,
    PROP_BF_UP,
    PROP_BF_DN,
    PROP_ENV_SEALED,
    PROP_ENV_OPEN,
    PROP_LETTER,
    PROP_SIREN_R,
    PROP_SIREN_B,
    PROP_SIREN_F_R,
    PROP_SIREN_F_B,
    PROP_HEADBAND,
    PROP_GOAL_FLAME_0,
    PROP_GOAL_FLAME_1,
    PROP_HOURGLASS_0,
    PROP_HOURGLASS_1,
    PROP_WATCH,
    PROP_WHALE_SPOUT_0,
    PROP_WHALE_SPOUT_1,
    PROP_WHALE_SPOUT_2,
    PROP_FIRE_0,
    PROP_FIRE_1,
    PROP_FIRE_2,
} MascotPropId;

typedef enum {
    SYM_HEART = 0,
    SYM_NOTE,
    SYM_SPARK,
    SYM_Z_SMALL,
    SYM_Z_BIG,
    SYM_BUBBLE_0,
    SYM_BUBBLE_1,
    SYM_QUESTION,
    SYM_BANG,
    SYM_SWEAT,
    SYM_TILDE,
    SYM_VEIN,
    SYM_FIRE_SPARK_0,
    SYM_FIRE_SPARK_1,
} MascotSymId;

// 默认站姿初始化
void mascot_pose_default(MascotPose *p);

// 绘制双宠 (origin 为角色底部中心基准坐标，标准身高 10 格)
void mascot_draw(uint16_t *fb, MascotKind kind, int cx, int base_y, int px_scale, const MascotPose *pose);

// 绘制迷你蟹 (牛仔 / 墨镜 / 巫师)
void mascot_draw_mini(uint16_t *fb, MiniCrabKind kind, int cx, int base_y, int px_scale, bool trick_frame);

// 绘制原版道具与情绪符号 (gx_px, gy_px 为该角色 16x10 锚点左上角像素坐标)
void mascot_draw_prop(uint16_t *fb, MascotPropId prop, int anchor_x, int anchor_y, int px_scale);
void mascot_draw_sym(uint16_t *fb, MascotSymId sym, int anchor_x, int anchor_y, int px_scale);

// 绘制地面柔和椭圆投影
void mascot_draw_shadow(uint16_t *fb, int cx, int y, int half_w);

// 统一 Obsidian Prism 暗曜玻璃舞台 (星空微光 + 底部烟熏玻璃展台 + Prism Rim 流动光条)
#define STAGE_TOP_Y 180
void mascot_stage_render(uint16_t *fb, float time_sec, bool draw_arch_glow);

#endif // MASCOT_ART_H
