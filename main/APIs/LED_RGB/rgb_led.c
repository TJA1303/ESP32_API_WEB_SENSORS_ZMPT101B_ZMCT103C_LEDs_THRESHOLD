/*
 * rgb_led.c
 *
 *  Created on: Oct 11, 2021
 *      Author: kjagu
 */

#include <stdbool.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "stdlib.h"

#include "driver/ledc.h"
#include "rgb_led.h"

//******************************************************************IMPLEMENTACIÓN SENSORES****************************************************
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"


static int raw;
static int raw2;
static float may = -1;
static float may2 = -1;
static int contador = 0;
static float tension;
static float current;
static float potencia;


//******************************************************************FUNCIONES DE LOS SENSORES***************************************************

//Función de inicialización del ADC
esp_err_t init_ADC(void)
{
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_WIDTH_BIT_12);
    return ESP_OK;
}


//Función para calcular tensión
void Cal_adc(void)
{
	for(;;)
	{
		raw = adc1_get_raw(ADC1_CHANNEL_0); 	//Voltaje
    	raw2 = adc1_get_raw(ADC1_CHANNEL_3);	//Corriente
    	float ADC = abs(raw - VREF);			//Restar cantidad de bits para voltaje (empezar en cero)
    	float VIADC = 3.55 * abs(raw2 - IREF) / Bits_ADC;	//Restar para que la corriente vuelva a cero
    	vTaskDelay(1 / portTICK_PERIOD_MS);
    	if (may < ADC)
    	{
        	may = ADC;
    	}
    	if (may2 < VIADC)
    	{
        	may2 = VIADC;
    	}
    	contador++;
    	if (contador > 3000)
    	{
        	tension = may / (sqrt(2) * 2.3969);		//Hallar valor RMS
        	current = may2 * mil / ResI;			//Ley de Ohm para hallar la corriente
        	if (tension < 10)
        	{
            	tension = 0;
        	}
        	potencia = tension * current;
        	printf("- - - - - - - - - - - - - - - - \n");
        	printf("current: %.3f A\n", current);
        	printf("voltage: %.3f V\n", tension);
        	printf("potencia: %.3f W\n", potencia);

        	contador = 0;
        	may = -1;
        	may2 = -1;
    	}

	}
}

// enviar al servidor (http server)
float tension_get(void){
    float tensionn = tension;
	printf("El contador va en: %d",contador);
    return tensionn;
}

float current_get(void){
    float currentt = current;
    return currentt;
}
float potencia_get(void){
    float potenciaa = potencia;
    return potenciaa;
}


//*************************************************************ACABA IMPLEMENTACIÓN SENSORES****************************************************

// RGB LED Configuration Array
ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM];

// handle for rgb_led_pwm_init
bool g_pwm_init_handle = false;



/**
 * Initializes the RGB LED settings per channel, including
 * the GPIO for each color, mode and timer configuration.
 */
void rgb_led_pwm_init(void)
{
	int rgb_ch;

	// Red
	ledc_ch[0].channel		= LEDC_CHANNEL_0;
	ledc_ch[0].gpio			= RGB_LED_RED_GPIO;
	ledc_ch[0].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[0].timer_index	= LEDC_TIMER_0;

	// Green
	ledc_ch[1].channel		= LEDC_CHANNEL_1;
	ledc_ch[1].gpio			= RGB_LED_GREEN_GPIO;
	ledc_ch[1].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[1].timer_index	= LEDC_TIMER_0;

	// Blue
	ledc_ch[2].channel		= LEDC_CHANNEL_2;
	ledc_ch[2].gpio			= RGB_LED_BLUE_GPIO;
	ledc_ch[2].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[2].timer_index	= LEDC_TIMER_0;

	// Configure timer zero
	ledc_timer_config_t ledc_timer =
	{
		.duty_resolution	= LEDC_TIMER_8_BIT,
		.freq_hz			= 100,
		.speed_mode			= LEDC_HIGH_SPEED_MODE,
		.timer_num			= LEDC_TIMER_0
	};
	ledc_timer_config(&ledc_timer);

	// Configure channels
	for (rgb_ch = 0; rgb_ch < RGB_LED_CHANNEL_NUM; rgb_ch++)
	{
		ledc_channel_config_t ledc_channel =
		{
			.channel	= ledc_ch[rgb_ch].channel,
			.duty		= 0,
			.hpoint		= 0,
			.gpio_num	= ledc_ch[rgb_ch].gpio,
			.intr_type	= LEDC_INTR_DISABLE,
			.speed_mode = ledc_ch[rgb_ch].mode,
			.timer_sel	= ledc_ch[rgb_ch].timer_index,
		};
		ledc_channel_config(&ledc_channel);
	}

	g_pwm_init_handle = true;
}

/**
 * Sets the RGB color.
 */
void rgb_led_set_color(uint8_t red, uint8_t green, uint8_t blue)
{	
	// Value should be 0 - 255 for 8 bit number
	printf("%d",g_pwm_init_handle);
	ledc_set_duty(ledc_ch[0].mode, ledc_ch[0].channel, red);
	ledc_update_duty(ledc_ch[0].mode, ledc_ch[0].channel);

	ledc_set_duty(ledc_ch[1].mode, ledc_ch[1].channel, green);
	ledc_update_duty(ledc_ch[1].mode, ledc_ch[1].channel);

	ledc_set_duty(ledc_ch[2].mode, ledc_ch[2].channel, blue);
	ledc_update_duty(ledc_ch[2].mode, ledc_ch[2].channel);
}

void rgb_led_wifi_app_started(void)
{
	if (g_pwm_init_handle == false)
	{
		rgb_led_pwm_init();
	}

	rgb_led_set_color(255, 102, 255);
}

void rgb_led_http_server_started(void)
{
	if (g_pwm_init_handle == false)
	{
		rgb_led_pwm_init();
	}

	rgb_led_set_color(204, 255, 51);
}


void rgb_led_wifi_connected(void)
{
	if (g_pwm_init_handle == false)
	{
		rgb_led_pwm_init();
	}

	rgb_led_set_color(0, 255, 153);
}
