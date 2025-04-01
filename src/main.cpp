/* 送信側(ESP32)のメインプログラムです */
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
// 必要なライブラリと自作クラスをインクルードします
#include "Secret.h"         // MACアドレス定義ファイルをインクルードします
#include "ESPNowManager.h"  // ESPNowManagerクラスをインクルードします
#include "Caterpillar.h"    // Caterpillarクラスをインクルードします

// ESPNowManagerクラスのインスタンスを作成します
ESPNowManager espNowManager;

// --- GPIOピン定義 ---
const int IN1 = 25;         // モーター1用ピン
const int IN2 = 26;         // モーター1用ピン
const int IN3 = 16;         // モーター2用ピン
const int IN4 = 15;         // モーター2用ピン
const int BUZZER = 27;      // ブザー用ピン
const int BATTERY = 35;     // バッテリー電圧測定用ピン (アナログ入力)
const int WHITE_LED = 17;   // 白色LED用ピン (現在は未使用かもしれません)
const int BLUE_LED = 18;    // 青色LED用ピン (現在は未使用かもしれません)

// --- LEDCチャンネル定義 (PWM制御用) ---
const int motorChannel1 = 0; // モーター1用LEDCチャンネル
const int motorChannel2 = 1; // モーター1用LEDCチャンネル
const int motorChannel3 = 2; // モーター2用LEDCチャンネル
const int motorChannel4 = 3; // モーター2用LEDCチャンネル
const int buzzerChannel = 4; // ブザー用LEDCチャンネル
const int redLedChannel = 5; // 赤色LED用LEDCチャンネル (未使用)
const int blueLedChannel = 6;// 青色LED用LEDCチャンネル (未使用)

/* --- 関数のプロトタイプ宣言です --- */
int getVoltage();                       // バッテリー電圧を取得する関数です
void initializePins();                  // ピンモードを設定する関数です (現在は未使用かもしれません)
int transformSlideValue(int slideVal);  // スライダー値をモーター速度に変換する関数です

// 通信相手(受信側)のMACアドレスを設定します (Secret.hから読み込み)
uint8_t receiver_mac[] = {MAC_ADDRESS_BYTE[0], MAC_ADDRESS_BYTE[1], MAC_ADDRESS_BYTE[2], MAC_ADDRESS_BYTE[3], MAC_ADDRESS_BYTE[4], MAC_ADDRESS_BYTE[5]};

// ESP-NOWで受信したデータの長さを格納する変数です
int receivedDataLength = 0;

// --- ESP-NOW データ構造体定義 ---
// 受信するデータの構造体です
struct ReceivedDataPacket {
  int slideVal1; int slideVal2;       // スライダー1, 2の値 (0-255)
  int sld_sw1_1; int sld_sw1_2;     // スライドスイッチ1の状態
  int sld_sw2_1; int sld_sw2_2;     // スライドスイッチ2の状態
  int sld_sw3_1; int sld_sw3_2;     // スライドスイッチ3の状態
  int sld_sw4_1; int sld_sw4_2;     // スライドスイッチ4の状態
  int sw1; int sw2; int sw3; int sw4; // ボタンスイッチ1-4の状態
  int sw5; int sw6; int sw7; int sw8; // ボタンスイッチ5-8の状態
};
// 受信データを格納する変数です
ReceivedDataPacket receivedData;
// 前回の受信データを比較用に保持する変数です
ReceivedDataPacket beforeReceiveData;

// 送信するデータの構造体です (必要に応じて変更してください)
struct SaneDataPacket {
  int val1; int val2; int val3; int val4; int val5; // 送信データ例
};

/* --- ESP-NOW コールバック関数 --- */

/**
 * @brief ESP-NOWでデータ受信時に呼び出されるコールバック関数です。
 * @param mac_addr [in] 送信元のMACアドレスです。
 * @param incomingData [in] 受信した生データへのポインタです。
 * @param len [in] 受信したデータの長さ（バイト数）です。
 */
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // 受信データサイズが期待通りか確認する方がより安全です
  if (len == sizeof(ReceivedDataPacket)) {
      memcpy(&receivedData, incomingData, sizeof(ReceivedDataPacket)); // 受信データをコピーします
      receivedDataLength = len; // 受信データ長を保存します
  } else {
      Serial.printf("Received data size mismatch. Expected: %d, Got: %d\n", sizeof(ReceivedDataPacket), len);
      receivedDataLength = 0; // サイズが違う場合は無効なデータとして扱う
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
  Serial.begin(115200); // シリアル通信を開始します (ボーレート: 115200)
  delay(1000);          // シリアル通信が安定するまで少し待ちます
  Serial.println("setup start");

  WiFi.mode(WIFI_STA);  // Wi-Fiをステーションモードに設定します
  WiFi.disconnect();    // 既存のWi-Fi接続を切断します
  delay(100);

  // 自身のMACアドレスをシリアルモニタに表示します (デバッグ用)
  Serial.print("My MAC address: ");
  Serial.println(WiFi.macAddress());

  // 送信先のMACアドレスをシリアルモニタに表示します
  Serial.printf("Send MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
    receiver_mac[0], receiver_mac[1], receiver_mac[2],
    receiver_mac[3], receiver_mac[4], receiver_mac[5]);

  // ESP-NOWを初期化します
  if (espNowManager.init()) {
      // 送受信コールバック関数を登録します
      esp_now_register_send_cb(OnDataSent);
      esp_now_register_recv_cb(OnDataRecv);
      // 通信相手とペアリングします
      espNowManager.pairDevice(receiver_mac);
  } else {
      Serial.println("ESP-NOW initialization failed!");
      // ESP-NOW初期化失敗時の処理を追加することも検討できます (例: LED点滅など)
  }
  Serial.println("setup finish");
}

/* --- グローバル変数 --- */
// Caterpillarクラスのインスタンスを作成します (モーター、ブザー制御用)
Caterpillar caterpillar(IN1, IN2, IN3, IN4, BUZZER, motorChannel1, motorChannel2, motorChannel3, motorChannel4, buzzerChannel);
// 送信するデータ用の構造体変数です
SaneDataPacket sendData;
// バッテリー電圧の測定値を格納する変数です
int battery_value = 0;
// loop内の処理間隔を管理するためのタイマー変数です
unsigned long previousMillis = 0;
// loop内の処理を実行する間隔 (ミリ秒) です
const int interval_1 = 20;
// スイッチカウント用の変数です (現在は未使用かもしれません)
int switchCount = -1;
// ブザー制御の初回ステップフラグです
int firstStep = 0;
// ESP-NOWの通信ロス回数をカウントする変数です
int lostCount = 0;
// スライダー1の生の値と変換後の速度です
int rawSlideVal_1 = 0;
int transformedSpeed_1 = 0;
// スライダー2の生の値と変換後の速度です
int rawSlideVal_2 = 0;
int transformedSpeed_2 = 0;

// ---------------------------------------------------------------------------------------------

/* --- メインループ関数 --- */

/**
 * @brief setup()完了後、繰り返し実行されるメインループ関数です。
 * 一定間隔でバッテリー電圧の取得、ESP-NOWでのデータ送受信、
 * 受信データに基づくモーターとブザーの制御を行います。
 */
void loop() {
  unsigned long currentMillis = millis(); // 現在時刻を取得します

  // 一定時間ごとに処理を実行します
  if (currentMillis - previousMillis >= interval_1) {
    previousMillis = currentMillis; // 実行時刻を更新します
    battery_value = getVoltage();   // バッテリー電圧を取得します
    // Serial.print(battery_value); // 電圧値のデバッグ表示 (現在は無効)

    /* ↓ここからメイン処理です↓ */
    // ESP-NOWでペアリング済みか確認します
    if (espNowManager.isPaired) {
      // 送信データを設定します (現在は固定値、必要に応じて変更してください)
      sendData.val1 = battery_value; // 例: バッテリー電圧を送る
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
      // スライダー1, 2の値を取得し、速度に変換します
      rawSlideVal_1 = receivedData.slideVal1;
      transformedSpeed_1 = transformSlideValue(rawSlideVal_1);
      rawSlideVal_2 = receivedData.slideVal2;
      transformedSpeed_2 = transformSlideValue(rawSlideVal_2);

      /* モーター1の制御 (スライダー1の値に基づく) */
      // 128以上なら前進方向、未満なら後進方向に制御します (128は速度0で停止)
      if (rawSlideVal_1 >= 128) {
        caterpillar.forward1(transformedSpeed_1);
      } else {
        caterpillar.backward1(transformedSpeed_1);
      }

      /* モーター2の制御 (スライダー2の値に基づく) */
      // 128以上なら前進方向、未満なら後進方向に制御します (128は速度0で停止)
      if (rawSlideVal_2 >= 128) {
        caterpillar.forward2(transformedSpeed_2);
      } else {
        caterpillar.backward2(transformedSpeed_2);
      }

      /* ブザーの制御 (SW1の状態に基づく) */
      // SW1が押されたら(0)ブザーを鳴らし、離されたら(1)止めます
      if (receivedData.sw1 == 0) {
        if (firstStep == 0) { // 最初の1回は鳴らさない (チャタリング対策？)
          caterpillar.buzzerOff();
          firstStep = 1;
        } else if (firstStep == 1) { // 2回目以降は鳴らす
          caterpillar.buzzerOn();
        }
      } else if (receivedData.sw1 == 1) { // スイッチが離されたら止める
        caterpillar.buzzerOff();
        firstStep = 0; // フラグをリセット
      }
    } else {
      // ペアリングされていない場合の処理です
      Serial.println("Not paired. Cannot send data.");
      // 安全のためモーターとブザーを停止します
      caterpillar.stop1();
      caterpillar.stop2();
      caterpillar.buzzerOff();
      // 再ペアリングを試みる処理などを追加することも検討できます
      // espNowManager.pairDevice(receiver_mac);
    }

    // データを受信したか確認します
    if (receivedDataLength > 0) {
      // デバッグ用: 受信データをシリアル表示します (現在は無効)
      # if 0
      Serial.print("Recv Data: ");
      Serial.print(receivedData.slideVal1);Serial.print(" ");
      Serial.print(receivedData.slideVal2);Serial.print(" ");
      // ... (他のデータも同様) ...
      Serial.println(receivedData.sw8);
      # endif
      receivedDataLength = 0; // 受信データ長をリセットします
      lostCount = 0;          // 通信ロス回数カウントをリセットします
    } else {
      // データを受信しなかった場合、通信ロスカウントを増やします
      lostCount++;
      // 一定時間データ受信がない場合 (通信ロス) の処理です
      if (lostCount > 10) { // 10回 * 20ms = 200ms程度受信がなければロスと判断
        Serial.println("Communication lost!");
        // 安全のためモーターとブザーを停止します
        caterpillar.stop1();
        caterpillar.stop2();
        caterpillar.buzzerOff();
        // lostCountが大きくなりすぎないようにリセットしても良いかもしれません
        // lostCount = 11; // or lostCount = 0;
      }
    }
  } // end of if (currentMillis - previousMillis >= interval_1)
} // end of loop()

/* --- 関数定義 --- */

/**
 * @brief LEDピンを初期化します。
 * @note 現在この関数はsetup()内で呼び出されていません。
 * @note WHITE_LEDに割り当てられているピン番号(17)とLEDCチャンネル(redLedChannel=5)が一致していません。
 */
void initializeLedPins() {
  const int freq = 5000;
  const int resolution = 8;
  // 赤色LEDチャンネルの設定 (ピン17を制御しようとしている？)
  ledcSetup(redLedChannel, freq, resolution);
  ledcAttachPin(WHITE_LED, redLedChannel); // ピン17をチャンネル5に接続
  // 青色LEDチャンネルの設定
  ledcSetup(blueLedChannel, freq, resolution);
  ledcAttachPin(BLUE_LED, blueLedChannel); // ピン18をチャンネル6に接続
}

/**
 * @brief バッテリー電圧をアナログピンから読み取り、計算して返します。
 * @return int 計算されたバッテリー電圧 (実際の電圧の100倍の値、例: 7.4V -> 740)。
 * @note 電圧計算に使用する抵抗値(R1, R2)は、実際の回路に合わせてください。
 * @note ESP32のADCの特性により、値が不安定な場合があります。必要に応じて平滑化処理を追加してください。
 */
int getVoltage() {
  int voltage_value = analogRead(BATTERY); // アナログピンから値を読み取ります
  // 分圧抵抗の値 (実際の回路に合わせてください)
  const int R1 = 10000;
  const int R2 = 10000;

  // 電圧を計算します (ESP32のADCは3.3V基準、12bit分解能(0-4095))
  // 計算式: Vout = Vin * R2 / (R1 + R2) -> Vin = Vout * (R1 + R2) / R2
  // Vout = analogRead値 * 3.3 / 4095.0
  // 注意: ESP32のADC基準電圧は内部で変動することがあるため、より正確な測定にはキャリブレーションが必要です。
  double voltage = (voltage_value * 3.3 / 4095.0) * (double)(R1 + R2) / R2;

  // 結果を100倍して整数で返します (例: 7.4V -> 740)
  return (int)(voltage * 100);
}

/**
 * @brief スライダー値(0-255)をモーター速度(0-255)に変換します。
 * 128を中央値0とし、それより大きい/小さい値に応じて速度を決定します。
 * @param slideVal [in] 変換元のスライダー値 (0-255)。
 * @return int 変換後のモーター速度 (0-255)。
 */
int transformSlideValue(int slideVal) {
  // 入力値を0-255の範囲に収めます
  slideVal = constrain(slideVal, 0, 255);

  if (slideVal == 128) {
    // 中央値(128)の場合は速度0を返します
    return 0;
  } else if (slideVal > 128) {
    // 128より大きい場合 (前進方向)
    // 差分 (1から127) を計算します
    int diff = slideVal - 128;
    // 差分を0-255の範囲に線形変換します
    // (diff / 127.0) は 0 < diff/127 <= 1 の範囲
    int scaledValue = round((double)diff * 255.0 / 127.0);
    // 変換後の値が範囲外にならないようにします
    return constrain(scaledValue, 0, 255);
  } else { // slideVal < 128
    // 128より小さい場合 (後進方向)
    // 128からの差分 (1から128) を計算します
    int diff = 128 - slideVal;
    // 差分を0-255の範囲に線形変換します (値が大きいほど高速になるように)
    // (diff / 128.0) は 0 < diff/128 <= 1 の範囲
    int scaledValue = round((double)diff * 255.0 / 128.0);
     // 変換後の値が範囲外にならないようにします
    return constrain(scaledValue, 0, 255);
  }
}
