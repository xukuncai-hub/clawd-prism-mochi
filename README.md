<!-- LOGO -->
<p align="center">
  <img src="pics/clawd_mochi_banner.png" alt="Clawd Mochi Logo" width="700"/>
</p>

# Clawd Mochi · Obsidian Prism 双宠动画剧场（18 幕）

[![Framework: ESP-IDF](https://img.shields.io/badge/Framework-ESP--IDF-E7352C.svg?style=flat-square&logo=espressif)](https://docs.espressif.com/projects/esp-idf/en/latest/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg?style=flat-square&logo=platformio)](https://platformio.org/)
[![Hardware: ESP32-C3](https://img.shields.io/badge/Hardware-ESP32--C3-blue.svg?style=flat-square&logo=espressif)](https://www.espressif.com/en/products/socs/esp32-c3)
[![Display: ST7789 IPS](https://img.shields.io/badge/Display-ST7789%20240x240%20IPS-green.svg?style=flat-square)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)
[![3D Case: MakerWorld](https://img.shields.io/badge/3D%20Print-MakerWorld-informational.svg?style=flat-square&logo=bambulab)](https://makerworld.com/en/models/2559505-clawd-mochi-physical-claude-code-mascot#profileId-2820000)

**Clawd Mochi** 是一款基于 **ESP32-C3 + ST7789 1.54 寸 240×240 IPS 彩屏** 打造的桌面像素萌宠摆件固件。

本项目深度融合了 [Obsidian Prism](https://github.com/RMvdesi/obsidian-prism) 的原版双宠像素语法与光学舞台美学：在自带流动 Prism Rim 光边的烟熏黑曜玻璃展台上，让 **克劳德橙蟹（Clawd）**、**小紫猫（Purple Cat）**、**蓝鲸 Deepy** 以及 **3 只迷你蟹** 演绎 **18 幕** 生动有趣的像素动画剧场，全程 **30+ FPS** 丝滑运行、零弹窗、纯离线插电即用！

> ⚠️ **声明**：本项目为独立开源极客作品，灵感源自 Anthropic 旗下 Claude Code 吉祥物 Clawd 与社区项目 Obsidian Prism。“Claude” 与 “Clawd” 为各自权利方的注册商标。

---

<p align="center">
  <img src="pics/clawd_mochi_3_4.jpeg" alt="Assembled Clawd Mochi on a desk" width="620"/>
</p>

---

## ✨ 核心特性 (Features)

- 🦀 **原版像素双宠演绎**：严格遵循 16×10 零黑描边纯方块语法与 ST7789 冷屏紫色校准，支持拉伸、压扁、揣手面包猫（Loaf）、飞机耳、炸毛尾、眨眼、挥钳举爪等丰富动作，并附带 **Rainbow Chroma 彩虹猫彩蛋**。
- 🎭 **18 幕剧场动画无重复洗牌轮播**：覆盖日常游乐、4 层叠叠乐高塔、彩虹飞翔、咖啡甜甜圈、激光红点大作战、结对编程、荧光棒打 Call、深夜拉面烤鱼、吹泡泡打盹、冲浪冰晶、盆栽开花、秘密情书、警灯巡逻、吵架和好、热血冲刺、捉迷藏、蓝鲸喷水与星空篝火晚会。
- 💎 **Obsidian Prism 光学玻璃舞台**：烟熏黑曜圆角展台、顶部 `1px` 镜面高光与 `2px` 流动四色光谱 Prism Rim 亮边、二阶阻尼弹簧物理与棱镜扫光转场，支持 **6 套配色方案** 一键热切换。
- ⚡ **队列式 SPI DMA 极速推屏**：40 MHz 硬件 SPI + 4×60 行连续排队 DMA 传输（`Flush ~23.2 ms`），实测全剧场稳定 **30+ FPS**，静态 RAM 仅占 `41.9%`（可用堆内存 `~193 KB`）。
- 🕹️ **单键全功能控制**：仅需板载 BOOT 按键（GPIO 9），即可实现单击切场景、双击换主题色、三击转屏、长按锁定动画。
- 🔌 **完全离线独立运行**：无需 Wi-Fi 与手机配对，上电即演；配备 3D 打印壳体 STL 模型，低成本复刻工位桌面摆件。

---

## 🎭 18 幕双宠动画剧场全览

| 编号 | 剧场 ID | 剧情与动作亮点 |
| :---: | :--- | :--- |
| **01** | `dual_play` | **双宠日常游乐场**（开机首屏）：互相张望眨眼 → 小跑靠近挥爪打招呼 → 弹簧同步高跳欢呼 → 贴贴冒红心 → 揣手洗脸 |
| **02** | `tower_stack` | **叠叠乐高塔**：Clawd 托举 → 小紫猫跃上头顶飞机耳平衡 → 牛仔/墨镜/巫师 3 只迷你蟹依次空降叠成 4 层高塔 |
| **03** | `rainbow_cat` | **彩虹极光小紫猫**：触发隐藏彩虹皮肤在星空中飞翔拖出 6 色光带，Clawd 戴墨镜狂奔欢呼 |
| **04** | `cafe_time` | **午后咖啡与甜甜圈**：Clawd 捧热咖啡惬意微啜，小紫猫大口啃草莓糖霜甜甜圈，头顶冒红心与波浪 |
| **05** | `laser_chase` | **激光红点大作战**：Clawd 发射红色激光点，小紫猫飞机耳瞪大眼左右弹簧飞扑抓不到摔扁冒问号 |
| **06** | `coding_duo` | **结对编程 & 灵光一闪**：Clawd 挥羽毛笔写代码，小紫猫翻书查阅 → 冥思冒问号 → 金色大灯泡叮点亮欢呼 |
| **07** | `concert_dance` | **荧光棒打 Call 演唱会**：舞台光束下围绕麦克风合唱，随后高举绿/粉双色荧光棒随节拍狂跳 |
| **08** | `feast_dinner` | **深夜干饭：拉面与烤鱼**：Clawd 举筷炫拉面，小紫猫啃烤鱼 → 光盘后双双压扁眯眼狂冒红心 |
| **09** | `sleepy_night` | **晚安打盹 & 鼻涕泡**：Clawd 伸大懒腰打哈欠后入睡，小紫猫蜷成揣手面包猫熟睡吹泡泡飘 `Zzz` |
| **10** | `surf_party` | **墨镜冲浪 & 魔法冰晶**：Clawd 戴墨镜踩冲浪板踏光浪，小紫猫施放旋转魔法冰晶，迷你巫师蟹助威 |
| **11** | `garden_bloom` | **盆栽开花 & 蝴蝶飞舞**：种下种子逐步发芽长高绽放花朵，招来蓝蝴蝶翩翩起舞 |
| **12** | `love_letter` | **神秘信封 & 拆信惊喜**：Clawd 送来封口信封，拆开变成爱心信纸展开在空中，羞涩冒红心 |
| **13** | `police_patrol` | **赛博警灯巡逻队**：头顶红蓝交替警灯闪烁扫射舞台，左右巡逻后齐齐立正敬礼 |
| **14** | `quarrel_makeup` | **生气闹别扭 & 贴贴和好**：背对背眯眼气鼓鼓冒井号 → 小紫猫偷偷回头望 → 弹簧蹦到一起贴贴和好 |
| **15** | `goal_sprint` | **热血特训 & 冲刺**：Clawd 绑红头带燃起金色斗志火焰奔跑，小紫猫举怀表沙漏计时加油 |
| **16** | `hide_peek` | **玻璃展台捉迷藏**：小紫猫躲在玻璃展台后探头偷看，突然弹跳出来吓 Clawd 一大跳 |
| **17** | `whale_splash` | **蓝鲸 Deepy 喷水狂欢**：蓝鲸 Deepy 客串在展台中央喷出三阶高水柱，Clawd 与小紫猫欢呼接水花 |
| **18** | `campfire_story` | **星空篝火晚会**：中央燃起跳跃像素篝火与飞舞火星，Clawd 端热茶暖手、小紫猫摇尾听故事 |

---

## 🎨 6 套 Obsidian Prism 光学舞台配色（双击 BOOT 热切换）

| 主题名称 | 暗曜基底 | 主色散光谱 |
| :--- | :--- | :--- |
| **OBSIDIAN**（深空黑曜 · 默认） | `#000002 → #050810` | 电光蓝 `#2091FF` / 鸢尾紫 `#AF5CFF` / 珊瑚红 `#FF446C` / 琥珀金 `#FFCC22` |
| **WARM**（暖炭熔岩） | `#020100 → #0C0705` | 熔岩橙 `#FF602A` / 暖珊瑚 `#FF8E4E` / 亮金 `#FFC066` / 暖白金 `#FFE8A0` |
| **COBALT**（深海电蓝） | `#000206 → #040C18` | 深海蓝 `#1869FF` / 电光青 `#00CDFF` / 冰青 `#60F2FF` / 冰白 `#C3F8FF` |
| **IRIS**（鸢尾极光） | `#020005 → #0A0514` | 紫罗兰 `#8030FF` / 亮鸢尾 `#B960FF` / 霓虹粉紫 `#EB70FF` / 薰衣草 `#F5C6FF` |
| **AMBER**（琥珀熔金） | `#030200 → #0E0903` | 焦糖金 `#EB690C` / 琥珀 `#FFA818` / 亮金 `#FFD84C` / 麦金 `#FFF4AC` |
| **MONO**（钛银晶体） | `#000002 → #080A0F` | 石墨灰 `#7387A5` / 钛银 `#AFC3DC` / 冰银 `#E1ECF8` / 纯白 `#FFFFFF` |

---

## 🎮 单键操作指南 (BOOT · GPIO 9)

| 操作 | 触发方式 | 功能 | 提示反馈 |
| :--- | :--- | :--- | :--- |
| **单击** | 短按 1 次 | 棱镜扫光转场切换下一幕（默认每 15 秒无重复洗牌轮播） | 18 ms 轻滴 |
| **双击** | 380 ms 内连按 2 次 | 热切换 6 套 Obsidian Prism 光学舞台配色 | 25 ms 轻滴 + 底部胶囊提示 |
| **三击** | 380 ms 内连按 3 次 | 屏幕旋转 90°（0° → 90° → 180° → 270° 循环） | 30 ms 轻滴 + 底部胶囊提示 |
| **长按** | 按住 ≥ 800 ms | 锁定当前动画 / 恢复自动轮播 | 45 ms 轻滴 + 底部胶囊提示 |

---

## 🔌 硬件接线表 (Pinout Matrix)

推荐硬件：**AirM2M CORE ESP32-C3** 或 **ESP32-C3 Super Mini** + **ST7789 1.54 寸 240×240 IPS 屏幕（SPI 接口）**。

| 屏幕引脚 (ST7789) | ESP32-C3 引脚 | 说明 |
| :--- | :--- | :--- |
| **SDA / MOSI** | **GPIO 10** | 硬件 SPI 高速数据线 |
| **SCL / SCK** | **GPIO 8** | 硬件 SPI 时钟线（40 MHz 高速通信） |
| **RES / RST** | **GPIO 2** | 硬件复位引脚 |
| **DC** | **GPIO 1** | 数据 / 命令切换引脚 |
| **CS** | **GPIO 4** | SPI 片选引脚 |
| **BLK / BL** | **GPIO 3** | 背光控制引脚（高电平点亮） |
| **VCC** | **3.3V** | ⚠️ **请务必连接 3.3V，切勿接 5V** |
| **GND** | **GND** | 电源共地 |
| **按键 (BOOT)** | **GPIO 9** | 板载 BOOT 按键（单击/双击/三击/长按） |
| **蜂鸣器 (可选)** | **GPIO 6** | 低电平触发蜂鸣器，空闲高阻静音无底噪 |

---

## 🖨️ 3D 打印外壳与硬件清单 (BOM)

### 3D 打印模型
仓库 `models/` 目录中已包含全套 3D 打印 STL / 3MF 模型文件：
- `models/clawd_mochi/`：原版经典 Clawd Mochi 壳体与支架
- `models/clawd_3d/`：优化版 3D 打印壳体
- `models/clawd_3d_squished_eyes/`：挤压大眼趣味版

也可以在 MakerWorld 上一键发送至拓竹 Bambu 打印机：
👉 [MakerWorld 3D 模型直达链接](https://makerworld.com/en/models/2559505-clawd-mochi-physical-claude-code-mascot#profileId-2820000)

### 硬件清单与成本估算
| 配件 | 规格建议 | 参考价格 |
| :--- | :--- | :---: |
| ESP32-C3 核心板 | Super Mini 或 合宙 AirM2M CORE ESP32-C3 | ~¥12 ($1.8) |
| 1.54 寸 IPS 屏幕 | ST7789 驱动，240×240 分辨率，SPI 接口 | ~¥15 ($2.2) |
| 蜂鸣器模块 | 低电平触发有源蜂鸣器（可选） | ~¥2 ($0.3) |
| 杜邦线 / 飞线 | 8~10 cm 细硅胶线 | ~¥2 ($0.3) |
| 3D 打印耗材 | PLA / PETG 约 30g | ~¥3 ($0.5) |
| M2 螺丝 | M2×6mm 螺丝 2 颗（固定屏幕） | ~¥1 ($0.1) |
| **总计** | | **~¥35 ($5~7)** |

---

## 🚀 编译与烧录指南 (Build & Flash)

本工程基于标准的 **PlatformIO + ESP-IDF** 开发环境：

### 方式一：VS Code + PlatformIO 插件（推荐）
1. 在 VS Code 中安装 **PlatformIO IDE** 插件。
2. 打开本项目根目录。
3. 点击底部状态栏 **Build (编译)**，接入开发板后点击 **Upload (烧录)** 即可。

### 方式二：命令行编译与烧录
```bash
# 1. 克隆项目
git clone https://github.com/xukuncai-hub/clawd-prism-mochi.git
cd clawd-prism-mochi

# 2. 编译固件
pio run

# 3. 连接开发板后一键上传
pio run -t upload

# 4. 开启串口监视器 (115200 波特率，实时查看 FPS / 渲染耗时 / 堆内存)
pio device monitor
```

---

## 📂 仓库目录结构

```text
clawd-prism-mochi/
├── CMakeLists.txt                 # ESP-IDF 顶级构建配置
├── platformio.ini                 # PlatformIO 工程环境配置 (airm2m_core_esp32c3)
├── sdkconfig.airm2m_core_esp32c3  # ESP32-C3 核心板配置
├── sdkconfig.defaults             # 默认配置（160MHz 主频、性能优化、主任务栈）
├── src/                           # 固件核心源码
│   ├── main.c                     # 主循环、单键状态机与 18 幕洗牌调度
│   ├── display.c / display.h      # ST7789 40MHz 队列式 SPI DMA 驱动与 Bayer 抖动
│   ├── prism_math.c / prism_math.h# 1024 级 Q12 定点正弦、Value Noise 与弹簧物理
│   ├── prism_gfx.c / prism_gfx.h  # Obsidian Prism 光学引擎、玻璃面板与 6 套配色
│   ├── mascot_art.c / mascot_art.h# Clawd / 小紫猫 / 蓝鲸 / 迷你蟹与道具点阵引擎
│   ├── font.c / font.h            # 8×8 ASCII 点阵字库
│   ├── scene.h                    # 18 幕动画剧场统一接口
│   ├── scenes/                    # 18 个独立剧场动画源码 (scene_*.c)
│   └── CMakeLists.txt             # 源码组件注册
├── models/                        # 3D 打印外壳模型 (STL / 3MF)
├── pics/                          # 项目实物图片与展示素材
├── legacy/                        # 历史版本归档 (Arduino / 早期特性实验)
├── LICENSE                        # MIT 开源协议
└── README.md                      # 项目说明文档
```

---

## 🙏 致谢与协议 (Credits & License)

- 像素双宠动作文法与 Obsidian Prism 光学玻璃展台设计参考了开源社区项目 [RMvdesi/obsidian-prism](https://github.com/RMvdesi/obsidian-prism)。
- 本项目采用 [MIT License](LICENSE) 开源协议，欢迎自由 Fork、制作与二次创作！
