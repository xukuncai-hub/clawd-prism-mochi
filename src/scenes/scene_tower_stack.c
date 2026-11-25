#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 02. 叠叠乐高塔 (Clawd 稳扎马步托举 -> 小紫猫弹簧跃上头顶平衡 -> 3 只迷你蟹依次空降叠成 4 层高塔)
#define CYCLE_SEC 15.0f

static float s_time = 0.0f;
static PrismSpring s_cat_y;
static PrismSpring s_m1_y, s_m2_y, s_m3_y;
static int s_stage = -1;

static void tower_init(void) {
    s_time = 0.0f;
    s_stage = -1;
    prism_spring_init(&s_cat_y, 0.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
    prism_spring_init(&s_m1_y, -180.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
    prism_spring_init(&s_m2_y, -210.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
    prism_spring_init(&s_m3_y, -240.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
}

static void tower_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) {
        s_time -= CYCLE_SEC;
        s_stage = -1;
        s_cat_y.pos = 0.0f; s_cat_y.vel = 0.0f;
        s_m1_y.pos = -180.0f; s_m1_y.vel = 0.0f;
        s_m2_y.pos = -210.0f; s_m2_y.vel = 0.0f;
        s_m3_y.pos = -240.0f; s_m3_y.vel = 0.0f;
    }

    int st = (s_time < 2.2f) ? 0 : (s_time < 4.5f) ? 1 : (s_time < 6.8f) ? 2 : (s_time < 9.2f) ? 3 : 4;
    if (st != s_stage) {
        s_stage = st;
    }

    // 底层 Clawd (scale 4, 高 40px, 顶部 y=144)
    // 第2层 小紫猫 (scale 4, 脚落于 Clawd 头顶 -36px 偏移)
    // 第3~5层 迷你蟹 (scale 3, 每层约 -22px)
    s_cat_y.target = (st >= 1) ? -36.0f : 0.0f;
    s_m1_y.target  = (st >= 2) ? -72.0f : -180.0f;
    s_m2_y.target  = (st >= 3) ? -94.0f : -210.0f;
    s_m3_y.target  = (st >= 4) ? -116.0f : -240.0f;

    prism_spring_step(&s_cat_y, dt);
    prism_spring_step(&s_m1_y, dt);
    prism_spring_step(&s_m2_y, dt);
    prism_spring_step(&s_m3_y, dt);
}

static void tower_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 8.0f);
    bool trick = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;

    // 高度越高摇晃微摆越明显
    float sway1 = (s_stage >= 1) ? 2.5f * p_sinf(s_time * 2.8f) : 0.0f;
    float sway2 = (s_stage >= 2) ? 4.5f * p_sinf(s_time * 2.8f - 0.5f) : 0.0f;
    float sway3 = (s_stage >= 3) ? 6.5f * p_sinf(s_time * 2.8f - 1.0f) : 0.0f;
    float sway4 = (s_stage >= 4) ? 8.5f * p_sinf(s_time * 2.8f - 1.5f) : 0.0f;

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    // 底层 Clawd (托举姿态)
    pc.claw_l = (s_stage >= 1) ? CLAW_OPEN : CLAW_STUB;
    pc.claw_r = (s_stage >= 1) ? CLAW_OPEN : CLAW_STUB;
    pc.eyes   = (s_stage >= 4) ? EYE_HAPPY : ((s_stage >= 1) ? EYE_UP : EYE_OPEN);
    pc.body   = (s_stage >= 2 && trick) ? BODY_SQUASH : BODY_STAND;

    // 小紫猫 (平衡姿态 -> 全员叠好后欢呼)
    int cat_cx = (s_stage >= 1) ? (120 + (int)sway1) : 176;
    if (s_stage == 0) {
        pk.eyes = EYE_LEFT;
    } else if (s_stage < 4) {
        pk.eyes = EYE_WIDE;
        pk.ears = EAR_AIRPLANE;
        pk.paws = PAW_CHEER;
        pk.tail = TAIL_PUFF;
    } else {
        pk.eyes = EYE_HAPPY;
        pk.ears = EAR_NOD;
        pk.paws = PAW_UP;
        pk.tail = TAIL_UP;
    }

    mascot_draw_shadow(fb, 120, STAGE_TOP_Y + 6, 30);
    if (s_stage == 0) mascot_draw_shadow(fb, cat_cx, STAGE_TOP_Y + 6, 26);

    // 4x 绘制主角色，3x 绘制三只迷你蟹，保证高塔完全落在屏幕内
    mascot_draw(fb, MASCOT_CLAWD, 120, base_y, 4, &pc);
    mascot_draw(fb, MASCOT_CAT, cat_cx, base_y + (int)s_cat_y.pos, 4, &pk);

    if (s_stage >= 2) {
        mascot_draw_mini(fb, MINI_COWBOY, 120 + (int)sway2, base_y + (int)s_m1_y.pos, 3, s_stage >= 4 && trick);
    }
    if (s_stage >= 3) {
        mascot_draw_mini(fb, MINI_SHADES, 120 + (int)sway3, base_y + (int)s_m2_y.pos, 3, s_stage >= 4 && trick);
    }
    if (s_stage >= 4) {
        mascot_draw_mini(fb, MINI_WIZARD, 120 + (int)sway4, base_y + (int)s_m3_y.pos, 3, trick);
        int bob = (int)(3.0f * p_sinf(s_time * 6.0f));
        mascot_draw_sym(fb, SYM_HEART, 44, 52 + bob, 4);
        mascot_draw_sym(fb, SYM_SPARK, 162, 46 - bob, 4);
    }
}

static void tower_cleanup(void) {
}

PrismScene scene_tower_stack = {
    .id = "tower_stack",
    .name = "TOWER STACK",
    .init = tower_init,
    .update = tower_update,
    .render = tower_render,
    .cleanup = tower_cleanup,
};
