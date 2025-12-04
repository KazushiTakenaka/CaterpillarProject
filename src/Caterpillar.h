#ifndef CATERPILLAR_H
#define CATERPILLAR_H

#include <Arduino.h>
#include "PinConfig.h" // ピン設定をインクルード

/**
 * @brief キャタピラ(モーター2系統)、ブザー、LEDの制御を行うクラスです。
 * LEDCライブラリを使用してPWM制御を行います。
 */
class Caterpillar {
public:
    // --- LEDC設定定数 ---
    static const int LEDC_FREQ = 5000;      // PWM周波数
    static const int LEDC_RESOLUTION = 8;   // PWM分解能 (8bit = 0-255)

    /**
     * @brief Caterpillarクラスのコンストラクタです。
     * 各ピンの初期化とLEDCの設定を行います。
     * @param in1 [in] モーター1制御ピン1 (例: IN1)
     * @param in2 [in] モーター1制御ピン2 (例: IN2)
     * @param in3 [in] モーター2制御ピン1 (例: IN3)
     * @param in4 [in] モーター2制御ピン2 (例: IN4)
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
    Caterpillar(int in1, int in2, int in3, int in4, int buzzerPin,
                int motorChannel1, int motorChannel2, int motorChannel3, int motorChannel4, int buzzerChannel,
                int whiteLedPin, int blueLedPin, int whiteLedChannel, int blueLedChannel);

    /**
     * @brief モーター1を前進方向に回転させます。
     * @param speed [in] モーターの速度 (0-255)
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void forward1(int speed);

    /**
     * @brief モーター1を後進方向に回転させます。
     * @param speed [in] モーターの速度 (0-255)
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void backward1(int speed);

    /**
     * @brief モーター1を停止させます。
     */
    void stop1();

    /**
     * @brief モーター2を前進方向に回転させます。
     * @param speed [in] モーターの速度 (0-255)
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void forward2(int speed);

    /**
     * @brief モーター2を後進方向に回転させます。
     * @param speed [in] モーターの速度 (0-255)
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void backward2(int speed);

    /**
     * @brief モーター2を停止させます。
     */
    void stop2();

    /**
     * @brief ブザーを鳴らします。
     */
    void buzzerOn();

    /**
     * @brief ブザーを停止します。
     */
    void buzzerOff();

    /**
     * @brief 白色LEDの明るさを設定します。
     * @param brightness [in] 明るさ (0-255)。
     */
    void setWhiteLed(int brightness);

    /**
     * @brief 青色LEDの明るさを設定します。
     * @param brightness [in] 明るさ (0-255)。
     */
    void setBlueLed(int brightness);

    /**
     * @brief バッテリー電圧をアナログピンから読み取り、計算して返します。
     * @return int 計算されたバッテリー電圧 (mV単位)。
     * @note 電圧計算に使用する抵抗値(R1, R2)は、実際の回路に合わせてください。
     * @note ESP32のADCの特性により、値が不安定な場合があります。必要に応じて平滑化処理を追加してください。
     */
    int getVoltage() const;

    /**
     * @brief スライダー値(0-255)をモーター速度(0-255)に変換します。
     * 128を中央値0とし、それより大きい/小さい値に応じて速度を決定します。
     * @param slideVal [in] 変換元のスライダー値 (0-255)。
     * @return int 変換後のモーター速度 (0-255)。
     */
    int transformSlideValue(int slideVal) const;

private:
    // --- ピン番号 ---
    int _in1, _in2, _in3, _in4;             // モーター制御ピン
    int _buzzerPin;                         // ブザー制御ピン
    int _batteryPin;                        // バッテリー電圧測定ピン
    int _whiteLedPin, _blueLedPin;          // LED制御ピン

    // --- LEDCチャンネル番号 ---
    int _motorChannel1, _motorChannel2;     // モーター1用LEDCチャンネル
    int _motorChannel3, _motorChannel4;     // モーター2用LEDCチャンネル
    int _buzzerChannel;                     // ブザー用LEDCチャンネル
    int _whiteLedChannel, _blueLedChannel;  // LED用LEDCチャンネル

    // --- プライベートヘルパー関数 ---
    void _setupLedcChannel(int channel, int pin);
};

#endif // CATERPILLAR_H
