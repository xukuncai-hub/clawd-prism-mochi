#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdlib.h>

typedef struct {
    int8_t ox;
    int8_t oy;
    uint8_t h;
    const char *const *rows;
} SpritePart;

// ── Clawd 身体部件 ──────────────────────────────────────────────────────────
static const char *const r_c_stand[]  = {"############","############","############","############","############","############","############","############"};
static const char *const r_c_tall[]   = {"############","############","############","############","############","############","############","############","############"};
static const char *const r_c_squash[] = {".ssssssssssss.","##############","##############","##############","##############","##############","##############"};
static const char *const r_c_flat[]   = {"ssssssssssss","############","############"};
static const char *const r_c_turnL[]  = {"###########s","###########s","###########s","###########s","###########s","###########s","###########s","###########s"};
static const char *const r_c_turnR[]  = {"s###########","s###########","s###########","s###########","s###########","s###########","s###########","s###########"};
static const char *const r_c_leanR[]  = {".############",".############",".############",".############","############.","############.","############.","############."};
static const char *const r_c_leanL[]  = {"############.","############.","############.","############.",".############",".############",".############",".############"};

static const SpritePart s_clawd_body[] = {
    [BODY_STAND]   = {2,  0, 8, r_c_stand},
    [BODY_TALL]    = {2, -1, 9, r_c_tall},
    [BODY_SQUASH]  = {1,  2, 7, r_c_squash},
    [BODY_LOAF]    = {1,  2, 7, r_c_squash},
    [BODY_STRETCH] = {2, -1, 9, r_c_tall},
    [BODY_LEAN_L]  = {2,  0, 8, r_c_leanL},
    [BODY_LEAN_R]  = {2,  0, 8, r_c_leanR},
    [BODY_TURN_L]  = {2,  0, 8, r_c_turnL},
    [BODY_TURN_R]  = {2,  0, 8, r_c_turnR},
    [BODY_FLAT]    = {2,  7, 3, r_c_flat},
};

// ── 小紫猫身体部件 ─────────────────────────────────────────────────────────
static const char *const r_k_stand[]   = {"############","############","############","############","############","############","############"};
static const char *const r_k_loaf[]    = {".ssssssssssss.","##############","##############","##############","##############","##############"};
static const char *const r_k_stretch[] = {"s###########","############","############",".###########",".###########","..##########","############","############"};
static const char *const r_k_turnL[]   = {"###########s","###########s","###########s","###########s","###########s","###########s","###########s"};
static const char *const r_k_turnR[]   = {"s###########","s###########","s###########","s###########","s###########","s###########","s###########"};
static const char *const r_k_leanR[]   = {".############",".############",".############",".############","############.","############.","############."};
static const char *const r_k_leanL[]   = {"############.","############.","############.","############.",".############",".############",".############"};

static const SpritePart s_cat_body[] = {
    [BODY_STAND]   = {2, 1, 7, r_k_stand},
    [BODY_TALL]    = {2, 0, 7, r_k_stand},
    [BODY_SQUASH]  = {1, 2, 7, r_c_squash},
    [BODY_LOAF]    = {1, 4, 6, r_k_loaf},
    [BODY_STRETCH] = {2, 0, 8, r_k_stretch},
    [BODY_LEAN_L]  = {2, 1, 7, r_k_leanL},
    [BODY_LEAN_R]  = {2, 1, 7, r_k_leanR},
    [BODY_TURN_L]  = {2, 1, 7, r_k_turnL},
    [BODY_TURN_R]  = {2, 1, 7, r_k_turnR},
    [BODY_FLAT]    = {2, 7, 3, r_c_flat},
};

// ── 眼睛 ───────────────────────────────────────────────────────────────────
static const char *const r_e_open[]   = {"o......o","o......o"};
static const char *const r_e_half[]   = {"o......o"};
static const char *const r_e_shut[]   = {"oo......oo"};
static const char *const r_e_happy[]  = {".o......o.","o.o....o.o"};
static const char *const r_e_squint[] = {"o......o",".o....o.","o......o"};
static const char *const r_e_wide[]   = {"oo......oo","oo......oo"};

static const SpritePart s_eyes[] = {
    [EYE_OPEN]   = {4, 2, 2, r_e_open},
    [EYE_LEFT]   = {3, 2, 2, r_e_open},
    [EYE_RIGHT]  = {5, 2, 2, r_e_open},
    [EYE_UP]     = {4, 1, 2, r_e_open},
    [EYE_HALF]   = {4, 3, 1, r_e_half},
    [EYE_SHUT]   = {3, 3, 1, r_e_shut},
    [EYE_HAPPY]  = {3, 2, 2, r_e_happy},
    [EYE_SQUINT] = {4, 2, 3, r_e_squint},
    [EYE_WIDE]   = {3, 2, 2, r_e_wide},
};

// ── 嘴巴 ───────────────────────────────────────────────────────────────────
static const char *const r_m_small[] = {".ss."};
static const char *const r_m_yawn[]  = {"ssss",".ss."};
static const SpritePart s_mouth[] = {
    [MOUTH_NONE]  = {0, 0, 0, NULL},
    [MOUTH_SMALL] = {6, 5, 1, r_m_small},
    [MOUTH_YAWN]  = {6, 5, 2, r_m_yawn},
};

// ── 腿 ─────────────────────────────────────────────────────────────────────
static const char *const r_l_stand[]  = {".#.#....#.#.",".#.#....#.#."};
static const char *const r_l_crouch[] = {".#.#....#.#."};
static const char *const r_l_tuck[]   = {"..#.#..#.#..","..#.#..#.#.."};
static const char *const r_l_walkA[]  = {".#.s....#.s.",".######.###."};
static const char *const r_l_walkB[]  = {".s.#....s.#.",".###.######."};
static const char *const r_l_flailA[] = {"###.#..###.#","##########.#"};
static const char *const r_l_flailB[] = {"..#.#..#.#..","....#..#...."};

static const SpritePart s_legs[] = {
    [LEG_NONE]    = {0, 0, 0, NULL},
    [LEG_STAND]   = {2, 8, 2, r_l_stand},
    [LEG_CROUCH]  = {2, 9, 1, r_l_crouch},
    [LEG_TUCK]    = {2, 8, 2, r_l_tuck},
    [LEG_WALK_A]  = {2, 8, 2, r_l_walkA},
    [LEG_WALK_B]  = {2, 8, 2, r_l_walkB},
    [LEG_FLAIL_A] = {2, 8, 2, r_l_flailA},
    [LEG_FLAIL_B] = {2, 8, 2, r_l_flailB},
};

// ── Clawd 左右钳 ───────────────────────────────────────────────────────────
static const char *const r_cl_stub[]  = {"##","##"};
static const char *const r_cl_front[] = {"ss","ss"};
static const char *const r_cl_upL[]   = {"##","##","#."};
static const char *const r_cl_upR[]   = {"##","##",".#"};
static const char *const r_cl_openL[] = {"##",".#","##"};
static const char *const r_cl_openR[] = {"##","#.","##"};
static const char *const r_cl_waveL[] = {"##.","##.","..#"};
static const char *const r_cl_waveR[] = {".##",".##","#.."};

static const SpritePart s_claw_l[] = {
    [CLAW_STUB]  = {0,  5, 2, r_cl_stub},
    [CLAW_FRONT] = {0,  5, 2, r_cl_front},
    [CLAW_UP]    = {0, -2, 3, r_cl_upL},
    [CLAW_OPEN]  = {0,  4, 3, r_cl_openL},
    [CLAW_WAVE]  = {-1,-1, 3, r_cl_waveL},
    [CLAW_WRITE] = {1,  6, 2, r_cl_front},
};

static const SpritePart s_claw_r[] = {
    [CLAW_STUB]  = {14,  5, 2, r_cl_stub},
    [CLAW_FRONT] = {14,  5, 2, r_cl_front},
    [CLAW_UP]    = {14, -2, 3, r_cl_upR},
    [CLAW_OPEN]  = {14,  4, 3, r_cl_openR},
    [CLAW_WAVE]  = {14, -1, 3, r_cl_waveR},
    [CLAW_WRITE] = {13,  6, 2, r_cl_front},
};

// ── 小紫猫耳朵 / 爪 / 尾巴 ─────────────────────────────────────────────────
static const char *const r_ka_stand[]    = {"#..........#","##........##"};
static const char *const r_ka_airplane[] = {"##............##","###..........###"};
static const char *const r_ka_twitchL[]  = {"...........#","##........##"};
static const char *const r_ka_twitchR[]  = {"#...........","##........##"};

static const SpritePart s_cat_ears[] = {
    [EAR_STAND]    = {2, -1, 2, r_ka_stand},
    [EAR_NOD]      = {2,  0, 2, r_ka_stand},
    [EAR_LOAF]     = {2,  2, 2, r_ka_stand},
    [EAR_AIRPLANE] = {0,  0, 2, r_ka_airplane},
    [EAR_TWITCH_L] = {2, -1, 2, r_ka_twitchL},
    [EAR_TWITCH_R] = {2, -1, 2, r_ka_twitchR},
};

static const char *const r_kp_cheer[]  = {"#..............#","#..............#"};
static const char *const r_kp_up[]     = {"##............##","##............##"};
static const char *const r_kp_upR[]    = {"##","##"};
static const char *const r_kp_groom[]  = {"ss","ss"};

static const SpritePart s_cat_paws[] = {
    [PAW_NONE]    = {0,  0, 0, NULL},
    [PAW_CHEER]   = {0,  2, 2, r_kp_cheer},
    [PAW_UP]      = {0, -1, 2, r_kp_up},
    [PAW_UP_R]    = {14, 0, 2, r_kp_upR},
    [PAW_GROOM_1] = {12, 4, 2, r_kp_groom},
    [PAW_GROOM_2] = {12, 3, 2, r_kp_groom},
    [PAW_WRITE]   = {13, 5, 2, r_kp_groom},
};

static const char *const r_kt_up[]   = {".s",".#",".#","##"};
static const char *const r_kt_sway[] = {".#",".#","s#","#."};
static const char *const r_kt_curl[] = {"##",".#","s#"};
static const char *const r_kt_loaf[] = {".#",".#","s#"};
static const char *const r_kt_puff[] = {".#","##","##","s#","#."};

static const SpritePart s_cat_tail[] = {
    [TAIL_UP]   = {14, 3, 4, r_kt_up},
    [TAIL_SWAY] = {14, 3, 4, r_kt_sway},
    [TAIL_CURL] = {14, 4, 3, r_kt_curl},
    [TAIL_LOAF] = {14, 6, 3, r_kt_loaf},
    [TAIL_PUFF] = {14, 2, 5, r_kt_puff},
};

// ── 蓝鲸 Deepy 部件 ────────────────────────────────────────────────────────
static const char *const r_w_stand[]  = {".##########.","############","##UUUUUUUU##","#UUUUUUUUUU#",".#UUUUUUUU#.","..TTTTTTTT.."};
static const char *const r_w_squash[] = {"..ssssssssss..",".############.","##############","##UUUUUUUUUU##",".#UUUUUUUUUU#.","..TTTTTTTTTT.."};
static const char *const r_w_paw_dn[] = {".s","ss"};
static const char *const r_w_paw_up[] = {"ss............ss",".s............s."};
static const char *const r_w_tail[]   = {"..#..#","..####","...##.","..###.",".#####","##s..."};

static const SpritePart s_whale_body[] = {
    [BODY_STAND]   = {2, 1, 6, r_w_stand},
    [BODY_TALL]    = {2, 0, 6, r_w_stand},
    [BODY_SQUASH]  = {1, 3, 6, r_w_squash},
};
static const SpritePart s_whale_paws_dn = {0, 6, 2, r_w_paw_dn};
static const SpritePart s_whale_paws_up = {0, 2, 2, r_w_paw_up};
static const SpritePart s_whale_tail    = {13, 1, 6, r_w_tail};

// ── 迷你蟹 ─────────────────────────────────────────────────────────────────
static const char *const r_mini_cowboy[] = {
    ".....HHHHHH.....","...HHHHHHHHHH...","..RRRRRRRRRRRR..","..############..",
    "..##o######o##..","################","..############..","...#.#....#.#..."
};
static const char *const r_mini_cowboy_t[] = {
    "....HHHHHH......","..HHHHHHHHHH....",".RRRRRRRRRRRR...","..############..",
    "..##o######o##..","################","..############..","..#..#....#..#.."
};
static const char *const r_mini_shades[] = {
    "..############..","....GGGG##GGGG..","..GGGGGGGGGGGG..",
    "################","..############..","...#.#....#.#..."
};
static const char *const r_mini_shades_t[] = {
    "..############..","..GwGGwG##GwGGwG","..GGGGGGGGGGGG..",
    "################","..############..","...#.#....#.#..."
};
static const char *const r_mini_wizard[] = {
    ".......MM.......","......MyMM......",".....MMMMMM.....","..MMMMMMMMMMMM..",
    "..############..","..##o######o##..","################","..############..","...#.#....#.#..."
};
static const char *const r_mini_wizard_t[] = {
    ".....yyyyyyyyyy.","....yyMyMM......",".....MMMMMM.....","..MMMMMMMMMMMM..",
    "..############..","..##o######o##..","################","..############y.","...#.#....#.#.y."
};

// ── 道具点阵 ───────────────────────────────────────────────────────────────
static const char *const r_pr_coffee0[]  = {".m..m.","m..m..","DDDDD.","DDnDDD","DDnDD.",".DDD.."};
static const char *const r_pr_coffee1[]  = {"m..m..",".m..m.","DDDDD.","DDnDDD","DDnDD.",".DDD.."};
static const char *const r_pr_donut0[]   = {".ppp.","pywpp","pp.pn","nnnnn",".nnn."};
static const char *const r_pr_donut1[]   = {".pp..","pyw..","pp.pn","nnnnn",".nnn."};
static const char *const r_pr_bowl[]     = {"wwwww","DBwBD",".DDD."};
static const char *const r_pr_bowle[]    = {"DgggD",".DDD."};
static const char *const r_pr_chop0[]    = {"...n","..n.",".n..","n..."};
static const char *const r_pr_chop1[]    = {"...n","..n.",".n..","w..."};
static const char *const r_pr_fish[]     = {".PPsPP.","DDDDDDD",".DDDDD."};
static const char *const r_pr_fishb[]    = {".M.M.M.","DDDDDDD",".DDDDD."};
static const char *const r_pr_stickL_G[] = {".....tt","WG...tt",".GG..tt","..GG.tt","...GGtt","....GGt",".....GG"};
static const char *const r_pr_stickU_G[] = {"WW","GG","GG","GG","GG","GG","GG"};
static const char *const r_pr_stickR_G[] = {"tt.....","tt...GW","tt..GG.","tt.GG..","ttGG...","tGG....","GG....."};
static const char *const r_pr_stickL_P[] = {".....tt","WN...tt",".NN..tt","..NN.tt","...NNtt","....NNt",".....NN"};
static const char *const r_pr_stickU_P[] = {"WW","NN","NN","NN","NN","NN","NN"};
static const char *const r_pr_stickR_P[] = {"tt.....","tt...NW","tt..NN.","tt.NN..","ttNN...","tNN....","NN....."};
static const char *const r_pr_mic[]      = {"ZZ","Zz","ZZ","Z.","Z.","ZZZZ"};
static const char *const r_pr_sheet[]    = {"cccccc","ckkccc","gggggg"};
static const char *const r_pr_quill[]    = {"..B",".B.","k.."};
static const char *const r_pr_book0[]    = {"BBBBB","BwwwB","BwgwB","BwwwB","BBBBB"};
static const char *const r_pr_book1[]    = {"BB.BB","BwBwB","BwgwB","BwBwB","BB.BB"};
static const char *const r_pr_bulb0[]    = {"..gg..",".g..g.",".g..g.","..cc.."};
static const char *const r_pr_bulb1[]    = {"y.yy.y",".yVVy.","yVVVVy","..yy..","..cc.."};
static const char *const r_pr_shades[]   = {"GGGG..GGGG","GEGG..GEGG"};
static const char *const r_pr_surf[]     = {"..AAAAAAAAAA..",".AAAAAAAAAAEA."};
static const char *const r_pr_wave0[]    = {"..E.","E.A.",".AA."};
static const char *const r_pr_wave1[]    = {"E...",".EA.","AA.."};
static const char *const r_pr_ice[]      = {"..av..",".aavv.","aaavvv",".aavv.","..av.."};
static const char *const r_pr_laser[]    = {".L.","LLL",".L."};
static const char *const r_pr_seed[]     = {"On","nO"};
static const char *const r_pr_p1[]       = {"O","l","l"};
static const char *const r_pr_p2[]       = {"O","l","l","l"};
static const char *const r_pr_p3[]       = {"..O..","..l..","O.l.O",".lll.","..l.."};
static const char *const r_pr_p4[]       = {"..O..","O.l.O",".lll.","O.l.O",".lll.","..l..","..l.."};
static const char *const r_pr_flower0[]  = {"...O...","O.....O",".O...O.",".......","O.....O",".......",".......","...O..."};
static const char *const r_pr_flower1[]  = {".q...q.","...q...","q.....q",".......",".q...q.",".......","...q..."};
static const char *const r_pr_bf_up[]    = {"B...B","BB.BB",".BkB.","..k.."};
static const char *const r_pr_bf_dn[]    = {".....",".....","BBkBB",".BkB."};
static const char *const r_pr_env0[]     = {"dddddd","ddwwdd","dwddwd","dwwwwd","dddddd"};
static const char *const r_pr_env1[]     = {"..dd..",".dwwd.","dddddd","dwwwwd","dwwwwd","dddddd"};
static const char *const r_pr_letter[]   = {"ddddddd","dcccccd","dccrccd","dcccccd","dcccccd","dccrccd","drrrrrd","ddddddd"};
static const char *const r_pr_siren_r[]  = {"..Lx..",".HLxx.",".LLxx.","llllll"};
static const char *const r_pr_siren_b[]  = {"..AC..",".EAii.",".AAii.","llllll"};
static const char *const r_pr_siren_fr[] = {"..HH..",".LHHL.",".LLLL.","llllll"};
static const char *const r_pr_siren_fb[] = {"..EE..",".AEAA.",".AAAA.","llllll"};
static const char *const r_pr_headband[] = {".LLLLLLLLLLLL.L","..HHHHHHHHHH..L"};
static const char *const r_pr_gflame0[]  = {"...Y...","..fYf..",".FfYfF.","FfYVYfF"};
static const char *const r_pr_gflame1[]  = {"..Y....",".fYf..Y","FfYVYf.",".FfYfF."};
static const char *const r_pr_hg0[]      = {"nnnnn",".byb.","..y..",".b.b.","nnnnn"};
static const char *const r_pr_hg1[]      = {"nnnnn",".b.b.","..y..",".byb.","nnnnn"};
static const char *const r_pr_watch[]    = {"..y..",".ywy.","ywnwy",".yyy."};
static const char *const r_pr_spout0[]   = {".B..B.","..AA..","..BA..","..AA.."};
static const char *const r_pr_spout1[]   = {".B....B.","B.A..A.B","..AB.BA.","...AA...","...BA...","...AA..."};
static const char *const r_pr_spout2[]   = {"..B....B..",".A......A.","B...BB...B","....AA....",".B..BA..B.","....AA...."};
static const char *const r_pr_fire0[]    = {"..f...Y..",".FfY.fYf.","FfYVYfYVf","FffYVYffF"};
static const char *const r_pr_fire1[]    = {".Y...f...",".fYf..fY.","fYVYfFfYf","FffYVYffF"};
static const char *const r_pr_fire2[]    = {"..Y...Y..",".fYf.fYf.","FfYVYfYVf","FFfYVYfFF"};

static const SpritePart s_props[] = {
    [PROP_COFFEE_0]   = {12,  3, 6, r_pr_coffee0},
    [PROP_COFFEE_1]   = {12,  3, 6, r_pr_coffee1},
    [PROP_DONUT_0]    = {12,  4, 5, r_pr_donut0},
    [PROP_DONUT_1]    = {12,  4, 5, r_pr_donut1},
    [PROP_BOWL]       = {12,  7, 3, r_pr_bowl},
    [PROP_BOWL_EMPTY] = {12,  8, 2, r_pr_bowle},
    [PROP_CHOP_0]     = {12,  4, 4, r_pr_chop0},
    [PROP_CHOP_1]     = {12,  3, 4, r_pr_chop1},
    [PROP_FISH]       = {11,  7, 3, r_pr_fish},
    [PROP_FISH_BONE]  = {11,  7, 3, r_pr_fishb},
    [PROP_STICK_L_G]  = { 9, -9, 7, r_pr_stickL_G},
    [PROP_STICK_UP_G] = {14, -9, 7, r_pr_stickU_G},
    [PROP_STICK_R_G]  = {14, -9, 7, r_pr_stickR_G},
    [PROP_STICK_L_P]  = { 9, -9, 7, r_pr_stickL_P},
    [PROP_STICK_UP_P] = {14, -9, 7, r_pr_stickU_P},
    [PROP_STICK_R_P]  = {14, -9, 7, r_pr_stickR_P},
    [PROP_MIC_STAND]  = { 6,  4, 6, r_pr_mic},
    [PROP_SHEET]      = {12,  8, 3, r_pr_sheet},
    [PROP_QUILL_0]    = {15,  3, 3, r_pr_quill},
    [PROP_QUILL_1]    = {16,  4, 3, r_pr_quill},
    [PROP_BOOK_0]     = {12,  4, 5, r_pr_book0},
    [PROP_BOOK_1]     = {12,  4, 5, r_pr_book1},
    [PROP_BULB_OFF]   = { 5, -8, 4, r_pr_bulb0},
    [PROP_BULB_ON]    = { 5, -9, 5, r_pr_bulb1},
    [PROP_SHADES]     = { 3,  2, 2, r_pr_shades},
    [PROP_SURFBOARD]  = { 1,  8, 2, r_pr_surf},
    [PROP_WAVE_0]     = {-3,  6, 3, r_pr_wave0},
    [PROP_WAVE_1]     = {-3,  6, 3, r_pr_wave1},
    [PROP_ICE_CRYSTAL]= { 5, -6, 5, r_pr_ice},
    [PROP_LASER_DOT]  = { 0,  0, 3, r_pr_laser},
    [PROP_SEED]       = {15,  8, 2, r_pr_seed},
    [PROP_PLANT_1]    = {16,  7, 3, r_pr_p1},
    [PROP_PLANT_2]    = {16,  6, 4, r_pr_p2},
    [PROP_PLANT_3]    = {14,  5, 5, r_pr_p3},
    [PROP_PLANT_4]    = {14,  3, 7, r_pr_p4},
    [PROP_FLOWER_0]   = {13,  0, 8, r_pr_flower0},
    [PROP_FLOWER_1]   = {13, -2, 7, r_pr_flower1},
    [PROP_BF_UP]      = {13, -4, 4, r_pr_bf_up},
    [PROP_BF_DN]      = {13, -4, 4, r_pr_bf_dn},
    [PROP_ENV_SEALED] = {12,  5, 5, r_pr_env0},
    [PROP_ENV_OPEN]   = {12,  4, 6, r_pr_env1},
    [PROP_LETTER]     = {14,  1, 8, r_pr_letter},
    [PROP_SIREN_R]    = { 5, -4, 4, r_pr_siren_r},
    [PROP_SIREN_B]    = { 5, -4, 4, r_pr_siren_b},
    [PROP_SIREN_F_R]  = { 5, -4, 4, r_pr_siren_fr},
    [PROP_SIREN_F_B]  = { 5, -4, 4, r_pr_siren_fb},
    [PROP_HEADBAND]   = { 1,  1, 2, r_pr_headband},
    [PROP_GOAL_FLAME_0]= {4, -7, 4, r_pr_gflame0},
    [PROP_GOAL_FLAME_1]= {4, -7, 4, r_pr_gflame1},
    [PROP_HOURGLASS_0]= {13,  1, 5, r_pr_hg0},
    [PROP_HOURGLASS_1]= {13,  1, 5, r_pr_hg1},
    [PROP_WATCH]      = {12,  4, 4, r_pr_watch},
    [PROP_WHALE_SPOUT_0]= {5,-3, 4, r_pr_spout0},
    [PROP_WHALE_SPOUT_1]= {4,-5, 6, r_pr_spout1},
    [PROP_WHALE_SPOUT_2]= {3,-5, 6, r_pr_spout2},
    [PROP_FIRE_0]     = { 3,  6, 4, r_pr_fire0},
    [PROP_FIRE_1]     = { 3,  6, 4, r_pr_fire1},
    [PROP_FIRE_2]     = { 3,  6, 4, r_pr_fire2},
};

// ── 情绪符号点阵 ───────────────────────────────────────────────────────────
static const char *const r_sy_heart[]   = {".r.r.","rrrrr",".rrr.","..r.."};
static const char *const r_sy_note[]    = {"..kk.","..k.k","..k..","kkk..","kk..."};
static const char *const r_sy_spark[]   = {"..y..","..y..","yyyyy","..y..","..y.."};
static const char *const r_sy_zS[]      = {"kkk",".k.","kkk"};
static const char *const r_sy_zB[]      = {"kkkk","..k.",".k..","kkkk"};
static const char *const r_sy_bub0[]    = {".b.","bEb",".b."};
static const char *const r_sy_bub1[]    = {".bb.","bEwb","bEEb",".bb."};
static const char *const r_sy_q[]       = {".kkk.","k...k","..kk.","..k..",".....","..k.."};
static const char *const r_sy_bang[]    = {".r.",".r.",".r.","...",".r."};
static const char *const r_sy_sweat[]   = {".B.","BbB","BbB",".B."};
static const char *const r_sy_tilde[]   = {".kkk.","k.k.k","...k."};
static const char *const r_sy_vein[]    = {".r.r.","rr.rr",".....","rr.rr",".r.r."};
static const char *const r_sy_fsp0[]    = {"..Y..","f...V","..f.."};
static const char *const r_sy_fsp1[]    = {".V..f","..Y..","f...."};

static const SpritePart s_syms[] = {
    [SYM_HEART]       = { 5, -7, 4, r_sy_heart},
    [SYM_NOTE]        = { 5, -8, 5, r_sy_note},
    [SYM_SPARK]       = { 5, -8, 5, r_sy_spark},
    [SYM_Z_SMALL]     = {12, -4, 3, r_sy_zS},
    [SYM_Z_BIG]       = {13, -9, 4, r_sy_zB},
    [SYM_BUBBLE_0]    = {12,  4, 3, r_sy_bub0},
    [SYM_BUBBLE_1]    = {12,  3, 4, r_sy_bub1},
    [SYM_QUESTION]    = { 5, -9, 6, r_sy_q},
    [SYM_BANG]        = { 6, -8, 5, r_sy_bang},
    [SYM_SWEAT]       = {13, -4, 4, r_sy_sweat},
    [SYM_TILDE]       = { 5, -7, 3, r_sy_tilde},
    [SYM_VEIN]        = { 5, -7, 5, r_sy_vein},
    [SYM_FIRE_SPARK_0]= { 5, -8, 3, r_sy_fsp0},
    [SYM_FIRE_SPARK_1]= { 5, -9, 3, r_sy_fsp1},
};

// ── 共享色调查表 (高亮适合 ST7789 屏) ──────────────────────────────────────
static bool resolve_prop_rgb(char ch, uint8_t *r, uint8_t *g, uint8_t *b) {
    switch (ch) {
        case 'k': *r = 175; *g = 168; *b = 160; return true;
        case 'w': *r = 251; *g = 248; *b = 241; return true;
        case 'c': *r = 236; *g = 223; *b = 198; return true;
        case 'g': *r = 168; *g = 161; *b = 148; return true;
        case 'B': *r = 96;  *g = 165; *b = 250; return true;
        case 'b': *r = 207; *g = 227; *b = 238; return true;
        case 'r': *r = 255; *g = 68;  *b = 88;  return true;
        case 'y': *r = 255; *g = 214; *b = 68;  return true;
        case 'D': *r = 111; *g = 140; *b = 166; return true;
        case 'p': *r = 255; *g = 145; *b = 168; return true;
        case 'n': *r = 152; *g = 104; *b = 68;  return true;
        case 'l': *r = 106; *g = 97;  *b = 88;  return true;
        case 'F': *r = 200; *g = 62;  *b = 40;  return true;
        case 'f': *r = 238; *g = 115; *b = 48;  return true;
        case 'Y': *r = 250; *g = 185; *b = 62;  return true;
        case 'V': *r = 255; *g = 244; *b = 220; return true;
        case 'm': *r = 185; *g = 195; *b = 210; return true;
        case 'G': *r = 98;  *g = 255; *b = 144; return true;
        case 't': *r = 90;  *g = 220; *b = 170; return true;
        case 'N': *r = 255; *g = 107; *b = 214; return true;
        case 'W': *r = 248; *g = 255; *b = 246; return true;
        case 'Z': *r = 35;  *g = 33;  *b = 32;  return true;
        case 'A': *r = 61;  *g = 139; *b = 255; return true;
        case 'E': *r = 214; *g = 236; *b = 255; return true;
        case 'L': *r = 255; *g = 59;  *b = 48;  return true;
        case 'H': *r = 255; *g = 214; *b = 210; return true;
        case 'R': *r = 255; *g = 59;  *b = 79;  return true;
        case 'P': *r = 232; *g = 132; *b = 104; return true;
        case 'M': *r = 222; *g = 214; *b = 204; return true;
        case 'a': *r = 198; *g = 224; *b = 255; return true;
        case 'v': *r = 135; *g = 165; *b = 215; return true;
        case 'O': *r = 120; *g = 232; *b = 112; return true;
        case 'q': *r = 255; *g = 152; *b = 198; return true;
        case 'Q': *r = 255; *g = 208; *b = 228; return true;
        case 'd': *r = 154; *g = 142; *b = 128; return true;
        case 'x': *r = 179; *g = 49;  *b = 42;  return true;
        case 'C': *r = 39;  *g = 83;  *b = 168; return true;
        case 'i': *r = 72;  *g = 130; *b = 220; return true;
        case 'U': *r = 229; *g = 240; *b = 255; return true;
        case 'T': *r = 168; *g = 203; *b = 255; return true;
        default: return false;
    }
}

// 7 色彩虹查表，用于彩虹小紫猫皮肤
static void resolve_rainbow_rgb(int gx, int gy, uint8_t phase, bool shadow, uint8_t *r, uint8_t *g, uint8_t *b) {
    static const uint8_t rb[6][3] = {
        {255, 72,  88},
        {255, 168, 48},
        {255, 232, 64},
        {82,  235, 130},
        {72,  188, 255},
        {178, 112, 255},
    };
    int idx = ((gx * 2 + gy * 2 + (phase >> 3)) & 31) * 6 / 32;
    if (idx >= 6) idx = 5;
    if (shadow) {
        *r = (uint8_t)((rb[idx][0] * 195) >> 8);
        *g = (uint8_t)((rb[idx][1] * 195) >> 8);
        *b = (uint8_t)((rb[idx][2] * 195) >> 8);
    } else {
        *r = rb[idx][0];
        *g = rb[idx][1];
        *b = rb[idx][2];
    }
}

static void draw_block(uint16_t *fb, int px, int py, int w, int h, uint16_t bus_c) {
    int x0 = px < 0 ? 0 : px;
    int y0 = py < 0 ? 0 : py;
    int x1 = px + w; if (x1 > DISP_W) x1 = DISP_W;
    int y1 = py + h; if (y1 > DISP_H) y1 = DISP_H;
    for (int y = y0; y < y1; y++) {
        uint16_t *row = fb + y * DISP_W;
        for (int x = x0; x < x1; x++) {
            row[x] = bus_c;
        }
    }
}

static void draw_part(uint16_t *fb, const SpritePart *pt, int ax, int ay, int px_scale,
                      uint8_t main_r, uint8_t main_g, uint8_t main_b,
                      uint8_t dark_r, uint8_t dark_g, uint8_t dark_b,
                      bool flip_x, bool rainbow, uint8_t rainbow_phase) {
    if (!pt || !pt->rows || pt->h == 0) return;
    uint16_t main_bus = rgb565_bus(main_r, main_g, main_b);
    uint16_t dark_bus = rgb565_bus(dark_r, dark_g, dark_b);
    uint16_t eye_bus  = rgb565_bus(20, 20, 19);

    for (int ry = 0; ry < pt->h; ry++) {
        const char *row = pt->rows[ry];
        int gy = pt->oy + ry;
        int py = ay + gy * px_scale;
        for (int rx = 0; row[rx] != '\0'; rx++) {
            char ch = row[rx];
            if (ch == '.' || ch == ' ') continue;
            int gx = pt->ox + rx;
            int draw_gx = flip_x ? (15 - gx) : gx;
            int px = ax + draw_gx * px_scale;

            uint16_t bus_c;
            if (ch == '#') {
                if (rainbow) {
                    uint8_t rr, rg, rb;
                    resolve_rainbow_rgb(gx, gy, rainbow_phase, false, &rr, &rg, &rb);
                    bus_c = rgb565_bus(rr, rg, rb);
                } else {
                    bus_c = main_bus;
                }
            } else if (ch == 's') {
                if (rainbow) {
                    uint8_t rr, rg, rb;
                    resolve_rainbow_rgb(gx, gy, rainbow_phase, true, &rr, &rg, &rb);
                    bus_c = rgb565_bus(rr, rg, rb);
                } else {
                    bus_c = dark_bus;
                }
            } else if (ch == 'o') {
                bus_c = eye_bus;
            } else {
                uint8_t pr, pg, pb;
                if (!resolve_prop_rgb(ch, &pr, &pg, &pb)) continue;
                bus_c = rgb565_bus(pr, pg, pb);
            }
            draw_block(fb, px, py, px_scale, px_scale, bus_c);
        }
    }
}

void mascot_pose_default(MascotPose *p) {
    p->body = BODY_STAND;
    p->eyes = EYE_OPEN;
    p->mouth = MOUTH_NONE;
    p->legs = LEG_STAND;
    p->claw_l = CLAW_STUB;
    p->claw_r = CLAW_STUB;
    p->ears = EAR_STAND;
    p->paws = PAW_NONE;
    p->tail = TAIL_UP;
    p->sink = 0;
    p->flip_x = false;
    p->rainbow = false;
    p->rainbow_phase = 0;
}

void mascot_draw(uint16_t *fb, MascotKind kind, int cx, int base_y, int px_scale, const MascotPose *pose) {
    MascotPose def;
    if (!pose) {
        mascot_pose_default(&def);
        pose = &def;
    }

    int ax = cx - 8 * px_scale;
    int ay = base_y - (10 - pose->sink) * px_scale;

    uint8_t mr, mg, mb, dr, dg, db;
    if (kind == MASCOT_CLAWD) {
        mr = 228; mg = 122; mb = 88;
        dr = 186; dg = 92;  db = 60;
    } else if (kind == MASCOT_WHALE) {
        mr = 78;  mg = 115; mb = 255;
        dr = 52;  dg = 80;  db = 207;
    } else {
        // 针对冷色温 ST7789 IPS 屏校准：提高 R、压低 G，呈现纯正亮丽的葡萄紫而非偏蓝
        mr = 196; mg = 82;  mb = 242;
        dr = 142; dg = 52;  db = 188;
    }

    if (kind == MASCOT_WHALE) {
        MascotBodyId wb = (pose->body == BODY_SQUASH) ? BODY_SQUASH : BODY_STAND;
        draw_part(fb, &s_whale_body[wb], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
        draw_part(fb, &s_whale_tail, ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
        draw_part(fb, (pose->paws == PAW_UP || pose->paws == PAW_CHEER) ? &s_whale_paws_up : &s_whale_paws_dn,
                  ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
        SpritePart eye = s_eyes[pose->eyes];
        eye.oy += (wb == BODY_SQUASH) ? 2 : 1;
        draw_part(fb, &eye, ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
    } else if (kind == MASCOT_CLAWD) {
        draw_part(fb, &s_legs[pose->legs], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
        draw_part(fb, &s_clawd_body[pose->body], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
        SpritePart eye = s_eyes[pose->eyes];
        if (pose->body == BODY_FLAT) {
            eye.oy = 8;
        } else if (pose->body == BODY_TALL) {
            eye.oy -= 1;
        } else if (pose->body == BODY_SQUASH) {
            eye.oy += 1;
        }
        draw_part(fb, &eye, ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
        if (pose->mouth != MOUTH_NONE) {
            draw_part(fb, &s_mouth[pose->mouth], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
        }
        draw_part(fb, &s_claw_l[pose->claw_l], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
        draw_part(fb, &s_claw_r[pose->claw_r], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
    } else {
        // 小紫猫 (Cat): 耳 -> 腿 -> 身 -> 尾 -> 眼(下移1格) -> 爪
        if (pose->body != BODY_FLAT) {
            draw_part(fb, &s_cat_ears[pose->ears], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
        }
        if (pose->body != BODY_LOAF && pose->body != BODY_FLAT) {
            draw_part(fb, &s_legs[pose->legs], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
        }
        draw_part(fb, &s_cat_body[pose->body], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
        draw_part(fb, &s_cat_tail[pose->tail], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);

        SpritePart eye = s_eyes[pose->eyes];
        if (pose->body == BODY_LOAF) {
            eye.oy += 3;
        } else if (pose->body == BODY_FLAT) {
            eye.oy = 8;
        } else {
            eye.oy += 1;
            if (pose->body == BODY_SQUASH) eye.oy += 1;
        }
        draw_part(fb, &eye, ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
        if (pose->mouth != MOUTH_NONE) {
            SpritePart m = s_mouth[pose->mouth];
            m.oy += 1;
            draw_part(fb, &m, ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, false, 0);
        }
        if (pose->paws != PAW_NONE) {
            draw_part(fb, &s_cat_paws[pose->paws], ax, ay, px_scale, mr, mg, mb, dr, dg, db, pose->flip_x, pose->rainbow, pose->rainbow_phase);
        }
    }
}

void mascot_draw_mini(uint16_t *fb, MiniCrabKind kind, int cx, int base_y, int px_scale, bool trick_frame) {
    int ax = cx - 8 * px_scale;
    int ay = base_y - 8 * px_scale;
    SpritePart pt;
    uint8_t mr = 228, mg = 122, mb = 88, dr = 186, dg = 92, db = 60;

    if (kind == MINI_COWBOY) {
        pt = (SpritePart){0, 0, 8, trick_frame ? r_mini_cowboy_t : r_mini_cowboy};
    } else if (kind == MINI_SHADES) {
        ay = base_y - 6 * px_scale;
        pt = (SpritePart){0, 0, 6, trick_frame ? r_mini_shades_t : r_mini_shades};
        mr = 232; mg = 136; mb = 104;
    } else {
        ay = base_y - 9 * px_scale;
        pt = (SpritePart){0, 0, 9, trick_frame ? r_mini_wizard_t : r_mini_wizard};
        mr = 188; mg = 76;  mb = 236;
        dr = 136; dg = 48;  db = 178;
    }
    draw_part(fb, &pt, ax, ay, px_scale, mr, mg, mb, dr, dg, db, false, false, 0);
}

void mascot_draw_prop(uint16_t *fb, MascotPropId prop, int anchor_x, int anchor_y, int px_scale) {
    if (prop > PROP_FIRE_2) return;
    draw_part(fb, &s_props[prop], anchor_x, anchor_y, px_scale, 228, 122, 88, 186, 92, 60, false, false, 0);
}

void mascot_draw_sym(uint16_t *fb, MascotSymId sym, int anchor_x, int anchor_y, int px_scale) {
    if (sym > SYM_FIRE_SPARK_1) return;
    draw_part(fb, &s_syms[sym], anchor_x, anchor_y, px_scale, 228, 122, 88, 186, 92, 60, false, false, 0);
}

void mascot_draw_shadow(uint16_t *fb, int cx, int y, int half_w) {
    if (half_w <= 2) return;
    for (int dy = -1; dy <= 1; dy++) {
        int py = y + dy;
        if ((unsigned)py >= DISP_H) continue;
        int w = (dy == 0) ? half_w : (half_w * 3) / 4;
        for (int dx = -w; dx <= w; dx++) {
            uint8_t sa = (uint8_t)(((w - abs(dx)) * ((dy == 0) ? 130 : 70)) / w);
            gfx_blend_px(fb, cx + dx, py, 2, 2, 5, sa);
        }
    }
}

typedef struct {
    uint8_t x, y;
    uint8_t phase;
    uint8_t spec;
} StageStar;

#define NUM_STAGE_STARS 26
static StageStar s_stars[NUM_STAGE_STARS];
static bool s_stars_inited = false;

void mascot_stage_render(uint16_t *fb, float time_sec, bool draw_arch_glow) {
    if (!s_stars_inited) {
        for (int i = 0; i < NUM_STAGE_STARS; i++) {
            s_stars[i].x = (uint8_t)p_rand_range(10, DISP_W - 10);
            s_stars[i].y = (uint8_t)p_rand_range(12, STAGE_TOP_Y - 14);
            s_stars[i].phase = (uint8_t)(p_rand() & 0xFF);
            s_stars[i].spec = (uint8_t)(p_rand() & 0xFF);
        }
        s_stars_inited = true;
    }

    gfx_clear_obsidian_bg(fb);

    uint8_t spec_shift = (uint8_t)(time_sec * 34.0f);

    // 1. 呼吸星辰点缀
    for (int i = 0; i < NUM_STAGE_STARS; i++) {
        uint8_t ph = (uint8_t)(s_stars[i].phase + (uint8_t)(time_sec * 38.0f));
        int16_t sv = p_sin_q12(ph << 2);
        uint8_t a = (uint8_t)p_clampi(65 + ((sv + 4096) >> 5), 35, 225);
        const uint8_t *sc = g_prism_pal.spectrum_rgb[(uint8_t)(s_stars[i].spec + spec_shift)];
        gfx_blend_px(fb, s_stars[i].x, s_stars[i].y, sc[0], sc[1], sc[2], a);
    }

    // 2. 顶部极光穹顶细光弧
    if (draw_arch_glow) {
        const int segs = 28;
        float px0 = 24.0f, py0 = 150.0f;
        for (int i = 1; i <= segs; i++) {
            float t = (float)i / (float)segs;
            float ang = PRISM_PI - t * PRISM_PI;
            float px1 = 120.0f + 96.0f * p_cosf(ang);
            float py1 = 150.0f - 88.0f * p_sinf(ang) + 3.0f * p_sinf(time_sec * 2.0f + t * 5.0f);
            const uint8_t *sc = g_prism_pal.spectrum_rgb[(uint8_t)(i * 9 + spec_shift)];
            gfx_draw_glow_line(fb, px0, py0, px1, py1, sc[0], sc[1], sc[2], 125);
            px0 = px1;
            py0 = py1;
        }
    }

    // 3. 底部烟熏黑曜玻璃展台 + 2px Prism Rim 动态流光亮边
    const int sx = 14, sy = STAGE_TOP_Y, sw = 212, sh = 46;
    gfx_draw_glass_panel(fb, sx, sy, sw, sh, 12, 232);

    int rx0 = sx + 12;
    int rx1 = sx + sw - 12;
    int span = rx1 - rx0;
    for (int px = rx0; px <= rx1; px++) {
        uint8_t sidx = (uint8_t)(((px - rx0) * 255) / span + spec_shift);
        const uint8_t *sc = g_prism_pal.spectrum_rgb[sidx];
        gfx_add_px(fb, px, sy, (sc[0] + 190) >> 1, (sc[1] + 190) >> 1, (sc[2] + 190) >> 1);
        gfx_add_px(fb, px, sy + 1, sc[0] >> 1, sc[1] >> 1, sc[2] >> 1);
    }
}
