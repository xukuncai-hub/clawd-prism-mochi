#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 13. 赛博警灯巡逻队 (头顶红蓝交替警灯闪烁 + 探照灯扫射舞台 -> 左右巡逻 -> 发现目标齐齐敬礼!)
#define CYCLE_SEC 12.0f

static float s_time = 0.0f;

static void patrol_init(void) {
    s_time = 0.0f;
}

static void patrol_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) s_time -= CYCLE_SEC;
}

static void patrol_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, false);

    int tick = (int)(s_time * 7.0f);
    bool flash = (tick & 1) != 0;
    bool siren_red = ((tick >> 1) & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;

    bool salute_phase = (s_time >= 7.5f);
    int walk_dx = salute_phase ? 0 : (int)(18.0f * p_sinf(s_time * 1.8f));
    int cx_c = 70 + walk_dx;
    int cx_k = 170 + walk_dx;

    // 1. 头顶警灯向外扫射的红蓝光束
    float sweep = 36.0f * p_sinf(s_time * 4.5f);
    if (siren_red) {
        gfx_draw_glow_line(fb, (float)cx_c, (float)(base_y - 56), (float)cx_c + sweep, 20.0f, 255, 52, 62, 185);
        gfx_draw_bloom_disc(fb, cx_c, base_y - 58, 16, 255, 50, 60, 140);
    } else {
        gfx_draw_glow_line(fb, (float)cx_k, (float)(base_y - 56), (float)cx_k - sweep, 20.0f, 52, 148, 255, 185);
        gfx_draw_bloom_disc(fb, cx_k, base_y - 58, 16, 52, 148, 255, 140);
    }

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    if (!salute_phase) {
        bool walk_right = p_cosf(s_time * 1.8f) >= 0.0f;
        pc.flip_x = !walk_right;
        pk.flip_x = !walk_right;
        pc.legs = flash ? LEG_WALK_A : LEG_WALK_B;
        pk.legs = flash ? LEG_WALK_B : LEG_WALK_A;
        pc.eyes = walk_right ? EYE_RIGHT : EYE_LEFT;
        pk.eyes = walk_right ? EYE_RIGHT : EYE_LEFT;
        pk.tail = TAIL_UP;
    } else {
        // 发现目标，齐齐立正敬礼！
        pc.eyes = EYE_HAPPY;
        pc.claw_r = CLAW_UP;
        pk.eyes = EYE_HAPPY;
        pk.paws = PAW_UP_R;
        pk.ears = EAR_STAND;
        pk.tail = TAIL_UP;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);
    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    // 头顶警灯
    mascot_draw_prop(fb, siren_red ? PROP_SIREN_F_R : PROP_SIREN_R, cx_c - 40, base_y - 50, 5);
    mascot_draw_prop(fb, siren_red ? PROP_SIREN_B : PROP_SIREN_F_B, cx_k - 40, base_y - 50, 5);

    if (salute_phase) {
        mascot_draw_sym(fb, SYM_SPARK, 120 - 40, base_y - 68, 5);
    }
}

static void patrol_cleanup(void) {
}

PrismScene scene_police_patrol = {
    .id = "police_patrol",
    .name = "PRISM PATROL",
    .init = patrol_init,
    .update = patrol_update,
    .render = patrol_render,
    .cleanup = patrol_cleanup,
};
