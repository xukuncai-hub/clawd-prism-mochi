#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>
#include <stdlib.h>

// 06. 结对编程 & 灵光一闪 (Clawd 疯狂挥笔写代码 + 小紫猫翻书查阅 -> 思考冒问号 -> 灯泡叮点亮 + 星芒欢呼!)
#define CYCLE_SEC 12.0f

static float s_time = 0.0f;

static void coding_init(void) {
    s_time = 0.0f;
}

static void coding_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) s_time -= CYCLE_SEC;
}

static void coding_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 8.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int cx_c = 66;
    int cx_k = 166;

    // 阶段：0..4.5s 埋头写代码翻书；4.5..7.2s 思考卡壳冒 ?；7.2..12.0s 叮！灯泡点亮高举欢呼！
    int phase = (s_time < 4.5f) ? 0 : (s_time < 7.2f) ? 1 : 2;

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    if (phase == 0) {
        pc.sink = 1;
        pc.legs = LEG_CROUCH;
        pc.body = clip ? BODY_STAND : BODY_TURN_R;
        pc.claw_r = CLAW_WRITE;
        pc.eyes = EYE_OPEN;

        pk.sink = 1;
        pk.legs = LEG_CROUCH;
        pk.ears = clip ? EAR_STAND : EAR_NOD;
        pk.paws = PAW_WRITE;
        pk.tail = clip ? TAIL_SWAY : TAIL_CURL;
        pk.flip_x = true;
    } else if (phase == 1) {
        pc.eyes = EYE_HALF;
        pc.claw_r = CLAW_FRONT;
        pk.eyes = EYE_SQUINT;
        pk.ears = EAR_NOD;
        pk.tail = TAIL_CURL;
        pk.flip_x = true;
    } else {
        int hop = abs((int)(12.0f * p_sinf((s_time - 7.2f) * 6.0f)));
        pc.body = clip ? BODY_SQUASH : BODY_STAND;
        pc.legs = clip ? LEG_CROUCH : LEG_STAND;
        pc.eyes = EYE_HAPPY;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;

        pk.ears = EAR_STAND;
        pk.eyes = EYE_HAPPY;
        pk.paws = PAW_UP;
        pk.tail = TAIL_PUFF;
        mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
        mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);
        mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y - hop / 2, 5, &pc);
        mascot_draw(fb, MASCOT_CAT,   cx_k, base_y - hop, 5, &pk);

        mascot_draw_prop(fb, PROP_BULB_ON, cx_k - 40, base_y - hop - 56, 5);
        gfx_draw_crystal_glint(fb, cx_k - 2, base_y - hop - 74, 14, 240, (uint8_t)(s_time * 50.0f));
        mascot_draw_sym(fb, SYM_SPARK, cx_c - 40, base_y - 54, 5);
        return;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 32);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 32);

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    // 道具：稿纸与羽毛笔 + 书本
    mascot_draw_prop(fb, PROP_SHEET, cx_c - 40, base_y - 45, 5);
    if (phase == 0) {
        mascot_draw_prop(fb, clip ? PROP_QUILL_0 : PROP_QUILL_1, cx_c - 40, base_y - 45, 5);
    }
    mascot_draw_prop(fb, (phase == 0 && clip) ? PROP_BOOK_1 : PROP_BOOK_0, cx_k - 98, base_y - 46, 5);

    if (phase == 1) {
        mascot_draw_prop(fb, PROP_BULB_OFF, cx_k - 40, base_y - 54, 5);
        mascot_draw_sym(fb, SYM_QUESTION, cx_c - 40, base_y - 54, 5);
    }
}

static void coding_cleanup(void) {
}

PrismScene scene_coding_duo = {
    .id = "coding_duo",
    .name = "CODING & EUREKA",
    .init = coding_init,
    .update = coding_update,
    .render = coding_render,
    .cleanup = coding_cleanup,
};
