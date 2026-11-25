#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "display.h"
#include "prism_math.h"
#include "prism_gfx.h"
#include "scene.h"

static const char *TAG = "CLAWD_DUO";

#define PIN_BOOT_BUTTON GPIO_NUM_9
#define PIN_BUZZER      GPIO_NUM_6

#define AUTO_CYCLE_US        15000000LL // 15 秒自动轮播 (确保每幕剧情完整演完)
#define CLICK_WINDOW_US      380000LL   // 380ms 连击判定窗口
#define DEBOUNCE_MIN_US      35000LL    // 35ms 最小有效按压
#define LONG_PRESS_US        800000LL   // 800ms 长按阈值
#define WIPE_DURATION_SEC    0.38f      // 棱镜扫光转场时长
#define TOAST_DURATION_SEC   1.35f      // 底部烟熏玻璃微胶囊提示时长

static PrismScene *s_scenes[] = {
    &scene_dual_play,      // 01. 双宠日常游乐场 (开机首屏)
    &scene_tower_stack,    // 02. 叠叠乐高塔
    &scene_rainbow_cat,    // 03. 彩虹极光小紫猫
    &scene_cafe_time,      // 04. 午后咖啡与甜甜圈
    &scene_laser_chase,    // 05. 激光红点大作战
    &scene_coding_duo,     // 06. 结对编程 & 灵光一闪
    &scene_concert_dance,  // 07. 荧光棒打 Call 演唱会
    &scene_feast_dinner,   // 08. 深夜干饭：拉面与烤鱼
    &scene_sleepy_night,   // 09. 晚安打盹 & 鼻涕泡
    &scene_surf_party,     // 10. 墨镜冲浪 & 魔法冰晶
    &scene_garden_bloom,   // 11. 盆栽开花 & 蝴蝶飞舞
    &scene_love_letter,    // 12. 神秘信封 & 拆信惊喜
    &scene_police_patrol,  // 13. 赛博警灯巡逻队
    &scene_quarrel_makeup, // 14. 生气闹别扭 & 贴贴和好
    &scene_goal_sprint,    // 15. 热血特训 & 冲刺
    &scene_hide_peek,      // 16. 玻璃展台捉迷藏
    &scene_whale_splash,   // 17. 蓝鲸 Deepy 喷水狂欢
    &scene_campfire_story, // 18. 星空篝火晚会
};

#define NUM_SCENES ((int)(sizeof(s_scenes) / sizeof(s_scenes[0])))

static int s_playlist[NUM_SCENES];
static int s_play_ptr = 0;

static void init_shuffle_playlist(bool keep_first) {
    for (int i = 0; i < NUM_SCENES; i++) {
        s_playlist[i] = i;
    }
    int start_i = keep_first ? 1 : 0;
    for (int i = NUM_SCENES - 1; i > start_i; i--) {
        int j = start_i + (int)(p_rand() % (uint32_t)(i - start_i + 1));
        int tmp = s_playlist[i];
        s_playlist[i] = s_playlist[j];
        s_playlist[j] = tmp;
    }
    s_play_ptr = 0;
}

static int next_shuffled_scene(void) {
    s_play_ptr++;
    if (s_play_ptr >= NUM_SCENES) {
        int last_id = s_playlist[NUM_SCENES - 1];
        init_shuffle_playlist(false);
        if (s_playlist[0] == last_id && NUM_SCENES > 1) {
            int sw = 1 + (int)(p_rand() % (uint32_t)(NUM_SCENES - 1));
            int tmp = s_playlist[0];
            s_playlist[0] = s_playlist[sw];
            s_playlist[sw] = tmp;
        }
    }
    return s_playlist[s_play_ptr];
}

static void buzzer_beep(int ms) {
    if (ms <= 0) return;
    gpio_set_direction(PIN_BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_BUZZER, 0);
    vTaskDelay(pdMS_TO_TICKS(ms));
    gpio_set_direction(PIN_BUZZER, GPIO_MODE_INPUT);
}

static char s_toast_text[32] = {0};
static float s_toast_timer = 0.0f;

static void show_toast(const char *msg) {
    snprintf(s_toast_text, sizeof(s_toast_text), "%s", msg);
    s_toast_timer = TOAST_DURATION_SEC;
}

void app_main(void) {
    ESP_LOGI(TAG, "==============================================");
    ESP_LOGI(TAG, " Clawd Mochi — Dual Mascot Prism Theater      ");
    ESP_LOGI(TAG, " Clawd & Purple Cat | 10 Scenes | Queued DMA  ");
    ESP_LOGI(TAG, "==============================================");

    prism_math_init();
    prism_gfx_init(PAL_OBSIDIAN);

    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << PIN_BOOT_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&btn_conf);

    gpio_config_t bz_conf = {
        .pin_bit_mask = (1ULL << PIN_BUZZER),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&bz_conf);

    if (!display_init()) {
        ESP_LOGE(TAG, "Display init failed!");
        return;
    }

    static uint16_t g_framebuffer[DISP_W * DISP_H] __attribute__((aligned(4)));

    init_shuffle_playlist(true);
    int cur_idx = s_playlist[s_play_ptr];
    s_scenes[cur_idx]->init();
    buzzer_beep(20);

    int64_t last_time = esp_timer_get_time();
    int64_t last_switch_time = last_time;

    bool btn_prev = false;
    bool long_press_done = false;
    int64_t btn_down_time = 0;
    int64_t last_release_time = 0;
    int click_count = 0;

    bool auto_cycle = true;
    float wipe_timer = WIPE_DURATION_SEC;

    int frame_cnt = 0;
    int64_t perf_timer = last_time;
    int64_t acc_render_us = 0;
    int64_t acc_flush_us = 0;

    while (1) {
        int64_t now = esp_timer_get_time();
        float dt = (float)(now - last_time) / 1000000.0f;
        last_time = now;
        if (dt < 0.001f) dt = 0.001f;
        if (dt > 0.04f) dt = 0.04f;

        bool btn_down = (gpio_get_level(PIN_BOOT_BUTTON) == 0);
        if (btn_down && !btn_prev) {
            btn_down_time = now;
            long_press_done = false;
        } else if (btn_down && btn_prev) {
            if (!long_press_done && (now - btn_down_time >= LONG_PRESS_US)) {
                long_press_done = true;
                click_count = 0;
                auto_cycle = !auto_cycle;
                last_switch_time = now;
                buzzer_beep(45);
                show_toast(auto_cycle ? "MODE: AUTO CYCLE" : "MODE: LOCKED");
                ESP_LOGI(TAG, "Long-press: Auto-cycle %s", auto_cycle ? "ON" : "OFF");
            }
        } else if (!btn_down && btn_prev) {
            int64_t held = now - btn_down_time;
            if (!long_press_done && held >= DEBOUNCE_MIN_US && held < LONG_PRESS_US) {
                click_count++;
                last_release_time = now;
            }
        }
        btn_prev = btn_down;

        if (!btn_down && click_count > 0 && (now - last_release_time >= CLICK_WINDOW_US)) {
            int clicks = click_count;
            click_count = 0;

            if (clicks == 1) {
                if (s_scenes[cur_idx]->cleanup) s_scenes[cur_idx]->cleanup();
                cur_idx = next_shuffled_scene();
                s_scenes[cur_idx]->init();
                last_switch_time = now;
                wipe_timer = WIPE_DURATION_SEC;
                buzzer_beep(18);
                ESP_LOGI(TAG, "Single-click -> [%s] %s", s_scenes[cur_idx]->id, s_scenes[cur_idx]->name);
            } else if (clicks == 2) {
                PrismPaletteId next_pal = (PrismPaletteId)((prism_gfx_get_palette() + 1) % PAL_COUNT);
                prism_gfx_set_palette(next_pal);
                char msg[32];
                snprintf(msg, sizeof(msg), "THEME: %s", prism_gfx_get_palette_name());
                show_toast(msg);
                buzzer_beep(25);
                ESP_LOGI(TAG, "Double-click -> Palette: %s", prism_gfx_get_palette_name());
            } else {
                uint8_t next_rot = (display_get_rotation() + 1) % 4;
                display_set_rotation(next_rot);
                char msg[32];
                snprintf(msg, sizeof(msg), "ROTATE: %d DEG", next_rot * 90);
                show_toast(msg);
                buzzer_beep(30);
                ESP_LOGI(TAG, "Triple-click -> Rotation: %d deg", next_rot * 90);
            }
        }

        if (auto_cycle && (now - last_switch_time >= AUTO_CYCLE_US)) {
            if (s_scenes[cur_idx]->cleanup) s_scenes[cur_idx]->cleanup();
            cur_idx = next_shuffled_scene();
            s_scenes[cur_idx]->init();
            last_switch_time = now;
            wipe_timer = WIPE_DURATION_SEC;
            ESP_LOGI(TAG, "Auto-cycle -> [%s] %s", s_scenes[cur_idx]->id, s_scenes[cur_idx]->name);
        }

        int64_t t_ren0 = esp_timer_get_time();
        s_scenes[cur_idx]->update(dt);
        s_scenes[cur_idx]->render(g_framebuffer);

        if (wipe_timer > 0.0f) {
            float progress = 1.0f - (wipe_timer / WIPE_DURATION_SEC);
            gfx_apply_prism_wipe(g_framebuffer, progress);
            wipe_timer -= dt;
            if (wipe_timer < 0.0f) wipe_timer = 0.0f;
        }

        if (s_toast_timer > 0.0f) {
            float fade_in = p_clampf((TOAST_DURATION_SEC - s_toast_timer) / 0.14f, 0.0f, 1.0f);
            float fade_out = p_clampf(s_toast_timer / 0.28f, 0.0f, 1.0f);
            uint8_t alpha = (uint8_t)(240.0f * (fade_in < fade_out ? fade_in : fade_out));
            gfx_draw_prism_toast(g_framebuffer, s_toast_text, alpha);
            s_toast_timer -= dt;
            if (s_toast_timer < 0.0f) s_toast_timer = 0.0f;
        }
        int64_t t_ren1 = esp_timer_get_time();

        int64_t t_flu0 = esp_timer_get_time();
        display_flush(g_framebuffer);
        int64_t t_flu1 = esp_timer_get_time();

        frame_cnt++;
        acc_render_us += (t_ren1 - t_ren0);
        acc_flush_us += (t_flu1 - t_flu0);

        if (now - perf_timer >= 2000000LL) {
            float fps = (float)frame_cnt * 1000000.0f / (float)(now - perf_timer);
            float ren_ms = (float)acc_render_us / (float)frame_cnt / 1000.0f;
            float flu_ms = (float)acc_flush_us / (float)frame_cnt / 1000.0f;
            ESP_LOGI("PERF", "[%s | %s] FPS: %.1f | Render: %.2f ms | Flush: %.2f ms | Heap: %lu B",
                     s_scenes[cur_idx]->id, prism_gfx_get_palette_name(),
                     fps, ren_ms, flu_ms, (unsigned long)esp_get_free_heap_size());
            frame_cnt = 0;
            acc_render_us = 0;
            acc_flush_us = 0;
            perf_timer = now;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
