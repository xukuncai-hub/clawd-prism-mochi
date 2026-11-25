#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>
#include <stdlib.h>

// 12. 神秘信封 & 拆信惊喜 (Clawd 小跑送来封口信封 -> 小紫猫好奇拍拍 -> 拆开信封飞出爱心信纸，羞涩贴贴冒红心!)
#define CYCLE_SEC 13.0f

static float s_time = 0.0f;
static PrismSpring s_clawd_x;

static void letter_init(void) {
    s_time = 0.0f;
    prism_spring_init(&s_clawd_x, 42.0f, PRISM_SPRING_GENTLE_K, PRISM_SPRING_GENTLE_C);
}

static void letter_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) {
        s_time -= CYCLE_SEC;
        s_clawd_x.pos = 42.0f;
        s_clawd_x.vel = 0.0f;
    }

    s_clawd_x.target = (s_time < 3.0f) ? 48.0f : 74.0f;
    prism_spring_step(&s_clawd_x, dt);
}

static void letter_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 7.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int cx_c = (int)s_clawd_x.pos;
    int cx_k = 168;

    // 阶段: 0..3.6s 送来信封; 3.6..6.4s 拆开信封; 6.4..13.0s 信纸展开飘爱意
    int phase = (s_time < 3.6f) ? 0 : (s_time < 6.4f) ? 1 : 2;

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    if (phase == 0) {
        pc.body = BODY_TURN_R;
        pc.legs = (s_time > 2.8f && clip) ? LEG_WALK_A : LEG_STAND;
        pc.claw_r = CLAW_FRONT;
        pk.eyes = EYE_LEFT;
        pk.ears = clip ? EAR_TWITCH_L : EAR_STAND;
        pk.flip_x = true;
    } else if (phase == 1) {
        pc.body = BODY_TURN_R;
        pc.eyes = EYE_HALF;
        pk.eyes = EYE_WIDE;
        pk.ears = EAR_NOD;
        pk.tail = TAIL_PUFF;
        pk.flip_x = true;
    } else {
        int hop = abs((int)(6.0f * p_sinf((s_time - 6.4f) * 4.5f)));
        pc.eyes = EYE_HAPPY;
        pc.body = clip ? BODY_SQUASH : BODY_STAND;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;

        pk.eyes = EYE_HAPPY;
        pk.ears = EAR_NOD;
        pk.paws = PAW_CHEER;
        pk.tail = TAIL_PUFF;
        mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
        mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);
        mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
        mascot_draw(fb, MASCOT_CAT,   cx_k, base_y - hop, 5, &pk);

        int float_y = base_y - 52 + (int)(4.0f * p_sinf(s_time * 3.0f));
        mascot_draw_prop(fb, PROP_LETTER, cx_c - 30, float_y, 5);
        int bob = (int)(4.0f * p_sinf(s_time * 5.0f));
        mascot_draw_sym(fb, SYM_HEART, cx_c - 40, base_y - 58 + bob, 5);
        mascot_draw_sym(fb, SYM_HEART, cx_k - 40, base_y - 58 - bob, 5);
        gfx_draw_crystal_glint(fb, 120, 54, 11, 215, (uint8_t)(s_time * 40.0f));
        return;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);
    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    mascot_draw_prop(fb, (phase == 0) ? PROP_ENV_SEALED : PROP_ENV_OPEN, cx_c - 32, base_y - 50, 5);
    if (phase == 1) {
        mascot_draw_sym(fb, SYM_SPARK, cx_k - 40, base_y - 56, 5);
    }
}

static void letter_cleanup(void) {
}

PrismScene scene_love_letter = {
    .id = "love_letter",
    .name = "SECRET LETTER",
    .init = letter_init,
    .update = letter_update,
    .render = letter_render,
    .cleanup = letter_cleanup,
};
