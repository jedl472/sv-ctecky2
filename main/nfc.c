#include "nfc.h"

static const char *TAG = "nfc";


typedef struct {
  uint8_t uid[7];
  uint8_t uid_len;
} nfc_event_t;

QueueHandle_t nfcEventQueue;

pn532_io_t pn532_io;
esp_err_t err;





void nfc_control_task(void *args) {
  ESP_LOGI(TAG, "waiting for a tag");

  nfc_event_t nfc_event;

  while (1) {
    err = pn532_read_passive_target_id(&pn532_io, PN532_BRTY_ISO14443A_106KBPS, nfc_event.uid, &(nfc_event.uid_len), 0);

    if (ESP_OK == err)
    {
      // Display some basic information about the card
      ESP_LOGI(TAG, "tag found with UID Length: %d bytes", nfc_event.uid_len);
      ESP_LOGI(TAG, "UID value:");
      ESP_LOG_BUFFER_HEX_LEVEL(TAG, nfc_event.uid, nfc_event.uid_len, ESP_LOG_INFO);
      

      xQueueSend(nfcEventQueue, &nfc_event, 0);


      err = pn532_in_list_passive_target(&pn532_io);
      if (err != ESP_OK) {
        ESP_LOGI(TAG, "Failed to inList passive target");
        continue;
      }

      NTAG2XX_MODEL ntag_model = NTAG2XX_UNKNOWN;
      err = ntag2xx_get_model(&pn532_io, &ntag_model);
      if (err != ESP_OK)
        continue;

      int page_max;
      switch (ntag_model) {
        case NTAG2XX_NTAG213:
          page_max = 45;
          ESP_LOGI(TAG, "found NTAG213 target (or maybe NTAG203)");
          break;

        case NTAG2XX_NTAG215:
          page_max = 135;
          ESP_LOGI(TAG, "found NTAG215 target");
          break;

        case NTAG2XX_NTAG216:
          page_max = 231;
          ESP_LOGI(TAG, "found NTAG216 target");
          break;

        default:
          ESP_LOGI(TAG, "Found unknown NTAG target!");
          continue;
      }

      for(int page=0; page < page_max; page+=4) {
        uint8_t buf[16];
        err = ntag2xx_read_page(&pn532_io, page, buf, 16);
        if (err == ESP_OK) {
          ESP_LOG_BUFFER_HEXDUMP(TAG, buf, 16, ESP_LOG_INFO);
        }
        else {
          ESP_LOGI(TAG, "Failed to read page %d", page);
          break;
        }
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

  }
}




void nfc_setup(void) {
  ESP_LOGI(TAG, "init PN532 in I2C mode");  

  vTaskDelay(1000 / portTICK_PERIOD_MS); // TODO presunout do mainu

  ESP_ERROR_CHECK(pn532_new_driver_i2c(NFC_SDA_PIN, NFC_SCL_PIN, NFC_RESET_PIN, NFC_IRQ_PIN, 0, &pn532_io));

  do {
    err = pn532_init(&pn532_io);
    if (err != ESP_OK) {
      ESP_LOGW(TAG, "failed to initialize PN532");
      pn532_release(&pn532_io);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  } while(err != ESP_OK);

  uint32_t version_data = 0;
  do {
    err = pn532_get_firmware_version(&pn532_io, &version_data);
    if (ESP_OK != err) {
      ESP_LOGI(TAG, "failed to fetch PN53x firmware version"); //original: failed to find PN53x board
      pn532_reset(&pn532_io);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  } while (ESP_OK != err);




  nfcEventQueue = xQueueCreate(10, sizeof(nfc_event_t));

  xTaskCreate(nfc_control_task, "nfc_control_task", 4096, NULL, 1, NULL);
}
