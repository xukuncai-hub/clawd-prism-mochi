#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <math.h>
#include <stdint.h>

// 01. 双宠日常游乐场 (开机首屏 · 14秒剧情循环：张望眨眼 -> 小跑靠近挥爪打招呼 -> 同步高跳欢呼 -> 贴贴冒红心 -> 揣手洗脸)
#define CYCLE_SEC 14.0f
#define SCALE     5

static float s_time = 0.0f;
static PrismSpring s_clawd_x, s_cat_x;
static PrismSpring s_clawd_y, s_cat_y;
static PrismSpring s_clawd_sq, s_cat_sq;
static float s_blink_cd = 2.0f;
static int s_last_phase = -1;

static void dual_play_init(void) {
    s_time = 0.0f;
    s_blink_cd = 1.6f;
    s_last_phase = -1;
    prism_spring_init(&s_clawd_x, 68.0f, PRISM_SPRING_GENTLE_K, PRISM_SPRING_GENTLE_C);
    prism_spring_init(&s_cat_x,   172.0f, PRISM_SPRING_GENTLE_K, PRISM_SPRING_GENTLE_C);
    prism_spring_init(&s_clawd_y, 0.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
    prism_spring_init(&s_cat_y,   0.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
    prism_spring_init(&s_clawd_sq, 1.0f, PRISM_SPRING_SNAPPY_K, PRISM_SPRING_SNAPPY_C);
    prism_spring_init(&s_cat_sq,   1.0f, PRISM_SPRING_SNAPPY_K, PRISM_SPRING_SNAPPY_C);
}

static void dual_play_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) {
        s_time -= CYCLE_SEC;
        s_last_phase = -1;
    }

    s_blink_cd -= dt;
    if (s_blink_cd <= -0.14f) {
        s_blink_cd = 1.8f + p_randf() * 1.6f;
    }

    // 剧情分段：
    // 0: 0..3.0s  互相张望眨眼
    // 1: 3.0..6.0s 小跑靠近并挥爪打招呼
    // 2: 6.0..9.5s 同步起跳欢呼 + 落地压扁回弹
    // 3: 9.5..14.0s 回到原位，小紫猫揣手洗脸，Clawd 原地蹦跳
    int phase = (s_time < 3.0f) ? 0 : (s_time < 6.0f) ? 1 : (s_time < 9.5f) ? 2 : 3;

    if (phase == 0) {
        s_clawd_x.target = 68.0f;
        s_cat_x.target   = 172.0f;
    } else if (phase == 1) {
        s_clawd_x.target = 84.0f;
        s_cat_x.target   = 156.0f;
    } else if (phase == 2) {
        s_clawd_x.target = 82.0f;
        s_cat_x.target   = 158.0f;
        if (s_last_phase != 2) {
            s_clawd_y.vel = -260.0f;
            s_cat_y.vel   = -285.0f;
        }
        // 7.6s 二次小跳
        if (s_time > 7.6f && s_time - dt <= 7.6f) {
            s_clawd_y.vel = -210.0f;
            s_cat_y.vel   = -220.0f;
        }
    } else {
        s_clawd_x.target = 70.0f;
        s_cat_x.target   = 170.0f;
        if (s_last_phase != 3) {
            s_clawd_y.vel = -160.0f;
        }
    }
    s_last_phase = phase;

    prism_spring_step(&s_clawd_x, dt);
    prism_spring_step(&s_cat_x, dt);
    prism_spring_step(&s_clawd_y, dt);
    prism_spring_step(&s_cat_y, dt);

    if (s_clawd_y.pos > 0.0f) { s_clawd_y.pos = 0.0f; s_clawd_y.vel = 0.0f; }
    if (s_cat_y.pos > 0.0f)   { s_cat_y.pos = 0.0f;   s_cat_y.vel = 0.0f; }
}

static void dual_play_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 8.0f);
    bool clip_mod = (tick & 1) != 0;
    bool slow_mod = ((tick >> 1) & 1) != 0;
    bool blinking = (s_blink_cd <= 0.0f);

    int phase = (s_time < 3.0f) ? 0 : (s_time < 6.0f) ? 1 : (s_time < 9.5f) ? 2 : 3;

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    int cx_c = (int)s_clawd_x.pos;
    int cy_c = STAGE_TOP_Y + 4 + (int)s_clawd_y.pos;
    int cx_k = (int)s_cat_x.pos;
    int cy_k = STAGE_TOP_Y + 4 + (int)s_cat_y.pos;

    if (phase == 0) {
        pc.eyes = blinking ? EYE_SHUT : (slow_mod ? EYE_RIGHT : EYE_OPEN);
        pk.eyes = blinking ? EYE_SHUT : (slow_mod ? EYE_LEFT : EYE_OPEN);
        pk.ears = slow_mod ? EAR_TWITCH_L : EAR_STAND;
        pk.tail = slow_mod ? TAIL_SWAY : TAIL_UP;
    } else if (phase == 1) {
        // 小跑靠近并挥爪打招呼
        bool moving = fabsf(s_clawd_x.vel) > 12.0f;
        if (moving) {
            pc.legs = clip_mod ? LEG_WALK_A : LEG_WALK_B;
            pk.legs = clip_mod ? LEG_WALK_B : LEG_WALK_A;
            pc.body = BODY_LEAN_R;
            pk.body = BODY_LEAN_L;
        } else {
            pc.eyes = EYE_HAPPY;
            pk.eyes = EYE_HAPPY;
            pc.claw_r = clip_mod ? CLAW_WAVE : CLAW_UP;
            pk.ears = EAR_NOD;
            pk.paws = clip_mod ? PAW_UP_R : PAW_NONE;
            pk.flip_x = true;
            pk.tail = TAIL_PUFF;
        }
    } else if (phase == 2) {
        // 欢呼跳跃
        bool in_air_c = (s_clawd_y.pos < -4.0f);
        bool in_air_k = (s_cat_y.pos < -4.0f);
        pc.eyes = EYE_HAPPY;
        pk.eyes = EYE_HAPPY;
        pc.body = in_air_c ? BODY_TALL : (clip_mod ? BODY_SQUASH : BODY_STAND);
        pc.legs = in_air_c ? LEG_TUCK : (clip_mod ? LEG_CROUCH : LEG_STAND);
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;

        pk.ears = in_air_k ? EAR_STAND : EAR_NOD;
        pk.legs = in_air_k ? LEG_TUCK : (clip_mod ? LEG_CROUCH : LEG_STAND);
        pk.paws = PAW_UP;
        pk.tail = clip_mod ? TAIL_PUFF : TAIL_UP;
    } else {
        // 小紫猫惬意揣手洗脸，Clawd 开心微蹦
        pc.body = slow_mod ? BODY_SQUASH : BODY_STAND;
        pc.legs = slow_mod ? LEG_CROUCH : LEG_STAND;
        pc.eyes = blinking ? EYE_SHUT : EYE_HAPPY;
        pc.claw_l = slow_mod ? CLAW_OPEN : CLAW_STUB;
        pc.claw_r = slow_mod ? CLAW_OPEN : CLAW_STUB;

        if (s_time < 11.8f) {
            pk.ears = EAR_NOD;
            pk.eyes = EYE_SHUT;
            pk.paws = clip_mod ? PAW_GROOM_1 : PAW_GROOM_2;
            pk.tail = TAIL_CURL;
        } else {
            pk.body = BODY_LOAF;
            pk.ears = EAR_LOAF;
            pk.eyes = blinking ? EYE_SHUT : EYE_HALF;
            pk.tail = TAIL_LOAF;
        }
    }

    // 绘制地面投影
    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, p_clampi(32 + (int)s_clawd_y.pos / 4, 14, 34));
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, p_clampi(32 + (int)s_cat_y.pos / 4, 14, 34));

    // 绘制双宠
    mascot_draw(fb, MASCOT_CLAWD, cx_c, cy_c, SCALE, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, cy_k, SCALE, &pk);

    // 情绪气泡与棱镜星芒装饰
    if (phase == 1 && fabsf(s_clawd_x.vel) <= 12.0f) {
        int bob = (int)(3.0f * p_sinf(s_time * 5.0f));
        mascot_draw_sym(fb, SYM_HEART, 120 - 40, cy_c - 58 + bob, SCALE);
    } else if (phase == 2) {
        int bob = (int)(4.0f * p_sinf(s_time * 6.0f));
        mascot_draw_sym(fb, SYM_HEART, cx_c - 40, cy_c - 56 + bob, SCALE);
        mascot_draw_sym(fb, SYM_NOTE,  cx_k - 40, cy_k - 56 - bob, SCALE);
        gfx_draw_crystal_glint(fb, 120, 48, 12, 220, (uint8_t)(s_time * 40.0f));
    }
}

static void dual_play_cleanup(void) {
}

PrismScene scene_dual_play = {
    .id = "dual_play",
    .name = "DUAL MASCOT PLAY",
    .init = dual_play_init,
    .update = dual_play_update,
    .render = dual_play_render,
    .cleanup = dual_play_cleanup,
};
