/* Edge Impulse ingestion SDK
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Include ----------------------------------------------------------------- */
#include "ei_run_classifier.h"
#include "hx_drv_tflm.h"
#include "SparkFunBME280.h"
#include "SparkFunCCS811.h"
//#include "model_metadata.h"

#define CCS811_ADDR 0x5B //Default I2C Address

//Global sensor objects
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;

// Features array put in RAM
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };


// setup sensors
void setup()
{
    hx_drv_share_switch(SHARE_MODE_I2CM);
    hx_drv_uart_initial(UART_BR_115200);
    //This begins the CCS811 sensor and prints error status of .begin()
    if (!myCCS811.begin())
    {
        hx_drv_uart_print("Problem with CCS811\n");
    }
    else
    {
        hx_drv_uart_print("CCS811 online\n");
    }

    //Initialize BME280
    //For I2C, enable the following and disable the SPI section
    myBME280.settings.commInterface = I2C_MODE;
    myBME280.settings.I2CAddress = 0x77;
    myBME280.settings.runMode = 3; //Normal mode
    myBME280.settings.tStandby = 0;
    myBME280.settings.filter = 4;
    myBME280.settings.tempOverSample = 5;
    myBME280.settings.pressOverSample = 5;
    myBME280.settings.humidOverSample = 5;

    //Calling .begin() causes the settings to be loaded
    hx_util_delay_ms(10);            //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
    uint8_t id = myBME280.begin(); //Returns ID of 0x60 if successful
    if (id != 0x60)
    {
        hx_drv_uart_print("Problem with BME280\n");
    }
    else
    {
        hx_drv_uart_print("BME280 online\n");
    }

    myCCS811.setDriveMode(1); //constant power mode
}


// Fill features array with sensors values
void getSensors() {
    for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i += 3) {
        //uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
        features[i + 0] = (float)myCCS811.getCO2();
        features[i + 1] = (float)myCCS811.getTVOC();
        features[i + 2] = (float)myBME280.readFloatHumidity();
        
        hx_util_delay_ms(EI_CLASSIFIER_INTERVAL_MS);
    }
}


int main(void)
{
    setup();
    hx_drv_tick_start();
    ei_printf("Edge Impulse standalone inferencing Himax WE-I Plus EVB\n");

    while (1) {

        if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            ei_printf("The size of your 'features' array is not correct. Expected %d items, but had %u\n",
                EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
            return 1;
        }

        getSensors();

        // Turn the features buffer in a signal which we can the classify
        signal_t signal;
        int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) {
            ei_printf("Failed to create signal from buffer (%d)\n", err);
        }

        ei_impulse_result_t result = { 0 };


        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, true);
        ei_printf("run_classifier returned: %d\n", res);

        if (res != 0) return 1;

        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);

        // print the predictions
        ei_printf("[");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("%f", result.classification[ix].value);
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf(", ");
    #else
            if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
                ei_printf(", ");
            }
    #endif
        }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf_float(result.anomaly);
    #endif
        ei_printf("]\n");

        // And wait 2 seconds
        ei_sleep(2000);
    }

    return 0;
}