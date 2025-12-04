/* 送信側(ESP32)のメインプログラムです */
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "PinConfig.h"      // ピン定義ファイルをインクルードします
#include "Secret.h"         // MACアドレス定義ファイルをインクルードします
#include "ESPNowManager.h"  // ESPNowManagerクラスをインクルードします
#include "Caterpillar.h"    // Caterpillarクラスをインクルードします
#include "DataStructures.h" // データ構造定義ファイルをインクルードします

// ESPNowManagerクラスのインスタンスを作成します
ESPNowManager espNowManager;

// Caterpillarクラスのインスタンスを作成します (モーター、ブザー、LED制御用)
Caterpillar caterpillar(IN1, IN2, IN3, IN4, BUZZER,
                        motorChannel1, motorChannel2, motorChannel3, motorChannel4, buzzerChannel,
                        WHITE_LED, BLUE_LED, whiteLedChannel, blueLedChannel);

// 通信相手(受信側)のMACアドレスを設定します (Secret.hから読み込み)
uint8_t receiver_mac[] = {MAC_ADDRESS_BYTE[0], MAC_ADDRESS_BYTE[1], MAC_ADDRESS_BYTE[2], MAC_ADDRESS_BYTE[3], MAC_ADDRESS_BYTE[4], MAC_ADDRESS_BYTE[5]};

// ESP-NOWで受信したデータの長さを格納する変数です
int receivedDataLength = 0;

// 受信データを格納する変数です
ReceivedDataPacket receivedData;
// 前回の受信データを比較用に保持する変数です
ReceivedDataPacket beforeReceiveData;
// 送信するデータ用の構造体変数です
SaneDataPacket sendData;

/* --- ESP-NOW コールバック関数 --- */

/**
 * @brief ESP-NOWでデータ受信時に呼び出されるコールバック関数です。
 * @param mac_addr [in] 送信元のMACアドレスです。
 * @param incomingData [in] 受信した生データへのポインタです。
 * @param len [in] 受信したデータの長さ（バイト数）です。
 */
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  if (len == sizeof(ReceivedDataPacket)) {
      memcpy(&receivedData, incomingData, sizeof(ReceivedDataPacket)); // 受信データをコピーします
      receivedDataLength = len;
  } else {
      Serial.printf("Received data size mismatch. Expected: %d, Got: %d\n", sizeof(ReceivedDataPacket), len);
      receivedDataLength = 0;
  }
}

/**
 * @brief ESP-NOWでデータ送信完了時に呼び出されるコールバック関数です。
 * @param mac_addr [in] 送信先のMACアドレスです。
 * @param status [in] 送信結果のステータスです (成功: ESP_NOW_SEND_SUCCESS)。
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // 送信ステータスの確認 (デバッグ用、現在は無効)
  # if 0
  Serial.print("送信ステータス:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  # endif
}

/* --- 初期設定関数 --- */

/**
 * @brief プログラム起動時に一度だけ実行される初期設定関数です。
 * シリアル通信、Wi-Fi、ESP-NOWの初期化とペアリングを行います。
 */
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("setup start");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("My MAC address: ");
  Serial.println(WiFi.macAddress());

  Serial.printf("Send MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
    receiver_mac[0], receiver_mac[1], receiver_mac[2],
    receiver_mac[3], receiver_mac[4], receiver_mac[5]);

  // ESP-NOWを初期化します
  if (espNowManager.init()) {
      // 送受信コールバック関数を登録します
      esp_now_register_send_cb(OnDataSent);
      esp_now_register_recv_cb(OnDataRecv);
      espNowManager.pairDevice(receiver_mac);
      // ペアリング状態に応じてLEDを制御
      if (espNowManager.isPaired) {
        caterpillar.setBlueLed(255); // ペアリング成功で点灯
      } else {
        caterpillar.setBlueLed(0); // ペアリング失敗で消灯
      }
  } else {
      Serial.println("ESP-NOW initialization failed!");
      caterpillar.setBlueLed(0);
  }
  Serial.println("setup finish");
}

/* --- グローバル変数 --- */
// loop内の処理を実行する間隔 (ミリ秒) です
const int LOOP_INTERVAL_MS = 20;
// 通信ロスと判断するしきい値
const int COMMUNICATION_LOST_THRESHOLD = 10; // 10回 * 20ms = 200ms程度受信がなければロスと判断

/* --- メインループ関数 --- */

/**
 * @brief setup()完了後、繰り返し実行されるメインループ関数です。
 * 一定間隔でバッテリー電圧の取得、ESP-NOWでのデータ送受信、
 * 受信データに基づくモーターとブザーの制御を行います。
 */
void loop() {
  static unsigned long previousMillis = 0; // loop内の処理間隔を管理するためのタイマー変数です
  static int firstStepBuzzer = 0;          // ブザー制御の初回ステップフラグです
  static int lostCount = 0;                // ESP-NOWの通信ロス回数をカウントする変数です

  unsigned long currentMillis = millis(); // 現在時刻を取得します

  // 一定時間ごとに処理を実行します
  if (currentMillis - previousMillis >= LOOP_INTERVAL_MS) {
    previousMillis = currentMillis; // 実行時刻を更新します

    int battery_value = caterpillar.getVoltage();
    // Serial.print("Battery: "); Serial.print(battery_value); Serial.println(" mV");
    if (battery_value < 330) { // 330mV = 3.3V
      // バッテリー電圧が3.3V未満の場合、白色LEDを点灯して警告します
      caterpillar.setWhiteLed(255); // 白色LEDを最大輝度で点灯
    } else {
      caterpillar.setWhiteLed(0); // 白色LEDを消灯
    }

    /* ↓ここからメイン処理です↓ */
    // ESP-NOWでペアリング済みか確認します
    if (espNowManager.isPaired) {
      caterpillar.setBlueLed(255); // ペアリング中は点灯
      // 送信データを設定します (現在は固定値、必要に応じて変更してください)
      sendData.val1 = battery_value;
      sendData.val2 = 2; sendData.val3 = 3; sendData.val4 = 4; sendData.val5 = 5;
      // データを送信します
      esp_err_t result = esp_now_send(receiver_mac, (uint8_t *)&sendData, sizeof(sendData));
      // 送信結果を確認します (エラー時のみ表示)
      if (result != ESP_OK) {
        Serial.print("Send Error: ");
        Serial.println(result);
      }

      // 今回受信したデータを次回比較用に保存します (現在は未使用)
      beforeReceiveData = receivedData;
      
      int rawSlideVal_1 = receivedData.slideVal1;
      int transformedSpeed_1 = caterpillar.transformSlideValue(rawSlideVal_1);
      int rawSlideVal_2 = receivedData.slideVal2;
      int transformedSpeed_2 = caterpillar.transformSlideValue(rawSlideVal_2);

      /* モーター1の制御 (スライダー1の値に基づく) */
      // 128以上なら前進方向、未満なら後進方向に制御します
      Serial.print("Slide1: "); Serial.print(rawSlideVal_1); Serial.print(" -> Speed1: "); Serial.print(transformedSpeed_1);
      Serial.print(" | Slide2: "); Serial.print(rawSlideVal_2); Serial.print(" -> Speed2: "); Serial.println(transformedSpeed_2);
      if (rawSlideVal_1 >= 128) {
        caterpillar.forward1(transformedSpeed_1);
      } else {
        caterpillar.backward1(transformedSpeed_1);
      }

      /* モーター2の制御 (スライダー2の値に基づく) */
      if (rawSlideVal_2 >= 128) {
        caterpillar.forward2(transformedSpeed_2);
      } else {
        caterpillar.backward2(transformedSpeed_2);
      }

      /* ブザーの制御 (SW1の状態に基づく) */
      // SW1が押されたら(0)ブザーを鳴らし、離されたら(1)止めます
      if (receivedData.sw1 == 0) {
        if (firstStepBuzzer == 0) {
          caterpillar.buzzerOn();
          firstStepBuzzer = 1;
        }
      } else { // receivedData.sw1 == 1
        caterpillar.buzzerOff();
        firstStepBuzzer = 0;
      }

      // データを受信したか確認します (ペアリング中のみ実行)
      if (receivedDataLength > 0) {
        # if 0
        // Serial.print("Recv Data: ");
        // Serial.print(receivedData.slideVal1);Serial.print(" ");
        // Serial.print(receivedData.slideVal2);Serial.print(" ");
        // ... (他のデータも同様) ...
        // Serial.println(receivedData.sw8);
        # endif
        receivedDataLength = 0; // 受信データ長をリセットします
        lostCount = 0;          // 通信ロス回数カウントをリセットします
      } else {
        lostCount++;
        if (lostCount > COMMUNICATION_LOST_THRESHOLD) { // 10回 * 20ms = 200ms程度受信がなければロスと判断
          // ペアリングされていない場合の処理です (ブリージングエフェクト)
          // 2000ms (2秒)周期で明るさを計算します
          float rad = (millis() % 2000) / 2000.0 * 2.0 * PI;
          // sinカーブを使い、0-255の範囲で滑らかな明るさの変化を生成します
          int brightness = (int)((sin(rad - PI / 2.0) + 1.0) / 2.0 * 255);
          caterpillar.setBlueLed(brightness);
          caterpillar.stop1();
          caterpillar.stop2();
          caterpillar.buzzerOff();
        }
      }
    } else {
      // 安全のためモーターとブザーを停止します
      caterpillar.stop1();
      caterpillar.stop2();
      caterpillar.buzzerOff();
      caterpillar.setBlueLed(0); // ペアリングが切れたら青LEDを消灯
    }
  }
}
