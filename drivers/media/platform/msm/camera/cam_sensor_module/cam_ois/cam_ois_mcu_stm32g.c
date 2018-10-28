/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/vmalloc.h>
#include <linux/ctype.h>
#include <linux/crc32.h>
#include <cam_sensor_cmn_header.h>
#include <cam_sensor_util.h>
#include <cam_sensor_io.h>
#include <cam_req_mgr_util.h>
#include "cam_debug_util.h"
#include <cam_sensor_i2c.h>
#include "cam_ois_mcu_stm32g.h"
#include "cam_ois_thread.h"
#include "cam_ois_core.h"

#define OIS_FW_STATUS_OFFSET	(0x00FC)
#define OIS_FW_STATUS_SIZE		(4)
#define OIS_HW_VERSION_SIZE 	(3)
#define OIS_MCU_VERSION_SIZE 	(4)
#define OIS_MCU_VDRINFO_SIZE 	(4)
#define OIS_HW_VERSION_OFFSET	(0xAFF1)
#define OIS_FW_VERSION_OFFSET	(0xAFED)
#define OIS_MCU_VERSION_OFFSET	(0x80F8)
#define OIS_MCU_VDRINFO_OFFSET	(0x807C)
#define OIS_FW_PATH "/vendor/lib/camera"
#define OIS_FW_DOM_NAME "ois_fw_dom.bin"
#define OIS_FW_SEC_NAME "ois_fw_sec.bin"
#define OIS_MCU_FW_NAME "ois_mcu_fw.bin"
#define OIS_USER_DATA_START_ADDR (0xB400)
#define OIS_FW_UPDATE_PACKET_SIZE (256)
//#define OIS_FW_UPDATE_PACKET_SIZE (32)
#define PROGCODE_SIZE			(1024 * 44)
#define MAX_RETRY_COUNT 		 (3)
#define CAMERA_OIS_EXT_CLK_12MHZ 0xB71B00
#define CAMERA_OIS_EXT_CLK_17MHZ 0x1036640
#define CAMERA_OIS_EXT_CLK_19MHZ 0x124F800
#define CAMERA_OIS_EXT_CLK_24MHZ 0x16E3600
#define CAMERA_OIS_EXT_CLK_26MHZ 0x18CBA80
#define CAMERA_OIS_EXT_CLK CAMERA_OIS_EXT_CLK_24MHZ

#undef CAM_INFO
#define CAM_INFO CAM_ERR
#undef CAM_DBG
#define CAM_DBG CAM_ERR

extern char ois_fw_full[40];
extern char ois_debug[40];

int total_fw_size;

//for mcu sysboot

/* Target specific definitions
 *  1. Startup delay 
 *     STM32 target needs at least t-ms delay after reset msecs
 *  2. Target I2C slave dddress
 */
const uint32_t sysboot_i2c_startup_delay = 50; /* msec */
const uint16_t sysboot_i2c_slave_address = 0x51;

/* STM32MCU PID */
const uint16_t product_id = 0x460;

/* Flash memory characteristics from target datasheet (msec unit) */
const uint32_t flash_prog_time = 37; /* per page or sector */
const uint32_t flash_full_erase_time = 40 * 32; /* 2K erase time(40ms) * 32 pages */
const uint32_t flash_page_erase_time = 36; /* per page or sector */

/* Memory map specific */

typedef struct
{
    uint32_t size;
    uint32_t count;
} sysboot_page_type;

typedef struct
{
    uint32_t flashbase;  /* flash memory starting address */
    uint32_t sysboot;    /* system memory starting address */
    uint32_t optionbyte; /* option byte starting address */
    sysboot_page_type *pages;
} sysboot_map_type;

/* Flash memory page(or sector) structure */
sysboot_page_type memory_pages[] = {
  {2048, 32},
  {   0,  0}
};

sysboot_map_type memory_map =
{
    0x08000000, /* flash memory starting address */
    0x1FFF0000, /* system memory starting address */
    0x1FFF7800, /* option byte starting address */
    (sysboot_page_type *)memory_pages,
};


static int ois_mcu_chip_command(struct cam_ois_ctrl_t *o_ctrl, int command);

/**
  * @brief  Connect to the device and do SYNC transaction.
  *         Reset(NRST) and BOOT0 pin control
  * @param  protocol
  * @retval 0: success, others are fail.
  */
int sysboot_connect(struct cam_ois_ctrl_t *o_ctrl)
{
    int ret = 0;
    CAM_ERR(CAM_OIS, "mcu sysboot_connect");

    /* STEP1. Turn to the MCU system boot mode */
    {
        /* Assert NRST reset */
        gpio_direction_output(o_ctrl->reset_ctrl_gpio, 0);
        /* Change BOOT pins to System Bootloader */
        gpio_direction_output(o_ctrl->boot0_ctrl_gpio, 1);
        /* NRST should hold down (Vnf(NRST) > 300 ns), considering capacitor, give enough time */
        mdelay(BOOT_NRST_PULSE_INTVL);
        /* Release NRST reset */
        gpio_direction_output(o_ctrl->reset_ctrl_gpio, 1);
        /* Put little delay for the target prepared */
        mdelay(BOOT_I2C_STARTUP_DELAY);
        gpio_direction_output(o_ctrl->boot0_ctrl_gpio, 0);
    }
    /* STEP2. Send SYNC frame then waiting for ACK */
    ret = ois_mcu_chip_command(o_ctrl, BOOT_I2C_CMD_SYNC);

    if (ret >= 0)
    {
        /* STEP3. When I2C mode, Turn to the MCU system boot mode once again for protocol == SYSBOOT_PROTO_I2C */
        {
			/* Assert NRST reset */
			gpio_direction_output(o_ctrl->reset_ctrl_gpio, 0);
			gpio_direction_output(o_ctrl->boot0_ctrl_gpio, 1);
			/* NRST should hold down (Vnf(NRST) > 300 ns), considering capacitor, give enough time */
			mdelay(BOOT_NRST_PULSE_INTVL);
			/* Release NRST reset */
			gpio_direction_output(o_ctrl->reset_ctrl_gpio, 1);
			/* Put little delay for the target prepared */
			mdelay(BOOT_I2C_STARTUP_DELAY);
			gpio_direction_output(o_ctrl->boot0_ctrl_gpio, 0);
        }
    }

    return ret;
}

/**
  * @brief  Disconnect the device
  *         Reset(NRST) and BOOT0 pin control
  * @param  protocol
  * @retval None
  */
void sysboot_disconnect(struct cam_ois_ctrl_t *o_ctrl)
{
    CAM_ERR(CAM_OIS, "sysboot disconnect");
    /* Change BOOT pins to Main flash */
    gpio_direction_output(o_ctrl->boot0_ctrl_gpio, 0);
    mdelay(1);
    /* Assert NRST reset */
    gpio_direction_output(o_ctrl->reset_ctrl_gpio, 0);
    /* NRST should hold down (Vnf(NRST) > 300 ns), considering capacitor, give enough time */
    mdelay(BOOT_NRST_PULSE_INTVL);
    /* Release NRST reset */
    gpio_direction_output(o_ctrl->reset_ctrl_gpio, 1);
	mdelay(150);
}



/**
  * @brief  Convert the device memory map to erase param. format.
  *         (start page and numbers to be erased)
  * @param  device memory address, length, erase ref.
  * @retval 0 is success, others are fail.
  */
int sysboot_conv_memory_map(struct cam_ois_ctrl_t *o_ctrl, uint32_t address, size_t len, sysboot_erase_param_type *erase)
{
    sysboot_page_type *map = memory_map.pages;
    int found = 0;
    int total_bytes = 0, total_pages = 0;
    int ix = 0;
    int unit = 0;
    CAM_ERR(CAM_OIS, "mcu");

    /* find out the matched starting page number and total page count */

    for (ix = 0; map[ix].size != 0; ++ix)
    {
        for (unit = 0; unit < map[ix].count; ++unit)
        {
            /* MATCH CASE: Starting address aligned and page number to be erased */
            if (address == memory_map.flashbase + total_bytes)
            {
                found++;
                erase->page = total_pages;
            }
            total_bytes += map[ix].size;
            total_pages++;
            /* MATCH CASE: End of page number to be erased */
            if ((found == 1) && (len <= total_bytes))
            {
                found++;
                erase->count = total_pages - erase->page;
            }
        }
    }

    if (found < 2)
    {
        /* Not aligned address or too much length inputted */
        return BOOT_ERR_DEVICE_MEMORY_MAP;
    }

    if ((address == memory_map.flashbase) && (erase->count == total_pages))
    {
        erase->page = 0xFFFF; /* mark the full erase */
    }

    return 0;
}


//sysboot.c
/**
  * @brief  Calculate 8-bit checksum.
  * @param  source data and length
  * @retval checksum value.
  */
uint8_t sysboot_checksum(uint8_t *src, uint32_t len)
{
    uint8_t csum = *src++;
    //CAM_ERR(CAM_OIS, "mcu");

    if (len)
    {
        while (--len)
        {
            csum ^= *src++;
        }
    }
    else
    {
        csum = 0; /* error (no length param) */
    }

    return csum;
}

//sysboot_i2c.c
//static uint8_t xmit[BOOT_I2C_ERASE_PARAM_LEN(BOOT_I2C_MAX_PAYLOAD_LEN)] = {0, };

/**
  * @brief  Waiting for an host ACK response
  * @param  timeout (msec)
  * @retval 0 is success, others are fail.
  */
static int sysboot_i2c_wait_ack(struct cam_ois_ctrl_t *o_ctrl, unsigned long timeout)
{
    int ret = 0;
    uint32_t retry = 3;
    unsigned char resp = 0;

    while(retry--)
    {
        ret = i2c_master_recv(o_ctrl->io_master_info.client, &resp,1);
        if(ret >= 0)
        {
            if(resp == BOOT_I2C_RESP_ACK)
            {
                //CAM_ERR(CAM_OIS, "[mcu] wait ack success 0x%x ",resp);
            }else{
                CAM_ERR(CAM_OIS, "[mcu] wait ack failed 0x%x ",resp);
            }
            //return resp;
            return 0;
        }
        else
        {
            CAM_ERR(CAM_OIS, "[mcu] failed resp is 0x%x ,ret is %d  ", resp,ret);
            if (time_after(jiffies,timeout))
            {
                ret = -ETIMEDOUT;
                break;
            }
            mdelay(BOOT_I2C_INTER_PKT_BACK_INTVL);
        }
    }
    return -1;

}
#if 0


/**
  * @brief  Transmit the raw packet datas.
  * @param  source, length, timeout (msec)
  * @retval 0 is success, others are fail.
  */
static int sysboot_i2c_send(struct cam_ois_ctrl_t *o_ctrl, uint8_t *cmd, uint32_t len, unsigned long timeout)
{
    int ret = 0;
    int retry = 0;
    int i = 0;

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, len);
        if (ret < 0)
        {

            if (time_after(jiffies,timeout))
            {
                ret = -ETIMEDOUT;
                break;
            }
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            CAM_ERR(CAM_OIS, "[mcu] send data fail ");
            continue;
        }
    }
    CAM_ERR(CAM_OIS, "client->addr=0x%x success send: %d Byte", o_ctrl->io_master_info.client->addr, ret);
    for(i = 0; i < ret; i++)
    {
        CAM_ERR(CAM_OIS, "[mcu] send data : 0x%x ", cmd[i]);
    }
    return ret;
}

/**
  * @brief  Receive the raw packet datas.
  * @param  destination, length, timeout (msec)
  * @retval 0 is success, others are fail.
  */

static int sysboot_i2c_recv(struct cam_ois_ctrl_t *o_ctrl, uint8_t *recv, uint32_t len, unsigned long timeout)
{
    int ret = 0;
    int retry = 0;
    int i = 0;

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_recv(o_ctrl->io_master_info.client, recv, len);

        if (ret < 0)
        {
            if (time_after(jiffies,timeout))
            {
                ret = -ETIMEDOUT;
                break;
            }
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            CAM_ERR(CAM_OIS, "[mcu] recv data fail ");
            continue;

        }
    }
    for(i = 0; i < ret; i++)
    {
        CAM_ERR(CAM_OIS, "[mcu] recv data : 0x%x ", recv[i]);
    }

    return ret;

}
#endif
/**
  * @brief  Get device PID or Get device BL version
  * @param  None
  * @retval 0 is success, others are fail.
  */
static int sysboot_i2c_get_info(struct cam_ois_ctrl_t *o_ctrl,
           uint8_t *cmd, uint32_t cmd_size, uint32_t data_size)
{
    uint8_t recv[BOOT_I2C_RESP_GET_ID_LEN] = {0, };
    int ret = 0;
    int retry = 0;

    CAM_ERR(CAM_OIS, "mcu 0x%x 0x%x", cmd[0], cmd[1]);
    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, cmd_size);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "mcu send data fail ret = %d", ret);
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "mcu wait ack fail ret = %d", ret);
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* receive payload */
        ret = i2c_master_recv(o_ctrl->io_master_info.client, recv, data_size);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "mcu receive payload fail ret = %d", ret);
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "mcu wait ack fail ret = %d", ret);
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
	if(cmd[0] == BOOT_I2C_CMD_GET_ID){
        memcpy((void *)&(o_ctrl->info.id), &recv[1], recv[0] + 1);
        o_ctrl->info.id = NTOHS(o_ctrl->info.id);
        CAM_ERR(CAM_OIS, "success get info id %d",o_ctrl->info.id);
	}else if(cmd[0] == BOOT_I2C_CMD_GET_VER){
        memcpy((void *)&(o_ctrl->info.ver), recv , 1);
        CAM_ERR(CAM_OIS, "success get info version %d",o_ctrl->info.ver);	
    }

        return 0;
    }

    return ret + cmd[0];
}

/**
  * @brief  SYNC transaction
  * @param  None
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_sync(struct cam_ois_ctrl_t *o_ctrl, uint8_t *cmd)
{
    int ret = 0;

    CAM_ERR(CAM_OIS, "mcu");
    /* set it and wait for it to be so */
    ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, 1);
	CAM_ERR(CAM_OIS,"i2c client addr 0x%x ",o_ctrl->io_master_info.client->addr);
	if(ret >= 0){
		CAM_ERR(CAM_OIS,"success connect to target mcu ");
	}else{
	    CAM_ERR(CAM_OIS,"failed connect to target mcu ");
	}
    return ret;
}

/**
  * @brief  Get device info.(PID, BL ver, etc,.)
  * @param  None
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_info(struct cam_ois_ctrl_t *o_ctrl)
{
    int ret = 0;
    CAM_ERR(CAM_OIS, "mcu");	
    memset((void *)&(o_ctrl->info), 0x00, sizeof(o_ctrl->info));
	ois_mcu_chip_command(o_ctrl, BOOT_I2C_CMD_GET_ID);
	ois_mcu_chip_command(o_ctrl, BOOT_I2C_CMD_GET_VER);
    return ret;
}

/**
  * @brief  Read the device memory
  * @param  source(address), destination, length
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_read(struct cam_ois_ctrl_t *o_ctrl, uint32_t address, uint8_t *dst, size_t len)
{
    uint8_t cmd[BOOT_I2C_REQ_CMD_LEN] = {0, };  //BOOT_I2C_REQ_CMD_LEN = 2
    uint8_t startaddr[BOOT_I2C_REQ_ADDRESS_LEN] = {0, };  //BOOT_I2C_REQ_ADDRESS_LEN = 5
    uint8_t nbytes[BOOT_I2C_READ_PARAM_LEN] = {0, }; //BOOT_I2C_READ_PARAM_LEN = 2
    int ret = 0;
    int retry = 0;

    /* build command */
    cmd[0] = BOOT_I2C_CMD_READ;
    cmd[1] = ~cmd[0];

    /* build address + checksum */
    *(uint32_t *)startaddr = HTONL(address);
    startaddr[BOOT_I2C_ADDRESS_LEN] = sysboot_checksum(startaddr, BOOT_I2C_ADDRESS_LEN);

    /* build number of bytes + checksum */
    nbytes[0] = len - 1;
    nbytes[1] = ~nbytes[0];
    CAM_ERR(CAM_OIS, "read address 0x%x",address);

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, sizeof(cmd));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* transmit address */
        ret = i2c_master_send(o_ctrl->io_master_info.client, startaddr, sizeof(startaddr));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
	
        /* transmit number of bytes */
        ret = i2c_master_send(o_ctrl->io_master_info.client, nbytes, sizeof(nbytes));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
	
        /* receive payload */
        ret = i2c_master_recv(o_ctrl->io_master_info.client, dst, len);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }	
        return 0;
    }

    return ret + BOOT_ERR_API_READ;
}

/**
  * @brief  Write the contents to the device memory
  * @param  destination(address), source, length
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_write(struct cam_ois_ctrl_t *o_ctrl, uint32_t address, uint8_t *src, size_t len)
{
    uint8_t cmd[BOOT_I2C_REQ_CMD_LEN] = {0, };
    uint8_t startaddr[BOOT_I2C_REQ_ADDRESS_LEN] = {0, };
    int ret = 0;
    int retry = 0;
    char * buf = NULL;
    /* build command */
    cmd[0] = BOOT_I2C_CMD_WRITE;
    cmd[1] = ~cmd[0];

    /* build address + checksum */
    *(uint32_t *)startaddr = HTONL(address);
    startaddr[BOOT_I2C_ADDRESS_LEN] = sysboot_checksum(startaddr, BOOT_I2C_ADDRESS_LEN);

    /* build number of bytes + checksum */
    CAM_ERR(CAM_OIS, "mcu address = 0x%x", address);

    buf = kzalloc(len + 2, GFP_KERNEL);
    if (!buf)
    	return -ENOMEM;
    buf[0] = len -1;
	memcpy(&buf[1], src, len);
	buf[len+1] = sysboot_checksum(buf, len + 1);

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, 2);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "[mcu] txdata fail ");
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "[mcu]mcu_wait_ack fail after txdata ");
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }


        /* transmit address */
        ret = i2c_master_send(o_ctrl->io_master_info.client, startaddr, 5);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "[mcu] txdata fail ");
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "[mcu]mcu_wait_ack fail after txdata ");
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
	
        /* transmit number of bytes + datas */
	
        ret = i2c_master_send(o_ctrl->io_master_info.client, buf, BOOT_I2C_WRITE_PARAM_LEN(len));
        if (ret < 0)
        {
        	CAM_ERR(CAM_OIS, "[mcu] txdata fail ");
        	mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
        	continue;
        }
        //mdelay(len);
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WRITE_TMOUT);
        if (ret < 0)
        {
            CAM_ERR(CAM_OIS, "[mcu]mcu_wait_ack fail after txdata ");
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        kfree(buf);

        return 0;
    }
    kfree(buf);

    return ret + BOOT_ERR_API_WRITE;
}

/**
  * @brief  Erase the device memory
  * @param  destination(address), length
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_erase(struct cam_ois_ctrl_t *o_ctrl, uint32_t address, size_t len)
{
    uint8_t cmd[BOOT_I2C_REQ_CMD_LEN] = {0, };
    sysboot_erase_param_type erase;
    uint8_t xmit_bytes = 0;
    int ret = 0;
    int retry = 0;
	uint8_t xmit[1024] = {0, };

    /* build command */
    cmd[0] = BOOT_I2C_CMD_ERASE;
    cmd[1] = ~cmd[0];

    /* build erase parameter */
    ret = sysboot_conv_memory_map(o_ctrl, address, len, &erase);
    if (ret < 0)
    {
        return ret + BOOT_ERR_API_ERASE;
    }
    CAM_ERR(CAM_OIS, "erase.page 0x%x",erase.page);

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* build full erase command */
        if (erase.page == 0xFFFF)
        {
            *(uint16_t *)xmit = (uint16_t)erase.page;
        }
        /* build page erase command */
        else
        {
            *(uint16_t *)xmit = HTONS((erase.count - 1));
        }
        xmit_bytes = sizeof(uint16_t);
        xmit[xmit_bytes] = sysboot_checksum(xmit, xmit_bytes);
        xmit_bytes++;
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, sizeof(cmd));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }

        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* transmit parameter */
	
        ret = i2c_master_send(o_ctrl->io_master_info.client, xmit, xmit_bytes);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
		//mdelay(2*32);
        ret = sysboot_i2c_wait_ack(o_ctrl, (erase.page == 0xFFFF) ? BOOT_I2C_FULL_ERASE_TMOUT : BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
		
        /* case of page erase */
        if (erase.page != 0xFFFF)
        {
            /* build page erase parameter */
            register int ix;
            register uint16_t *pbuf = (uint16_t *)xmit;
            for (ix = 0; ix < erase.count; ++ix)
            {
                pbuf[ix] = HTONS((erase.page + ix));
            }
			CAM_ERR(CAM_OIS, "erase.count %d",erase.count);
            CAM_ERR(CAM_OIS, "&pbuf[ix] %p,xmit %p",&pbuf[ix],xmit);
            xmit_bytes = 2 * erase.count;
            *((uint8_t *)&pbuf[ix]) = sysboot_checksum(xmit, xmit_bytes);
			CAM_ERR(CAM_OIS, "xmit_bytes %d",xmit_bytes);
			xmit_bytes++;
            /* transmit parameter */
            ret = i2c_master_send(o_ctrl->io_master_info.client, xmit, xmit_bytes);
            if (ret)
            {
                mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
                continue;
            }
			//mdelay(2*32);
            /* wait for ACK response */
            ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_PAGE_ERASE_TMOUT(erase.count + 1));
            if (ret < 0)
            {
                mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
                continue;
            }
        }
        CAM_ERR(CAM_OIS, "erase finish");
        return 0;
    }

    return ret + BOOT_ERR_API_ERASE;
}

/**
  * @brief  Go to specific address of the device (for starting application)
  * @param  branch destination(address)
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_go(struct cam_ois_ctrl_t *o_ctrl, uint32_t address)
{
    uint8_t cmd[BOOT_I2C_REQ_CMD_LEN] = {0, };
    uint8_t startaddr[BOOT_I2C_REQ_ADDRESS_LEN] = {0, };
    int ret = 0;
    int retry = 0;

    /* build command */
    cmd[0] = BOOT_I2C_CMD_GO;
    cmd[1] = ~cmd[0];

    /* build address + checksum */
    *(uint32_t *)startaddr = HTONL(address);
    startaddr[BOOT_I2C_ADDRESS_LEN] = sysboot_checksum(startaddr, BOOT_I2C_ADDRESS_LEN);
    CAM_ERR(CAM_OIS, "mcu");

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, sizeof(cmd));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* transmit address */
        ret = i2c_master_send(o_ctrl->io_master_info.client, startaddr, sizeof(startaddr));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_WAIT_RESP_TMOUT + 200); /* 200??? */
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }

        return 0;
    }

    return ret + BOOT_ERR_API_GO;
}

/**
  * @brief  Unprotect the write protect
  * @param  None
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_write_unprotect(struct cam_ois_ctrl_t *o_ctrl)
{
    uint8_t cmd[BOOT_I2C_REQ_CMD_LEN] = {0, };
    int ret = 0;
    int retry = 0;

    /* build command */
    cmd[0] = BOOT_I2C_CMD_WRITE_UNPROTECT;
    cmd[1] = ~cmd[0];
    CAM_ERR(CAM_OIS, "mcu");

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, sizeof(cmd));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_FULL_ERASE_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_FULL_ERASE_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }

        return 0;
    }

    return ret + BOOT_ERR_API_WRITE_UNPROTECT;
}

/**
  * @brief  Unprotect the read protect
  * @param  None
  * @retval 0 is success, others are fail.
  */
int sysboot_i2c_read_unprotect(struct cam_ois_ctrl_t *o_ctrl)
{
    uint8_t cmd[BOOT_I2C_REQ_CMD_LEN] = {0, };
    int ret = 0;
    int retry = 0;

    /* build command */
    cmd[0] = BOOT_I2C_CMD_READ_UNPROTECT;
    cmd[1] = ~cmd[0];
    CAM_ERR(CAM_OIS, "mcu");

    for (retry = 0; retry < BOOT_I2C_SYNC_RETRY_COUNT; ++retry)
    {
        /* transmit command */
        ret = i2c_master_send(o_ctrl->io_master_info.client, cmd, sizeof(cmd));
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_FULL_ERASE_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }
        /* wait for ACK response */
        ret = sysboot_i2c_wait_ack(o_ctrl, BOOT_I2C_FULL_ERASE_TMOUT);
        if (ret < 0)
        {
            mdelay(BOOT_I2C_SYNC_RETRY_INTVL);
            continue;
        }

        return 0;
    }

    return ret + BOOT_ERR_API_READ_UNPROTECT;
}

/* ---------------------------------------------------------------------- */

static int ois_mcu_chip_command(struct cam_ois_ctrl_t *o_ctrl, int command)
{
    /* build command */
    uint8_t cmd[BOOT_I2C_REQ_CMD_LEN] = {0, };
    int ret = 0;
    CAM_ERR(CAM_OIS, "[mcu] start");

    /* execute the command */
    switch(command)
    {
    case BOOT_I2C_CMD_GET:
        cmd[0] = 0x00;
        break;

    case BOOT_I2C_CMD_GET_VER:
        cmd[0] = 0x01;
        cmd[1] = ~cmd[0];
        ret = sysboot_i2c_get_info(o_ctrl, cmd ,2 ,1);
        break;

    case BOOT_I2C_CMD_GET_ID:
        cmd[0] = 0x02;
        cmd[1] = ~cmd[0];
        ret = sysboot_i2c_get_info(o_ctrl, cmd ,2 ,3);
        break;

    case BOOT_I2C_CMD_READ:
        cmd[0] = 0x11;
        break;

    case BOOT_I2C_CMD_WRITE:
        cmd[0] = 0x31;
        break;

    case BOOT_I2C_CMD_ERASE:
        cmd[0] = 0x44;
        break;

    case BOOT_I2C_CMD_GO:
        cmd[0] = 0x21;
        break;

    case BOOT_I2C_CMD_WRITE_UNPROTECT:
        cmd[0] = 0x73;
        break;

    case BOOT_I2C_CMD_READ_UNPROTECT:
        cmd[0] = 0x92;
        break;

    case BOOT_I2C_CMD_SYNC:
		/* UNKNOWN command */
        cmd[0] = 0xFF;
		sysboot_i2c_sync(o_ctrl, cmd);
        break;

    default:
        break;
        return -EINVAL;
    }

    return ret ;

}


/**
  * @brief  Validation check for TARGET
  * @param  None
  * @retval 0: success, others are fail
  */
int target_validation(struct cam_ois_ctrl_t *o_ctrl)
{
    int ret = 0;
    CAM_DBG(CAM_OIS, "Start target validation");
    /* Connection ------------------------------------------------------------- */
    ret = sysboot_connect(o_ctrl);
    if (ret < 0)
    {
        CAM_DBG(CAM_OIS, "Error: Cannot connect to the target (%d) but skip", ret);
        goto validation_fail;
    }
    CAM_DBG(CAM_OIS, "1. Connection OK");

    ret = sysboot_i2c_info(o_ctrl);
    if (ret < 0)
    {
        CAM_DBG(CAM_OIS, "Error: Failed to collect the target info  (%d)", ret);
        goto validation_fail;
    }

    CAM_DBG(CAM_OIS, " 2. Get target info OK Target PID: 0x%X, Bootloader version: 0x%X", o_ctrl->info.id, o_ctrl->info.ver);

    return 0;

validation_fail:
    sysboot_disconnect(o_ctrl);
    CAM_DBG(CAM_OIS, " Failed: target disconnected");

    return -1;

}

/**
  * @brief  Getting STATUS of the TARGET empty check
  * @param  None
  * @retval 0: empty check reset, 1: empty check set, others are fail
  */
int target_empty_check_status(struct cam_ois_ctrl_t *o_ctrl)
{
    uint32_t value = 0;
    int ret = 0;
    CAM_ERR(CAM_OIS, "mcu");

    /* Read first flash memory word ------------------------------------------- */
    ret = sysboot_i2c_read(o_ctrl, memory_map.flashbase, (uint8_t *)&value, sizeof(value));

    if (ret < 0)
    {
        CAM_DBG(CAM_OIS, "[INF] Error: Failed to read word for empty check (%d)", ret);
        goto empty_check_status_fail;
    }

    CAM_DBG(CAM_OIS, "[INF] Flash Word: 0x%08X", value);

    if (value == 0xFFFFFFFF)
    {
        return 1;
    }

    return 0;

empty_check_status_fail:

    return -1;
}

int target_option_update(struct cam_ois_ctrl_t *o_ctrl){
	int ret = 0;
	uint32_t optionbyte = 0;
	int retry = 3;
	CAM_ERR(CAM_OIS, "[mao]read option byte begin ");


    for(retry = 0; retry < 3; retry ++ ){
	    ret = sysboot_i2c_read(o_ctrl,memory_map.optionbyte, (uint8_t *)&optionbyte, sizeof(optionbyte));
	    if(ret < 0){
		    ret = sysboot_i2c_read_unprotect(o_ctrl);
		    if(ret < 0){
			    CAM_ERR(CAM_OIS, "[mao]ois_mcu_read_unprotect failed ");
		    }else{
		        CAM_ERR(CAM_OIS, "[mao]ois_mcu_read_unprotect ok ");
		    }
			//try connection again
		    continue;
	    }

        if (optionbyte & (1 << 24)) {
        /* Option byte write ---------------------------------------------------- */
            optionbyte &= ~(1 << 24);
	        ret = sysboot_i2c_write(o_ctrl,memory_map.optionbyte, (uint8_t *)&optionbyte, sizeof(optionbyte));
            if(ret < 0){
				msleep(1);
				continue;
            }
			CAM_ERR(CAM_OIS, "[mao]write option byte ok "); 
			//try connection again
		}else{
		    CAM_ERR(CAM_OIS, "[mao]option byte is 0, return success ");
			return 0;
		}
    }

	return ret;
}

int target_read_hwver(struct cam_ois_ctrl_t *o_ctrl){
	int ret = 0;
	int i = 0;

	uint32_t addr[4] = {0, };
	uint8_t dst = 0;
	uint32_t address = 0;
	
	for(i = 0; i<4 ; i++){
		addr[i] = 0x80F8+i+memory_map.flashbase;
	    address = addr[i];
	    ret = sysboot_i2c_read(o_ctrl,address,&dst,1);

	    if(ret < 0){
		    CAM_ERR(CAM_OIS,"read fwver addr 0x%x fail",address);
	    }else{
	    	CAM_ERR(CAM_OIS,"read fwver addr 0x%x dst 0x%x",address,dst);
	    }
	}
	return ret ;
}

int target_read_vdrinfo(struct cam_ois_ctrl_t *o_ctrl){
	int ret = 0;
	int i = 0;
	uint32_t addr[4] = {0, };
	unsigned char dst[4] ;
	unsigned char data[256] ;
	uint32_t address = 0;
	
	for(i = 0; i<4 ; i++){
		addr[i] = 0x807C+i+memory_map.flashbase;
	    address = addr[i];
	    ret = sysboot_i2c_read(o_ctrl,address,dst,4);

	    if(ret < 0){
		    CAM_ERR(CAM_OIS,"read fwver addr 0x%x fail",address);
	    }else{
	    	CAM_ERR(CAM_OIS,"read fwver addr 0x%x dst [0] 0x%x,[1] 0x%x,[2] 0x%x,[3] 0x%x,",
				address,dst[0],dst[1],dst[2],dst[3]);
	    }
	}
	address = memory_map.flashbase + 0x8000;
    ret = sysboot_i2c_read(o_ctrl,address,data,256);
	//strncpy(dst,data+0x7c,4);
	strncpy(dst,data+124,4);
	CAM_ERR(CAM_OIS,"read fwver addr 0x%x dst [0] 0x%x,[1] 0x%x,[2] 0x%x,[3] 0x%x,",
		address + 0x7C,dst[0],dst[1],dst[2],dst[3]);	
	return ret ;	
}


int target_empty_check_clear(struct cam_ois_ctrl_t * o_ctrl)
{
	int ret = 0;
	uint32_t optionbyte = 0;
	
	 /* Option Byte read ------------------------------------------------------- */
	 ret = sysboot_i2c_read(o_ctrl, memory_map.optionbyte, (uint8_t *)&optionbyte, sizeof(optionbyte));
	 if (ret < 0) {
       CAM_ERR(CAM_OIS,"Option Byte read fail");
	   goto empty_check_clear_fail;
	 }
	
	 CAM_ERR(CAM_OIS,"Option Byte read 0x%x ",optionbyte);

	
	 /* Option byte write (dummy: readed value) -------------------------------- */
	 ret = sysboot_i2c_write(o_ctrl, memory_map.optionbyte, (uint8_t *)&optionbyte, sizeof(optionbyte));
	 if (ret < 0) {
	   CAM_ERR(CAM_OIS,"Option Byte write fail");
	   goto empty_check_clear_fail;
	 }
	 CAM_ERR(CAM_OIS,"Option Byte write 0x%x ",optionbyte);

	
	 /* Put little delay for Target program option byte and self-reset */
	 mdelay(50);
		
	 return 0;

empty_check_clear_fail:
	
	 return -1;

}

int target_normal_on(struct cam_ois_ctrl_t * o_ctrl)
{
    int ret = 0;
	/* Release NRST reset */
	gpio_direction_output(o_ctrl->reset_ctrl_gpio, 1);
	/* Put little delay for the target prepared */
	mdelay(1);
	gpio_direction_output(o_ctrl->boot0_ctrl_gpio, 0);
	mdelay(1);
    return ret;
}

// ois

int cam_ois_i2c_byte_read(struct cam_ois_ctrl_t *o_ctrl, uint32_t addr, uint16_t *data)
{
	int rc = 0;
	uint32_t temp;

	o_ctrl->io_master_info.client->addr = 0xA2;
    //o_ctrl->io_master_info.client->addr = 0x1C;
	rc = camera_io_dev_read(&o_ctrl->io_master_info, addr, &temp,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0) {
		CAM_DBG(CAM_OIS, "ois i2c byte read failed addr : 0x%x data : 0x%x", addr, *data);
		o_ctrl->io_master_info.client->addr = 0x51;
		return rc;
	}
	*data = (uint16_t)temp;

	CAM_DBG(CAM_OIS, "addr = 0x%x data: 0x%x", addr, *data);
	o_ctrl->io_master_info.client->addr = 0x51;
	return rc;
}

int cam_ois_i2c_byte_write(struct cam_ois_ctrl_t *o_ctrl, uint32_t addr, uint16_t data)
{
	int rc = 0;
	struct cam_sensor_i2c_reg_array reg_setting;

	reg_setting.reg_addr = addr;
	reg_setting.reg_data = data;
    o_ctrl->io_master_info.client->addr = 0xA2;
	rc = cam_qup_i2c_write(&o_ctrl->io_master_info, &reg_setting,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

	if (rc < 0) {
		CAM_DBG(CAM_OIS, "ois i2c byte write failed addr : 0x%x data : 0x%x", addr, data);
		o_ctrl->io_master_info.client->addr = 0x51;
		return rc;
	}

	CAM_DBG(CAM_OIS, "addr = 0x%x data: 0x%x", addr, data);
	o_ctrl->io_master_info.client->addr = 0x51;
	return rc;
}

int cam_ois_wait_idle(struct cam_ois_ctrl_t *o_ctrl, int retries)
{
	uint16_t status = 0;
	int ret = 0;

	/* check ois status if it`s idle or not */
	/* OISSTS register(0x0001) 1Byte read */
	/* 0x01 == IDLE State */
	do {
		ret = cam_ois_i2c_byte_read(o_ctrl, 0x0001, &status);
		if (status == 0x01)
			break;
		if (--retries < 0) {
			if (ret < 0) {
				CAM_ERR(CAM_OIS, "failed due to i2c fail");
				return -EIO;
			}
			CAM_ERR(CAM_OIS, "ois status is not idle, current status %d", status);
			return -EBUSY;
		}
		usleep_range(10000, 11000);
	} while (status != 0x01);
	return 0;
}

int cam_ois_init(struct cam_ois_ctrl_t *o_ctrl)
{
	uint16_t status = 0;
	uint32_t read_value = 0;
	int rc = 0, retries = 0;
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
	struct cam_hw_param *hw_param = NULL;
	uint32_t *hw_cam_position = NULL;
#endif

	CAM_INFO(CAM_OIS, "E");

	retries = 20;
	do {
		rc = cam_ois_i2c_byte_read(o_ctrl, 0x0001, &status);
		if ((status == 0x01) ||
			(status == 0x13))
			break;
		if (--retries < 0) {
			if (rc < 0) {
				CAM_ERR(CAM_OIS, "failed due to i2c fail %d", rc);
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
				if (rc < 0) {
					msm_is_sec_get_sensor_position(&hw_cam_position);
					if (hw_cam_position != NULL) {
						switch (*hw_cam_position) {
						case CAMERA_0:
							if (!msm_is_sec_get_rear_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[R][OIS] Err\n");
									hw_param->i2c_ois_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;

						case CAMERA_1:
							if (!msm_is_sec_get_front_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[F][OIS] Err\n");
									hw_param->i2c_ois_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
						case CAMERA_2:
							if (!msm_is_sec_get_rear2_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[R2][OIS] Err\n");
									hw_param->i2c_ois_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
						case CAMERA_3:
							if (!msm_is_sec_get_iris_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[I][OIS] Err\n");
									hw_param->i2c_ois_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;
#endif

						default:
							CAM_ERR(CAM_HWB, "[NON][OIS] Unsupport\n");
							break;
						}
					}
				}
#endif

				break;
			}
			CAM_ERR(CAM_OIS, "ois status is 0x01 or 0x13, current status %d", status);
			break;
		}
		usleep_range(5000, 5050);
	} while ((status != 0x01) && (status != 0x13));

	// OIS Shift Setting
	rc = cam_ois_set_shift(o_ctrl);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois shift calibration enable failed, i2c fail %d", rc);
		return rc;
	}

	// VDIS Setting
	rc = cam_ois_set_ggfadeup(o_ctrl, 1000);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois set vdis setting ggfadeup failed %d", rc);
		return rc;
	}
	rc = cam_ois_set_ggfadedown(o_ctrl, 1000);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois set vdis setting ggfadedown failed %d", rc);
		return rc;
	}

	// OIS Hall Center Read
	rc = camera_io_dev_read(&o_ctrl->io_master_info, 0x021A, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois read hall X center failed %d", rc);
		return rc;
	}
	o_ctrl->x_center =
		((read_value >> 8) & 0xFF) | ((read_value & 0xFF) << 8);
	CAM_DBG(CAM_OIS, "ois read hall x center %d", o_ctrl->x_center);

	rc = camera_io_dev_read(&o_ctrl->io_master_info, 0x021C, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois read hall Y center failed %d", rc);
		return rc;
	}
	o_ctrl->y_center =
		((read_value >> 8) & 0xFF) | ((read_value & 0xFF) << 8);
	CAM_DBG(CAM_OIS, "ois read hall y center %d", o_ctrl->y_center);

	// Compensation Angle Setting
	rc = cam_ois_set_angle_for_compensation(o_ctrl);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois set angle for compensation failed %d", rc);
		return rc;
	}

	// Init Setting(Dual OIS Setting)
	mutex_lock(&(o_ctrl->i2c_init_data_mutex));
	rc = cam_ois_apply_settings(o_ctrl, &o_ctrl->i2c_init_data);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois set dual ois setting failed %d", rc);

	rc = delete_request(&o_ctrl->i2c_init_data);
	if (rc < 0) {
		CAM_WARN(CAM_OIS,
			"Failed deleting Init data: rc: %d", rc);
		rc = 0;
	}
	mutex_unlock(&(o_ctrl->i2c_init_data_mutex));

	// Read error register
	rc = camera_io_dev_read(&o_ctrl->io_master_info, 0x0004, &o_ctrl->err_reg,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "get ois error register value failed, i2c fail");
		return rc;
	}

	o_ctrl->ois_mode = 0;

	CAM_INFO(CAM_OIS, "X");

	return rc;
}

int cam_ois_get_fw_status(struct cam_ois_ctrl_t *o_ctrl)
{
	int rc = 0;
	uint32_t i = 0;
	uint8_t status_arr[OIS_FW_STATUS_SIZE];
	uint32_t status = 0;

	rc = camera_io_dev_read_seq(&o_ctrl->io_master_info,
		OIS_FW_STATUS_OFFSET, status_arr, CAMERA_SENSOR_I2C_TYPE_WORD, OIS_FW_STATUS_SIZE);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "i2c read fail");

	for (i = 0; i < OIS_FW_STATUS_SIZE; i++)
		status |= status_arr[i] << (i * 8);

	// In case previous update failed, (like removing the battery during update)
	// Module itself set the 0x00FC ~ 0x00FF register as error status
	// So if previous fw update failed, 0x00FC ~ 0x00FF register value is '4451'
	if (status == 4451) { //previous fw update failed, 0x00FC ~ 0x00FF register value is 4451
		return -1;
	}
	return 0;
}

int32_t cam_ois_read_phone_ver(struct cam_ois_ctrl_t *o_ctrl)
{
	struct file *filp = NULL;
	mm_segment_t old_fs;
	char	char_ois_ver[OIS_VER_SIZE + 1] = "";
	char	ois_bin_full_path[256] = "";
	int ret = 0, i = 0;
	char	ois_core_ver;

	loff_t pos;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	ois_core_ver = o_ctrl->cal_ver[0];

	sprintf(ois_bin_full_path, "%s/%s", OIS_FW_PATH, OIS_MCU_FW_NAME);
	sprintf(o_ctrl->load_fw_name, ois_bin_full_path); // to use in fw_update

	CAM_INFO(CAM_OIS, "OIS FW : %s", ois_bin_full_path);

	filp = filp_open(ois_bin_full_path, O_RDONLY, 0440);
	if (IS_ERR(filp)) {
		CAM_ERR(CAM_OIS, "No OIS FW with %c exists in the system. Load from OIS module.", ois_core_ver);
		CAM_ERR(CAM_OIS, "fail to open %s, %ld", ois_bin_full_path, PTR_ERR(filp));
		set_fs(old_fs);
		return -1;
	}

	pos = OIS_MCU_VERSION_OFFSET;
	ret = vfs_read(filp, char_ois_ver, sizeof(char) * OIS_MCU_VERSION_SIZE, &pos);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "Fail to read OIS FW.");
		ret = -1;
		goto ERROR;
	}

	pos = OIS_MCU_VDRINFO_OFFSET;
	ret = vfs_read(filp, char_ois_ver + OIS_MCU_VERSION_SIZE,
		sizeof(char) * (OIS_VER_SIZE - OIS_MCU_VERSION_SIZE), &pos);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "Fail to read OIS FW.");
		ret = -1;
		goto ERROR;
	}

	for (i = 0; i < OIS_VER_SIZE; i++) {
		if (!isalnum(char_ois_ver[i])) {
			CAM_ERR(CAM_OIS, "version char (%c) is not alnum type.", char_ois_ver[i]);
			ret = -1;
			goto ERROR;
		}
	}

	memcpy(o_ctrl->phone_ver, char_ois_ver, OIS_VER_SIZE * sizeof(char));
	CAM_INFO(CAM_OIS, "%c%c%c%c%c%c%c%c",
		o_ctrl->phone_ver[0], o_ctrl->phone_ver[1],
		o_ctrl->phone_ver[2], o_ctrl->phone_ver[3],
		o_ctrl->phone_ver[4], o_ctrl->phone_ver[5],
		o_ctrl->phone_ver[6], o_ctrl->phone_ver[7]);

ERROR:
	if (filp) {
		filp_close(filp, NULL);
		filp = NULL;
	}
	set_fs(old_fs);
	return ret;
}

int32_t cam_ois_read_module_ver(struct cam_ois_ctrl_t *o_ctrl)
{
	uint32_t read_value = 0;
	camera_io_dev_read(&o_ctrl->io_master_info, 0x00FB, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	o_ctrl->module_ver[0] = read_value & 0xFF; //core version
	o_ctrl->module_ver[1] = (read_value >> 8) & 0xFF; //driver ic
	if (!isalnum(read_value & 0xFF) && !isalnum((read_value>>8)&0xFF)) {
		CAM_ERR(CAM_OIS, "version char is not alnum type.");
		return -1;
	}

	camera_io_dev_read(&o_ctrl->io_master_info, 0x00F8, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	o_ctrl->module_ver[2] = read_value & 0xFF; //gyro sensor
	o_ctrl->module_ver[3] = (read_value >> 8) & 0xFF; //driver ic
	if (!isalnum(read_value&0xFF) && !isalnum((read_value>>8)&0xFF)) {
		CAM_ERR(CAM_OIS, "version char is not alnum type.");
		return -1;
	}

	camera_io_dev_read(&o_ctrl->io_master_info, 0x007C, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	o_ctrl->module_ver[4] = (read_value >> 8) & 0xFF; //year
	o_ctrl->module_ver[5] = read_value & 0xFF; //month
	if (!isalnum(read_value&0xFF) && !isalnum((read_value>>8)&0xFF)) {
		CAM_ERR(CAM_OIS, "version char is not alnum type.");
		return -1;
	}

	camera_io_dev_read(&o_ctrl->io_master_info, 0x007E, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	o_ctrl->module_ver[6] = (read_value >> 8) & 0xFF; //iteration 0
	o_ctrl->module_ver[7] = read_value & 0xFF; //iteration 1
	if (!isalnum(read_value&0xFF) && !isalnum((read_value>>8)&0xFF)) {
		CAM_ERR(CAM_OIS, "version char is not alnum type.");
		return -1;
	}

	CAM_INFO(CAM_OIS, "%c%c%c%c%c%c%c%c",
		o_ctrl->module_ver[0], o_ctrl->module_ver[1],
		o_ctrl->module_ver[2], o_ctrl->module_ver[3],
		o_ctrl->module_ver[4], o_ctrl->module_ver[5],
		o_ctrl->module_ver[6], o_ctrl->module_ver[7]);

	return 0;
}

int32_t cam_ois_read_manual_cal_info(struct cam_ois_ctrl_t *o_ctrl)
{
	int rc = 0;
	uint8_t user_data[OIS_VER_SIZE+1] = {0, };
	uint8_t version[20] = {0, };
	uint16_t val = 0;

	version[0] = 0x21;
	version[1] = 0x43;
	version[2] = 0x65;
	version[3] = 0x87;
	version[4] = 0x23;
	version[5] = 0x01;
	version[6] = 0xEF;
	version[7] = 0xCD;
	version[8] = 0x00;
	version[9] = 0x74;
	version[10] = 0x00;
	version[11] = 0x00;
	version[12] = 0x04;
	version[13] = 0x00;
	version[14] = 0x00;
	version[15] = 0x00;
	version[16] = 0x01;
	version[17] = 0x00;
	version[18] = 0x00;
	version[19] = 0x00;

	rc = camera_io_dev_write_seq(&o_ctrl->io_master_info, 0x0100,
		version, CAMERA_SENSOR_I2C_TYPE_WORD, 20);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois i2c read word failed addr : 0x%x", 0x0100);
	usleep_range(5000, 6000);

	rc |= cam_ois_i2c_byte_read(o_ctrl, 0x0118, &val); //Core version
	user_data[0] = (uint8_t)(val & 0x00FF);

	rc |= cam_ois_i2c_byte_read(o_ctrl, 0x0119, &val); //Gyro Sensor
	user_data[1] = (uint8_t)(val & 0x00FF);

	rc |= cam_ois_i2c_byte_read(o_ctrl, 0x011A, &val); //Driver IC
	user_data[2] = (uint8_t)(val & 0x00FF);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois i2c read word failed addr : 0x%x", 0x0100);

	memcpy(o_ctrl->cal_ver, user_data, (OIS_VER_SIZE) * sizeof(uint8_t));
	o_ctrl->cal_ver[OIS_VER_SIZE] = '\0';

	CAM_INFO(CAM_OIS, "Core version = 0x%02x, Gyro sensor = 0x%02x, Driver IC = 0x%02x",
		o_ctrl->cal_ver[0], o_ctrl->cal_ver[1], o_ctrl->cal_ver[2]);

	return 0;
}

int cam_ois_set_shift(struct cam_ois_ctrl_t *o_ctrl)
{
	int rc = 0;

	CAM_DBG(CAM_OIS, "Enter");
	CAM_INFO(CAM_OIS, "SET :: SHIFT_CALIBRATION");

	rc = cam_ois_i2c_byte_write(o_ctrl, 0x0039, 0x01);	 // OIS shift calibration enable
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois shift calibration enable failed, i2c fail");
		goto ERROR;
	}

ERROR:

	CAM_DBG(CAM_OIS, "Exit");
	return rc;
}

int cam_ois_set_angle_for_compensation(struct cam_ois_ctrl_t *o_ctrl)
{
	int rc = 0;
	uint8_t write_data[4] = {0, };

	CAM_INFO(CAM_OIS, "Enter");

	/* angle compensation 1.5->1.25
	   before addr:0x0000, data:0x01
	   write 0x3F558106
	   write 0x3F558106
	*/

	write_data[0] = 0x06;
	write_data[1] = 0x81;
	write_data[2] = 0x55;
	write_data[3] = 0x3F;

	rc = camera_io_dev_write_seq(&o_ctrl->io_master_info, 0x0348, write_data,
		CAMERA_SENSOR_I2C_TYPE_WORD, 4);

	if (rc < 0) {
		CAM_ERR(CAM_OIS, "i2c failed");
	}

	write_data[0] = 0x06;
	write_data[1] = 0x81;
	write_data[2] = 0x55;
	write_data[3] = 0x3F;

	rc = camera_io_dev_write_seq(&o_ctrl->io_master_info, 0x03D8, write_data,
		CAMERA_SENSOR_I2C_TYPE_WORD, 4);

	if (rc < 0) {
		CAM_ERR(CAM_OIS, "i2c failed");
	}

	return rc;
}
int cam_ois_set_ggfadeup(struct cam_ois_ctrl_t *o_ctrl, uint16_t value)
{
	int rc = 0;
	uint8_t data[2] = "";

	CAM_INFO(CAM_OIS, "Enter %d", value);

	data[0] = value & 0xFF;
	data[1] = (value >> 8) & 0xFF;

	rc = camera_io_dev_write_seq(&o_ctrl->io_master_info, 0x0238, data,
		CAMERA_SENSOR_I2C_TYPE_WORD, 2);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois set ggfadeup failed, i2c fail");

	CAM_INFO(CAM_OIS, "Exit");
	return rc;
}

int cam_ois_set_ggfadedown(struct cam_ois_ctrl_t *o_ctrl, uint16_t value)
{
	int rc = 0;
	uint8_t data[2] = "";

	CAM_INFO(CAM_OIS, "Enter %d", value);

	data[0] = value & 0xFF;
	data[1] = (value >> 8) & 0xFF;

	rc = camera_io_dev_write_seq(&o_ctrl->io_master_info, 0x023A, data,
		CAMERA_SENSOR_I2C_TYPE_WORD, 2);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois set ggfadedown failed, i2c fail");

	CAM_INFO(CAM_OIS, "Exit");
	return rc;
}


int cam_ois_create_shift_table(struct cam_ois_ctrl_t *o_ctrl, uint8_t *shift_data)
{
	int i = 0, j = 0, k = 0;
	int16_t dataX[9] = {0, }, dataY[9] = {0, };
	uint16_t tempX = 0, tempY = 0;
	uint32_t addr_en[2] = {0x00, 0x01};
	uint32_t addr_x[2] = {0x10, 0x40};
	uint32_t addr_y[2] = {0x22, 0x52};

	if (!o_ctrl || !shift_data)
		goto ERROR;

	CAM_DBG(CAM_OIS, "Enter");

	for (i = 0; i < 2; i++) {
		if (shift_data[addr_en[i]] != 0x11) {
			o_ctrl->shift_tbl[i].ois_shift_used = false;
			continue;
		}
		o_ctrl->shift_tbl[i].ois_shift_used = true;

		for (j = 0; j < 9; j++) {
			// ACT #1 Shift X : 0x0210 ~ 0x0220 (2byte), ACT #2 Shift X : 0x0240 ~ 0x0250 (2byte)
			tempX = (uint16_t)(shift_data[addr_x[i] + (j * 2)] |
				(shift_data[addr_x[i] + (j * 2) + 1] << 8));
			if (tempX > 32767)
				tempX -= 65536;
			dataX[j] = (int16_t)tempX;

			// ACT #1 Shift Y : 0x0222 ~ 0x0232 (2byte), ACT #2 Shift X : 0x0252 ~ 0x0262 (2byte)
			tempY = (uint16_t)(shift_data[addr_y[i] + (j * 2)] |
				(shift_data[addr_y[i] + (j * 2) + 1] << 8));
			if (tempY > 32767)
				tempY -= 65536;
			dataY[j] = (int16_t)tempY;
		}

		for (j = 0; j < 9; j++)
			CAM_INFO(CAM_OIS, "module%d, dataX[%d] = %5d / dataY[%d] = %5d",
				i + 1, j, dataX[j], j, dataY[j]);

		for (j = 0; j < 8; j++) {
			for (k = 0; k < 64; k++) {
				o_ctrl->shift_tbl[i].ois_shift_x[k + (j << 6)] =
					((((int32_t)dataX[j + 1] - dataX[j])  * k) >> 6) + dataX[j];
				o_ctrl->shift_tbl[i].ois_shift_y[k + (j << 6)] =
					((((int32_t)dataY[j + 1] - dataY[j])  * k) >> 6) + dataY[j];
			}
		}
	}

	CAM_DBG(CAM_OIS, "Exit");
	return 0;

ERROR:
	CAM_ERR(CAM_OIS, "create ois shift table fail");
	return -1;
}

int cam_ois_shift_calibration(struct cam_ois_ctrl_t *o_ctrl, uint16_t af_position, uint16_t subdev_id)
{
	int8_t data[4] = {0, };
	int rc = 0;

	//CAM_DBG(CAM_OIS, "cam_ois_shift_calibration %d, subdev: %d", af_position, subdev_id);

	if (!o_ctrl)
		return -1;

	if (!o_ctrl->is_power_up) {
		CAM_WARN(CAM_OIS, "ois is not power up");
		return 0;
	}
	if (!o_ctrl->is_servo_on) {
		CAM_WARN(CAM_OIS, "ois serve is not on yet");
		return 0;
	}

	if (af_position >= NUM_AF_POSITION) {
		CAM_ERR(CAM_OIS, "af position error %u", af_position);
		return -1;
	}

	if (o_ctrl->shift_tbl[0].ois_shift_used && subdev_id == 0) {
		data[0] = (o_ctrl->shift_tbl[0].ois_shift_x[af_position] & 0x00FF);
		data[1] = ((o_ctrl->shift_tbl[0].ois_shift_x[af_position] >> 8) & 0x00FF);
		data[2] = (o_ctrl->shift_tbl[0].ois_shift_y[af_position] & 0x00FF);
		data[3] = ((o_ctrl->shift_tbl[0].ois_shift_y[af_position] >> 8) & 0x00FF);

		CAM_DBG(CAM_OIS, "write for WIDE %d", subdev_id);

		rc = camera_io_dev_write_seq(&o_ctrl->io_master_info,
			0x004C, data, CAMERA_SENSOR_I2C_TYPE_WORD, 4);
		if (rc < 0)
			CAM_ERR(CAM_OIS, "write module#1 ois shift calibration error");
	} else if (o_ctrl->shift_tbl[1].ois_shift_used && subdev_id == 2) {
		data[0] = (o_ctrl->shift_tbl[1].ois_shift_x[af_position] & 0x00FF);
		data[1] = ((o_ctrl->shift_tbl[1].ois_shift_x[af_position] >> 8) & 0x00FF);
		data[2] = (o_ctrl->shift_tbl[1].ois_shift_y[af_position] & 0x00FF);
		data[3] = ((o_ctrl->shift_tbl[1].ois_shift_y[af_position] >> 8) & 0x00FF);

		CAM_DBG(CAM_OIS, "write for TELE %d", subdev_id);

		rc = camera_io_dev_write_seq(&o_ctrl->io_master_info,
			0x0098, data, CAMERA_SENSOR_I2C_TYPE_WORD, 4);
		if (rc < 0)
			CAM_ERR(CAM_OIS, "write module#2 ois shift calibration error");
	}

	return rc;
}

int32_t cam_ois_read_user_data_section(struct cam_ois_ctrl_t *o_ctrl, uint16_t addr, int size, uint8_t *user_data)
{
	uint8_t read_data[0x02FF] = {0, }, shift_data[0xFF] = {0, };
	int rc = 0, i = 0;
	uint16_t read_status = 0;
	struct cam_sensor_i2c_reg_array reg_setting;

	/* OIS Servo Off */
	if (cam_ois_i2c_byte_write(o_ctrl, 0x0000, 0) < 0)
		goto ERROR;

	if (cam_ois_wait_idle(o_ctrl, 10) < 0) {
		CAM_ERR(CAM_OIS, "wait ois idle status failed");
		goto ERROR;
	}

	/* User Data Area & Address Setting - 1Page */
	rc = cam_ois_i2c_byte_write(o_ctrl, 0x000F, 0x40);	// DLFSSIZE_W Register(0x000F) : Size = 4byte * Value
	memset(&reg_setting, 0, sizeof(struct cam_sensor_i2c_reg_array));
	reg_setting.reg_addr = 0x0010;
	reg_setting.reg_data = 0x0000;
	rc |= cam_qup_i2c_write(&o_ctrl->io_master_info, &reg_setting,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	rc |= cam_ois_i2c_byte_write(o_ctrl, 0x000E, 0x04); // DFLSCMD Register(0x000E) = READ
	if (rc < 0)
		goto ERROR;

	for (i = MAX_RETRY_COUNT; i > 0; i--) {
		if (cam_ois_i2c_byte_read(o_ctrl, 0x000E, &read_status) < 0)
			goto ERROR;
		if (read_status == 0x14) /* Read Complete? */
			break;
		usleep_range(10000, 11000); // give some delay to wait
	}
	if (i < 0) {
		CAM_ERR(CAM_OIS, "DFLSCMD Read command fail");
		goto ERROR;
	}

	/* OIS Data Header Read */
	rc = camera_io_dev_read_seq(&o_ctrl->io_master_info,
		0x0100, read_data,
		CAMERA_SENSOR_I2C_TYPE_WORD, 0xFF);
	if (rc < 0)
		goto ERROR;

	/* copy Cal-Version */
	CAM_INFO(CAM_OIS, "userdata cal ver : %c %c %c %c %c %c %c",
			read_data[0], read_data[1], read_data[2], read_data[3],
			read_data[4], read_data[5], read_data[6]);
	memcpy(user_data, read_data, size * sizeof(uint8_t));


	/* User Data Area & Address Setting - 2Page */
	rc = cam_ois_i2c_byte_write(o_ctrl, 0x000F, 0x40);	// DLFSSIZE_W Register(0x000F) : Size = 4byte * Value
	reg_setting.reg_addr = 0x0010;
	reg_setting.reg_data = 0x0001;
	rc |= cam_qup_i2c_write(&o_ctrl->io_master_info, &reg_setting,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD); // Data Write Start Address Offset : 0x0000
	rc |= cam_ois_i2c_byte_write(o_ctrl, 0x000E, 0x04); // DFLSCMD Register(0x000E) = READ
	if (rc < 0)
		goto ERROR;

	for (i = MAX_RETRY_COUNT; i >= 0; i--) {
		if (cam_ois_i2c_byte_read(o_ctrl, 0x000E, &read_status) < 0)
			goto ERROR;
		if (read_status == 0x14) /* Read Complete? */
			break;
		usleep_range(10000, 11000); // give some delay to wait
	}
	if (i < 0) {
		CAM_ERR(CAM_OIS, "DFLSCMD Read command fail");
		goto ERROR;
	}

	/* OIS Cal Data Read */
	rc = camera_io_dev_read_seq(&o_ctrl->io_master_info,
		0x0100, read_data + 0x0100,
		CAMERA_SENSOR_I2C_TYPE_WORD, 0xFF);
	if (rc < 0)
		goto ERROR;

	/* User Data Area & Address Setting - 3Page */
	rc = cam_ois_i2c_byte_write(o_ctrl, 0x000F, 0x40);  // DLFSSIZE_W Register(0x000F) : Size = 4byte * Value
	reg_setting.reg_addr = 0x0010;
	reg_setting.reg_data = 0x0002;
	rc |= cam_qup_i2c_write(&o_ctrl->io_master_info, &reg_setting,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD); // Data Write Start Address Offset : 0x0000
	rc |= cam_ois_i2c_byte_write(o_ctrl, 0x000E, 0x04); // DFLSCMD Register(0x000E) = READ
	if (rc < 0)
		goto ERROR;

	for (i = MAX_RETRY_COUNT; i >= 0; i--) {
		if (cam_ois_i2c_byte_read(o_ctrl, 0x000E, &read_status) < 0)
			goto ERROR;
		if (read_status == 0x14) /* Read Complete? */
			break;
		usleep_range(10000, 11000); // give some delay to wait
	}
	if (i < 0) {
		CAM_ERR(CAM_OIS, "DFLSCMD Read command fail");
		goto ERROR;
	}

	/* OIS Shift Info Read */
	/* OIS Shift Calibration Read */
	rc = camera_io_dev_read_seq(&o_ctrl->io_master_info,
		0x0100, shift_data,
		CAMERA_SENSOR_I2C_TYPE_WORD, 0xFF);
	if (rc < 0)
		goto ERROR;

	memset(&o_ctrl->shift_tbl, 0, sizeof(o_ctrl->shift_tbl));
	cam_ois_create_shift_table(o_ctrl, shift_data);
ERROR:
	return rc;
}

int32_t cam_ois_read_cal_info(struct cam_ois_ctrl_t *o_ctrl,
	uint32_t *chksum_rumba, uint32_t *chksum_line, uint32_t *is_different_crc)
{
	int rc = 0;
	uint8_t user_data[OIS_VER_SIZE + 1] = {0, };

	rc = camera_io_dev_read(&o_ctrl->io_master_info, 0x007A, chksum_rumba,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD); // OIS Driver IC cal checksum
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois i2c read word failed addr : 0x%x", 0x7A);

	rc = camera_io_dev_read(&o_ctrl->io_master_info, 0x021E, chksum_line,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD); // Line cal checksum
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois i2c read word failed addr : 0x%x", 0x021E);

	rc = camera_io_dev_read(&o_ctrl->io_master_info, 0x0004, is_different_crc,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois i2c read word failed addr : 0x%x", 0x0004);

	CAM_INFO(CAM_OIS, "cal checksum(rumba : %d, line : %d), compare_crc = %d",
		*chksum_rumba, *chksum_line, *is_different_crc);

	if (cam_ois_read_user_data_section(o_ctrl, OIS_USER_DATA_START_ADDR, OIS_VER_SIZE, user_data) < 0) {
		CAM_ERR(CAM_OIS, " failed to read user data");
		return -1;
	}

	memcpy(o_ctrl->cal_ver, user_data, (OIS_VER_SIZE) * sizeof(uint8_t));
	o_ctrl->cal_ver[OIS_VER_SIZE] = '\0';

	CAM_INFO(CAM_OIS, "cal version = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x(%s)",
		o_ctrl->cal_ver[0], o_ctrl->cal_ver[1],
		o_ctrl->cal_ver[2], o_ctrl->cal_ver[3],
		o_ctrl->cal_ver[4], o_ctrl->cal_ver[5],
		o_ctrl->cal_ver[6],
		o_ctrl->cal_ver);

	return 0;
}

int32_t cam_ois_check_extclk(struct cam_ois_ctrl_t *o_ctrl)
{
	uint16_t pll_multi = 0, pll_divide = 0;
	int ret = 0;
	uint8_t clk_arr[4];
	uint32_t cur_clk = 0, new_clk = 0;

	if (cam_ois_wait_idle(o_ctrl, 20) < 0) {
		CAM_ERR(CAM_OIS, "wait ois idle status failed");
		ret = -1;
		goto error;
	}

	/* Check current EXTCLK in register(0x03F0-0x03F3) */
	ret = camera_io_dev_read_seq(&o_ctrl->io_master_info,
		0x03F0, clk_arr, CAMERA_SENSOR_I2C_TYPE_WORD, 4);
	if (ret < 0)
		CAM_ERR(CAM_OIS, "i2c read fail");

	cur_clk = (clk_arr[3] << 24) | (clk_arr[2] << 16) |
				(clk_arr[1] << 8) | clk_arr[0];

	CAM_INFO(CAM_OIS, "[OIS_FW_DBG] cur_clk = %u", cur_clk);

	if (cur_clk != CAMERA_OIS_EXT_CLK) {
		new_clk = CAMERA_OIS_EXT_CLK;
		clk_arr[0] = CAMERA_OIS_EXT_CLK & 0xFF;
		clk_arr[1] = (CAMERA_OIS_EXT_CLK >> 8) & 0xFF;
		clk_arr[2] = (CAMERA_OIS_EXT_CLK >> 16) & 0xFF;
		clk_arr[3] = (CAMERA_OIS_EXT_CLK >> 24) & 0xFF;

		switch (new_clk) {
		case 0xB71B00:
			pll_multi = 0x08;
			pll_divide = 0x03;
			break;
		case 0x1036640:
			pll_multi = 0x09;
			pll_divide = 0x05;
			break;
		case 0x124F800:
			pll_multi = 0x05;
			pll_divide = 0x03;
			break;
		case 0x16E3600:
			pll_multi = 0x04;
			pll_divide = 0x03;
			break;
		case 0x18CBA80:
			pll_multi = 0x06;
			pll_divide = 0x05;
			break;
		default:
			CAM_INFO(CAM_OIS, "cur_clk: 0x%08x", cur_clk);
			ret = -EINVAL;
			goto error;
		}

		/* Set External Clock(0x03F0-0x03F3) Setting */
		ret = camera_io_dev_write_seq(&o_ctrl->io_master_info, 0x03F0, clk_arr,
			CAMERA_SENSOR_I2C_TYPE_WORD, 4);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "i2c write fail 0x03F0");

		/* Set PLL Multiple(0x03F4) Setting */
		ret = cam_ois_i2c_byte_write(o_ctrl, 0x03F4, pll_multi);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "i2c write fail 0x03F4");

		/* Set PLL Divide(0x03F5) Setting */
		ret = cam_ois_i2c_byte_write(o_ctrl, 0x03F5, pll_divide);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "i2c write fail 0x03F5");

		/* External Clock & I2C setting write to OISDATASECTION(0x0003) */
		ret = cam_ois_i2c_byte_write(o_ctrl, 0x0003, 0x01);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "i2c write fail 0x0003");

		/* Wait for Flash ROM Write */
		msleep(200);

		/* S/W Reset */
		/* DFLSCTRL register(0x000D) */
		ret = cam_ois_i2c_byte_write(o_ctrl, 0x000D, 0x01);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "i2c write fail 0x000D");

		/* Set DFLSCMD register(0x000E) = 6(Reset) */
		ret = cam_ois_i2c_byte_write(o_ctrl, 0x000E, 0x06);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "i2c write fail 0x000E");
		/* Wait for Restart */
		msleep(50);

		CAM_INFO(CAM_OIS, "Apply EXTCLK for ois %u", new_clk);
	} else {
		CAM_INFO(CAM_OIS, "Keep current EXTCLK %u", cur_clk);
	}

error:
	return	ret;
}

uint16_t cam_ois_calcchecksum(unsigned char *data, int size)
{
	int i = 0;
	uint16_t result = 0;

	for (i = 0; i < size; i += 2)
		result = result + (0xFFFF & (((*(data + i + 1)) << 8) | (*(data + i))));

	return result;
}

int32_t cam_ois_fw_update(struct cam_ois_ctrl_t *o_ctrl,
	bool is_force_update)
{
	int ret = 0;
	unsigned char sendData[OIS_FW_UPDATE_PACKET_SIZE] = "";
	int i = 0;
	int block = 0;
	struct file *ois_filp = NULL;
	unsigned char *buffer = NULL;
	uint8_t vdr_info[4] = {0, };
	int fw_size = 0;
	uint32_t address = 0;
	uint32_t sendSize = 0;
	mm_segment_t old_fs;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	CAM_INFO(CAM_OIS, " ENTER");
	sprintf(o_ctrl->load_fw_name,"/vendor/lib/camera/ois_mcu_fw.bin");
	/*file open */
	ois_filp = filp_open(o_ctrl->load_fw_name, O_RDONLY, 0);
	if (IS_ERR(ois_filp)) {
		CAM_ERR(CAM_OIS, "[OIS_FW_DBG] fail to open file %s", o_ctrl->load_fw_name);
		ret = -1;
		goto ERROR;
	}else{
	    CAM_ERR(CAM_OIS, "[OIS_FW_DBG] success to open file %s", o_ctrl->load_fw_name);
	}	

	fw_size = ois_filp->f_path.dentry->d_inode->i_size;
	CAM_INFO(CAM_OIS, "fw size %d Bytes", fw_size);
	buffer = vmalloc(fw_size);
	memset(buffer, 0x00, fw_size);
	ois_filp->f_pos = 0;
    total_fw_size = fw_size;

	ret = vfs_read(ois_filp, (char __user *)buffer, fw_size, &ois_filp->f_pos);
	if (ret != fw_size) {
		CAM_ERR(CAM_OIS, "[OIS_FW_DBG] failed to read file");
		ret = -1;
		goto ERROR;
	}
	strncpy(vdr_info,buffer+0x807c,4);
	CAM_ERR(CAM_OIS, "read vdr info in bin file 0x%x",*(uint32_t *)vdr_info);
	CAM_ERR(CAM_OIS, "----------year  : %d ----------",(vdr_info[0]-0x41)+2018);
	CAM_ERR(CAM_OIS, "----------month : %d ----------",(vdr_info[1]-0x41)+1);
	CAM_ERR(CAM_OIS, "----------day   : %d ----------",(vdr_info[2]-0x30)+0);
	CAM_ERR(CAM_OIS, "----------time  : %d ----------",(vdr_info[3]-0x30)+0);	
	address = memory_map.flashbase;
	/* Write UserProgram Data */
	for (block = 0; block <= (fw_size / OIS_FW_UPDATE_PACKET_SIZE); block++) {
		CAM_ERR(CAM_OIS, "remain bytes : %d ,addr 0x%x ",fw_size-block*OIS_FW_UPDATE_PACKET_SIZE,address);
		if((fw_size - block * OIS_FW_UPDATE_PACKET_SIZE) <= 0){
			break;
		}
		if(fw_size - block * OIS_FW_UPDATE_PACKET_SIZE > OIS_FW_UPDATE_PACKET_SIZE){
			sendSize = OIS_FW_UPDATE_PACKET_SIZE;
		}else{
			sendSize = fw_size - block * OIS_FW_UPDATE_PACKET_SIZE;
		}
		for(i = 0; i<sendSize; i++ ){
			sendData[i] = buffer[OIS_FW_UPDATE_PACKET_SIZE *block+i];
		}
		ret = sysboot_i2c_write(o_ctrl, address, sendData, sendSize);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "[OIS_FW_DBG] addr 0x%x : i2c byte write failed",address);  
		address += sendSize;
	}
	CAM_INFO(CAM_OIS, "[OIS_FW_DBG] ois fw update done");

ERROR:
	if (ois_filp) {
		filp_close(ois_filp, NULL);
		ois_filp = NULL;
	}	
	if (buffer) {
		vfree(buffer);
		buffer = NULL;
	}
	set_fs(old_fs);
	return ret;
}

int32_t cam_ois_dump_fw(struct cam_ois_ctrl_t *o_ctrl)
{
	int ret = 0;
	unsigned char dstData[OIS_FW_UPDATE_PACKET_SIZE] = "";
	int block = 0;
	struct file *ois_dump_filp = NULL;
	unsigned char *buffer = NULL;
	int fw_size = 0;
	uint32_t address = 0;
	uint32_t dstSize = 0;
	char dump_file[256];
	mm_segment_t old_fs;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	CAM_INFO(CAM_OIS, " ENTER");
	sprintf(dump_file,"/system/etc/ois_mcu_dumpfw.bin");
	/*dump file open */
	ois_dump_filp = filp_open(dump_file, O_WRONLY|O_APPEND|O_CREAT, 0666);
	if (IS_ERR(ois_dump_filp)) {
		CAM_ERR(CAM_OIS, "[OIS_FW_DBG] fail to open file %s", dump_file);
		ret = -1;
		goto ERROR;
	}else{
	    CAM_ERR(CAM_OIS, "[OIS_FW_DBG] success to open file %s", dump_file);
	}	

	fw_size = total_fw_size;
	CAM_INFO(CAM_OIS, "read fw size %d Bytes", fw_size);
	buffer = vmalloc(fw_size);
	memset(buffer, 0x00, fw_size);

	address = memory_map.flashbase;
	/* Write UserProgram Data */
	for (block = 0; block <= (fw_size / OIS_FW_UPDATE_PACKET_SIZE); block++) {
		if(fw_size - block * OIS_FW_UPDATE_PACKET_SIZE > OIS_FW_UPDATE_PACKET_SIZE){
			dstSize = OIS_FW_UPDATE_PACKET_SIZE;
		}else{
			dstSize = fw_size - block * OIS_FW_UPDATE_PACKET_SIZE;
		}
		
		ret = sysboot_i2c_read(o_ctrl, address, dstData, dstSize);
		if (ret < 0)
			CAM_ERR(CAM_OIS, "[OIS_FW_DBG] addr 0x%x : i2c byte read failed",address);
        strncpy(buffer + OIS_FW_UPDATE_PACKET_SIZE *block, dstData,dstSize);		
		address += OIS_FW_UPDATE_PACKET_SIZE;
	}

	if(!IS_ERR(ois_dump_filp)){
     	ois_dump_filp->f_pos = 0;
     	ret = vfs_write(ois_dump_filp, (char __user *)buffer, fw_size, &ois_dump_filp->f_pos);
     	if(ret < 0){
     		CAM_ERR(CAM_OIS, "[OIS_FW_DBG] failed to write dump file");
     	}
	}	
	CAM_INFO(CAM_OIS, "[OIS_FW_DBG] ois dump fw done");

ERROR:
	if (ois_dump_filp) {
		if(IS_ERR(ois_dump_filp)){
			
		}else{
		   filp_close(ois_dump_filp, NULL);
		}
		ois_dump_filp = NULL;
	}	
	if (buffer) {
		vfree(buffer);
		buffer = NULL;
	}
	set_fs(old_fs);
	return ret;
}

int cam_ois_test_write_read(struct cam_ois_ctrl_t *o_ctrl)
{
    int ret = 0;
	int address = 0;
	unsigned char src[4] = {0, };
	unsigned char dst[4] = {0, };
	address = memory_map.flashbase;
	src[0] = 0xAB;
	src[1] = 0xCD;
	src[2] = 0xEF;
	src[3] = 0xAD;
	ret = sysboot_i2c_write(o_ctrl,address,src, 4);
	ret = sysboot_i2c_read(o_ctrl,address,dst, 4);
	CAM_ERR(CAM_OIS,"dst[0] %2X , dst[1] %X, dst[2] %X, dst[3] %X",
		dst[0],dst[1],dst[2],dst[3]);
	return ret;
}

// check ois version to see if it is available for selftest or not
void cam_ois_version(struct cam_ois_ctrl_t *o_ctrl)
{
	int ret = 0;
	uint16_t val_a = 0, val_b = 0,val_c = 0, val_d = 0;


	ret = cam_ois_i2c_byte_read(o_ctrl, 0x00F8, &val_a);
	if (ret < 0)
		CAM_ERR(CAM_OIS, "i2c read fail");

	ret = cam_ois_i2c_byte_read(o_ctrl, 0x00F9, &val_b);
	if (ret < 0)
		CAM_DBG(CAM_OIS, "i2c read fail");
	
	ret = cam_ois_i2c_byte_read(o_ctrl, 0x00FA, &val_c);
	if (ret < 0)
		CAM_DBG(CAM_OIS, "i2c read fail");	
	
	ret = cam_ois_i2c_byte_read(o_ctrl, 0x00FB, &val_d);
	if (ret < 0)
		CAM_DBG(CAM_OIS, "i2c read fail");

	CAM_INFO(CAM_OIS, "OIS version :val_a 0x%x, val_b 0x%x ,val_c 0x%x , val_d 0x%x ",
		      val_a,val_b,val_c,val_d);
	CAM_INFO(CAM_OIS, "End");
}

// check ois version to see if it is available for selftest or not
void cam_ois_vdrinfo(struct cam_ois_ctrl_t *o_ctrl)
{
	int ret = 0;
	uint16_t val_a = 0, val_b = 0,val_c = 0, val_d = 0;


	ret = cam_ois_i2c_byte_read(o_ctrl, 0x007c, &val_a);
	if (ret < 0)
		CAM_ERR(CAM_OIS, "i2c read fail");

	ret = cam_ois_i2c_byte_read(o_ctrl, 0x007d, &val_b);
	if (ret < 0)
		CAM_DBG(CAM_OIS, "i2c read fail");
	
	ret = cam_ois_i2c_byte_read(o_ctrl, 0x007e, &val_c);
	if (ret < 0)
		CAM_DBG(CAM_OIS, "i2c read fail");	
	
	ret = cam_ois_i2c_byte_read(o_ctrl, 0x007f, &val_d);
	if (ret < 0)
		CAM_DBG(CAM_OIS, "i2c read fail");

	CAM_INFO(CAM_OIS, "OIS version :val_a 0x%x, val_b 0x%x ,val_c 0x%x , val_d 0x%x ",
		      val_a,val_b,val_c,val_d);
	CAM_INFO(CAM_OIS, "End");
}

int cam_ois_compare_vdrinfo(struct cam_ois_ctrl_t *o_ctrl){
	int ret = 0;
	int tmp = 0;
	int i = 0;
	for(i = 0; i < 4; i++){
	    tmp = o_ctrl->phone_ver[4+i] - o_ctrl->module_ver[4+i];
		if(tmp > 0){			
			ret = 1;
			break;
		}else if(tmp == 0){
		    continue;
		}else if(tmp < 0){
		    ret = -1;
			break;
		}
	}
	return ret;
}

/* get offset from module for line test */
void cam_ois_offset_test(struct cam_ois_ctrl_t *o_ctrl,
	long *raw_data_x, long *raw_data_y, bool is_need_cal)
{
	int i = 0;
	uint16_t val = 0;
	uint16_t x = 0, y = 0;
	int x_sum = 0, y_sum = 0, sum = 0;
	int retries = 0, avg_count = 20;

	if (is_need_cal) { // with calibration , offset value will be renewed.
		if (cam_ois_i2c_byte_write(o_ctrl, 0x14, 0x01) < 0)
			CAM_ERR(CAM_OIS, "i2c write fail");

		retries = avg_count;
		do {
			cam_ois_i2c_byte_read(o_ctrl, 0x14, &val);

			CAM_DBG(CAM_OIS, "[read_val_0x0014::0x%04x]", val);

			usleep_range(10000, 11000);

			if (--retries < 0) {
				CAM_ERR(CAM_OIS, "Read register failed!, data 0x0014 val = 0x%04x", val);
				break;
			}
		} while (val);
	}

	retries = avg_count;
	for (i = 0; i < retries; retries--) {
		cam_ois_i2c_byte_read(o_ctrl, 0x0248, &val);
		x = val;
		cam_ois_i2c_byte_read(o_ctrl, 0x0249, &val);
		x_sum = (val << 8) | x;

		if (x_sum > 0x7FFF)
			x_sum = -((x_sum ^ 0xFFFF) + 1);
		sum += x_sum;
	}
	sum = sum * 10 / avg_count;
	*raw_data_x = sum * 1000 / 131 / 10;

	sum = 0;

	retries = avg_count;
	for (i = 0; i < retries; retries--) {
		cam_ois_i2c_byte_read(o_ctrl, 0x024A, &val);
		y = val;
		cam_ois_i2c_byte_read(o_ctrl, 0x024B, &val);
		y_sum = (val << 8) | y;

		if (y_sum > 0x7FFF)
			y_sum = -((y_sum ^ 0xFFFF) + 1);

		sum += y_sum;
	}
	sum = sum * 10 / avg_count;
	*raw_data_y = sum * 1000 / 131 / 10;

	CAM_INFO(CAM_OIS, "end");

	cam_ois_version(o_ctrl);
}

/* ois module itselt has selftest function for line test.  */
/* it excutes by setting register and return the result */
uint32_t cam_ois_self_test(struct cam_ois_ctrl_t *o_ctrl)
{
	int ret = 0;
	uint16_t val = 0;
	int retries = 30;
	u16 reg_val = 0;
	u8 x = 0, y = 0;
	u16 x_gyro_log = 0, y_gyro_log = 0;

	CAM_INFO(CAM_OIS, "cam_is_ois_self_test : start");

	ret = cam_ois_i2c_byte_write(o_ctrl, 0x0014, 0x08);
	if (ret < 0)
		CAM_ERR(CAM_OIS, "i2c write fail");

	do {
		cam_ois_i2c_byte_read(o_ctrl, 0x0014, &val);
		CAM_DBG(CAM_OIS, "[read_val_0x0014::%x]", val);

		usleep_range(10000, 11000);
		if (--retries < 0) {
			CAM_ERR(CAM_OIS, "Read register failed!, data 0x0014 val = 0x%04x", val);
			break;
		}
	} while (val);

	cam_ois_i2c_byte_read(o_ctrl, 0x0004, &val);

	cam_ois_i2c_byte_read(o_ctrl, 0x00EC, &reg_val);
	x = reg_val;
	cam_ois_i2c_byte_read(o_ctrl, 0x00ED, &reg_val);
	x_gyro_log = (reg_val << 8) | x;
	cam_ois_i2c_byte_read(o_ctrl, 0x00EE, &reg_val);
	y = reg_val;
	cam_ois_i2c_byte_read(o_ctrl, 0x00EF, &reg_val);
	y_gyro_log = (reg_val << 8) | y;
	CAM_INFO(CAM_OIS, "(GSTLOG0=%d, GSTLOG1=%d)", x_gyro_log, y_gyro_log);

	CAM_INFO(CAM_OIS, "test done, reg: 0x04 data : val = 0x%04x", val);
	CAM_INFO(CAM_OIS, "end");
	return val;
}

bool cam_ois_sine_wavecheck(struct cam_ois_ctrl_t *o_ctrl, int threshold,
	struct cam_ois_sinewave_t *sinewave, int *result, int num_of_module)
{
	uint16_t buf = 0, val = 0;
	int i = 0, ret = 0, retries = 10;
	int sinx_count = 0, siny_count = 0;
	uint32_t u16_sinx_count = 0, u16_siny_count = 0;
	uint32_t u16_sinx = 0, u16_siny = 0;
	int result_addr[2] = {0x00C0, 0x00E4};

	ret = cam_ois_i2c_byte_write(o_ctrl, 0x0052, (uint16_t)threshold); /* error threshold level. */

	if (num_of_module == 1)
		ret |= cam_ois_i2c_byte_write(o_ctrl, 0x00BE, 0x01); /* select module */
	else if (num_of_module == 2) {
		ret |= cam_ois_i2c_byte_write(o_ctrl, 0x00BE, 0x03); /* select module */
		ret |= cam_ois_i2c_byte_write(o_ctrl, 0x005B, (uint16_t)threshold); /* Module#2 error threshold level. */
	}
	ret |= cam_ois_i2c_byte_write(o_ctrl, 0x0053, 0x00); /* count value for error judgement level. */
	ret |= cam_ois_i2c_byte_write(o_ctrl, 0x0054, 0x05); /* frequency level for measurement. */
	ret |= cam_ois_i2c_byte_write(o_ctrl, 0x0055, 0x2A); /* amplitude level for measurement. */
	ret |= cam_ois_i2c_byte_write(o_ctrl, 0x0056, 0x03); /* dummy pluse setting. */
	ret |= cam_ois_i2c_byte_write(o_ctrl, 0x0057, 0x02); /* vyvle level for measurement. */
	ret |= cam_ois_i2c_byte_write(o_ctrl, 0x0050, 0x01); /* start sine wave check operation */

	if (ret < 0) {
		CAM_ERR(CAM_OIS, "i2c write fail");
		*result = 0xFF;
		return false;
	}

	retries = 30;
	do {
		ret = cam_ois_i2c_byte_read(o_ctrl, 0x0050, &val);
		if (ret < 0) {
			CAM_ERR(CAM_OIS, "i2c read fail");
			break;
		}

		msleep(100);

		if (--retries < 0) {
			CAM_ERR(CAM_OIS, "sine wave operation fail.");
			*result = 0xFF;
			return false;
		}
	} while (val);

	ret = cam_ois_i2c_byte_read(o_ctrl, 0x0051, &buf);
	if (ret < 0)
		CAM_ERR(CAM_OIS, "i2c read fail");

	*result = (int)buf;
	CAM_INFO(CAM_OIS, "MCERR(0x51)=%d", buf);

	for (i = 0; i < num_of_module ; i++) {
		ret = camera_io_dev_read(&o_ctrl->io_master_info, result_addr[i], &u16_sinx_count,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
		sinx_count = ((u16_sinx_count & 0xFF00) >> 8) | ((u16_sinx_count &	0x00FF) << 8);
		if (sinx_count > 0x7FFF)
			sinx_count = -((sinx_count ^ 0xFFFF) + 1);

		ret |= camera_io_dev_read(&o_ctrl->io_master_info, result_addr[i] + 2, &u16_siny_count,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
		siny_count = ((u16_siny_count & 0xFF00) >> 8) | ((u16_siny_count &	0x00FF) << 8);
		if (siny_count > 0x7FFF)
			siny_count = -((siny_count ^ 0xFFFF) + 1);

		ret |= camera_io_dev_read(&o_ctrl->io_master_info, result_addr[i] + 4, &u16_sinx,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
		sinewave[i].sin_x = ((u16_sinx & 0xFF00) >> 8) | ((u16_sinx &  0x00FF) << 8);
		if (sinewave[i].sin_x > 0x7FFF)
			sinewave[i].sin_x = -((sinewave[i].sin_x ^ 0xFFFF) + 1);

		ret |= camera_io_dev_read(&o_ctrl->io_master_info, result_addr[i] + 6, &u16_siny,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
		sinewave[i].sin_y = ((u16_siny & 0xFF00) >> 8) | ((u16_siny &  0x00FF) << 8);
		if (sinewave[i].sin_y > 0x7FFF)
			sinewave[i].sin_y = -((sinewave[i].sin_y ^ 0xFFFF) + 1);

		if (ret < 0)
			CAM_ERR(CAM_OIS, "i2c read fail");

		CAM_INFO(CAM_OIS, "[Module#%d] threshold = %d, sinx = %d, siny = %d, sinx_count = %d, siny_count = %d",
			i + 1, threshold, sinewave[i].sin_x, sinewave[i].sin_y, sinx_count, siny_count);
	}

	if (buf == 0x0)
		return true;
	else
		return false;
}

int cam_ois_check_fw(struct cam_ois_ctrl_t *o_ctrl)
{
	int rc = 0;
	int is_force_update = 0;
	int empty_check_en = 0;
	int is_need_update = 0;

	CAM_INFO(CAM_OIS, "E");

	rc = cam_ois_power_up(o_ctrl);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "OIS Power up failed");
		goto end;
	}
	target_normal_on(o_ctrl);	
	o_ctrl->io_master_info.client->addr = 0xA2;
	rc = cam_ois_read_module_ver(o_ctrl);
	o_ctrl->io_master_info.client->addr = 0x51;
	if(rc < 0){
		CAM_ERR(CAM_OIS,"cam_ois_read_module_ver failed");
	}
	rc = cam_ois_read_phone_ver(o_ctrl);
	if(rc < 0){
		CAM_ERR(CAM_OIS,"cam_ois_read_phone_ver failed");
	}
	// compare vdr info
    is_need_update = cam_ois_compare_vdrinfo(o_ctrl);
	if(is_need_update <= 0){
		CAM_ERR(CAM_OIS,"No need update MCU FW");
	}else if(is_need_update == 1){
        //enter system bootloader mode
        CAM_ERR(CAM_OIS,"need update MCU FW, enter system bootloader mode");
    	rc = target_validation(o_ctrl);
    	if(rc < 0){
    	    CAM_ERR(CAM_OIS,"mcu connect failed");
    	}
    
	    CAM_INFO(CAM_OIS, "update OIS FW from phone");

    	//target_read_hwver
    	target_read_hwver(o_ctrl);
    	//target_read_vdrinfo
    	target_read_vdrinfo(o_ctrl);
    	//check_option_byte
    	target_option_update(o_ctrl);
    	//check empty status
    	empty_check_en = target_empty_check_status(o_ctrl);
    	//erase
    	sysboot_i2c_erase(o_ctrl,memory_map.flashbase,65536 - 2048);
    
    	//update FW
    	cam_ois_fw_update(o_ctrl, is_force_update);
    	//target_read_hwver
    	target_read_hwver(o_ctrl);
    	//target_read_vdrinfo
    	target_read_vdrinfo(o_ctrl);
    	if(empty_check_en > 0){
    		target_empty_check_clear(o_ctrl);
    	}else{
    	    //sysboot_disconnect
    	    sysboot_disconnect(o_ctrl);
    	}
    	//read version
    	cam_ois_version(o_ctrl);
    	//read vdrinfo
    	cam_ois_vdrinfo(o_ctrl);
		// read mcu fw info again
    	o_ctrl->io_master_info.client->addr = 0xA2;
    	rc = cam_ois_read_module_ver(o_ctrl);
    	o_ctrl->io_master_info.client->addr = 0x51;
    	if(rc < 0){
    		CAM_ERR(CAM_OIS,"cam_ois_read_module_ver failed");
    	}
		//compare if download success or not
		rc = cam_ois_compare_vdrinfo(o_ctrl);
		if(rc == 0){
			CAM_ERR(CAM_OIS,"MCU Fw download success");
		}else{
		    CAM_ERR(CAM_OIS,"MCU Fw download failed");
		}
	}
	//cam_ois_power_down(o_ctrl);
end:
	CAM_INFO(CAM_OIS, "X");
	return rc;
}

int32_t cam_ois_set_debug_info(struct cam_ois_ctrl_t *o_ctrl, uint16_t mode)
{
	uint16_t status_reg = 0;
	int rc = 0;
	char exif_tag[6] = "ssois"; //defined exif tag for ois

	CAM_DBG(CAM_OIS, "Enter");

	if (cam_ois_i2c_byte_read(o_ctrl, 0x01, &status_reg) < 0) //read Status register
		CAM_ERR(CAM_OIS, "get ois status register value failed, i2c fail");

	snprintf(ois_debug, 40, "%s%s %s %s %x %x %x\n", exif_tag,
		(o_ctrl->module_ver[0] == '\0') ? ("ISNULL") : (o_ctrl->module_ver),
		(o_ctrl->phone_ver[0] == '\0') ? ("ISNULL") : (o_ctrl->phone_ver),
		(o_ctrl->cal_ver[0] == '\0') ? ("ISNULL") : (o_ctrl->cal_ver),
		o_ctrl->err_reg, status_reg, mode);

	CAM_INFO(CAM_OIS, "ois exif debug info %s", ois_debug);
	CAM_DBG(CAM_OIS, "Exit");

	return rc;
}

int cam_ois_get_ois_mode(struct cam_ois_ctrl_t *o_ctrl, uint16_t *mode)
{
	if (!o_ctrl)
		return -1;

	*mode = o_ctrl->ois_mode;
	return 0;
}

/*** Have to lock/unlock ois_mutex, before/after call this function ***/
int cam_ois_set_ois_mode(struct cam_ois_ctrl_t *o_ctrl, uint16_t mode)
{
	int rc = 0;

	if (!o_ctrl)
		return 0;

	if (mode == o_ctrl->ois_mode)
		return 0;

	rc = cam_ois_i2c_byte_write(o_ctrl, 0x0002, mode);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "i2c write fail");

	rc = cam_ois_i2c_byte_write(o_ctrl, 0x0000, 0x01); //servo on
	if (rc < 0)
		CAM_ERR(CAM_OIS, "i2c write fail");

	o_ctrl->ois_mode = mode;
	o_ctrl->is_servo_on = true;

	CAM_INFO(CAM_OIS, "set ois mode %d", mode);

	return rc;
}

int cam_ois_set_ois_op_mode(struct cam_ois_ctrl_t *o_ctrl)
{
	int ret = 0;

	if (!o_ctrl)
		return -1;

	ret = cam_ois_i2c_byte_write(o_ctrl, 0x00BE, 0x03); //op mode as dual

	if (ret < 0) {
		CAM_ERR(CAM_OIS, "i2c write fail");
		return ret;
	}

	return 0;
}

int cam_ois_fixed_aperture(struct cam_ois_ctrl_t *o_ctrl)
{
	uint8_t data[2] = { 0, };
	int rc = 0, val = 0;

	// OIS CMD(Fixed Aperture)
	val = o_ctrl->x_center;
	CAM_DBG(CAM_OIS, "Write X center %d", val);
	data[0] = val & 0xFF;
	data[1] = (val >> 8) & 0xFF;
	rc = camera_io_dev_write_seq(&o_ctrl->io_master_info,
			0x0022, data, CAMERA_SENSOR_I2C_TYPE_WORD,
			CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "Failed write X center");

	val = o_ctrl->y_center;
	CAM_DBG(CAM_OIS, "Write Y center %d", val);
	data[0] = val & 0xFF;
	data[1] = (val >> 8) & 0xFF;
	rc = camera_io_dev_write_seq(&o_ctrl->io_master_info,
			0x0024, data, CAMERA_SENSOR_I2C_TYPE_WORD,
			CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "Failed write Y center");

	// OIS fixed
	rc = cam_ois_set_ois_mode(o_ctrl, 0x02);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, "ois set fixed mode failed %d", rc);
		return rc;
	}
	return rc;
}



