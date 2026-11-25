#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <math.h>
#include <stdint.h>

// 05. 激光红点大作战 (Clawd 举钳发射激光点 -> 小紫猫飞机耳瞪大眼左右飞扑抓不到，摔扁冒问号，Clawd 狂笑)
static float s_time = 0.0f;
static PrismSpring s_cat_x;
static PrismSpring s_cat_y;
static float s_dot_x = 165.0f;
static float s_pounce_cd = 0.8f;
static float s_splat_timer = 0.0f;

static void laser_init(void) {
    s_time = 0.0f;
    s_dot_x = 175.0f;
    s_pounce_cd = 0.7f;
    s_splat_timer = 0.0f;
    prism_spring_init(&s_cat_x, 155.0f, PRISM_SPRING_SNAPPY_K, PRISM_SPRING_SNAPPY_C);
    prism_spring_init(&s_cat_y, 0.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
}

static void laser_update(float dt) {
    s_time += dt;
    if (s_time > 1000.0f) s_time -= 1000.0f;

    s_pounce_cd -= dt;
    if (s_pounce_cd <= 0.0f) {
        // 小紫猫扑向当前红点，红点同时瞬移到另一侧！
        s_cat_x.target = s_dot_x;
        s_cat_y.vel = -210.0f;
        s_dot_x = (s_dot_x > 155.0f) ? 116.0f : 196.0f;
        s_pounce_cd = 1.45f;
        s_splat_timer = 0.65f;
    }

    if (s_splat_timer > 0.0f) {
        s_splat_timer -= dt;
    }

    prism_spring_step(&s_cat_x, dt);
    prism_spring_step(&s_cat_y, dt);
    if (s_cat_y.pos > 0.0f) { s_cat_y.pos = 0.0f; s_cat_y.vel = 0.0f; }
}

static void laser_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 9.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;

    int cx_c = 54;
    int cx_k = (int)s_cat_x.pos;
    int cy_k = base_y + (int)s_cat_y.pos;

    // 1. Clawd 拿着激光笔偷笑
    MascotPose pc;
    mascot_pose_default(&pc);
    pc.eyes = EYE_HAPPY;
    pc.body = clip ? BODY_SQUASH : BODY_STAND;
    pc.claw_r = CLAW_FRONT;
    pc.claw_l = CLAW_STUB;

    // 2. 小紫猫飞机耳瞪大眼飞扑或扑空摔扁
    MascotPose pk;
    mascot_pose_default(&pk);
    bool in_air = (s_cat_y.pos < -6.0f);
    bool splat = (!in_air && s_splat_timer > 0.0f && s_splat_timer < 0.35f);
    pk.flip_x = (s_dot_x < (float)cx_k);
    if (splat) {
        pk.body = BODY_FLAT;
        pk.eyes = EYE_SQUINT;
        pk.legs = LEG_NONE;
    } else if (in_air) {
        pk.body = BODY_STRETCH;
        pk.ears = EAR_AIRPLANE;
        pk.eyes = EYE_WIDE;
        pk.legs = LEG_TUCK;
        pk.paws = PAW_CHEER;
        pk.tail = TAIL_PUFF;
    } else {
        pk.body = clip ? BODY_SQUASH : BODY_STAND;
        pk.ears = EAR_AIRPLANE;
        pk.eyes = EYE_WIDE;
        pk.legs = clip ? LEG_CROUCH : LEG_STAND;
        pk.tail = TAIL_PUFF;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 28);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, p_clampi(30 + (int)s_cat_y.pos / 4, 14, 32));

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, cy_k, 5, &pk);

    // 3. 激光光束从 Clawd 钳尖射向地面红点
    float lx0 = (float)(cx_c + 40);
    float ly0 = (float)(base_y - 20);
    float lx1 = s_dot_x;
    float ly1 = (float)(STAGE_TOP_Y + 2);
    gfx_draw_glow_line(fb, lx0, ly0, lx1, ly1, 255, 48, 64, 185);
    gfx_draw_bloom_disc(fb, (int)lx1, (int)ly1, 7, 255, 40, 60, 230);
    gfx_draw_bloom_disc(fb, (int)lx1, (int)ly1, 3, 255, 220, 220, 255);

    if (splat) {
        mascot_draw_sym(fb, SYM_QUESTION, cx_k - 40, cy_k - 44, 5);
    } else if (in_air) {
        mascot_draw_sym(fb, SYM_BANG, cx_k - 40, cy_k - 56, 5);
    }
}

static void laser_cleanup(void) {
}

PrismScene scene_laser_chase = {
    .id = "laser_chase",
    .name = "LASER CHASE",
    .init = laser_init,
    .update = laser_update,
    .render = laser_render,
    .cleanup = laser_cleanup,
};
