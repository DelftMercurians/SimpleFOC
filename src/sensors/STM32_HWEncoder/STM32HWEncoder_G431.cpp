#include "STM32HWEncoder_G431.h"

#if defined(_STM32_DEF_)

/*
  HardwareEncoder(int cpr)
*/
STM32HWEncoder_G431::STM32HWEncoder_G431(unsigned int _ppr, int8_t pinA, int8_t pinB, int8_t pinI) {
    cpr = _ppr * 4; // 4x for quadrature
    _pinA = digitalPinToPinName(pinA);
    _pinB = digitalPinToPinName(pinB);
    _pinI = digitalPinToPinName(pinI);
}

/*
  Shaft angle calculation
*/
float STM32HWEncoder_G431::getSensorAngle() { 
    return _2PI * encoder_handle.Instance->CNT / static_cast<float>(cpr);
}

// getter for index pin
int STM32HWEncoder_G431::needsSearch() { return false; }

// private function used to determine if encoder has index
int STM32HWEncoder_G431::hasIndex() { return 0; }

// encoder initialisation of the hardware pins
void STM32HWEncoder_G431::init() {
    // GPIO configuration
    TIM_TypeDef *InstanceA = TIM4;
    if (!IS_TIM_ENCODER_INTERFACE_INSTANCE(InstanceA)) {
        initialized = false;
        return;
    }
    TIM_TypeDef *InstanceB = TIM4;
    if (InstanceA != InstanceB) {
        initialized = false;
        return;
    }

    const PinMap PinMap_TIM4[] = {
        {PB_6,       TIM4,  STM_PIN_DATA_EXT(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF2_TIM4, 1, 0)}, // TIM4_CH1
        {PB_7,       TIM4,  STM_PIN_DATA_EXT(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF2_TIM4, 2, 0)}, // TIM4_CH2
        {PB_8,       TIM4,  STM_PIN_DATA_EXT(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF2_TIM4, 3, 0)}, // TIM4_CH3
        {NC,         NP,    0}
    };

    pinmap_pinout(_pinA, PinMap_TIM4);
    pinmap_pinout(_pinB, PinMap_TIM4);

    // TODO check width:
    //IS_TIM_32B_COUNTER_INSTANCE(InstanceA);

    // set up timer for encoder
    encoder_handle.Init.Period = cpr - 1;
    encoder_handle.Init.Prescaler = 0;
    encoder_handle.Init.ClockDivision = 0;
    encoder_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    encoder_handle.Init.RepetitionCounter = 0;
    encoder_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    TIM_Encoder_InitTypeDef encoder_config;

    encoder_config.EncoderMode = TIM_ENCODERMODE_TI12;

    encoder_config.IC1Polarity = TIM_ICPOLARITY_RISING;
    encoder_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC1Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC1Filter = 0;

    encoder_config.IC2Polarity = TIM_ICPOLARITY_RISING;
    encoder_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC2Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC2Filter = 0;

    encoder_handle.Instance = InstanceA; // e.g. TIM4;
    enableTimerClock(&encoder_handle);

    if (HAL_TIM_Encoder_Init(&encoder_handle, &encoder_config) != HAL_OK) {
        initialized = false;
        return;
    }

    if (HAL_TIM_Encoder_Start(&encoder_handle, TIM_CHANNEL_1) != HAL_OK) {
        initialized = false;
        return;
    }

    initialized = true;
    // // GPIO configuration
    // // TIM_TypeDef *InstanceA = TIM4;
    // // TIM_TypeDef *InstanceB = TIM4;
    // pinmap_pinout(_pinA, PinMap_TIM);
    // pinmap_pinout(_pinB, PinMap_TIM);

    // // TODO check width:
    // //IS_TIM_32B_COUNTER_INSTANCE(InstanceA);

    // // set up timer for encoder
    // encoder_handle.Init.Period = cpr - 1;
    // encoder_handle.Init.Prescaler = 0;
    // encoder_handle.Init.ClockDivision = 0;
    // encoder_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    // encoder_handle.Init.RepetitionCounter = 0;
    // encoder_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    // TIM_Encoder_InitTypeDef encoder_config;

    // encoder_config.EncoderMode = TIM_ENCODERMODE_TI12;

    // encoder_config.IC1Polarity = TIM_ICPOLARITY_RISING;
    // encoder_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    // encoder_config.IC1Prescaler = TIM_ICPSC_DIV1;
    // encoder_config.IC1Filter = 0;

    // encoder_config.IC2Polarity = TIM_ICPOLARITY_RISING;
    // encoder_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    // encoder_config.IC2Prescaler = TIM_ICPSC_DIV1;
    // encoder_config.IC2Filter = 0;

    // encoder_handle.Instance = TIM4; // e.g. TIM4;
    // enableTimerClock(&encoder_handle);

    // if (HAL_TIM_Encoder_Init(&encoder_handle, &encoder_config) != HAL_OK) {
    //     initialized = false;
    //     return;
    // }

    // if (HAL_TIM_Encoder_Start(&encoder_handle, TIM_CHANNEL_1) != HAL_OK) {
    //     initialized = false;
    //     return;
    // }

    // initialized = true;
}

#endif