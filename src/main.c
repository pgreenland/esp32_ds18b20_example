/* ESP-IDF */
#include "driver/gpio.h"

/* ESP Logging */
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Local modules */
#include "one_wire.h"
#include "ds18b20.h"

/* Local macros */
#define mARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Private variables - log tag */
static const char *TAG = "MAIN";

/* UART instance and GPIO pin to use for 1-Wire bus */
static const uart_port_t eUART = UART_NUM_1;
static const gpio_num_t eGPIO = GPIO_NUM_6;

void app_main()
{
	/* Initialize 1-Wire bus */
	ONE_WIRE_Init(eUART, eGPIO);

	/* Search for a limited number of devices on the 1-Wire bus */
	stONE_WIRE_ROM_Code_t astROMCodes[10];
	uint8_t uiDeviceCount = 0;
	uint8_t uiLastDiscrepancy = 0;
	do
	{
		if (ONE_WIRE_Search(eUART, &astROMCodes[uiDeviceCount], &uiLastDiscrepancy))
		{
			/* Successfully found a device, convert ROM code to string */
			char acROMCodeStr[ONE_WIRE_ROM_CODE_STR_BUF_SIZE];
			ONE_WIRE_ROMCodeToStr(&astROMCodes[uiDeviceCount], acROMCodeStr, sizeof(acROMCodeStr));
			ESP_LOGI(TAG, "Found device ROM code: %s", acROMCodeStr);

			/* Check device type */
			if (DS18B20_FAMILY_CODE == ONE_WIRE_GetFamilyCode(&astROMCodes[uiDeviceCount]))
			{
				/* Device is a DS18B20 temperature sensor */
				ESP_LOGI(TAG, "\tDevice is a DS18B20 temperature sensor");

				/* Increment device count */
				uiDeviceCount++;
			}
			else
			{
				/* Device type is unknown */
				ESP_LOGI(TAG, "\tDevice type is UNKNOWN");
			}
		}
		else
		{
			ESP_LOGI(TAG, "No 1-Wire devices found");
		}
	} while (	 (0U != uiLastDiscrepancy)
			  && (uiDeviceCount < mARRAY_SIZE(astROMCodes))
			);

	/* Demonstrate reduction in resolution */
	for (uint8_t i = 0U; i < uiDeviceCount; i++)
	{
		/* Set resolution to 10-bit */
		if (DS18B20_SetResolution(eUART, &astROMCodes[i], eDS18B20_Resolution_10Bit))
		{
			ESP_LOGI(TAG, "Set DS18B20 [%d] resolution to 10-bit", i);
		}
		else
		{
			ESP_LOGW(TAG, "Failed to set resolution for DS18B20 [%d]", i);
		}
	}

	/* Main loop - read temperature from each of the DS18B20 devices found */
	bool bIndividualReads = true;
	for (;;)
	{
		if (bIndividualReads)
		{
			/* Perform individual temperature conversions and reading */
			ESP_LOGI(TAG, "Performing individual temperature conversions and readings");
			for (uint8_t i = 0U; i < uiDeviceCount; i++)
			{
				/* Read temperature */
				float fTemperature;
				if (DS18B20_ConvertAndReadTemp(eUART, &astROMCodes[i], &fTemperature))
				{
					ESP_LOGI(TAG, "Temperature [%d]: %.2f degC", i, fTemperature);
				}
				else
				{
					ESP_LOGW(TAG, "Failed to read temperature from DS18B20 [%d]", i);
				}
			}
		}
		else
		{
			/* Issue simultaneous conversion to all devices */
			ESP_LOGI(TAG, "Issuing simultaneous temperature conversion to all DS18B20 devices");
			if (!DS18B20_SimultaneousConvert(eUART))
			{
				ESP_LOGW(TAG, "Failed to issue simultaneous temperature conversion");
			}

			/* Read temperature from each device */
			for (uint8_t i = 0U; i < uiDeviceCount; i++)
			{
				/* Read temperature */
				float fTemperature;
				if (DS18B20_ReadTemp(eUART, &astROMCodes[i], &fTemperature))
				{
					ESP_LOGI(TAG, "Temperature [%d]: %.2f degC", i, fTemperature);
				}
				else
				{
					ESP_LOGW(TAG, "Failed to read temperature from DS18B20 [%d]", i);
				}
			}
		}

		/* Toggle read method for next iteration */
		bIndividualReads = !bIndividualReads;

		/* Wait before next reading */
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
