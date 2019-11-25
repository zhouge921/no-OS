/***************************************************************************//**
 *   @file   ad_fmcdaq2_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *******************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "delay.h"
#include "ad9144.h"
#include "ad9523.h"
#include "ad9680.h"
#include "config.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "axi_adxcvr.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "spi.h"
#include "spi_extra.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "stdio.h"

#ifdef XILINX
#include <xparameters.h>
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GPIO_TRIG		43
#define GPIO_ADC_PD		42
#define GPIO_DAC_TXEN		41
#define GPIO_DAC_RESET		40
#define GPIO_CLKD_SYNC		38
#define GPIO_ADC_FDB		36
#define GPIO_ADC_FDA		35
#define GPIO_DAC_IRQ		34
#define GPIO_CLKD_STATUS_1	33
#define GPIO_CLKD_STATUS_0	32

#define DMA_BUFFER		 0

enum ad9523_channels {
	DAC_DEVICE_CLK,
	DAC_DEVICE_SYSREF,
	DAC_FPGA_CLK,
	DAC_FPGA_SYSREF,
	ADC_DEVICE_CLK,
	ADC_DEVICE_SYSREF,
	ADC_FPGA_CLK,
	ADC_FPGA_SYSREF,
};

/***************************************************************************//**
 * @brief main
 ******************************************************************************/
int fmcdaq2_reconfig(struct ad9144_init_param *p_ad9144_param,
			 struct adxcvr *p_ad9144_xcvr,
		     struct ad9680_init_param *p_ad9680_param,
			 struct adxcvr *p_ad9680_xcvr,
		     struct ad9523_platform_data *p_ad9523_param)
{

	uint8_t mode = 0;

	printf ("Available sampling rates:\n");
	printf ("\t1 - ADC 1000 MSPS; DAC 1000 MSPS\n");
	printf ("\t2 - ADC  500 MSPS; DAC 1000 MSPS\n");
	printf ("\t3 - ADC  500 MSPS; DAC  500 MSPS\n");
	printf ("\t4 - ADC  600 MSPS; DAC  600 MSPS\n");
	printf ("\t5 - ADC 1000 MSPS; DAC 2000 MSPS (2x interpolation)\n");
	printf ("choose an option [default 1]:\n");

	mode = getc(stdin);

	switch (mode) {
	case '5':
		/* REF clock = 100 MHz */
		p_ad9523_param->channels[DAC_DEVICE_CLK].channel_divider = 10;
		p_ad9144_param->pll_ref_frequency_khz = 100000;

		/* DAC at 2 GHz using the internal PLL and 2 times interpolation */
		p_ad9144_param->interpolation = 2;
		p_ad9144_param->pll_enable = 1;
		p_ad9144_param->pll_dac_frequency_khz = 2000000;
		break;
	case '4':
		printf ("4 - ADC  600 MSPS; DAC  600 MSPS\n");
		p_ad9523_param->pll2_vco_diff_m1 = 5;
		(&p_ad9523_param->channels[DAC_FPGA_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[DAC_DEVICE_CLK])->
		channel_divider = 1;
		(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[ADC_FPGA_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[ADC_DEVICE_CLK])->
		channel_divider = 1;
		(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->
		channel_divider = 128;
		p_ad9144_param->lane_rate_kbps = 6000000;
		p_ad9144_xcvr->lane_rate_khz = 6000000;
		p_ad9144_xcvr->ref_rate_khz = 300000;
		p_ad9680_param->lane_rate_kbps = 6000000;
		p_ad9680_xcvr->lane_rate_khz = 6000000;
		p_ad9680_xcvr->ref_rate_khz = 300000;
#ifdef XILINX
		p_ad9144_xcvr->lpm_enable = 0;
		p_ad9144_xcvr->cpll_enable = 1;
		p_ad9144_xcvr->out_clk_sel = 4;

		p_ad9680_xcvr->lpm_enable = 1;
		p_ad9680_xcvr->cpll_enable = 1;
		p_ad9680_xcvr->out_clk_sel = 4;
#endif
		break;
	case '3':
		printf ("3 - ADC  500 MSPS; DAC  500 MSPS\n");
		p_ad9523_param->pll2_vco_diff_m1 = 3;
		(&p_ad9523_param->channels[DAC_FPGA_CLK])->
		channel_divider = 4;
		(&p_ad9523_param->channels[DAC_DEVICE_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[ADC_FPGA_CLK])->
		channel_divider = 4;
		(&p_ad9523_param->channels[ADC_DEVICE_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->
		channel_divider = 256;
		p_ad9144_param->lane_rate_kbps = 5000000;
		p_ad9144_xcvr->lane_rate_khz = 5000000;
		p_ad9144_xcvr->ref_rate_khz = 250000;
		p_ad9680_param->lane_rate_kbps = 5000000;
		p_ad9680_xcvr->lane_rate_khz = 5000000;
		p_ad9680_xcvr->ref_rate_khz = 250000;
#ifdef XILINX
		p_ad9144_xcvr->lpm_enable = 1;
		p_ad9144_xcvr->cpll_enable = 1;
		p_ad9144_xcvr->out_clk_sel = 4;

		p_ad9680_xcvr->lpm_enable = 1;
		p_ad9680_xcvr->cpll_enable = 1;
		p_ad9680_xcvr->out_clk_sel = 4;
#endif
		break;
	case '2':
		printf ("2 - ADC  500 MSPS; DAC 1000 MSPS\n");
		p_ad9523_param->pll2_vco_diff_m1 = 3;
		(&p_ad9523_param->channels[DAC_FPGA_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[DAC_DEVICE_CLK])->
		channel_divider = 1;
		(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[ADC_FPGA_CLK])->
		channel_divider = 4;
		(&p_ad9523_param->channels[ADC_DEVICE_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->
		channel_divider = 256;
		p_ad9144_param->lane_rate_kbps = 10000000;
		p_ad9144_xcvr->lane_rate_khz = 10000000;
		p_ad9144_xcvr->ref_rate_khz = 500000;
		p_ad9680_param->lane_rate_kbps = 5000000;
		p_ad9680_xcvr->lane_rate_khz = 5000000;
		p_ad9680_xcvr->ref_rate_khz = 250000;
#ifdef XILINX
		p_ad9144_xcvr->lpm_enable = 0;
		p_ad9144_xcvr->cpll_enable = 0;
		p_ad9144_xcvr->out_clk_sel = 4;

		p_ad9680_xcvr->lpm_enable = 1;
		p_ad9680_xcvr->cpll_enable = 1;
		p_ad9680_xcvr->out_clk_sel = 4;
#endif
		break;
	default:
		printf ("1 - ADC 1000 MSPS; DAC 1000 MSPS\n");
		p_ad9144_xcvr->ref_rate_khz = 500000;
		p_ad9680_xcvr->ref_rate_khz = 500000;
		break;
	}

	return(0);
}

/***************************************************************************//**
 * @brief main
 ******************************************************************************/
int main(void)
{
	int n;

	spi_init_param	ad9523_spi_param;
	spi_init_param	ad9144_spi_param;
	spi_init_param	ad9680_spi_param;

#ifdef ALTERA
	ad9523_spi_param.type = NIOS_II_SPI;
	ad9144_spi_param.type = NIOS_II_SPI;
	ad9680_spi_param.type = NIOS_II_SPI;
#endif
#ifdef ZYNQ_PS7
	ad9523_spi_param.type = ZYNQ_PS7_SPI;
	ad9144_spi_param.type = ZYNQ_PS7_SPI;
	ad9680_spi_param.type = ZYNQ_PS7_SPI;
#endif
#ifdef ZYNQ_PSU
	ad9523_spi_param.type = ZYNQ_PSU_SPI;
	ad9144_spi_param.type = ZYNQ_PSU_SPI;
	ad9680_spi_param.type = ZYNQ_PSU_SPI;
#endif
#ifdef MICROBLAZE
	struct xil_spi_init_param xil_spi_param = {
			.type = SPI_PL,
			.device_id = 0,
	};
	ad9523_spi_param.extra = &xil_spi_param;
	ad9144_spi_param.extra = &xil_spi_param;
	ad9680_spi_param.extra = &xil_spi_param;
#endif
	ad9523_spi_param.chip_select = SPI_CHIP_SELECT(0);
	ad9144_spi_param.chip_select = SPI_CHIP_SELECT(1);
	ad9680_spi_param.chip_select = SPI_CHIP_SELECT(2);
	ad9523_spi_param.mode = SPI_MODE_0;
	ad9144_spi_param.mode = SPI_MODE_0;
	ad9680_spi_param.mode = SPI_MODE_0;

	struct ad9523_channel_spec	ad9523_channels[8];
	struct ad9523_platform_data	ad9523_pdata;
	struct ad9523_init_param	ad9523_param;
	struct ad9144_init_param	ad9144_param;
	struct ad9680_init_param	ad9680_param;

	ad9523_param.spi_init = ad9523_spi_param;
	ad9144_param.spi_init = ad9144_spi_param;
	ad9680_param.spi_init = ad9680_spi_param;

	struct ad9523_dev *ad9523_device;
	struct ad9144_dev *ad9144_device;
	struct ad9680_dev *ad9680_device;

	struct axi_dac		  *ad9144_core;
	struct axi_dac_channel ad9144_channels[2];
	struct axi_jesd204_tx *ad9144_jesd;
	struct axi_dmac		*ad9144_dma;
	struct adxcvr		*ad9144_xcvr;
	struct axi_adc		*ad9680_core;
	struct axi_jesd204_rx *ad9680_jesd;
	struct adxcvr		*ad9680_xcvr;
	struct axi_dmac	     *ad9680_dma;
//	dmac_xfer               rx_xfer;
//	dmac_xfer               tx_xfer;

	struct axi_dac_init	ad9144_core_init;
	struct jesd204_tx_init ad9144_jesd_init;
	struct jesd204_tx_config ad9144_jesd_config;
	//dac_channel		  ad9144_channels[2];
	struct axi_dmac_init  ad9144_dma_init;
	struct adxcvr_init	  ad9144_xcvr_init;
	struct axi_adc_init	  ad9680_core_init;
	struct jesd204_rx_init ad9680_jesd_init;
	struct adxcvr_init	   ad9680_xcvr_init;
	struct axi_dmac_init ad9680_dma_init;
//	dmac_xfer               rx_xfer;
//	dmac_xfer               tx_xfer;


//******************************************************************************
// setup the base addresses
//******************************************************************************

#ifdef XILINX
	ad9144_xcvr_init.base = XPAR_AXI_AD9144_XCVR_BASEADDR;
	ad9144_core_init.base = XPAR_AXI_AD9144_CORE_BASEADDR;
	ad9144_jesd_init.base = XPAR_AXI_AD9144_JESD_TX_AXI_BASEADDR;
	ad9144_dma_init.base = XPAR_AXI_AD9144_DMA_BASEADDR;
	ad9680_xcvr_init.base = XPAR_AXI_AD9680_XCVR_BASEADDR;
	ad9680_core_init.base = XPAR_AXI_AD9680_CORE_BASEADDR;
	ad9680_jesd_init.base = XPAR_AXI_AD9680_JESD_RX_AXI_BASEADDR;
	ad9680_dma_init.base = XPAR_AXI_AD9680_DMA_BASEADDR;
#endif

#ifdef ZYNQ
	rx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
	tx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x900000;
#endif

#ifdef MICROBLAZE
	//rx_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000;
	//tx_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x900000;
#endif

#ifdef ALTERA
	ad9144_xcvr.base_address =
		AD9144_JESD204_LINK_MANAGEMENT_BASE;
	ad9144_xcvr.dev.link_pll.base_address =
		AD9144_JESD204_LINK_PLL_RECONFIG_BASE;
	ad9144_xcvr.dev.atx_pll.base_address =
		AD9144_JESD204_LANE_PLL_RECONFIG_BASE;
	ad9144_core.base_address =
		AXI_AD9144_CORE_BASE;
	ad9680_xcvr.base_address =
		AD9680_JESD204_LINK_MANAGEMENT_BASE;
	ad9680_xcvr.dev.link_pll.base_address =
		AD9680_JESD204_LINK_PLL_RECONFIG_BASE;
	ad9680_core.base_address =
		AXI_AD9680_CORE_BASE;
	ad9144_jesd.base_address =
		AD9144_JESD204_LINK_RECONFIG_BASE;
	ad9680_jesd.base_address =
		AD9680_JESD204_LINK_RECONFIG_BASE;

	ad9144_xcvr.dev.channel_pll[0].type = cmu_tx_type;
	ad9680_xcvr.dev.channel_pll[0].type = cmu_cdr_type;
	ad9144_xcvr.dev.channel_pll[0].base_address = AVL_ADXCFG_0_RCFG_S0_BASE;
	ad9680_xcvr.dev.channel_pll[0].base_address = AVL_ADXCFG_0_RCFG_S1_BASE;

	ad9680_dma.base_address = AXI_AD9680_DMA_BASE;
	ad9144_dma.base_address = AXI_AD9144_DMA_BASE;
	rx_xfer.start_address =  0x800000;
	tx_xfer.start_address =  0x900000;

#endif

//******************************************************************************
// clock distribution device (AD9523) configuration
//******************************************************************************

	ad9523_pdata.num_channels = 8;
	ad9523_pdata.channels = &ad9523_channels[0];
	ad9523_param.pdata = &ad9523_pdata;
	ad9523_init(&ad9523_param);

	// dac device-clk-sysref, fpga-clk-sysref

	ad9523_channels[DAC_DEVICE_CLK].channel_num = 1;
	ad9523_channels[DAC_DEVICE_CLK].channel_divider = 1;
	ad9523_channels[DAC_DEVICE_SYSREF].channel_num = 7;
	ad9523_channels[DAC_DEVICE_SYSREF].channel_divider = 128;
	ad9523_channels[DAC_FPGA_CLK].channel_num = 9;
	ad9523_channels[DAC_FPGA_CLK].channel_divider = 2;
	ad9523_channels[DAC_FPGA_SYSREF].channel_num = 8;
	ad9523_channels[DAC_FPGA_SYSREF].channel_divider = 128;

	// adc device-clk-sysref, fpga-clk-sysref

	ad9523_channels[ADC_DEVICE_CLK].channel_num = 13;
	ad9523_channels[ADC_DEVICE_CLK].channel_divider = 1;
	ad9523_channels[ADC_DEVICE_SYSREF].channel_num = 6;
	ad9523_channels[ADC_DEVICE_SYSREF].channel_divider = 128;
	ad9523_channels[ADC_FPGA_CLK].channel_num = 4;
	ad9523_channels[ADC_FPGA_CLK].channel_divider = 2;
	ad9523_channels[ADC_FPGA_SYSREF].channel_num = 5;
	ad9523_channels[ADC_FPGA_SYSREF].channel_divider = 128;

	// VCXO 125MHz

	ad9523_pdata.vcxo_freq = 125000000;
	ad9523_pdata.spi3wire = 1;
	ad9523_pdata.osc_in_diff_en = 1;
	ad9523_pdata.pll2_charge_pump_current_nA = 413000;
	ad9523_pdata.pll2_freq_doubler_en = 0;
	ad9523_pdata.pll2_r2_div = 1;
	ad9523_pdata.pll2_ndiv_a_cnt = 0;
	ad9523_pdata.pll2_ndiv_b_cnt = 6;
	ad9523_pdata.pll2_vco_diff_m1 = 3;
	ad9523_pdata.pll2_vco_diff_m2 = 0;
	ad9523_pdata.rpole2 = 0;
	ad9523_pdata.rzero = 7;
	ad9523_pdata.cpole1 = 2;

	ad9144_xcvr_init.ref_rate_khz = 500000;
	ad9680_xcvr_init.ref_rate_khz = 500000;

//******************************************************************************
// DAC (AD9144) and the transmit path (AXI_ADXCVR,
//	JESD204, AXI_AD9144, TX DMAC) configuration
//******************************************************************************

	//xcvr_getconfig(&ad9144_xcvr);
	//ad9144_xcvr.reconfig_bypass = 1;
#ifdef XILINX
	ad9144_xcvr_init.cpll_enable = 0;
#endif
	ad9144_xcvr_init.lane_rate_khz = 10000000;

	ad9144_jesd_init.octets_per_frame = 1;
	ad9144_jesd_init.frames_per_multiframe = 32;
	ad9144_jesd_init.subclass = 1;

	ad9144_channels[0].dds_dual_tone = 0;
	ad9144_channels[0].dds_frequency_0 = 33*1000*1000;
	ad9144_channels[0].dds_phase_0 = 0;
	ad9144_channels[0].dds_scale_0 = 500000;
	ad9144_channels[0].pat_data = 0xb1b0a1a0;
	ad9144_channels[1].dds_dual_tone = 0;
	ad9144_channels[1].dds_frequency_0 = 11*1000*1000;
	ad9144_channels[1].dds_phase_0 = 0;
	ad9144_channels[1].dds_scale_0 = 500000;
	ad9144_channels[1].pat_data = 0xd1d0c1c0;
	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_DDS;

	ad9144_param.lane_rate_kbps = 10000000;
	ad9144_param.spi3wire = 1;
	ad9144_param.interpolation = 1;
	ad9144_param.pll_enable = 0;
	ad9144_param.jesd204_subclass = 1;
	ad9144_param.jesd204_scrambling = 1;
	ad9144_param.jesd204_mode = 4;
	for(n=0; n<ARRAY_SIZE(ad9144_param.jesd204_lane_xbar); n++)
		ad9144_param.jesd204_lane_xbar[n] = n;

	ad9144_core_init.num_channels = 2;
	ad9144_core->channels = &ad9144_channels[0];

	ad9144_param.stpl_samples[0][0] =
		(0xb1b0a1a0 >> 0)  & 0xffff;
	ad9144_param.stpl_samples[0][1] =
		(0xb1b0a1a0 >> 16) & 0xffff;
	ad9144_param.stpl_samples[0][2] =
		(0xb1b0a1a0 >> 0)  & 0xffff;
	ad9144_param.stpl_samples[0][3] =
		(0xb1b0a1a0 >> 16) & 0xffff;
	ad9144_param.stpl_samples[1][0] =
		(0xd1d0c1c0 >> 0)  & 0xffff;
	ad9144_param.stpl_samples[1][1] =
		(0xd1d0c1c0 >> 16) & 0xffff;
	ad9144_param.stpl_samples[1][2] =
		(0xd1d0c1c0 >> 0)  & 0xffff;
	ad9144_param.stpl_samples[1][3] =
		(0xd1d0c1c0 >> 16) & 0xffff;

//******************************************************************************
// ADC (AD9680) and the receive path ( AXI_ADXCVR,
//	JESD204, AXI_AD9680, TX DMAC) configuration
//******************************************************************************

	ad9680_param.lane_rate_kbps = 10000000;

//	xcvr_getconfig(&ad9680_xcvr);
#ifdef XILINX
	ad9144_xcvr_init.cpll_enable = 0;
#endif
	ad9680_xcvr_init.lane_rate_khz = ad9680_param.lane_rate_kbps;

	//ad9680_jesd.scramble_enable = 1;
	ad9680_jesd_init.octets_per_frame = 1;
	ad9680_jesd_init.frames_per_multiframe = 32;
	ad9680_jesd_init.subclass = 1;

	ad9680_core_init.num_channels = 2;
	//ad9680_core_init.resolution = 14;

//******************************************************************************
// configure the receiver DMA
//******************************************************************************

	ad9680_dma_init.direction = DMA_DEV_TO_MEM;
	//ad9680_dma.transfer = &rx_xfer;
	//rx_xfer.id = 0;
	//rx_xfer.no_of_samples = 32768;

	ad9144_dma_init.direction = DMA_MEM_TO_DEV;
	//ad9144_dma.transfer = &tx_xfer;
	ad9144_dma_init.flags = DMA_LAST;
	//tx_xfer.id = 0;
	//tx_xfer.no_of_samples = dac_buffer_load(ad9144_core, tx_xfer.start_address);

	// change the default JESD configurations, if required
	fmcdaq2_reconfig(&ad9144_param,
			 ad9144_xcvr,
			 &ad9680_param,
			 ad9680_xcvr,
			 ad9523_param.pdata);

//******************************************************************************
// bring up the system
//******************************************************************************

	// setup GPIOs

	gpio_desc *clkd_sync;
	gpio_desc *dac_reset;
	gpio_desc *dac_txen;
	gpio_desc *adc_pd;

	struct gpio_init_param clkd_sync_param;
	struct gpio_init_param dac_reset_param;
	struct gpio_init_param dac_txen_param;
	struct gpio_init_param adc_pd_param;

#ifdef XILINX
	struct xil_gpio_init_param daq2_gpio_param = {
#ifdef PLATFORM_MB
			.type = GPIO_PL,
#else
			.type = GPIO_PS,
#endif
			.device_id = XPAR_AXI_GPIO_DEVICE_ID
	};
#endif

	clkd_sync_param.number = GPIO_CLKD_SYNC;
	clkd_sync_param.extra = &daq2_gpio_param;
	dac_reset_param.number = GPIO_DAC_RESET;
	dac_reset_param.extra = &daq2_gpio_param;
	dac_txen_param.number = GPIO_DAC_TXEN;
	dac_txen_param.extra = &daq2_gpio_param;
	adc_pd_param.number = GPIO_ADC_PD;
	adc_pd_param.extra = &daq2_gpio_param;

	gpio_get(&clkd_sync, &clkd_sync_param);
	gpio_get(&dac_reset, &dac_reset_param);
	gpio_get(&dac_txen,  &dac_txen_param);
	gpio_get(&adc_pd, &adc_pd_param);

	gpio_set_value(clkd_sync, 0);
	gpio_set_value(dac_reset, 0);
	gpio_set_value(dac_txen,  0);
	gpio_set_value(adc_pd,    1);
	mdelay(5);

	gpio_set_value(clkd_sync, 1);
	gpio_set_value(dac_reset, 1);
	gpio_set_value(dac_txen,  1);
	gpio_set_value(adc_pd,    0);

	// setup clocks

	ad9523_setup(&ad9523_device, &ad9523_param);

	// Recommended DAC JESD204 link startup sequence
	//   1. FPGA JESD204 Link Layer
	//   2. FPGA JESD204 PHY Layer
	//   3. DAC
	//
	// Recommended ADC JESD204 link startup sequence
	//   1. ADC
	//   2. FPGA JESD204 PHY Layer
	//   2. FPGA JESD204 Link Layer
	//
	// Both sequences are interleaved here so that the transceivers which might
	// be shared between the DAC and ADC link are enabled at the same time.


	// ADC
	ad9680_setup(&ad9680_device, &ad9680_param);

	// DAC FPGA JESD204 link layer
	axi_jesd204_tx_init(&ad9144_jesd, &ad9144_jesd_init);

	// ADC and DAC FPGA JESD204 PHY layer
#ifdef ALTERA
	xcvr_setup(&ad9144_xcvr);
	xcvr_setup(&ad9680_xcvr);
#endif
#ifdef XILINX
	if (!ad9144_xcvr_init.cpll_enable) {	// DAC_XCVR controls the QPLL reset
		adxcvr_init(&ad9144_xcvr, &ad9144_xcvr_init);
		adxcvr_init(&ad9680_xcvr, &ad9680_xcvr_init);
	} else {				// ADC_XCVR controls the CPLL reset
		adxcvr_init(&ad9680_xcvr, &ad9680_xcvr_init);
		adxcvr_init(&ad9144_xcvr, &ad9144_xcvr_init);
	}
#endif

	// ADC FPGA JESD204 link layer
	axi_jesd204_rx_init(&ad9680_jesd, &ad9680_jesd_init);

	// DAC
	ad9144_setup(&ad9144_device, &ad9144_param);

	// JESD core status
	axi_jesd204_tx_status_read(ad9144_jesd);
	axi_jesd204_rx_status_read(ad9680_jesd);

	// interface core set up
	axi_adc_init(&ad9680_core, &ad9680_core_init);
	axi_dac_init(&ad9144_core, &ad9144_core_init);

	ad9144_status(ad9144_device);

//******************************************************************************
// transport path testing
//******************************************************************************

	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_SED;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_SED;
	axi_dac_data_setup(ad9144_core);
	ad9144_short_pattern_test(ad9144_device, &ad9144_param);

	// PN7 data path test

	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_PN23;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_PN23;
	axi_dac_data_setup(ad9144_core);
	ad9144_param.prbs_type = AD9144_PRBS7;
	ad9144_datapath_prbs_test(ad9144_device, &ad9144_param);

	// PN15 data path test

	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_PN31;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_PN31;
	axi_dac_data_setup(ad9144_core);
	ad9144_param.prbs_type = AD9144_PRBS15;
	ad9144_datapath_prbs_test(ad9144_device, &ad9144_param);

//******************************************************************************
// receive path testing
//******************************************************************************

	ad9680_test(ad9680_device, AD9680_TEST_PN9);
//	if(adc_pn_mon(ad9680_core, ADC_PN9) == -1) {
//		printf("%s ad9680 - PN9 sequence mismatch!\n", __func__);
//	};
	ad9680_test(ad9680_device, AD9680_TEST_PN23);
//	if(adc_pn_mon(ad9680_core, ADC_PN23A) == -1) {
//		printf("%s ad9680 - PN23 sequence mismatch!\n", __func__);
//	};

	// default data

#if DMA_BUFFER
	ad9144_channels[0].sel = DAC_SRC_DMA;
	ad9144_channels[1].sel = DAC_SRC_DMA;
	dac_data_setup(&ad9144_core);

	if(!dmac_start_transaction(ad9144_dma)) {
		printf("daq2: transmit data from memory\n");
	};
#else
	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_DDS;
	axi_dac_data_setup(ad9144_core);

	printf("daq2: setup and configuration is done\n");
#endif
//******************************************************************************
// external loopback - capture data with DMA
//******************************************************************************

	ad9680_test(ad9680_device, AD9680_TEST_OFF);
	//if(!dmac_start_transaction(ad9680_dma)) {
		printf("daq2: RX capture done.\n");
	//};

	/* Memory deallocation for devices and spi */
	ad9144_remove(ad9144_device);
	ad9523_remove(ad9523_device);
	ad9680_remove(ad9680_device);

	/* Memory deallocation for gpios */
	gpio_remove(clkd_sync);
	gpio_remove(dac_reset);
	gpio_remove(dac_txen);
	gpio_remove(adc_pd);

	return(0);
}
