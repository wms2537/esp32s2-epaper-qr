#include "epaper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "QrCode.hpp"

#define COLORED 0
#define UNCOLORED 1

extern "C"
{

	static void printQr(qrcodegen::QrCode qr, Paint* paint_, Epd* epd, uint8_t* frame_)
	{
    int size = qr.getSize();
    int border = 4;
    int multiply = (200 - border * 2)/size;
    for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
        int x_i = (x * multiply) + border;
        int y_i = (y * multiply) + border;
        for (int x_ii = x_i; x_ii < x_i + multiply; x_ii++)
        {
          for (int y_ii = y_i; y_ii < y_i + multiply; y_ii++)
          {
            paint_->DrawPixel(x_ii, y_ii, qr.getModule(x, y) ? COLORED : UNCOLORED);
          }
        }
        
      }
		}
		epd->Display(frame_);
	}

	void app_main()
	{
    // gpio_reset_pin(GPIO_NUM_18);
    // /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_INPUT_OUTPUT);
    if(gpio_set_level(GPIO_NUM_18, 0)!=ESP_OK) {
      ESP_LOGI("EPD", "GPIO failed to init");
    }
    ESP_LOGI("EPD", "IO init and clear");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Epd epd;
    unsigned char *frame_ = (unsigned char *)malloc(epd.width * epd.height / 8);
    Paint paint_(frame_, epd.width, epd.height);

    paint_.Clear(UNCOLORED);
    paint_.SetRotate(2);
		ESP_LOGI("EPD", "e-Paper init and clear");
		epd.LDirInit();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    const char *text = "Hello, world!";             // User-supplied text
    const qrcodegen::QrCode::Ecc errCorLvl = qrcodegen::QrCode::Ecc::ECC_LOW; // Error correction level

    // Make and print the QR Code symbol
    const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(text, errCorLvl);
    ESP_LOGI("qr", "size: %d", qr.getSize());
    printQr(qr, &paint_, &epd, frame_);

    // if (ok)
		// 	printQr(qrcode, &paint_, &epd, frame_);
		epd.Sleep();
    ESP_LOGI("qr", "end");
    // gpio_set_level(GPIO_NUM_18, 0);
  }
}