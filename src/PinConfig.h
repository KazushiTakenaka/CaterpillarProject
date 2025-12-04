#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// --- GPIOピン定義 ---
const int IN1 = 25;         // モーター1用ピン
const int IN2 = 26;         // モーター1用ピン
const int IN3 = 16;         // モーター2用ピン
const int IN4 = 15;         // モーター2用ピン
const int BUZZER = 27;      // ブザー用ピン
const int BATTERY = 35;     // バッテリー電圧測定用ピン (アナログ入力)
const int WHITE_LED = 17;   // 白色LED用ピン
const int BLUE_LED = 18;    // 青色LED用ピン

// --- LEDCチャンネル定義 (PWM制御用) ---
const int motorChannel1 = 0; // モーター1用LEDCチャンネル
const int motorChannel2 = 1; // モーター1用LEDCチャンネル
const int motorChannel3 = 2; // モーター2用LEDCチャンネル
const int motorChannel4 = 3; // モーター2用LEDCチャンネル
const int buzzerChannel = 4; // ブザー用LEDCチャンネル
const int whiteLedChannel = 5; // 白色LED用LEDCチャンネル
const int blueLedChannel = 6;// 青色LED用LEDCチャンネル

#endif // PIN_CONFIG_H
