#define N_MUESTRAS 32    // T
#define PI 3.14



void config_DAC(void);
void config_timer_dac(void);
void genera_muestras(void);

extern uint16_t muestras[N_MUESTRAS];
