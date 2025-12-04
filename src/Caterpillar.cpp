#include "Caterpillar.h"

/**
 * @brief Caterpillarクラスのコンストラクタです。
 * 各ピンの初期化とLEDCの設定を行います。
 * @param in1 [in] モーター1制御ピン1
 * @param in2 [in] モーター1制御ピン2
 * @param in3 [in] モーター2制御ピン1
 * @param in4 [in] モーター2制御ピン2
 * @param buzzerPin [in] ブザー制御ピン
 * @param motorChannel1 [in] モーター1用LEDCチャンネル1
 * @param motorChannel2 [in] モーター1用LEDCチャンネル2
 * @param motorChannel3 [in] モーター2用LEDCチャンネル1
 * @param motorChannel4 [in] モーター2用LEDCチャンネル2
 * @param buzzerChannel [in] ブザー用LEDCチャンネル
 * @param whiteLedPin [in] 白色LED用ピン
 * @param blueLedPin [in] 青色LED用ピン
 * @param whiteLedChannel [in] 白色LED用LEDCチャンネル
 * @param blueLedChannel [in] 青色LED用LEDCチャンネル
 */
Caterpillar::Caterpillar(int in1, int in2, int in3, int in4, int buzzerPin,
                         int motorChannel1, int motorChannel2, int motorChannel3, int motorChannel4, int buzzerChannel,
                         int whiteLedPin, int blueLedPin, int whiteLedChannel, int blueLedChannel)
    : _in1(in1), _in2(in2), _in3(in3), _in4(in4),
      _buzzerPin(buzzerPin),
      _batteryPin(BATTERY), // BATTERYピンはPinConfig.hから取得
      _whiteLedPin(whiteLedPin), _blueLedPin(blueLedPin),
      _motorChannel1(motorChannel1), _motorChannel2(motorChannel2),
      _motorChannel3(motorChannel3), _motorChannel4(motorChannel4),
      _buzzerChannel(buzzerChannel),
      _whiteLedChannel(whiteLedChannel), _blueLedChannel(blueLedChannel) {

    // モーター用チャンネル設定
    _setupLedcChannel(_motorChannel1, _in1);
    _setupLedcChannel(_motorChannel2, _in2);
    _setupLedcChannel(_motorChannel3, _in3);
    _setupLedcChannel(_motorChannel4, _in4);

    // ブザー用チャンネル設定
    _setupLedcChannel(_buzzerChannel, _buzzerPin);

    // LED用チャンネル設定
    _setupLedcChannel(_whiteLedChannel, _whiteLedPin);
    _setupLedcChannel(_blueLedChannel, _blueLedPin);

    // バッテリー電圧測定ピンを設定
    pinMode(_batteryPin, INPUT);
}

/**
 * @brief LEDCチャンネルを設定するプライベートヘルパー関数です。
 * @param channel [in] 設定するLEDCチャンネル。
 * @param pin [in] チャンネルに割り当てるピン番号。
 */
void Caterpillar::_setupLedcChannel(int channel, int pin) {
    ledcSetup(channel, LEDC_FREQ, LEDC_RESOLUTION);
    ledcAttachPin(pin, channel);
    ledcWrite(channel, 0); // 初期状態はOFF
}

/**
 * @brief モーター1を前進方向に回転させます。
 * @param speed [in] モーターの速度 (0-255)。
 * @note この実装ではIN1, IN2両方に同じPWM値を出力します。
 */
void Caterpillar::forward1(int speed) {
    ledcWrite(_motorChannel1, speed);
    ledcWrite(_motorChannel2, speed);
}

/**
 * @brief モーター1を後進方向に回転させます。
 * @param speed [in] モーターの速度 (0-255)
 * @note この実装ではIN1をLOW(0)にし、IN2にPWM値を出力します。
 */
void Caterpillar::backward1(int speed) {
    ledcWrite(_motorChannel1, 0);
    ledcWrite(_motorChannel2, speed);
}

/**
 * @brief モーター1を停止させます。
 * 両方の制御ピンへの出力を0にします。
 */
void Caterpillar::stop1() {
    ledcWrite(_motorChannel1, 0);
    ledcWrite(_motorChannel2, 0);
}

/**
 * @brief モーター2を前進方向に回転させます。
 * @param speed [in] モーターの速度 (0-255)。
 * @note モーター1と同様の実装です。
 */
void Caterpillar::forward2(int speed) {
    ledcWrite(_motorChannel3, speed);
    ledcWrite(_motorChannel4, speed);
}

/**
 * @brief モーター2を後進方向に回転させます。
 * @param speed [in] モーターの速度 (0-255)。
 * @note モーター1と同様の実装です。
 */
void Caterpillar::backward2(int speed) {
    ledcWrite(_motorChannel3, 0);
    ledcWrite(_motorChannel4, speed);
}

/**
 * @brief モーター2を停止させます。
 */
void Caterpillar::stop2() {
    ledcWrite(_motorChannel3, 0);
    ledcWrite(_motorChannel4, 0);
}

/**
 * @brief ブザーを指定した周波数で鳴らします。
 * @note 現在は周波数1000Hz固定です。
 */
void Caterpillar::buzzerOn() {
    ledcWriteTone(_buzzerChannel, 1000);
}

/**
 * @brief ブザーを停止します。
 * ブザー用チャンネルへの出力を0にします。
 */
void Caterpillar::buzzerOff() {
    ledcWrite(_buzzerChannel, 0);
}

/**
 * @brief 白色LEDの明るさを設定します。
 * @param brightness [in] 明るさ (0-255)。
 */
void Caterpillar::setWhiteLed(int brightness) {
    ledcWrite(_whiteLedChannel, brightness);
}

/**
 * @brief 青色LEDの明るさを設定します。
 * @param brightness [in] 明るさ (0-255)。
 */
void Caterpillar::setBlueLed(int brightness) {
    ledcWrite(_blueLedChannel, brightness);
}

/**
 * @brief バッテリー電圧をアナログピンから読み取り、計算して返します。
 * @return int 計算されたバッテリー電圧 (mV単位)。
 * @note 電圧計算に使用する抵抗値(R1, R2)は、実際の回路に合わせてください。
 * @note ESP32のADCの特性により、値が不安定な場合があります。必要に応じて平滑化処理を追加してください。
 */
int Caterpillar::getVoltage() const {
    int voltage_value = analogRead(_batteryPin);
    // 分圧抵抗の値 (実際の回路に合わせてください)
    const int R1 = 10000;
    const int R2 = 20000;

    // 電圧を計算します (ESP32のADCは3.3V基準、12bit分解能(0-4095))
    // 計算式: Vout = Vin * R2 / (R1 + R2) -> Vin = Vout * (R1 + R2) / R2
    // Vout = analogRead値 * 3.3 / 4095.0
    // 注意: ESP32のADC基準電圧は内部で変動することがあるため、より正確な測定にはキャリブレーションが必要です。
    double voltage = (voltage_value * 3.3 / 4095.0) * (double)(R1 + R2) / R2;

    return (int)(voltage * 100); // V -> mV 変換
}

/**
 * @brief スライダー値(0-255)をモーター速度(0-255)に変換します。
 * 128を中央値0とし、それより大きい/小さい値に応じて速度を決定します。
 * @param slideVal [in] 変換元のスライダー値 (0-255)。
 * @return int 変換後のモーター速度 (0-255)。
 */
int Caterpillar::transformSlideValue(int slideVal) const {
    // 入力値を0-255の範囲に収めます
    slideVal = constrain(slideVal, 0, 255);

    if (slideVal == 128) {
        return 0;
    } else if (slideVal > 128) {
        int diff = slideVal - 128;
        // 差分を0-255の範囲に線形変換します
        int scaledValue = round((double)diff * 255.0 / 127.0);
        return constrain(scaledValue, 0, 255);
    } else { // slideVal < 128
        int diff = 128 - slideVal;
        int scaledValue = round((double)diff * 255.0 / 128.0);
        return constrain(scaledValue, 0, 255);
    }
}
