#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 08. 深夜干饭：拉面与烤鱼 (Clawd 举筷大口炫拉面，小紫猫大快朵颐烤鱼 -> 光盘行动满足冒红心)
#define CYCLE_SEC 12.0f

static float s_time = 0.0f;

static void feast_init(void) {
    s_time = 0.0f;
}

static void feast_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) s_time -= CYCLE_SEC;
}

static void feast_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 7.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int cx_c = 62;
    int cx_k = 172;

    // 0..7.5s 干饭；7.5..12.0s 吃光满足冒红心
    bool finished = (s_time >= 7.5f);

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    if (!finished) {
        pc.body = clip ? BODY_SQUASH : BODY_STAND;
        pc.legs = clip ? LEG_CROUCH : LEG_STAND;
        pc.eyes = clip ? EYE_HALF : EYE_OPEN;
        pc.claw_r = CLAW_FRONT;

        pk.body = clip ? BODY_SQUASH : BODY_STAND;
        pk.legs = clip ? LEG_CROUCH : LEG_STAND;
        pk.eyes = clip ? EYE_OPEN : EYE_HALF;
        pk.ears = clip ? EAR_NOD : EAR_STAND;
        pk.tail = clip ? TAIL_SWAY : TAIL_UP;
        pk.flip_x = true;
    } else {
        pc.body = BODY_SQUASH;
        pc.legs = LEG_CROUCH;
        pc.eyes = EYE_HAPPY;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;

        pk.body = BODY_SQUASH;
        pk.legs = LEG_CROUCH;
        pk.eyes = EYE_HAPPY;
        pk.ears = EAR_NOD;
        pk.paws = PAW_CHEER;
        pk.tail = TAIL_PUFF;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 32);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 32);

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    // 道具：拉面碗 + 筷子 / 烤鱼盘 + 鱼骨
    mascot_draw_prop(fb, finished ? PROP_BOWL_EMPTY : PROP_BOWL, cx_c - 40, base_y - 50, 5);
    if (!finished) {
        mascot_draw_prop(fb, clip ? PROP_CHOP_0 : PROP_CHOP_1, cx_c - 40, base_y - 50, 5);
    }
    mascot_draw_prop(fb, finished ? PROP_FISH_BONE : PROP_FISH, cx_k - 104, base_y - 50, 5);

    if (finished) {
        int bob = (int)(4.0f * p_sinf(s_time * 5.0f));
        mascot_draw_sym(fb, SYM_HEART, cx_c - 40, base_y - 56 + bob, 5);
        mascot_draw_sym(fb, SYM_HEART, cx_k - 40, base_y - 56 - bob, 5);
    }
}

static void feast_cleanup(void) {
}

PrismScene scene_feast_dinner = {
    .id = "feast_dinner",
    .name = "RAMEN & FISH FEAST",
    .init = feast_init,
    .update = feast_update,
    .render = feast_render,
    .cleanup = feast_cleanup,
};
