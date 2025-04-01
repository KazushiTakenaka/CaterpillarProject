#ifndef CATERPILLAR_H
#define CATERPILLAR_H

#include <Arduino.h>

/**
 * @brief キャタピラ(モーター2系統)とブザーの制御を行うクラスです。
 * LEDCライブラリを使用してPWM制御を行います。
 */
class Caterpillar {
public:
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
    Caterpillar(int in1, int in2, int in3, int in4, int buzzer,
        int motorChannel1, int motorChannel2, int motorChannel3, int motorChannel4, int buzzerChannel);

    /**
     * @brief モーター1を前進方向に回転させます。
     * @param forward_int1 [in] モーターの速度 (0-255)。
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void forward1(int forward_int1);

    /**
     * @brief モーター1を後進方向に回転させます。
     * @param backward_int1 [in] モーターの速度 (0-255)。
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void backward1(int backward_int1);

    /**
     * @brief モーター1を停止させます。
     */
    void stop1();

    /**
     * @brief モーター2を前進方向に回転させます。
     * @param forward_int2 [in] モーターの速度 (0-255)。
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void forward2(int forward_int2);

    /**
     * @brief モーター2を後進方向に回転させます。
     * @param backward_int2 [in] モーターの速度 (0-255)。
     * @note 実際の動作はモータードライバの接続方法に依存します。
     */
    void backward2(int backward_int2);

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

private:
    // --- ピン番号 ---
    int IN1, IN2, IN3, IN4; // モーター制御ピン
    int BUZZER;             // ブザー制御ピン

    // --- LEDCチャンネル番号 ---
    int motorChannel1, motorChannel2; // モーター1用LEDCチャンネル
    int motorChannel3, motorChannel4; // モーター2用LEDCチャンネル
    int buzzerChannel;                // ブザー用LEDCチャンネル
};

#endif // CATERPILLAR_H
