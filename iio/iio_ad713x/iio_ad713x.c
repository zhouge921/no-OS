/***************************************************************************//**
 *   @file   iio_ad713x.c
 *   @brief  Implementation of iio_ad713x.c.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "error.h"
#include "xml.h"
#include "util.h"
#include "iio_ad713x.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static const char ad713x_xml[] =
//		"<device id=\"cf-ad9361-lpc\" name=\"cf-ad9361-lpc\" >"
		"<device id=\"generic\" name=\"generic\" >"
				"<channel id=\"voltage0\" type=\"input\" >"
					"<scan-element index=\"0\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage0_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage0_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage0_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"
				"<channel id=\"voltage1\" type=\"input\" >"
					"<scan-element index=\"1\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage1_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage1_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage1_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"
				"<channel id=\"voltage2\" type=\"input\" >"
					"<scan-element index=\"2\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage2_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage2_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage2_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"
				"<channel id=\"voltage3\" type=\"input\" >"
					"<scan-element index=\"3\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage3_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage3_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage3_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"

			"<channel id=\"voltage4\" type=\"input\" >"
					"<scan-element index=\"4\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage0_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage0_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage0_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"
				"<channel id=\"voltage5\" type=\"input\" >"
					"<scan-element index=\"5\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage1_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage1_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage1_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"
				"<channel id=\"voltage6\" type=\"input\" >"
					"<scan-element index=\"6\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage2_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage2_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage2_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"
				"<channel id=\"voltage7\" type=\"input\" >"
					"<scan-element index=\"7\" format=\"le:S12/16&gt;&gt;0\" />"
					"<attribute name=\"calibphase\" filename=\"in_voltage3_calibphase\" />"
					"<attribute name=\"calibbias\" filename=\"in_voltage3_calibbias\" />"
					"<attribute name=\"calibscale\" filename=\"in_voltage3_calibscale\" />"
					"<attribute name=\"samples_pps\" filename=\"in_voltage_samples_pps\" />"
					"<attribute name=\"sampling_frequency\" filename=\"in_voltage_sampling_frequency\" />"
				"</channel>"

				"<buffer-attribute name=\"watermark\" />"
				"<buffer-attribute name=\"data_available\" />"
				"<debug-attribute name=\"pseudorandom_err_check\" />"
				"<debug-attribute name=\"direct_reg_access\" />"
			"</device>"
		;

/**
 * iio_axi_dac_init() - Init and create iio_axi_dac.
 * @iio_axi_adc:	Pointer to iio_axi_dac.
 * @init:	Init parameters.
 * Return: SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_ad713x_init(struct iio_713x **iio_713x,
			 struct iio_713x_init_par *init)
{
	*iio_713x = calloc(1, sizeof(struct iio_713x));
	if (!(*iio_713x))
		return FAILURE;
	(*iio_713x)->num_channels = init->num_channels;
	(*iio_713x)->spi_eng_msg_cmds = init->spi_eng_msg_cmds;
	(*iio_713x)->msg_cmd_len = init->msg_cmd_len;
	(*iio_713x)->tx_buf_addr = init->tx_buf_addr;
	(*iio_713x)->rx_buf_addr = init->rx_buf_addr;
	(*iio_713x)->dcache_invalidate_range = init->dcache_invalidate_range;

	return SUCCESS;
}

/**
 * iio_axi_dac_remove() - Free the resources allocated by iio_axi_dac_init().
 * @iio_axi_dac:	Pointer to iio_axi_adc.
 * Return: SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_ad713x_remove(struct iio_713x *iio_713x)
{
	free(iio_713x);

	return SUCCESS;
}

/**
 * iio_axi_dac_delete_device() - Delete iio_device.
 * @iio_device:	Structure describing a device, channels and attributes.
 * Return: SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_ad713x_delete_device(struct iio_device *iio_device)
{
	if(iio_device)
		free(iio_device);

	return SUCCESS;
}

/**
 * iio_axi_dac_create_device() - Create structure describing a device, channels and attributes.
 * @device:	Device name.
 * @num_ch:	Number of channels that the device has.
 * Return: iio_device or NULL, in case of failure.
 */
struct iio_device *iio_ad713x_create_device(const char *device_name,
		uint16_t num_ch)
{
	struct iio_device *iio_device;

	iio_device = calloc(1, sizeof(struct iio_device));
	if (!iio_device)
		return NULL;

	iio_device->name = device_name;
	iio_device->num_ch = num_ch;
	iio_device->attributes = NULL; /* no device attribute */
	iio_device->channels = NULL;

	return iio_device;
}

ssize_t iio_ad713x_read_dev(void *iio_inst, char *pbuf, size_t offset,
			     size_t bytes_count, uint32_t ch_mask)
{
	struct iio_713x *iio_713x_inst;
	uint32_t i, j = 0, current_ch = 0;
	uint16_t *pbuf16;
	size_t samples;

	iio_713x_inst = (struct iio_713x *)iio_inst;
	pbuf16 = (uint16_t*)pbuf;
	samples = (bytes_count * iio_713x_inst->num_channels) / hweight8(
				  ch_mask);
	samples /= 2; /* because of uint16_t *pbuf16 = (uint16_t*)pbuf; */
	offset = (offset * iio_713x_inst->num_channels) / hweight8(ch_mask);

	for (i = 0; i < samples; i++)
	{

		if (ch_mask & BIT(current_ch)) {
//			pbuf16[j] = *(uint16_t*)(iio_713x_inst->rx_buf_addr + offset + i * 2);
			pbuf16[j] = i;
			j++;
		}

		if (current_ch < iio_713x_inst->num_channels - 1)
			current_ch++;
		else
			current_ch = 0;
	}

	return bytes_count;
}

ssize_t iio_ad713x_get_xml(char** xml, struct iio_device *iio_dev) {

	*xml = calloc(1, strlen(ad713x_xml) + 1);
	if (!(*xml))
		return -ENOMEM;

	memcpy(*xml, ad713x_xml, strlen(ad713x_xml));

	return SUCCESS;
}
