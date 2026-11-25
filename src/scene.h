#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>

typedef struct {
    const char *id;
    const char *name;
    void (*init)(void);
    void (*update)(float dt);
    void (*render)(uint16_t *fb);
    void (*cleanup)(void);
} PrismScene;

extern PrismScene scene_dual_play;
extern PrismScene scene_tower_stack;
extern PrismScene scene_rainbow_cat;
extern PrismScene scene_cafe_time;
extern PrismScene scene_laser_chase;
extern PrismScene scene_coding_duo;
extern PrismScene scene_concert_dance;
extern PrismScene scene_feast_dinner;
extern PrismScene scene_sleepy_night;
extern PrismScene scene_surf_party;
extern PrismScene scene_garden_bloom;
extern PrismScene scene_love_letter;
extern PrismScene scene_police_patrol;
extern PrismScene scene_quarrel_makeup;
extern PrismScene scene_goal_sprint;
extern PrismScene scene_hide_peek;
extern PrismScene scene_whale_splash;
extern PrismScene scene_campfire_story;

#endif // SCENE_H
