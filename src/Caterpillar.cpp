#include "Caterpillar.h"

/**
 * @brief Caterpillarクラスのコンストラクタです。
 * 各ピンの初期化とLEDCの設定を行います。
 * @param in1 [in] モーター1制御ピン1 (例: IN1)。
 * @param in2 [in] モーター1制御ピン2 (例: IN2)。
 * @param in3 [in] モーター2制御ピン1 (例: IN3)。
 * @param in4 [in] モーター2制御ピン2 (例: IN4)。
 * @param buzzer [in] ブザー制御ピン。
 * @param motorChannel1 [in] モーター1用LEDCチャンネル1。
 * @param motorChannel2 [in] モーター1用LEDCチャンネル2。
 * @param motorChannel3 [in] モーター2用LEDCチャンネル1。
 * @param motorChannel4 [in] モーター2用LEDCチャンネル2。
 * @param buzzerChannel [in] ブザー用LEDCチャンネル。
 */
Caterpillar::Caterpillar(int in1, int in2, int in3, int in4, int buzzer, int motorChannel1, int motorChannel2, int motorChannel3, int motorChannel4, int buzzerChannel)
    : IN1(in1), IN2(in2), IN3(in3), IN4(in4), BUZZER(buzzer), motorChannel1(motorChannel1), motorChannel2(motorChannel2), motorChannel3(motorChannel3), motorChannel4(motorChannel4), buzzerChannel(buzzerChannel) {

    // LEDC (PWM) の設定を行います
    const int freq = 5000; // PWM周波数 (5kHz)
    const int resolution = 8; // PWM分解能 (8bit = 0-255)

    // モーター1用チャンネル設定
    ledcSetup(motorChannel1, freq, resolution); // チャンネルを設定
    ledcAttachPin(IN1, motorChannel1);          // IN1ピンをチャンネルに接続
    ledcSetup(motorChannel2, freq, resolution);
    ledcAttachPin(IN2, motorChannel2);

    // モーター2用チャンネル設定
    ledcSetup(motorChannel3, freq, resolution);
    ledcAttachPin(IN3, motorChannel3);
    ledcSetup(motorChannel4, freq, resolution);
    ledcAttachPin(IN4, motorChannel4);

    // ブザー用チャンネル設定
    ledcSetup(buzzerChannel, freq, resolution);
    ledcAttachPin(BUZZER, buzzerChannel);
}

/**
 * @brief モーター1を前進方向に回転させます。
 * @param forward_int1 [in] モーターの速度 (0-255)。
 * @note この実装ではIN1, IN2両方に同じPWM値を出力します。
 */
void Caterpillar::forward1(int forward_int1) {
    ledcWrite(motorChannel1, forward_int1);
    ledcWrite(motorChannel2, forward_int1);
}

/**
 * @brief モーター1を後進方向に回転させます。
 * @param backward_int1 [in] モーターの速度 (0-255)
 * @note この実装ではIN1をLOW(0)にし、IN2にPWM値を出力します。
 */
void Caterpillar::backward1(int backward_int1) {
    ledcWrite(motorChannel1, 0);
    ledcWrite(motorChannel2, backward_int1);
}

/**
 * @brief モーター1を停止させます。
 * 両方の制御ピンへの出力を0にします。
 */
void Caterpillar::stop1() {
    ledcWrite(motorChannel1, 0);
    ledcWrite(motorChannel2, 0);
}

/**
 * @brief モーター2を前進方向に回転させます。
 * @param forward_int2 [in] モーターの速度 (0-255)。
 * @note モーター1と同様の実装です。
 */
void Caterpillar::forward2(int forward_int2) {
    ledcWrite(motorChannel3, forward_int2);
    ledcWrite(motorChannel4, forward_int2);
}

/**
 * @brief モーター2を後進方向に回転させます。
 * @param backward_int2 [in] モーターの速度 (0-255)。
 * @note モーター1と同様の実装です。
 */
void Caterpillar::backward2(int backward_int2) {
    ledcWrite(motorChannel3, 0);
    ledcWrite(motorChannel4, backward_int2); 
}

/**
 * @brief モーター2を停止させます。
 */
void Caterpillar::stop2() {
    ledcWrite(motorChannel3, 0);
    ledcWrite(motorChannel4, 0);
}

/**
 * @brief ブザーを指定した周波数で鳴らします。
 * @note 現在は周波数1000Hz固定です。
 */
void Caterpillar::buzzerOn() {
    ledcWriteTone(buzzerChannel, 1000);
}

/**
 * @brief ブザーを停止します。
 * ブザー用チャンネルへの出力を0にします。
 */
void Caterpillar::buzzerOff() {
    ledcWrite(buzzerChannel, 0);
}
