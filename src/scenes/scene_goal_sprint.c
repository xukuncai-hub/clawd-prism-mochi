#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>
#include <stdlib.h>

// 15. 热血特训 & 冲刺 (Clawd 绑上红色热血头带背后燃起金色火焰疯狂倒腾小腿，小紫猫举怀表沙漏计时呐喊加油!)
static float s_time = 0.0f;

static void sprint_init(void) {
    s_time = 0.0f;
}

static void sprint_update(float dt) {
    s_time += dt;
    if (s_time > 1000.0f) s_time -= 1000.0f;
}

static void sprint_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 12.0f);
    bool fast = (tick & 1) != 0;
    bool slow = ((tick >> 2) & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int cx_c = 70 + (int)(6.0f * p_sinf(s_time * 5.0f));
    int cx_k = 172;

    // 速度线特效
    for (int i = 0; i < 6; i++) {
        int ly = 72 + i * 16;
        int lx = 18 + (((int)(s_time * 120.0f) + i * 29) % 45);
        gfx_draw_aa_line(fb, (float)lx, (float)ly, (float)(lx + 24), (float)ly,
                         g_prism_pal.specular_rgb[0],
                         g_prism_pal.specular_rgb[1],
                         g_prism_pal.specular_rgb[2], 130, true);
    }

    // Clawd 绑热血头带 + 燃起金色斗志火焰疯狂奔跑
    MascotPose pc;
    mascot_pose_default(&pc);
    pc.body = BODY_LEAN_R;
    pc.legs = fast ? LEG_WALK_A : LEG_WALK_B;
    pc.eyes = EYE_SQUINT;
    pc.claw_l = fast ? CLAW_FRONT : CLAW_STUB;
    pc.claw_r = fast ? CLAW_STUB : CLAW_FRONT;

    // 小紫猫拿着沙漏和怀表计时加油
    MascotPose pk;
    mascot_pose_default(&pk);
    pk.body = slow ? BODY_SQUASH : BODY_STAND;
    pk.legs = slow ? LEG_CROUCH : LEG_STAND;
    pk.ears = fast ? EAR_NOD : EAR_STAND;
    pk.eyes = EYE_HAPPY;
    pk.paws = PAW_CHEER;
    pk.tail = TAIL_PUFF;
    pk.flip_x = true;

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);

    // 背后燃起的斗志火焰先画
    mascot_draw_prop(fb, fast ? PROP_GOAL_FLAME_0 : PROP_GOAL_FLAME_1, cx_c - 40, base_y - 50, 5);
    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y - (fast ? 3 : 0), 5, &pc);
    mascot_draw_prop(fb, PROP_HEADBAND, cx_c - 40, base_y - 50 - (fast ? 3 : 0), 5);

    mascot_draw(fb, MASCOT_CAT, cx_k, base_y, 5, &pk);
    mascot_draw_prop(fb, slow ? PROP_HOURGLASS_0 : PROP_HOURGLASS_1, cx_k - 104, base_y - 52, 5);
    mascot_draw_prop(fb, PROP_WATCH, cx_k - 36, base_y - 52, 5);

    mascot_draw_sym(fb, SYM_SPARK, cx_c - 40, base_y - 58, 5);
}

static void sprint_cleanup(void) {
}

PrismScene scene_goal_sprint = {
    .id = "goal_sprint",
    .name = "TURBO SPRINT",
    .init = sprint_init,
    .update = sprint_update,
    .render = sprint_render,
    .cleanup = sprint_cleanup,
};
