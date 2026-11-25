#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 10. 墨镜冲浪 & 魔法冰晶 (Clawd 戴墨镜踩冲浪板踏光浪，小紫猫施放旋转魔法冰晶，迷你巫师蟹助威)
static float s_time = 0.0f;

static void surf_init(void) {
    s_time = 0.0f;
}

static void surf_update(float dt) {
    s_time += dt;
    if (s_time > 1000.0f) s_time -= 1000.0f;
}

static void surf_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 6.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;

    int cx_c = 70 + (int)(10.0f * p_sinf(s_time * 2.2f));
    int wave_bob = (int)(4.0f * p_cosf(s_time * 4.4f));
    int cx_k = 172;

    // 1. Clawd 戴墨镜踩冲浪板左右冲浪
    MascotPose pc;
    mascot_pose_default(&pc);
    pc.body = clip ? BODY_LEAN_R : BODY_TURN_R;
    pc.legs = LEG_CROUCH;
    pc.claw_l = CLAW_OPEN;
    pc.claw_r = CLAW_UP;
    pc.eyes = EYE_HAPPY;

    // 2. 小紫猫挥爪施放悬浮魔法冰晶
    MascotPose pk;
    mascot_pose_default(&pk);
    pk.body = clip ? BODY_STAND : BODY_SQUASH;
    pk.ears = clip ? EAR_STAND : EAR_NOD;
    pk.eyes = EYE_HAPPY;
    pk.paws = clip ? PAW_UP : PAW_CHEER;
    pk.tail = TAIL_PUFF;

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 36);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);

    // 冲浪板与浪花
    mascot_draw_prop(fb, PROP_SURFBOARD, cx_c - 40, base_y - 46 + wave_bob, 5);
    mascot_draw_prop(fb, clip ? PROP_WAVE_0 : PROP_WAVE_1, cx_c - 40, base_y - 46 + wave_bob, 5);

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y - 4 + wave_bob, 5, &pc);
    mascot_draw_prop(fb, PROP_SHADES, cx_c - 40, base_y - 54 + wave_bob, 5);

    mascot_draw(fb, MASCOT_CAT, cx_k, base_y, 5, &pk);

    // 悬浮旋转的蓝白魔法冰晶 + 棱镜星芒
    int ice_y = 66 + (int)(8.0f * p_sinf(s_time * 3.0f));
    mascot_draw_prop(fb, PROP_ICE_CRYSTAL, cx_k - 40, ice_y, 5);
    gfx_draw_crystal_glint(fb, cx_k, ice_y - 16, 12, 225, (uint8_t)(s_time * 48.0f));

    // 迷你巫师蟹在中间台面助威
    mascot_draw_mini(fb, MINI_WIZARD, 124, base_y, 3, clip);
}

static void surf_cleanup(void) {
}

PrismScene scene_surf_party = {
    .id = "surf_party",
    .name = "SURF & ICE MAGIC",
    .init = surf_init,
    .update = surf_update,
    .render = surf_render,
    .cleanup = surf_cleanup,
};
