/*
 * Driver for pwm demo on ybs board.
 *
 */
#define DEBUG
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

//address
#define FIREFLY_SPI_READ_ID_CMD 0x9F

#define FIREFLY_SPI_PRINT_ID(rbuf) \
	do { \
		if (status == 0) \
			dev_dbg(&spi->dev, "%s: ID = %02x %02x %02x %02x %02x\n", __FUNCTION__, \
				rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]); \
		else \
			dev_err(&spi->dev, "%s: read ID error\n", __FUNCTION__); \
	}while(0)


/******************************************************************************
* 函数名：（通讯方式一）ybs_spi_read_w25x_id_0
* 功  能：通过spi向reg写入tbuf后读出rbuf
* 输  入：spi：spi设备树
* 返  回：无
*/
static int ybs_spi_read_w25x_id_0(struct spi_device *spi)
{	
	int	status;
	char tbuf[]={FIREFLY_SPI_READ_ID_CMD};
	char rbuf[5];

	struct spi_transfer	t = {
		.tx_buf		= tbuf,
		.len		= sizeof(tbuf),
	};

	struct spi_transfer     r = {
		.rx_buf         = rbuf,
		.len            = sizeof(rbuf),
	};
	struct spi_message      m;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	spi_message_add_tail(&r, &m);
	status = spi_sync(spi, &m);

	printk("%s ID = %02x %02x %02x %02x %02x\n", __FUNCTION__, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
	return status;
}

/******************************************************************************
* 函数名：（通讯方式二）ybs_spi_read_w25x_id_1
* 功  能：通过spi向reg写入tbuf后读出rbuf
* 输  入：spi：spi设备树
* 返  回：无
*/
static int ybs_spi_read_w25x_id_1(struct spi_device *spi)
{
	int	status;
	char tbuf[] = {FIREFLY_SPI_READ_ID_CMD};
	char rbuf[5];

	status = spi_write_then_read(spi, tbuf, sizeof(tbuf), rbuf, sizeof(rbuf));
	printk("%s ID = %02x %02x %02x %02x %02x\n", __FUNCTION__, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
	return status;
}

static int ybs_spi_probe(struct spi_device *spi)
{
    int ret = 0;
    struct device_node __maybe_unused *np = spi->dev.of_node;

    dev_dbg(&spi->dev, "ybs SPI demo program\n");
    printk("ybs spi demo\r\n");
	if(!spi)	
		return -ENOMEM;

	dev_dbg(&spi->dev, "ybs_spi_probe: setup mode %d, %s%s%s%s%u bits/w, %u Hz max\n",
			(int) (spi->mode & (SPI_CPOL | SPI_CPHA)),
			(spi->mode & SPI_CS_HIGH) ? "cs_high, " : "",
			(spi->mode & SPI_LSB_FIRST) ? "lsb, " : "",
			(spi->mode & SPI_3WIRE) ? "3wire, " : "",
			(spi->mode & SPI_LOOP) ? "loopback, " : "",
			spi->bits_per_word, spi->max_speed_hz);

	ybs_spi_read_w25x_id_0(spi);
	ybs_spi_read_w25x_id_1(spi);
	
    return ret;
}

static int ybs_spi_remove(struct spi_device *spi)
{
    return 0;
}


static struct of_device_id ybs_match_table[] = {
	{ .compatible = "ybs,rk3308-spi",},
	{},
};

static struct spi_driver ybs_spi_driver = {
	.driver = {
		.name = "ybs-spi",
		.of_match_table = of_match_ptr(ybs_match_table),
	},
	.probe = ybs_spi_probe,
	.remove = ybs_spi_remove,
};

static int ybs_spi_init(void)
{
	int retval;
	retval = spi_register_driver(&ybs_spi_driver);
	printk(KERN_ALERT "register ybs_spi_init spi return v = :%d\n",retval);
	return retval;
}

module_init(ybs_spi_init);

static void ybs_spi_exit(void)
{
	spi_unregister_driver(&ybs_spi_driver);
}
module_exit(ybs_spi_exit);

MODULE_AUTHOR("zhang <zhangdexing02@countrygarden.com.cn>");
MODULE_DESCRIPTION("ybs SPI demo driver");
MODULE_ALIAS("platform:ybs-spi");
MODULE_LICENSE("GPL");
