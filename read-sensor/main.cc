#include "SparkFunBME280.h"
#include "SparkFunCCS811.h"
#include "hx_drv_tflm.h"

#define CCS811_ADDR 0x5B //Default I2C Address

//Global sensor objects
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;

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

// print sensors values over UART
// see protocol here: https://docs.edgeimpulse.com/docs/cli-data-forwarder#protocol
void printData()
{
    hx_drv_uart_print("%d,%d,%d\n", myCCS811.getCO2(), myCCS811.getTVOC(),(uint32_t)(int)myBME280.readFloatHumidity());
}

void loop()
{
    myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
    printData();

    hx_util_delay_ms(1000); //Wait for next reading
}

int main(int argc, char *argv[])
{
    setup();
    while (true)
    {
        loop();
    }
    return 0;
}