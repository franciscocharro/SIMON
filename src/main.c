#include "main.h"
//#include "stm32f4xx_gpio.h"
//static uint8_t lastButtonStatus = RESET;
static uint16_t state, c_led;
static uint8_t i, i_round, i_max, i_check;
static uint16_t leds_v[20], buttons_v[4];

int main() {

    init();
    delay(1000);
    do {
        loop();
    } while (1);
}

void init() {
    initLeds();
    initButton();
    initBuzz();
    initRandomNum();
    initGeneral();
    //initTimer();

}

void loop() {
    if (state == 0)
        StartGame();
    if (state == 1)
        NewRound();
    if (state == 2)
        PlayLeds();
    if (state == 3)
        WaitForButton();
    if (state >= 4)
        GameOver();
}

void initLeds() {

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOB, &gpio);

}

void initButton() {

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;

   GPIO_Init(GPIOE, &gpio);
}

void initBuzz() {

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    //gpio.GPIO_Pin = USER_BUTTON;
    gpio.GPIO_Pin = GPIO_Pin_14;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOB, &gpio);

}

void initGeneral() {
    digitalWriteB(1,4);
    digitalWriteB(1,5);
    digitalWriteB(1,6);
    digitalWriteB(1,7);
    GPIOC->BSRRL = (1 << 0);

    i_round = 0;
    i_max = 20;
    i_check = 0;
    for(i=0;i<i_max;i++){
        leds_v[i]=0;
    }

    state = 0;

}
void initRandomNum(){
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE); //random_gen
    RNG_DeInit();
    RNG_Cmd(ENABLE);
}

void delay(uint32_t ms) {
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}
void digitalWriteB(uint16_t bit_value, uint16_t pin){
    if (bit_value){
        GPIOB->BSRRL = (1 << pin);
        GPIOD->BSRRH = (1 << 4);
    }
    else{
        GPIOB->BSRRH = (1 << pin);
        GPIOD->BSRRL = (1 << 4);
    }
}

void StartGame(){
      digitalWriteB(0,4);
      digitalWriteB(0,5);
      digitalWriteB(0,6);
      digitalWriteB(0,7);
      delay(4000);
      digitalWriteB(1,4);
      digitalWriteB(1,5);
      digitalWriteB(1,6);
      digitalWriteB(1,7);
      delay(2000);
    state++;
}
void NewRound(){
    leds_v[i_round]=(uint8_t)(RNG_GetRandomNumber()%4)+4;
    i_check = 0;
    state++;
    delay(2000);
}

void PlayLeds(){
    for (i = 0;i<i_round+1; i++){
        digitalWriteB(0,leds_v[i]);
        delay(2000);
        digitalWriteB(1,leds_v[i]);
        delay(500);
    }
    state++;
}

void WaitForButton(){
    buttons_v[0] = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0);
    buttons_v[1] = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1);
    buttons_v[2] = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
    buttons_v[3] = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3);
    for(i=0;i<4;i++){
        if(buttons_v[i]==1){
            c_led=i+4;
            if(leds_v[i_check]==c_led){
                digitalWriteB(0,c_led);
                delay(1000);
                digitalWriteB(1,c_led);
                delay(100);
                if(i_check >=i_round){
                state=1;
                i_round++;
                }
                else{
                    i_check++;
                    state = 3;
                }
            }
            else
                state=4;
        }
    }
      buttons_v[0] = 0;
      buttons_v[1] = 0;
      buttons_v[2] = 0;
      buttons_v[3] = 0;
}

void GameOver(){
    for(i=0;i<3;i++)
        digitalWriteB(0,4);
        digitalWriteB(0,5);
        digitalWriteB(0,6);
        digitalWriteB(0,7);
        delay(2000);
        digitalWriteB(1,4);
        digitalWriteB(1,5);
        digitalWriteB(1,6);
        digitalWriteB(1,7);
        delay(1000);
        if(i==3){
            state++;
        }
    }

void initTimer(void)
{
NVIC_InitTypeDef NVIC_InitStructure;
/* Enable the TIM2 gloabal Interrupt */
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

/* TIM2 clock enable */
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

TIM_TimeBaseInitTypeDef timerInitStructure;
timerInitStructure.TIM_Prescaler = 3000;
timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
timerInitStructure.TIM_Period = 500;
timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
timerInitStructure.TIM_RepetitionCounter = 0;
TIM_TimeBaseInit(TIM2, &timerInitStructure);
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        state=4;
    }
}
