/***************************************************************************//**
 *   @file   platform_drivers.c
 *   @brief  Implementation of Generic Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/*************************** Global variables *********************************/
/******************************************************************************/
#ifdef ZYNQ
XSpiPs  m_spi;
XSpiPs_Config  *m_spi_config;
#endif

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(gpio_desc **desc,
		 uint8_t gpio_number)
{
	gpio_desc *dev;

	if (gpio_number < 32)
		return FAILURE;

	dev = (gpio_desc *)malloc(sizeof(*dev));
	if (!dev)
		return FAILURE;

	dev->number = gpio_number;
	dev->id 	= 0;

#ifdef ZYNQ_PS7
	dev->type = ZYNQ_PS7_GPIO;
#endif
#ifdef ZYNQ_PSU
	dev->type = ZYNQ_PSU_GPIO;
#endif
#ifdef NIOS_II
	dev->type = NIOS_II_GPIO;
#endif
#ifdef MICROBLAZE
	dev->type = MICROBLAZE_GPIO;
#endif

	*desc = dev;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(gpio_desc *desc)
{
	free(desc);

	return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_input(gpio_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_output(gpio_desc *desc,
			      uint8_t value)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (value) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_direction(gpio_desc *desc,
			   uint8_t *direction)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (direction) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value)
{
	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	pstatus = -1;
	ppos = desc->number - 32;
	pmask = 0x1 << ppos;

	switch(desc->type) {
#ifdef ZYNQ_PS7
	case ZYNQ_PS7_GPIO:
		pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
		Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata | pmask));
		pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c8);
		Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c8), (pdata | pmask));
		pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
		Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x004c),
			  ((pdata & ~pmask) | (value << ppos)));
		pstatus = 0;
		break;
#endif
#ifdef ZYNQ_PSU
	case ZYNQ_PSU_GPIO:
		pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
		Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata | pmask));
		pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0308);
		Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0308), (pdata | pmask));
		pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
		Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0050),
			  ((pdata & ~pmask) | (value << ppos)));
		pstatus = 0;
		break;
#endif
#ifdef NIOS_II
	case NIOS_II_GPIO:
		pdata = IORD_32DIRECT(SYS_GPIO_OUT_BASE, 0x0);
		IOWR_32DIRECT(SYS_GPIO_OUT_BASE, 0x0,
			      ((pdata & ~pmask) | (value << ppos)));
		pstatus = 0;
		break;
#endif
#ifdef MICROBLAZE
	case MICROBLAZE_GPIO:
		pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0xc);
		Xil_Out32((XPAR_AXI_GPIO_BASEADDR + 0xc), (pdata & ~pmask));
		pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0x8);
		Xil_Out32((XPAR_AXI_GPIO_BASEADDR + 0x8),
			  ((pdata & ~pmask) | (value << ppos)));
		pstatus = 0;
		break;
#endif
	default:
		break;
	}

	return(pstatus);
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_value(gpio_desc *desc,
		       uint8_t *value)
{
	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
#ifdef ZYNQ
	uint32_t pmask;
#endif

	pstatus = -1;
	ppos = desc->number - 32;
#ifdef ZYNQ
	pmask = 0x1 << ppos;
#endif

	switch(desc->type) {
#ifdef ZYNQ_PS7
	case ZYNQ_PS7_GPIO:
		pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
		Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata & ~pmask));
		pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
		*value = (pdata >> ppos) & 0x1;
		pstatus = 0;
		break;
#endif
#ifdef ZYNQ_PSU
	case ZYNQ_PSU_GPIO:
		pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
		Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata & ~pmask));
		pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
		*value = (pdata >> ppos) & 0x1;
		pstatus = 0;
		break;
#endif
#ifdef NIOS_II
	case NIOS_II_GPIO:
		pdata = IORD_32DIRECT(SYS_GPIO_OUT_BASE, 0x0);
		*value = (pdata >> ppos) & 0x1;
		pstatus = 0;
		break;
#endif
#ifdef MICROBLAZE
	case MICROBLAZE_GPIO:
		pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0x8);
		*value = (pdata >> ppos) & 0x1;
		pstatus = 0;
		break;
#endif
	default:
		break;
	}

	return(pstatus);
}

/***************************************************************************//**
 * @brief ad_gpio_set_range
 ******************************************************************************/

int32_t ad_gpio_set_range(uint8_t start_pin, uint8_t num_pins, uint8_t data)
{

	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	if (start_pin < 32) {
		return(-1);
	}

	pstatus = -1;
	ppos = start_pin - 32;
	pmask = ((1 << num_pins) - 1) << ppos;

#ifdef ZYNQ_PS7

	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata | pmask));
	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c8);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c8), (pdata | pmask));
	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x004c),
		  ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

#ifdef ZYNQ_PSU

	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata | pmask));
	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0308);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0308), (pdata | pmask));
	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0050),
		  ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

#ifdef NIOS_II

	pdata = IORD_32DIRECT(SYS_GPIO_OUT_BASE, 0x0);
	IOWR_32DIRECT(SYS_GPIO_OUT_BASE, 0x0, ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

#ifdef MICROBLAZE

	pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0xc);
	Xil_Out32((XPAR_AXI_GPIO_BASEADDR + 0xc), (pdata & ~pmask));
	pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0x8);
	Xil_Out32((XPAR_AXI_GPIO_BASEADDR + 0x8),
		  ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

	return(pstatus);
}

/***************************************************************************//**
 * @brief ad_gpio_get_range
 ******************************************************************************/

int32_t ad_gpio_get_range(uint8_t start_pin, uint8_t num_pins, uint32_t *data)
{

	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	if (start_pin < 32) {
		return(-1);
	}

	pstatus = -1;
	ppos = start_pin - 32;
	pmask = ((1 << num_pins) - 1) << ppos;

#ifdef ZYNQ_PS7

	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata & ~pmask));
	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;

#endif

#ifdef ZYNQ_PSU

	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata & ~pmask));
	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;

#endif

#ifdef NIOS_II

	pdata = IORD_32DIRECT(SYS_GPIO_OUT_BASE, 0x0);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;

#endif

#ifdef MICROBLAZE

	pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0x8);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;

#endif

	return(pstatus);
}

/***************************************************************************//**
 * @brief do_div
 ******************************************************************************/
uint64_t do_div(uint64_t* n, uint64_t base)
{
	uint64_t mod = 0;

	mod = *n % base;
	*n = *n / base;

	return mod;
}

/***************************************************************************//**
 * @brief ad_reg_write_16
 ******************************************************************************/
void ad_reg_write_16(uint32_t addr, uint32_t data)
{
	uint32_t m_data;

	m_data = ad_reg_read(addr & ~0x3);
	if ((addr & 0x3) == 0)
		m_data = (m_data & ~0xffff) | data;
	else
		m_data = (m_data & 0xffff) | (data<<16);
	ad_reg_write((addr & ~0x3), m_data);
}

#ifdef ALTERA
void ad_platform_init(void)
{
	return;
}

void ad_platform_close(void)
{
	alt_printf("%c", 0x04);
	return;
}
#endif

#ifdef MICROBLAZE
void usleep(uint32_t us_count)
{
	uint32_t count;

	ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x4), 0x00);
	for (count = 0; count < us_count; count++) {
		ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x0), 0x20);
		ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x0), 0x80);
		while (ad_reg_read(XPAR_AXI_TIMER_BASEADDR + 0x8) < 100) {}
	}
	ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x0), 0x20);
}
#endif

/***************************************************************************//**
 * @brief ad_uart_read
 ******************************************************************************/
uint8_t ad_uart_read()
{
#ifdef ZYNQ_PS7

	u32 RecievedByte;
	int32_t timeout = 100000000;
	/* Wait until there is data */
	while (!XUartPs_IsReceiveData(STDIN_BASEADDRESS)&&timeout>0) {
		timeout--;
	}
	RecievedByte = XUartPs_ReadReg(STDIN_BASEADDRESS, XUARTPS_FIFO_OFFSET);
	/* Return the byte received */
	return (uint8_t)RecievedByte;
#else
	uint8_t RecievedByte = 0;

	RecievedByte = getc(stdin);

	return RecievedByte;
#endif
}

/***************************************************************************//**
 * @brief ad_pow2 Create a mask for a given number of bit
 ******************************************************************************/
uint32_t ad_pow2(uint32_t number)
{

	uint32_t index;
	uint32_t mask = 1;

	for (index=1; index < number; index++) {
		mask = (mask << 1) ^ 1;
	}

	return mask;
}
