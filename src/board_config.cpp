// MIT License

// Copyright (c) 2019 Jacob Dahl

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <board_config.hpp>
#include <Time.hpp>

static void init_FTM0(void);
static void init_FTM1(void);
static void init_serial(void);

// CMSIS compliance or something...
extern "C" void SystemInit(void)
{
	init_FTM0();
	init_FTM1();
	init_serial();
}

#define PS_DIV_1 0b000
#define PS_DIV_2 0b001
#define PS_DIV_4 0b010
#define PS_DIV_8 0b011
#define PS_DIV_16 0b100
#define PS_DIV_32 0b101
#define PS_DIV_64 0b110
#define PS_DIV_128 0b111

#define FTM_PRESCALE_533_NANO_SEC 32
#define FTM_SYS_CLK 1
#define DEFAULT_FTM_MOD (61440 - 1)

//---- PRECISION TIMER ----//
// Frequency: 60MHz (F_BUS)
// counter: 16bit
// prescaler: 32
// modulo: 65535
// resolution: 0.533us
// overflow rate: 34.952ms --> 28.61Hz
static void init_FTM0(void)
{
	// Disable write protection to change the settings -- TODO reenable write protection?
	uint32_t mode = FTM0_FMS;
	if (mode & FTM_FMS_WPEN)
	{
		FTM0_MODE |= FTM_MODE_WPDIS;
	}

	FTM0_CNT = 0x0000; //reset count to zero
	FTM0_MOD = 0xFFFF; //max modulus = 65535 (gives count = 65,536 on roll-over)
	// Turn the timer on and configure with our settings
	FTM0_SC = FTM_SC_CLKS(FTM_SYS_CLK) // Set to system clock
			| FTM_SC_PS(PS_DIV_32) // set prescaler for desired resolution / overflow rate
			| FTM_SC_TOIE; // enable overflow interrupt

	FTM0_MODE |= FTM_MODE_FTMEN;

	NVIC_ENABLE_IRQ(IRQ_FTM0);
}

//---- DISPATCH TIMER ----//
// Frequency: 60MHz (F_BUS)
// counter: 16bit
// prescaler: 1
// modulo: 60,000
// resolution: 16.6666ns
// overflow rate: 1ms --> 1kHz
static void init_FTM1(void)
{
	// Disable write protection to change the settings -- TODO reenable write protection?
	uint32_t mode = FTM1_FMS;
	if (mode & FTM_FMS_WPEN)
	{
		FTM1_MODE |= FTM_MODE_WPDIS;
	}

	FTM1_CNT = 0x0000; //reset count to zero
	FTM1_MOD = 0xEA60; //max modulus = 60,000 (@60Mhz, 60,000 ticks == 1ms)
	// Turn the timer on and configure with our settings
	FTM1_SC = FTM_SC_CLKS(FTM_SYS_CLK) // Set to system clock
			| FTM_SC_PS(PS_DIV_1) // set prescaler for desired resolution / overflow rate
			| FTM_SC_TOIE; // enable overflow interrupt

	FTM1_MODE |= FTM_MODE_FTMEN;

	NVIC_ENABLE_IRQ(IRQ_FTM1);
}

static void init_serial(void)
{
	// init the USB interface
	Serial.begin(9600);
	SYS_INFO("\n---------- App ----------\n");
}

// So that we know if we've blown a stack
extern "C" void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
	// This must be the LED
	pinMode(13, OUTPUT);

	while (1)
	{
		// TODO: This has literally never worked
		digitalWrite(13, HIGH);
	}
}
