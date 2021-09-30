#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "status.h"
#include "main.h"
#include "crc16.h"
#include "vcp.h"

#define SEND_SHORT_STATUS       1

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
extern USBD_Class_cb_TypeDef  USBD_MSC_CDC_cb;

/** @defgroup APP_VCP_Private_Variables
  * @{
  */ 
  
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
   
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;


/**
 * Указатель на нашу структуру - пакет на прием и передачу. 
 * Небольшой размер буфера, поэтому память динамически не выделяем
 */
static XCHG_DATA_PACK xchg_buf;

/* Адрес устройства */
static DEV_ADDR_PACK dev_addr;

/* Щетчики обмена */
static UART_COUNTS_PACK uart_cnt_a2341; //-  ЗДЕСЬ МАЖЕТ ПО ПАМЯТИ!!!
static UART_COUNTS_PACK xchg_counts;




/****************************** статические функции***************************/
static void vcp_rx_reset(int);
static void vcp_send_data(void*, int);

/**
 * Инициализация виртуального порта 
 */
void vcp_init(void)
{
    dev_addr.ver = 0;			/* Версия ПО: 1, 2, 3, 4 итд */
    dev_addr.rev = 120;			/* Ревизия ПО 0, 1, 2 итд */
    dev_addr.time = 1498422990;		/* Время компиляции */
    dev_addr.addr = 7;			/* Адрес платы */

    memset(&xchg_buf, 0, sizeof(xchg_buf));
    memset(&xchg_counts, 0, sizeof(UART_COUNTS_PACK));

    /* USB configuration */
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc,  &USBD_MSC_CDC_cb, &USR_cb);
}


/**
 * данные из COM-порта в приемный буфер. Это не функция прерывания, поэтому
 * можно передавать данные и и делать прочие вещи
 */
void vcp_rx_data(uint8_t * buf, int len)
{
#if 1
	VCP_DataTx(buf, len);
#else

    while (len--) {

	volatile uint8_t rx_byte = *buf++;

	/* Вычисляем CRC16. При правильном приеме последние числа д.б. = 0 */
	xchg_buf.rx_ind = (uint8_t) ((xchg_buf.crc16 >> 8) & 0xff);
	xchg_buf.crc16 = (uint16_t) (((xchg_buf.crc16 & 0xff) << 8) | (rx_byte & 0xff));
	xchg_buf.crc16 ^= get_crc16_table(xchg_buf.rx_ind);


	/* Пришел самый первый байт посылки и наш адрес */
	if (xchg_buf.rx_beg == 0 && rx_byte == DEV_ADDR) {	/* Первый байт приняли: СТАРТ - FF */
	    xchg_buf.rx_beg = 1;	/* Один байт приняли */
	    xchg_buf.crc16 = rx_byte;	/* Контрольная сумма равна первому байту */
	    xchg_buf.rx_tag = 0;	/* Метка для отладки */
	    xchg_buf.rx_cnt = 0;	/* Счетчик пакетов - первый байт приняли */
	} else {		/* Пришли последующие байты */
	    if (xchg_buf.rx_cnt == 1) {
		if (rx_byte != 0) {	/* рвем передачу */
		    vcp_rx_reset(RX_CMD_ERR);
		}
	    } else if (xchg_buf.rx_cnt == 2) {
		if (rx_byte == 0) {	/* рвем передачу-нуля быть не может */
		    vcp_rx_reset(RX_CMD_ERR);
		}
		xchg_buf.rx_len = rx_byte;	/* Второй байт это длина всей следующей посылки - не может быть 0! */
		/* 3...все остальное считается как единая посылка если она есть. Или как команда модему и пр. */
	    } else if (xchg_buf.rx_cnt > 2 && xchg_buf.rx_cnt < (xchg_buf.rx_len + 3)) {	/* Данные начинаются с 4-го байта */
		xchg_buf.rx_buf[xchg_buf.rx_cnt - 3] = rx_byte;	/* В приемный буфер принятую посылку */
	    } else if (xchg_buf.rx_cnt == (xchg_buf.rx_len + 3)) {	/* Ст. байт контрольной суммы  */
		xchg_buf.rx_tag = (uint16_t) rx_byte << 8;	/* label BADD - для отладки */
	    } else if (xchg_buf.rx_cnt == (xchg_buf.rx_len + 4)) {	/* Мл. байт контрольной суммы  */
		xchg_buf.rx_tag |= rx_byte;

		/*  Crc16 правильная ? */
		if (xchg_buf.crc16 == 0 || xchg_buf.rx_tag == RX_LABEL) {
		    vcp_rx_reset(RX_OK);
                    

		    /* какая команда? */
		    xchg_buf.rx_cmd = xchg_buf.rx_buf[0];

		    /* Разбор команд  */
		    switch (xchg_buf.rx_cmd) {

		    case UART_CMD_COMMAND_PC:
			vcp_send_data(&dev_addr, sizeof(DEV_ADDR_PACK));
			break;

			/* Выдать статус устройства - будет в буфере передачи */
		    case UART_CMD_GET_DSP_STATUS:
                        DEV_STATUS_PACK  stat;
                        status_get(&stat);
			vcp_send_data(&stat, sizeof(DEV_STATUS_PACK));
			break;

			/* Счетчики обмена */
		    case UART_CMD_GET_COUNTS:
			vcp_send_data(&xchg_counts, sizeof(UART_COUNTS_PACK));
			break;

			/* Синхронизация часов RTC */
		    case UART_CMD_SYNC_RTC_CLOCK:
			static int time;
                        memcpy(&time, xchg_buf.rx_buf + 1, sizeof(time));
                        status_sync_time(time);                       
                        // поставить статус "есть время"
                        status_get(&stat);
			vcp_send_data(&stat, SEND_SHORT_STATUS);
			break;

                      /* Сброс платы  */
		    case UART_CMD_DSP_RESET:
                        status_get(&stat);
			vcp_send_data(&stat, SEND_SHORT_STATUS);
                        vTaskDelay(50);
                        status_dev_reset();
			break;


                    // Включить насос                        
		    case UART_CMD_PUMP_ON:
                      status_pump_on();
                      status_get(&stat);
                      vcp_send_data(&stat, SEND_SHORT_STATUS);                    
                      break;

                  // вЫключить насос                      
                    case UART_CMD_PUMP_OFF:
                      status_pump_off();
                      status_get(&stat);
                      vcp_send_data(&stat, SEND_SHORT_STATUS);
                      break;
                        
                      /* Старт измерения  */
		    case UART_CMD_DEV_START:
                        status_get(&stat);
			vcp_send_data(&stat, SEND_SHORT_STATUS);
			break;

                        /* Стоп измерений */
		    case UART_CMD_DEV_STOP:
                        status_get(&stat);
			vcp_send_data(&stat, SEND_SHORT_STATUS);
			break;

                         /* Запуск тестирования */ 
		    case UART_CMD_INIT_TEST:
                        status_get(&stat);
			vcp_send_data(&stat, SEND_SHORT_STATUS);
			break;

		    default:
                        vcp_rx_reset(RX_CMD_ERR);
			break;
		    }
		} else {
		    vcp_rx_reset(RX_CRC_ERR);	/* Ошибка CRC16 */
		}
	    } else {
		vcp_rx_reset(RX_CMD_ERR);
	    }
	}
	xchg_buf.rx_cnt++;	/* Счетчик пакетов */
        if(xchg_buf.rx_cnt >=BUF_IN_SIZE)
           xchg_buf.rx_cnt = 0;
    }
#endif
}

/**
 * Сброс приемных счетчиков
 */
static void vcp_rx_reset(int err)
{
    /* Всегда сбрасываем */
    xchg_buf.rx_beg = 0;
    xchg_buf.rx_cnt = 0;
    xchg_buf.rx_len = 0;
       
    switch (err) {
    case RX_OK:
	xchg_counts.rx_pack++;	/* Принятые пакеты */
	break;

    case RX_CMD_ERR:
	xchg_counts.rx_cmd_err++;	/* Ошибка в команде */
	break;

    case RX_STAT_ERR:
	xchg_counts.rx_stat_err++;	/* Ошибка набегания и пр. */
	break;

    case RX_CRC_ERR:
	xchg_counts.rx_crc_err++;	/* Ошибка в CRC */
	break;

    default:
	break;
    }
}


/**
 * Послать любые данные в порт
 */
static void vcp_send_data(void* data, int len)
{
	/* Скопируем в буфер вывода с 1-го символа */
	memcpy(xchg_buf.tx_buf + 1, data, len);
	xchg_buf.tx_buf[0] = len; /* в 0-м байте длина посылки */
	add_crc16(xchg_buf.tx_buf);
	VCP_DataTx(xchg_buf.tx_buf, len + 3);
        xchg_counts.tx_pack++;
}
