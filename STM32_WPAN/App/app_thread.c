/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : App/app_thread.c
  * Description        : Thread Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "utilities_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_thread.h"
#include "stm32wbxx_core_interface_def.h"
#include "openthread_api_wb.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32_lpm.h"
#include "stm32_seq.h"
#if (CFG_USB_INTERFACE_ENABLE != 0)
#include "vcp.h"
#include "vcp_conf.h"
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "stdio.h"
#include "../../Drivers/OLED/ssd1306.h"
#include "node_config.h"
#include "../../Drivers/SPS30/sps30.h"
#include "sensor_config.h"
#include "../../Drivers/Common/CommonHelpers.h"
#include "../../Drivers/Common/DataStructures.h"
#include "../../Drivers/StateMachine/StateMachine.h"
#include "../../Drivers/HDC/HDC2080.h"
#include "../../Drivers/ADS1110/ADS.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define C_SIZE_CMD_STRING       256U

/* USER CODE BEGIN PD */
#define SEC_5	(5 * 1000 * 1000 / CFG_TS_TICK_VAL)
#define SEC_30	(30 * 1000 * 1000 / CFG_TS_TICK_VAL)
#define MIN_5	(5 * 60 * 1000 * 1000 / CFG_TS_TICK_VAL)
#define MIN_10	(10 * 60 * 1000 * 1000 / CFG_TS_TICK_VAL)
#define MIN_30	(30 * 60 * 1000 * 1000 / CFG_TS_TICK_VAL)

uint8_t standbyTimerId = 255;
uint32_t sensor_measurement_interval = 0;

uint8_t sps_running = 0;
uint8_t sps_should_stop = 0; //stop sps sensor on disconnect

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/**
 * This macro checks for the specified status, which is expected to commonly be successful, and branches to the local
 * label 'exit' if the status is unsuccessful.
 *
 *  @param[in]  aStatus     A scalar status to be evaluated against zero (0).
 *
 */
#define SuccessOrExit(aStatus) \
    do                         \
    {                          \
        if ((aStatus) != 0)    \
        {                      \
            goto exit;         \
        }                      \
    } while (false)

/**
 * This macro checks for the specified condition, which is expected to commonly be true, and both executes @a ... and
 * branches to the local label 'exit' if the condition is false.
 *
 *  @param[in]  aCondition  A Boolean expression to be evaluated.
 *  @param[in]  ...         An expression or block to execute when the assertion fails.
 *
 */
#define VerifyOrExit(aCondition, ...) \
    do                                \
    {                                 \
        if (!(aCondition))            \
        {                             \
            __VA_ARGS__;              \
            goto exit;                \
        }                             \
    } while (false)

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
static void APP_THREAD_CheckWirelessFirmwareInfo(void);
static void APP_THREAD_DeviceConfig(void);
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext);
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode);
#if (CFG_FULL_LOW_POWER == 0)
static void Send_CLI_To_M0(void);
#endif /* (CFG_FULL_LOW_POWER == 0) */
static void Send_CLI_Ack_For_OT(void);
static void HostTxCb( void );
static void Wait_Getting_Ack_From_M0(void);
static void Receive_Ack_From_M0(void);
static void Receive_Notification_From_M0(void);
#if (CFG_HW_LPUART1_ENABLED == 1)
extern void MX_LPUART1_UART_Init(void);
#endif
#if (CFG_HW_USART1_ENABLED == 1)
extern void MX_USART1_UART_Init(void);
#endif
#if (CFG_USB_INTERFACE_ENABLE != 0)
static uint32_t ProcessCmdString(uint8_t* buf , uint32_t len);
#else
#if (CFG_FULL_LOW_POWER == 0)
static void RxCpltCallback(void);
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

/* USER CODE BEGIN PFP */
static otError UdpBind(uint16_t aPort);
static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
static void APP_THREAD_UdpSend(void);
//static otError UdpSendSPS30(struct sps30_measurement meas);
int send_meas_data_packet(uint8_t meas_data_type, uint8_t *meas_data, uint8_t meas_data_length);
int send_sps30_meas_data(struct sps30_measurement meas);
int send_ads_data(struct ADS_data *meas);
int send_hdc_data(struct HDC_data *meas);

static void APP_THREAD_SPS_WarmUP(void);
static void StartTask();
static void RestartTask();
static void StopTask();
void RunSendMeas();

static void calculate_times(uint32_t *interval);
static uint32_t get_timeout(uint32_t ms);

static int is_sps30_connected();

static void warm_up(void);

/* USER CODE END PFP */

/* Private variables -----------------------------------------------*/
#if (CFG_USB_INTERFACE_ENABLE != 0)
static uint8_t TmpString[C_SIZE_CMD_STRING];
static uint8_t VcpRxBuffer[sizeof(TL_CmdSerial_t)];        /* Received Data over USB are stored in this buffer */
static uint8_t VcpTxBuffer[sizeof(TL_EvtPacket_t) + 254U]; /* Transmit buffer over USB */
#else
#if (CFG_FULL_LOW_POWER == 0)
static uint8_t aRxBuffer[C_SIZE_CMD_STRING];
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

#if (CFG_FULL_LOW_POWER == 0)
static uint8_t CommandString[C_SIZE_CMD_STRING];
#endif /* (CFG_FULL_LOW_POWER == 0) */
static __IO uint16_t indexReceiveChar = 0;
static __IO uint16_t CptReceiveCmdFromUser = 0;

static TL_CmdPacket_t *p_thread_otcmdbuffer;
static TL_EvtPacket_t *p_thread_notif_M0_to_M4;
static __IO uint32_t  CptReceiveMsgFromM0 = 0;
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_TH_Config_t ThreadConfigBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ThreadOtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ThreadNotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ThreadCliCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ThreadCliNotBuffer;
extern uint8_t g_ot_notification_allowed;

/* USER CODE BEGIN PV */
otUdpSocket mSocket;
static bool run_meas = false;

const uint16_t NODE_ID_ = NODE_ID;



otNetworkKey networkKey = {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xAA, 0x55}};
const otMeshLocalPrefix mesh_local_prefix = {{0xDE, 0xAD, 0xBE, 0xAF, 0x00, 0x00, 0x00, 0x00}};
#define C_PANID                 0x2255U
#define C_CHANNEL_NB            20U
#define UDP_PORT 1234

uint16_t udpPort = UDP_PORT;

static SensorType connected_sensors[4] = {None, None, None, None};
static uint32_t time_interval = 0;
static States current_state = Off;

/* USER CODE END PV */

/* Functions Definition ------------------------------------------------------*/

void APP_THREAD_Init( void )
{
  /* USER CODE BEGIN APP_THREAD_INIT_1 */
  char dev_name[20];

  /* USER CODE END APP_THREAD_INIT_1 */

  SHCI_CmdStatus_t ThreadInitStatus;

  /* Check the compatibility with the Coprocessor Wireless Firmware loaded */
  APP_THREAD_CheckWirelessFirmwareInfo();

#if (CFG_USB_INTERFACE_ENABLE != 0)
  VCP_Init(&VcpTxBuffer[0], &VcpRxBuffer[0]);
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

  /* Register cmdbuffer */
  APP_THREAD_RegisterCmdBuffer(&ThreadOtCmdBuffer);

  /**
   * Do not allow standby in the application
   */
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP_THREAD, UTIL_LPM_DISABLE);

  /* Init config buffer and call TL_THREAD_Init */
  APP_THREAD_TL_THREAD_INIT();

  /* Configure UART for sending CLI command from M4 */
  APP_THREAD_Init_UART_CLI();

  /* Send Thread start system cmd to M0 */
  ThreadInitStatus = SHCI_C2_THREAD_Init();

  /* Prevent unused argument(s) compilation warning */
  UNUSED(ThreadInitStatus);

  /* Register task */
  /* Create the different tasks */
  UTIL_SEQ_RegTask( 1<<(uint32_t)CFG_TASK_MSG_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_THREAD_ProcessMsgM0ToM4);

  /* USER CODE BEGIN INIT TASKS */

  UTIL_SEQ_RegTask( 1<<(uint32_t)CFG_TASK_UDP_SEND, UTIL_SEQ_RFU, APP_THREAD_UdpSend);
  UTIL_SEQ_RegTask( 1<<(uint32_t)CFG_TASK_SPS_WARMUP, UTIL_SEQ_RFU, warm_up);

  //HW_TS_Create(CFG_TIM_PROC_ID_ISR, &standbyTimerId, hw_ts_SingleShot, APP_THREAD_UdpSend); //next will be this function, start the timer from warmup
    if ( HW_TS_Create(CFG_TIM_PROC_ID_ISR, &standbyTimerId, hw_ts_SingleShot, RunSendMeas) == hw_ts_Successful )
	{
		printf("HW_TS_Create successful\r\n");
	}
	else
	{
		printf("HW_TS_Create failed\r\n");
	}
  /* USER CODE END INIT TASKS */

  /* Initialize and configure the Thread device*/
  APP_THREAD_DeviceConfig();

  /* USER CODE BEGIN APP_THREAD_INIT_2 */

#ifndef AUTO_SCAN_SENSORS
	#ifdef SPS30_enabled

  	for(int i=0;i<3;i++)
	{
		if(sps30_probe() == 0)
		{
			int ret = sps30_start_measurement();
			if (ret < 0)
			{
			    printf("error starting measurement\r\n");
			}
			else
			{
				printf("measurements started\r\n");
				break;
			}
		}
		HAL_Delay(500);
	}


	/*
	uint8_t fw_major;
	uint8_t fw_minor;
	int16_t ret = sps30_read_firmware_version(&fw_major, &fw_minor);
	if (ret)
	{
		printf("error reading firmware version\r\n");
	}
	else
	{
		printf("FW: %u.%u\r\n", fw_major, fw_minor);
	}
	*/

	#endif
#endif // AUTO_SCAN_SENSORS

#ifdef AUTO_SCAN_SENSORS
  	printf("Scanning all bus for connected sensors\r\n");
  	for (uint8_t i = 0; i < 4; i++)
  	{
  		printf("Scanning bus %d...\r\n", i);
  		connected_sensors[i] = i2c_sensor_scan(i);
  		get_sensor_name_by_type(connected_sensors[i], dev_name, 20);
  		printf("Found device on bus %d: %s\r\n", i, dev_name);
  		init_sensor_on_bus_by_type(connected_sensors[i], i);
  	}
#endif

  /* USER CODE END APP_THREAD_INIT_2 */
}

/**
  * @brief  Trace the error or the warning reported.
  * @param  ErrId :
  * @param  ErrCode
  * @retval None
  */
void APP_THREAD_Error(uint32_t ErrId, uint32_t ErrCode)
{
  /* USER CODE BEGIN APP_THREAD_Error_1 */

  /* USER CODE END APP_THREAD_Error_1 */
  switch(ErrId)
  {
  case ERR_REC_MULTI_MSG_FROM_M0 :
    APP_THREAD_TraceError("ERROR : ERR_REC_MULTI_MSG_FROM_M0 ", ErrCode);
    break;
  case ERR_THREAD_SET_STATE_CB :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_STATE_CB ",ErrCode);
    break;
  case ERR_THREAD_ERASE_PERSISTENT_INFO :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_ERASE_PERSISTENT_INFO ",ErrCode);
    break;
  case ERR_THREAD_SET_NETWORK_KEY :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_NETWORK_KEY ",ErrCode);
    break;
  case ERR_THREAD_CHECK_WIRELESS :
    APP_THREAD_TraceError("ERROR : ERR_THREAD_CHECK_WIRELESS ",ErrCode);
    break;
  /* USER CODE BEGIN APP_THREAD_Error_2 */
  case ERR_THREAD_SET_CHANNEL :
      APP_THREAD_TraceError("ERROR : ERR_THREAD_SET_CHANNEL ",ErrCode);
      break;

  /* USER CODE END APP_THREAD_Error_2 */
  default :
    APP_THREAD_TraceError("ERROR Unknown ", 0);
    break;
  }
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief Thread initialization.
 * @param  None
 * @retval None
 */
static void APP_THREAD_DeviceConfig(void)
{
  otError error;
  error = otSetStateChangedCallback(NULL, APP_THREAD_StateNotif, NULL);
  if (error != OT_ERROR_NONE)
  {
    APP_THREAD_Error((uint32_t)ERR_THREAD_SET_STATE_CB, (uint32_t)ERR_INTERFACE_FATAL);
  }

  /* USER CODE BEGIN DEVICECONFIG */


    error = otInstanceErasePersistentInfo(NULL);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_ERASE_PERSISTENT_INFO,error);
    }
    otInstanceFinalize(NULL);
    otInstanceInitSingle();
    error = otSetStateChangedCallback(NULL, APP_THREAD_StateNotif, NULL);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_SET_STATE_CB,error);
    }
    error = otLinkSetChannel(NULL, C_CHANNEL_NB);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_SET_CHANNEL,error);
    }
    error = otLinkSetPanId(NULL, C_PANID);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_SET_PANID,error);
    }
    error = otThreadSetNetworkKey(NULL, &networkKey);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_SET_NETWORK_KEY,error);
    }
/*
    otNetifAddress aAddress;
    error = otIp6AddressFromString("DEAD:BEAF:0000:0000:0123:aaaa:2222:0020", &aAddress);
    if (error != OT_ERROR_NONE)
    {
        		printf("otIp6AddressFromString error\r\n");
        		while(1);
    }

    error = otIp6AddUnicastAddress (NULL,  &aAddress);
   	if (error != OT_ERROR_NONE)
    {
   		      printf("otIp6AddUnicastAddress error\r\n");
   		      while(1);
   	 }
*/
    error = otThreadSetMeshLocalPrefix(NULL, &mesh_local_prefix);
    if (error != OT_ERROR_NONE)
    {
    	printf("otThreadSetMeshLocalPrefix error\r\n");
    	while(1);
    }


    error = otIp6SetEnabled(NULL, true);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_IPV6_ENABLE,error);
    }
    error = otThreadSetEnabled(NULL, true);
    if (error != OT_ERROR_NONE)
    {
      APP_THREAD_Error(ERR_THREAD_START,error);
    }


    memset(&mSocket, 0, sizeof(mSocket));

    /* Open socket */
    otUdpOpen(NULL, &mSocket, HandleUdpReceive, NULL);
    UdpBind(UDP_PORT);
  /* USER CODE END DEVICECONFIG */
}

/**
 * @brief Thread notification when the state changes.
 * @param  aFlags  : Define the item that has been modified
 *         aContext: Context
 *
 * @retval None
 */
static void APP_THREAD_StateNotif(uint32_t NotifFlags, void *pContext)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pContext);

  /* USER CODE BEGIN APP_THREAD_STATENOTIF */
  static uint32_t child_notif = 0;
  /* USER CODE END APP_THREAD_STATENOTIF */

  if ((NotifFlags & (uint32_t)OT_CHANGED_THREAD_ROLE) == (uint32_t)OT_CHANGED_THREAD_ROLE)
  {
	  switch (otThreadGetDeviceRole(NULL))
	      {
	      case OT_DEVICE_ROLE_DISABLED:
	        /* USER CODE BEGIN OT_DEVICE_ROLE_DISABLED */
	    	HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, 0);
	    	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, 0);
	    	printf("Disabled Device\r\n");
	        /* USER CODE END OT_DEVICE_ROLE_DISABLED */
	        break;
	      case OT_DEVICE_ROLE_DETACHED:
	        /* USER CODE BEGIN OT_DEVICE_ROLE_DETACHED */
	        HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, 0);
	    	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, 0);
	    	run_meas = false;
	    	printf("Detached Device\r\n");
	    	StopTask();
	    	ssd1306_Fill(Black);

	    	char buffer[100];
	    	snprintf (buffer, 100,"Thread Detached!");
	    	ssd1306_SetCursor(0, 0);
	    	ssd1306_WriteString(buffer, Font_6x8, White);
	    	ssd1306_UpdateScreen();
	        /* USER CODE END OT_DEVICE_ROLE_DETACHED */
	        break;
	      case OT_DEVICE_ROLE_CHILD:
	        /* USER CODE BEGIN OT_DEVICE_ROLE_CHILD */
	        HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, 0);
	    	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, 1);
	    	if(run_meas == false)
	    	{
	    		HAL_Delay(1000U);
	    		run_meas = true;
	    		//UTIL_SEQ_SetTask(TASK_UDP_SEND, CFG_SCH_PRIO_1);
	    		printf("Child Device\r\n");
	    		RestartTask(); //stops if running and starts a new one
	    	}
	        /* USER CODE END OT_DEVICE_ROLE_CHILD */
	        break;
	      case OT_DEVICE_ROLE_ROUTER :
	        /* USER CODE BEGIN OT_DEVICE_ROLE_ROUTER */
	    	 HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, 1);
	    	 HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, 0);
	    	 if(run_meas == false)
	    	 {
	    	 	   HAL_Delay(1000U);
	    	 	   run_meas = true;
	    	 	   //UTIL_SEQ_SetTask(TASK_UDP_SEND, CFG_SCH_PRIO_1);
	    	 	  printf("Router Device\r\n");
	    	 	   RestartTask(); //stops if running and starts a new one
	    	 }
	        /* USER CODE END OT_DEVICE_ROLE_ROUTER */
	        break;
	      case OT_DEVICE_ROLE_LEADER :
	        /* USER CODE BEGIN OT_DEVICE_ROLE_LEADER */
	        HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, 1);
	    	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, 1);
	    	if(run_meas == false)
	    	{
	    		HAL_Delay(1000U);
	    		run_meas = true;
	    		//UTIL_SEQ_SetTask(TASK_UDP_SEND, CFG_SCH_PRIO_1);
	    		printf("Leader Device\r\n");
	    		RestartTask(); //stops if running and starts a new one
	    	}
	        /* USER CODE END OT_DEVICE_ROLE_LEADER */
	        break;
	      default:
	        /* USER CODE BEGIN DEFAULT */
	    	HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, 0);
	    	HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, 0);
	    	run_meas = false;
	    	StopTask();
	    	printf("Default Device\r\n");
	        /* USER CODE END DEFAULT */
	        break;
	      }
  }
}

/**
  * @brief  Warn the user that an error has occurred.In this case,
  *         the LEDs on the Board will start blinking.
  *
  * @param  pMess  : Message associated to the error.
  * @param  ErrCode: Error code associated to the module (OpenThread or other module if any)
  * @retval None
  */
static void APP_THREAD_TraceError(const char * pMess, uint32_t ErrCode)
{
  /* USER CODE BEGIN TRACE_ERROR */

  /* USER CODE END TRACE_ERROR */
}

/**
 * @brief Check if the Coprocessor Wireless Firmware loaded supports Thread
 *        and display associated information
 * @param  None
 * @retval None
 */
static void APP_THREAD_CheckWirelessFirmwareInfo(void)
{
  WirelessFwInfo_t wireless_info_instance;
  WirelessFwInfo_t* p_wireless_info = &wireless_info_instance;

  if (SHCI_GetWirelessFwInfo(p_wireless_info) != SHCI_Success)
  {
    APP_THREAD_Error((uint32_t)ERR_THREAD_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
  }
  else
  {
    APP_DBG("**********************************************************");
    APP_DBG("WIRELESS COPROCESSOR FW:");
    /* Print version */
    APP_DBG("VERSION ID = %d.%d.%d", p_wireless_info->VersionMajor, p_wireless_info->VersionMinor, p_wireless_info->VersionSub);

    switch(p_wireless_info->StackType)
    {
    case INFO_STACK_TYPE_THREAD_FTD :
      APP_DBG("FW Type : Thread FTD");
      break;
    case INFO_STACK_TYPE_THREAD_MTD :
      APP_DBG("FW Type : Thread MTD");
      break;
    case INFO_STACK_TYPE_BLE_THREAD_FTD_STATIC :
      APP_DBG("FW Type : Static Concurrent Mode BLE/Thread");
      break;
    default :
      /* No Thread device supported ! */
      APP_THREAD_Error((uint32_t)ERR_THREAD_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
      break;
    }
    APP_DBG("**********************************************************");
  }
}
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

static uint16_t Swap16(uint16_t v)
{
    return (((v & 0x00ffU) << 8) & 0xff00) | (((v & 0xff00U) >> 8) & 0x00ff);
}

static uint16_t HostSwap16(uint16_t v)
{
    return Swap16(v);
}

static otError UdpBind(uint16_t aPort)
{
    otError    error;
    otSockAddr sockaddr;
    otNetifIdentifier netif = OT_NETIF_THREAD;

    memset(&sockaddr, 0, sizeof(sockaddr));

    /* "::" specifies the IPv6 Unspecified Address */
    error = otIp6AddressFromString("::", &sockaddr.mAddress);
    SuccessOrExit(error);

    sockaddr.mPort    = aPort;

    error = otUdpBind(NULL, &mSocket, &sockaddr, netif);

exit:
    return error;
}

static uint32_t get_timeout(uint32_t ms)
{
	return (ms * 1000 * 1000 / CFG_TS_TICK_VAL);
}

static int is_sps30_connected()
{
	int sps_connected = 0;

	for (uint8_t i = 0; i < 4; i++)
	{
		if ( connected_sensors[i] == SPS30 )
		{
			sps_connected = 1;
		}
	}
	return sps_connected;
}

//don't stop sps for now
static void calculate_times(uint32_t *interval)
{
	int sps_connected = is_sps30_connected();
	switch(current_state)
	{
		case Off: //times for warm up
			//probably not needed
			if (sps_connected && !sps_running)
			{
				*interval = get_timeout(30000);
			}
			else
			{
				*interval = 0;
			}
			break;
		case WarmUP: //times before measurement
			if (sps_connected && sps_running != 1 && SEND_INTERVAL_MS > 30000 )
			{
				*interval = get_timeout(SEND_INTERVAL_MS);
			}
			else if ( sps_connected && sps_running != 1 )
			{
				*interval = get_timeout(30000); //if less than 30s set for sending
			}
			else
			{
				*interval = get_timeout(SEND_INTERVAL_MS);
			}
			break;
		case Measurement: //times after measurement
			*interval = get_timeout(SEND_INTERVAL_MS);
			break;
	}
}

static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
  int     length;
  static  uint32_t l_receive_count = 0;
  uint8_t udpBuffferReceived[256];

  printf("l_receive_count = %ld\r\n", l_receive_count++);
  printf("Received %d bytes from \r\n", otMessageGetLength(aMessage) - otMessageGetOffset(aMessage));
  printf(
         "%x:%x:%x:%x:%x:%x:%x:%x %d \r\n", HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[0]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[1]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[2]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[3]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[4]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[5]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[6]),
         HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[7]), aMessageInfo->mPeerPort);

  length = otMessageRead(aMessage, otMessageGetOffset(aMessage), udpBuffferReceived, sizeof(udpBuffferReceived) - 1);

  struct sps30_measurement *mp;

  mp = (struct sps30_measurement *) udpBuffferReceived;
  struct sps30_measurement m = *mp;

  ssd1306_Fill(Black);

   char buffer[100];

   snprintf (buffer, 100,"%.2f %.2f" ,  m.nc_10p0, m.typical_particle_size);
   ssd1306_SetCursor(0, 0);
   ssd1306_WriteString(buffer, Font_6x8, White);

   snprintf (buffer, 100,"%.2f %.2f %.2f %.2f %.2f %.2f" , m.nc_0p5, m.nc_1p0, m.nc_2p5, m.nc_4p0, m.nc_10p0, m.typical_particle_size);
   ssd1306_SetCursor(0, 20);
   ssd1306_WriteString(buffer, Font_6x8, White);

   snprintf (buffer, 100,"%.2f %.2f %.2f %.2f" , m.mc_1p0, m.mc_2p5, m.mc_4p0, m.mc_10p0);
   ssd1306_SetCursor(0, 40);
   ssd1306_WriteString(buffer, Font_6x8, White);

   ssd1306_UpdateScreen();


/*
  if(strcmp((char const*)udpBuffferReceived, (char const*)udpBufffer) == 0)
  {
    APP_DBG("Comparison OK!");
    BSP_LED_Toggle(LED_BLUE);
  }
  */
}

static void APP_THREAD_SPS_WarmUP(void)
{
	int16_t ret = sps30_start_measurement();
	if (ret < 0)
	{
		printf("error starting SPS30\r\n");
		return;
	}
	if (!standbyTimerId)
		printf("Timer id not set\r\n");
	else
		HW_TS_Start(standbyTimerId, SEC_30);
}

static void APP_THREAD_SPS_Stop(void)
{
	int16_t ret = sps30_stop_measurement();
	RestartTask();
	if (ret < 0)
	{
		printf("error stopping SPS30\r\n");
		return;
	}

}


static void APP_THREAD_UdpSend(void)
{
  static int time_cnt = 0;
  char buffer[100];
  calculate_times(&time_interval);
  //HW_TS_Create(CFG_TIM_PROC_ID_ISR, &standbyTimerId, hw_ts_SingleShot, APP_THREAD_UdpSend); //next will be this function, start the timer from warmup
  //UTIL_SEQ_SetTask(TASK_UDP_SEND, CFG_SCH_PRIO_1);
  //HW_TS_Start(standbyTimerId, time_interval);
  current_state = Measurement;

  printf("Next measurement: %ld\r\n", time_interval);

  int current_line = 0;

  //HAL_Delay(500U);
  //if(time_cnt >=20)  //wait n*delay time
  if(1)
  {
	  time_cnt = 0;

	  ssd1306_Fill(Black);

	  struct sps30_measurement m;
	  struct ADS_data ads;
	  struct HDC_data hdc;

	  for (uint8_t i = 0; i < 4; i++)
	  {
		  switch(connected_sensors[i])
		  {
		  case ADS:
			  ads_measure_single(i, ADS_SAMPLES_PER_SEC, ADS_GAIN, &ads.raw);
			  ads.milivolts = ads_get_mV(&ads.raw, ADS_SAMPLES_PER_SEC, ADS_GAIN);
			  send_ads_data(&ads);
			  snprintf (buffer, 100,"Raw: %d   mV: %.2f" , ads.raw, ads.milivolts);
			  ssd1306_SetCursor(current_line, 0);
			  ssd1306_WriteString(buffer, Font_6x8, White);
			  current_line += 20;
			  break;
		  case HDC2080:
			  hdc2080_measure(i, &hdc.temperature, &hdc.humidity);
			  send_hdc_data(&hdc);
			  snprintf (buffer, 100,"Temp: %.2f Hum: %d" , hdc.temperature, hdc.humidity);
			  ssd1306_SetCursor(current_line, 0);
			  ssd1306_WriteString(buffer, Font_6x8, White);
			  current_line += 20;
			  break;
		  case SPS30:
			  int16_t ret = sps30_read_measurement(&m);
			  if (ret < 0)
			  {
				printf("error reading measurement\r\n");
				//ssd1306_Fill(Black);
				//snprintf (buffer, 100,"SPS30 read error!");
				//ssd1306_SetCursor(0, 0);
				//ssd1306_WriteString(buffer, Font_6x8, White);
				//ssd1306_UpdateScreen();
				//if(run_meas)
				//	    UTIL_SEQ_SetTask(TASK_UDP_SEND, CFG_SCH_PRIO_1);
				//RestartTask();
				return;
			  }
			  snprintf (buffer, 100,"nc2.5 %.2f n/cm3  " , m.nc_2p5);
			  ssd1306_SetCursor(current_line, 0);
			  ssd1306_WriteString(buffer, Font_6x8, White);

			  snprintf (buffer, 100,"nc10 %.2f n/cm3  " , m.nc_10p0);
			  ssd1306_SetCursor(current_line, 13);
			  ssd1306_WriteString(buffer, Font_6x8, White);

			  snprintf (buffer, 100,"mc2.5 %.2f ug/m3   " , m.mc_2p5);
			  ssd1306_SetCursor(current_line, 26);
			  ssd1306_WriteString(buffer, Font_6x8, White);

			  snprintf (buffer, 100,"mc10 %.2f ug/m3   ",  m.mc_10p0);
			  ssd1306_SetCursor(current_line, 39);
			  ssd1306_WriteString(buffer, Font_6x8, White);
			  send_sps30_meas_data(m);
			  current_line += 20;
			  break;
		  default:
			  break;
		  }
	  }

	#ifdef emulate_sps30
	  static int cnt = 0;

	  m.nc_10p0 = cnt;
	  m.typical_particle_size = cnt;
	  m.nc_0p5 = cnt;
	  m.mc_1p0 = cnt;
	  m.mc_2p5 = 0;
	  m.mc_4p0 = 0;
	  m.mc_10p0 = 0;
	  m.nc_1p0 = 0;
	  m.nc_2p5 = 0;
	  m.nc_4p0 = 0;
	  cnt++;
	#else

	#endif


	  /* Send Udp request */
	  /*
	  if(run_meas)
	  	  send_sps30_meas_data(m);
	  */

	  /*
	  ssd1306_Fill(Black);


	  snprintf (buffer, 100,"nc2.5 %.2f n/cm3  " , m.nc_2p5);
	  ssd1306_SetCursor(0, 0);
	  ssd1306_WriteString(buffer, Font_6x8, White);

	  snprintf (buffer, 100,"nc10 %.2f n/cm3  " , m.nc_10p0);
	  ssd1306_SetCursor(0, 13);
	  ssd1306_WriteString(buffer, Font_6x8, White);

	  snprintf (buffer, 100,"mc2.5 %.2f ug/m3   " , m.mc_2p5);
	  ssd1306_SetCursor(0, 26);
	  ssd1306_WriteString(buffer, Font_6x8, White);

	  snprintf (buffer, 100,"mc10 %.2f ug/m3   ",  m.mc_10p0);
	  ssd1306_SetCursor(0, 39);
	  ssd1306_WriteString(buffer, Font_6x8, White);

	  ssd1306_UpdateScreen();
	  */

  }
  else
  {
	  time_cnt++;
  }

  //if(run_meas)
	//  UTIL_SEQ_SetTask(TASK_UDP_SEND, CFG_SCH_PRIO_1);
  //APP_THREAD_SPS_Stop();
}

int send_sps30_meas_data(struct sps30_measurement meas)
{
	return send_meas_data_packet(meas_data_type_SPS30, (uint8_t *)&meas, 10*sizeof(float));
}

int send_ads_data(struct ADS_data *meas)
{
	return send_meas_data_packet(meas_data_type_ADS, (uint8_t *)meas, sizeof(struct ADS_data));
}

int send_hdc_data(struct HDC_data *meas)
{
	return send_meas_data_packet(meas_data_type_HDC, (uint8_t *)meas, sizeof(struct HDC_data));
}
/*
static otError UdpSendSPS30(struct sps30_measurement meas)
{
    otError       error;
    otMessageInfo messageInfo;
    otMessage *   message = NULL;


    uint8_t udpBufffer[256] = "";
    memcpy(udpBufffer, (void *)&meas, 10*sizeof(float));

    memset(&messageInfo, 0, sizeof(messageInfo));

  // error = otIp6AddressFromString("ff02::1", &messageInfo.mPeerAddr);
    error = otIp6AddressFromString("DEAD:BEAF:0000:0000:0000:0000:0000:0000", &messageInfo.mPeerAddr);
    SuccessOrExit(error);

    messageInfo.mPeerPort    = udpPort;

    message = otUdpNewMessage(NULL, NULL);
    VerifyOrExit(message != NULL, error = OT_ERROR_NO_BUFS);

    error = otMessageAppend(message, udpBufffer, 10*sizeof(float));
    SuccessOrExit(error);

    printf("Sending UDP message %s", udpBufffer);
    error = otUdpSend(NULL, &mSocket, message, &messageInfo);

exit:

    if (error != OT_ERROR_NONE && message != NULL)
    {
        printf("UdpSend failed with error : %s", error);
        otMessageFree(message);
    }

    return error;
}
*/
int send_meas_data_packet(uint8_t meas_data_type, uint8_t *meas_data, uint8_t meas_data_length)
{
    otError       error;
    otMessageInfo messageInfo;
    otMessage *   message = NULL;

    if(meas_data_length > 128)
    	return -10;

    uint8_t udpBufffer[256];
    uint8_t udpbuffer_size = 0;

    memset(&messageInfo, 0, sizeof(messageInfo));
    error = otIp6AddressFromString("DEAD:BEAF:0000:0000:0000:0000:0000:0000", &messageInfo.mPeerAddr);
    if(error != OT_ERROR_NONE)
    	return -1;
    messageInfo.mPeerPort    = udpPort;

    //udp packet format
    //16bit node ID
    //8bit meas_data_type
    //8bit meas_data_length
    // meas_data_length * byte data


    *((uint16_t*)&udpBufffer[0]) = NODE_ID_;
    udpBufffer[2] = meas_data_type;
    udpBufffer[3] = meas_data_length;
    udpbuffer_size += 4;

    memcpy(udpBufffer+4, (void *)meas_data, meas_data_length);
    udpbuffer_size += meas_data_length;

    message = otUdpNewMessage(NULL, NULL);
    error = otMessageAppend(message, udpBufffer, udpbuffer_size);
    if(error != OT_ERROR_NONE)
       	return -2;

    error = otUdpSend(NULL, &mSocket, message, &messageInfo);
    if(error != OT_ERROR_NONE)
           	return -3;

    return 0;
}

/*
static void StartTaskBySensorType(SensorType type, uint8_t connector)
{

}
*/

static void warm_up(void)
{
	uint8_t sps_available = 0;
	printf("Warmup with timer id %d\r\n", standbyTimerId);
	if (current_state == Off && !standbyTimerId )
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			if ( init_sensor_on_bus_by_type(connected_sensors[i], i) != HAL_OK )
			{
				return;
			}
			if ( connected_sensors[i] == SPS30 )
			{
				sps_available = 1;
			}
		}
		sps_running = sps_available;
		current_state = WarmUP;
		calculate_times(&time_interval);
		//APP_THREAD_UdpSend
		HW_TS_Create(CFG_TIM_PROC_ID_ISR, &standbyTimerId, hw_ts_SingleShot, APP_THREAD_UdpSend);
		HW_TS_Start(standbyTimerId, SEC_5);
		current_state = Measurement;
		printf("Next measurement after warm up: %ld\r\n", SEC_5);
	}
}

static void StartTask()
{
	/*
	printf("SetTask called\r\n");
	UTIL_SEQ_SetTask(CFG_TASK_SPS_WARMUP, CFG_SCH_PRIO_1);
	printf("SetTask call ended\r\n");
	//warm_up();
	 */

	uint8_t sps_available = 0;
	printf("Warmup with timer id %d\r\n", standbyTimerId);
	if (current_state == Off && !standbyTimerId )
	{
		printf("Init connected sensors...\r\n");
		for (uint8_t i = 0; i < 4; i++)
		{
			if ( init_sensor_on_bus_by_type(connected_sensors[i], i) != HAL_OK )
			{
				return;
			}
			if ( connected_sensors[i] == SPS30 )
			{
				sps_available = 1;
			}
		}
		sps_running = sps_available;
		current_state = WarmUP;
		calculate_times(&time_interval);
		//APP_THREAD_UdpSend
		HW_TS_Start(standbyTimerId, SEC_5);
		//current_state = Measurement;
		//HAL_Delay(time_interval);
		//UTIL_SEQ_SetTask(CFG_TASK_UDP_SEND, CFG_SCH_PRIO_1);
		printf("Next measurement after warm up: %d\r\n", SEC_5);
		//UTIL_LPM_EnterLowPower();
	}
}


void RunSendMeas() {
	printf("Timer fired\r\n");
	//UTIL_SEQ_SetTask(CFG_TASK_UDP_SEND, CFG_SCH_PRIO_1);
}

/*
static void StopTaskBySensorID(uint8_t id)
{
	if (id > 3)
	{
		return;
	}
	if ( timersId[id] != 255 )
	{
		HW_TS_Stop(timersId[id]);
		HW_TS_Delete(timersId[id]);
		timersId[i] = 255;
	}
}
*/

static void StopTask()
{
	if (current_state != Off)
	{
		HW_TS_Stop(standbyTimerId);
		//HW_TS_Delete(standbyTimerId);
		//standbyTimerId = 0;
		current_state = Off;
		for (uint8_t i = 0; i < 4; i++)
		{

			if ( connected_sensors[i] == SPS30 )
			{
				if (sps_should_stop)
				{
					deinit_sensor_on_bus_by_type(connected_sensors[i], i);
					sps_running = 0;
				}
			}
			else
			{
				deinit_sensor_on_bus_by_type(connected_sensors[i], i);
			}
		}
	}


	//stop all running timers
	/*
	for (int i = 0; i < 4; i++)
	{
		StopTaskBySensorID(i);
	}
	*/
}

static void RestartTask()
{
	StopTask();
	StartTask();
}

/* USER CODE END FD_LOCAL_FUNCTIONS */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void APP_THREAD_RegisterCmdBuffer(TL_CmdPacket_t* p_buffer)
{
  p_thread_otcmdbuffer = p_buffer;
}

Thread_OT_Cmd_Request_t* THREAD_Get_OTCmdPayloadBuffer(void)
{
  return (Thread_OT_Cmd_Request_t*)p_thread_otcmdbuffer->cmdserial.cmd.payload;
}

Thread_OT_Cmd_Request_t* THREAD_Get_OTCmdRspPayloadBuffer(void)
{
  return (Thread_OT_Cmd_Request_t*)((TL_EvtPacket_t *)p_thread_otcmdbuffer)->evtserial.evt.payload;
}

Thread_OT_Cmd_Request_t* THREAD_Get_NotificationPayloadBuffer(void)
{
  return (Thread_OT_Cmd_Request_t*)(p_thread_notif_M0_to_M4)->evtserial.evt.payload;
}

static void Ot_Cmd_Transfer_Common(void)
{

  /* OpenThread OT command cmdcode range 0x280 .. 0x3DF = 352 */
  p_thread_otcmdbuffer->cmdserial.cmd.cmdcode = 0x280U;
  /* Size = otCmdBuffer->Size (Number of OT cmd arguments : 1 arg = 32bits so multiply by 4 to get size in bytes)
   * + ID (4 bytes) + Size (4 bytes) */
  uint32_t l_size = ((Thread_OT_Cmd_Request_t*)(p_thread_otcmdbuffer->cmdserial.cmd.payload))->Size * 4U + 8U;
  p_thread_otcmdbuffer->cmdserial.cmd.plen = l_size;

  TL_OT_SendCmd();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_M0();

}

/**
 * @brief  This function is used to transfer the Ot commands from the
 *         M4 to the M0.
 *
 * @param   None
 * @return  None
 */
void Ot_Cmd_Transfer(void)
{
  Ot_Cmd_Transfer_Common();
}

/**
 * @brief  This function is used to transfer the Ot commands from the
 *         M4 to the M0 with Notification M0 to M4 allowed.
 *
 * @param   None
 * @return  None
 */
void Ot_Cmd_TransferWithNotif(void)
{
  /* Flag to specify to UTIL_SEQ_EvtIdle that M0 to M4 notifications are allowed */
  g_ot_notification_allowed = 1U;

  Ot_Cmd_Transfer_Common();
}

/**
 * @brief  This function is called when acknowledge from OT command is received from the M0+.
 *
 * @param   Otbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_OT_CmdEvtReceived( TL_EvtPacket_t * Otbuffer )
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Otbuffer);

  Receive_Ack_From_M0();

  /* Does not allow OpenThread M0 to M4 notification */
  g_ot_notification_allowed = 0U;
}

/**
 * @brief  This function is called when notification from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_THREAD_NotReceived( TL_EvtPacket_t * Notbuffer )
{
  p_thread_notif_M0_to_M4 = Notbuffer;

  Receive_Notification_From_M0();
}

/**
  * @brief  This function is called before sending any ot command to the M0
  *         core. The purpose of this function is to be able to check if
  *         there are no notifications coming from the M0 core which are
  *         pending before sending a new ot command.
  * @param  None
  * @retval None
  */
void Pre_OtCmdProcessing(void)
{
  UTIL_SEQ_WaitEvt(EVENT_SYNCHRO_BYPASS_IDLE);
}

/**
  * @brief  This function waits for getting an acknowledgment from the M0.
  *
  * @param  None
  * @retval None
  */
static void Wait_Getting_Ack_From_M0(void)
{
  UTIL_SEQ_WaitEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
  * @brief  Receive an acknowledgment from the M0+ core.
  *         Each command send by the M4 to the M0 are acknowledged.
  *         This function is called under interrupt.
  * @param  None
  * @retval None
  */
static void Receive_Ack_From_M0(void)
{
  UTIL_SEQ_SetEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
  * @brief  Receive a notification from the M0+ through the IPCC.
  *         This function is called under interrupt.
  * @param  None
  * @retval None
  */
static void Receive_Notification_From_M0(void)
{
  CptReceiveMsgFromM0++;
  UTIL_SEQ_SetTask(TASK_MSG_FROM_M0_TO_M4,CFG_SCH_PRIO_0);
}

#if (CFG_USB_INTERFACE_ENABLE != 0)
#else
#if (CFG_FULL_LOW_POWER == 0)
static void RxCpltCallback(void)
{
  /* Filling buffer and wait for '\r' char */
  if (indexReceiveChar < C_SIZE_CMD_STRING)
  {
    CommandString[indexReceiveChar++] = aRxBuffer[0];
    if (aRxBuffer[0] == '\r')
    {
      CptReceiveCmdFromUser = 1U;

      /* UART task scheduling*/
      UTIL_SEQ_SetTask(1U << CFG_TASK_SEND_CLI_TO_M0, CFG_SCH_PRIO_0);
    }
  }

  /* Once a character has been sent, put back the device in reception mode */
  HW_UART_Receive_IT(CFG_CLI_UART, aRxBuffer, 1U, RxCpltCallback);
}
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

#if (CFG_USB_INTERFACE_ENABLE != 0)
/**
 * @brief Process the command strings.
 *        As soon as a complete command string has been received, the task
 *        in charge of sending the command to the M0 is scheduled
 * @param  None
 * @retval None
 */
static uint32_t  ProcessCmdString( uint8_t* buf , uint32_t len )
{
  uint32_t i,j,tmp_start;
  tmp_start = 0;
  uint32_t res = 0;

  i= 0;
  while ((buf[i] != '\r') && (i < len))
  {
    i++;
  }

  if (i != len)
  {
    memcpy(CommandString, buf,(i+1));
    indexReceiveChar = i + 1U; /* Length of the buffer containing the command string */
    UTIL_SEQ_SetTask(1U << CFG_TASK_SEND_CLI_TO_M0, CFG_SCH_PRIO_0);
    tmp_start = i;
    for (j = 0; j < (len - tmp_start - 1U) ; j++)
    {
      buf[j] = buf[tmp_start + j + 1U];
    }
    res = len - tmp_start - 1U;
  }
  else
  {
    res = len;
  }
  return res; /* Remaining characters in the temporary buffer */
}
#endif/* (CFG_USB_INTERFACE_ENABLE != 0) */

#if (CFG_FULL_LOW_POWER == 0)
/**
 * @brief Process sends receive CLI command to M0.
 * @param  None
 * @retval None
 */
static void Send_CLI_To_M0(void)
{
  memset(ThreadCliCmdBuffer.cmdserial.cmd.payload, 0x0U, 255U);
  memcpy(ThreadCliCmdBuffer.cmdserial.cmd.payload, CommandString, indexReceiveChar);
  ThreadCliCmdBuffer.cmdserial.cmd.plen = indexReceiveChar;
  ThreadCliCmdBuffer.cmdserial.cmd.cmdcode = 0x0;

  /* Clear receive buffer, character counter and command complete */
  CptReceiveCmdFromUser = 0;
  indexReceiveChar = 0;
  memset(CommandString, 0, C_SIZE_CMD_STRING);

  APP_DBG("[Send_CLI_To_M0] payload : %s", ThreadCliCmdBuffer.cmdserial.cmd.payload);

  TL_CLI_SendCmd();
}
#endif /* (CFG_FULL_LOW_POWER == 0) */

/**
 * @brief Send notification for CLI TL Channel.
 * @param  None
 * @retval None
 */
static void Send_CLI_Ack_For_OT(void)
{

  /* Notify M0 that characters have been sent to UART */
  TL_THREAD_CliSendAck();
}

/**
 * @brief Perform initialization of CLI UART interface.
 * @param  None
 * @retval None
 */
void APP_THREAD_Init_UART_CLI(void)
{
#if (CFG_FULL_LOW_POWER == 0)
  UTIL_SEQ_RegTask( 1<<CFG_TASK_SEND_CLI_TO_M0, UTIL_SEQ_RFU,Send_CLI_To_M0);
#endif /* (CFG_FULL_LOW_POWER == 0) */

#if (CFG_USB_INTERFACE_ENABLE != 0)
#else
#if (CFG_FULL_LOW_POWER == 0)
  MX_USART1_UART_Init();
  HW_UART_Receive_IT(CFG_CLI_UART, aRxBuffer, 1, RxCpltCallback);
#endif /* (CFG_FULL_LOW_POWER == 0) */
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */
}

/**
 * @brief Perform initialization of TL for THREAD.
 * @param  None
 * @retval None
 */
void APP_THREAD_TL_THREAD_INIT(void)
{
  ThreadConfigBuffer.p_ThreadOtCmdRspBuffer = (uint8_t*)&ThreadOtCmdBuffer;
  ThreadConfigBuffer.p_ThreadNotAckBuffer = (uint8_t*)ThreadNotifRspEvtBuffer;
  ThreadConfigBuffer.p_ThreadCliRspBuffer = (uint8_t*)&ThreadCliCmdBuffer;
  ThreadConfigBuffer.p_ThreadCliNotBuffer = (uint8_t*)&ThreadCliNotBuffer;

  TL_THREAD_Init( &ThreadConfigBuffer );
}

/**
 * @brief  This function is called when notification on CLI TL Channel from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_THREAD_CliNotReceived( TL_EvtPacket_t * Notbuffer )
{
  TL_CmdPacket_t* l_CliBuffer = (TL_CmdPacket_t*)Notbuffer;
  uint8_t l_size = l_CliBuffer->cmdserial.cmd.plen;

  /* WORKAROUND: if string to output is "> " then respond directly to M0 and do not output it */
  if (strcmp((const char *)l_CliBuffer->cmdserial.cmd.payload, "> ") != 0)
  {
    /* Write to CLI UART */
#if (CFG_USB_INTERFACE_ENABLE != 0)
    VCP_SendData( l_CliBuffer->cmdserial.cmd.payload, l_size, HostTxCb);
#else
    HW_UART_Transmit_IT(CFG_CLI_UART, l_CliBuffer->cmdserial.cmd.payload, l_size, HostTxCb);
#endif /*USAGE_OF_VCP */
  }
  else
  {
    Send_CLI_Ack_For_OT();
  }
}

/**
 * @brief  End of transfer callback for CLI UART sending.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void HostTxCb(void)
{
  Send_CLI_Ack_For_OT();
}

/**
 * @brief Process the messages coming from the M0.
 * @param  None
 * @retval None
 */
void APP_THREAD_ProcessMsgM0ToM4(void)
{
  if (CptReceiveMsgFromM0 != 0)
  {
    /* If CptReceiveMsgFromM0 is > 1. it means that we did not serve all the events from the radio */
    if (CptReceiveMsgFromM0 > 1U)
    {
      APP_THREAD_Error(ERR_REC_MULTI_MSG_FROM_M0, 0);
    }
    else
    {
      OpenThread_CallBack_Processing();
    }
    /* Reset counter */
    CptReceiveMsgFromM0 = 0;
  }
}

#if (CFG_USB_INTERFACE_ENABLE != 0)
/**
 * @brief  This function is called when thereare some data coming
 *         from the Hyperterminal via the USB port
 *         Data received over USB OUT endpoint are sent over CDC interface
 *         through this function.
 * @param  Buf: Buffer of data received
 * @param  Len: Number of data received (in bytes)
 * @retval Number of characters remaining in the buffer and not yet processed
 */
void VCP_DataReceived(uint8_t* Buf , uint32_t *Len)
{
  uint32_t i,flag_continue_checking = TRUE;
  uint32_t char_remaining = 0;
  static uint32_t len_total = 0;

  /* Copy the characters in the temporary buffer */
  for (i = 0; i < *Len; i++)
  {
    TmpString[len_total++] = Buf[i];
  }

  /* Process the buffer commands one by one     */
  /* A command is limited by a \r caracaters    */
  while (flag_continue_checking == TRUE)
  {
    char_remaining = ProcessCmdString(TmpString,len_total);
    /* If char_remaining is equal to len_total, it means that the command string is not yet
     * completed.
     * If char_remaining is equal to 0, it means that the command string has
     * been entirely processed.
     */
    if ((char_remaining == 0) || (char_remaining == len_total))
    {
      flag_continue_checking = FALSE;
    }
    len_total = char_remaining;
  }
}
#endif /* (CFG_USB_INTERFACE_ENABLE != 0) */

/* USER CODE BEGIN FD_WRAP_FUNCTIONS */

/* USER CODE END FD_WRAP_FUNCTIONS */
