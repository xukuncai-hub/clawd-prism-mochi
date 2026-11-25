#include "display.h"

#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ST7789";
static spi_device_handle_t s_spi = NULL;

static uint8_t s_rotation = 1;
static uint16_t s_xstart = 80;
static uint16_t s_ystart = 0;

static void lcd_cmd(uint8_t cmd) {
    gpio_set_level(TFT_DC, 0);
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    spi_device_polling_transmit(s_spi, &t);
}

static void lcd_data(const uint8_t *data, int len) {
    if (len <= 0) return;
    gpio_set_level(TFT_DC, 1);
    spi_transaction_t t = {
        .length = (size_t)len * 8,
        .tx_buffer = data,
    };
    spi_device_polling_transmit(s_spi, &t);
}

bool display_init(void) {
    ESP_LOGI(TAG, "Initializing ST7789 Display (40MHz Queued SPI DMA)...");

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TFT_DC) | (1ULL << TFT_RST) | (1ULL << TFT_BLK),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    gpio_set_level(TFT_BLK, 0);
    gpio_set_level(TFT_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(TFT_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = TFT_MOSI,
        .sclk_io_num = TFT_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = DISP_W * 60 * 2 + 8,
    };

    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %d", ret);
        return false;
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = TFT_CS,
        .queue_size = 7,
        .flags = SPI_DEVICE_NO_DUMMY,
    };

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &s_spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %d", ret);
        return false;
    }

    // ST7789 寄存器初始化序列
    lcd_cmd(0x01); // SWRESET
    vTaskDelay(pdMS_TO_TICKS(150));

    lcd_cmd(0x11); // SLPOUT
    vTaskDelay(pdMS_TO_TICKS(120));

    lcd_cmd(0x3A); // COLMOD 16-bit RGB565
    const uint8_t colmod = 0x55;
    lcd_data(&colmod, 1);

    lcd_cmd(0x21); // INVON (IPS 屏正色必需)

    display_set_rotation(1);

    lcd_cmd(0x13); // NORON
    vTaskDelay(pdMS_TO_TICKS(10));

    lcd_cmd(0x29); // DISPON
    vTaskDelay(pdMS_TO_TICKS(50));

    display_set_backlight(true);
    ESP_LOGI(TAG, "ST7789 initialized.");
    return true;
}

void display_set_rotation(uint8_t r) {
    s_rotation = r % 4;
    uint8_t madctl = 0;
    switch (s_rotation) {
        case 0:
            madctl = 0x00;
            s_xstart = 0;
            s_ystart = 0;
            break;
        case 1:
            madctl = 0xA0; // MY=1, MV=1 (Clawd Mochi 原生方向)
            s_xstart = 80;
            s_ystart = 0;
            break;
        case 2:
            madctl = 0xC0; // MX=1, MY=1
            s_xstart = 0;
            s_ystart = 80;
            break;
        case 3:
            madctl = 0x60; // MX=1, MV=1
            s_xstart = 0;
            s_ystart = 0;
            break;
    }
    lcd_cmd(0x36);
    lcd_data(&madctl, 1);
}

uint8_t display_get_rotation(void) {
    return s_rotation;
}

void display_set_backlight(bool on) {
    gpio_set_level(TFT_BLK, on ? 1 : 0);
}

void display_flush(const uint16_t *buffer) {
    if (!buffer || !s_spi) return;

    lcd_cmd(0x2A);
    uint16_t x0 = s_xstart;
    uint16_t x1 = s_xstart + DISP_W - 1;
    uint8_t ca[4] = {(uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF), (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF)};
    lcd_data(ca, 4);

    lcd_cmd(0x2B);
    uint16_t y0 = s_ystart;
    uint16_t y1 = s_ystart + DISP_H - 1;
    uint8_t ra[4] = {(uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF), (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF)};
    lcd_data(ra, 4);

    lcd_cmd(0x2C);

    // 4 x 60 行 (每块 28.8KB < ESP32-C3 32KB DMA 限制)，连续排队消除块间空闲间隙
    gpio_set_level(TFT_DC, 1);
    const int lines_per_chunk = 60;
    const size_t chunk_bytes = (size_t)DISP_W * lines_per_chunk * 2;
    const uint8_t *raw_buf = (const uint8_t *)buffer;

    spi_transaction_t trans[4];
    memset(trans, 0, sizeof(trans));

    int queued = 0;
    for (int i = 0; i < 4; i++) {
        trans[i].length = chunk_bytes * 8;
        trans[i].tx_buffer = raw_buf + (size_t)i * chunk_bytes;
        if (spi_device_queue_trans(s_spi, &trans[i], portMAX_DELAY) == ESP_OK) {
            queued++;
        }
    }

    for (int i = 0; i < queued; i++) {
        spi_transaction_t *rtrans = NULL;
        spi_device_get_trans_result(s_spi, &rtrans, portMAX_DELAY);
    }
}
