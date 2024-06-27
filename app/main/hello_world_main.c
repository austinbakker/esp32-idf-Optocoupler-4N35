/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "driver/gpio.h"

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#define GPIO_OUTPUT_PIN_SEL 3
#include "esp_tls.h"
#include "esp_wifi.h"
#include "nvs_flash.h" //non volatile storage
int retry_num = 0;
const char *ssid = "vameqb-gn";
const char *pass = "2807dc2807";

bool isConnected = false;
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  if (event_id == WIFI_EVENT_STA_START)
  {
    printf("WIFI CONNECTING....\n");
  }
  else if (event_id == WIFI_EVENT_STA_CONNECTED)
  {
    printf("WiFi CONNECTED\n");
    isConnected = true;
  }
  else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    isConnected = false;
    printf("WiFi lost connection\n");
    if (retry_num < 5)
    {
      esp_wifi_connect();
      retry_num++;
      printf("Retrying to Connect...\n");
    }
  }
  else if (event_id == IP_EVENT_STA_GOT_IP)
  {
    printf("Wifi got IP...\n\n");
  }
}

void wifi_connection()
{
  //                          s1.4
  // 2 - Wi-Fi Configuration Phase
  esp_netif_init();
  esp_event_loop_create_default();     // event loop                    s1.2
  esp_netif_create_default_wifi_sta(); // WiFi station                      s1.3
  wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_initiation); //
  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
  wifi_config_t wifi_configuration = {
      .sta = {
          .ssid = "",
          .password = "",

      }

  };
  strcpy((char *)wifi_configuration.sta.ssid, ssid);
  strcpy((char *)wifi_configuration.sta.password, pass);
  // esp_log_write(ESP_LOG_INFO, "Kconfig", "SSID=%s, PASS=%s", ssid, pass);
  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
  // 3 - Wi-Fi Start Phase
  esp_wifi_start();
  esp_wifi_set_mode(WIFI_MODE_STA);
  // 4- Wi-Fi Connect Phase
  esp_wifi_connect();
  printf("wifi_init_softap finished. SSID:%s  password:%s\n", ssid, pass);
}
void app_main(void)
{
  printf("Hello world!\n");

  /* Print chip information */
  esp_chip_info_t chip_info;
  uint32_t flash_size;
  esp_chip_info(&chip_info);
  printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
         CONFIG_IDF_TARGET,
         chip_info.cores,
         (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
         (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
         (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

  unsigned major_rev = chip_info.revision / 100;
  unsigned minor_rev = chip_info.revision % 100;
  printf("silicon revision v%d.%d, ", major_rev, minor_rev);
  if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
  {
    printf("Get flash size failed");
    return;
  }

  printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

  printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

  // for (int i = 10; i >= 0; i--) {
  // printf("Restarting in %d seconds...\n", i);
  // }
  // printf("Restarting now.\n");
  // fflush(stdout);
  // esp_restart();

  vTaskDelay(5000 / portTICK_PERIOD_MS);

  int cnt = 0;
  // ESP_ERROR_CHECK(esp_netif_init());
  // ESP_ERROR_CHECK(esp_event_loop_create_default());
  // wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT(); // sets up wifi wifi_init_config struct with default values
  // esp_wifi_init(&wifi_initiation);                                 // wifi initialised with dafault wifi_initiation
  // esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
  // esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
  // wifi_config_t config = {
  //     .ap = {
  //         .ssid = "vameqb-gn",
  //         .password = "2807dc2807",
  //     }};

  // esp_wifi_start();
  // esp_wifi_connect(); // connect with saved ssid and pass
  // c // start connection with configurations provided in funtion
  // esp_wifi_init(&config);
  // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  // ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &config));
  // ESP_ERROR_CHECK(esp_wifi_start());
  nvs_flash_init(); // this
  wifi_connection();
  while (1)
  {
    if (isConnected)
    {

      printf("cnt: %d\n", cnt++);
      // zero-initialize the config structure.
      gpio_config_t io_conf = {};
      // disable interrupt
      io_conf.intr_type = GPIO_INTR_DISABLE;
      // set as output mode
      io_conf.mode = GPIO_MODE_OUTPUT;
      // bit mask of the pins that you want to set,e.g.GPIO18/19
      io_conf.pin_bit_mask = GPIO_NUM_3;
      // disable pull-down mode
      io_conf.pull_down_en = 0;
      // disable pull-up mode
      io_conf.pull_up_en = 0;
      // configure GPIO with the given settings
      gpio_config(&io_conf);
      gpio_set_direction(GPIO_NUM_3, GPIO_MODE_OUTPUT);
      gpio_set_level(GPIO_NUM_3, cnt % 2);

      //
      // https://setstate-s4p47tqn7a-uc.a.run.app

      esp_http_client_config_t config = {
          .url = "https://getstate-s4p47tqn7a-uc.a.run.app",
          // .url = "http://google.com",
      };
      esp_http_client_handle_t client = esp_http_client_init(&config);

      // GET
      esp_err_t err = esp_http_client_perform(client);
      if (err == ESP_OK)
      {
        ESP_LOGI("TAG", "HTTP GET Status = %d, content_length = %" PRId64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        char buffer[1024] = {0};
        int data_read = esp_http_client_read_response(client, buffer, 1024);
        if (data_read >= 0)
        {
          printf("buffer data %s \n", buffer);
          ESP_LOG_BUFFER_HEX("TAG", buffer, data_read);
        }
      }
      else
      {
        ESP_LOGE("TAG", "HTTP GET request failed: %s", esp_err_to_name(err));
      }
      esp_http_client_close(client);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
