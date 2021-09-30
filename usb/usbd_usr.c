#include "usbd_usr.h"
#include "usbd_ioreq.h"



/** @defgroup USBD_USR_Private_Variables
* @{
*/

USBD_Usr_cb_TypeDef USR_cb = {
    USBD_USR_Init,
    USBD_USR_DeviceReset,
    USBD_USR_DeviceConfigured,
    USBD_USR_DeviceSuspended,
    USBD_USR_DeviceResumed,
    USBD_USR_DeviceConnected,
    USBD_USR_DeviceDisconnected,
};

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{
    /* Initialize LEDs */
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);

    /* Initialize the LCD */
#if defined (USE_STM322xG_EVAL)
    STM322xG_LCD_Init();
#elif defined(USE_STM324xG_EVAL)
    STM324xG_LCD_Init();
#elif defined (USE_STM3210C_EVAL)
    STM3210C_LCD_Init();
#else
//#error "Missing define: Evaluation board (ie. USE_STM322xG_EVAL)"
#endif

  //  LCD_LOG_Init();

#ifdef USE_USB_OTG_HS
    LCD_LOG_SetHeader("USB OTG HS Composite Dev");
#else
    //LCD_LOG_SetHeader("USB OTG FS Composite Dev");
#endif
    //LCD_UsrLog("> USB device library started.\n");
    //LCD_LOG_SetFooter("     USB Device Library v1.1.0");
}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed)
{
    switch (speed) {
    case USB_OTG_SPEED_HIGH:
	//LCD_LOG_SetFooter("     USB Device Library v1.1.0 [HS]");
	break;

    case USB_OTG_SPEED_FULL:
	//LCD_LOG_SetFooter("     USB Device Library v1.1.0 [FS]");
	break;
    default:
	//LCD_LOG_SetFooter("     USB Device Library v1.1.0 [??]");
    }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConfigured(void)
{
    //LCD_UsrLog("> Composite Interface configured.\n");
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
    //LCD_UsrLog("> USB Device in Suspend Mode.\n");
    /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
    //LCD_UsrLog("> USB Device in Idle Mode.\n");
    /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConnected(void)
{
    //LCD_UsrLog("> USB Device Connected.\n");
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceDisconnected(void)
{
    //LCD_UsrLog("> USB Device Disconnected.\n");
}

