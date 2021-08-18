/**
  ******************************************************************************
  * @file    sensor_service.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   Add a sample service using a vendor specific profile.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include <BLEProcessCommand.h>
#include <EmbeddedProto_service.h>
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"

//Added User includes

/** @addtogroup Applications
 *  @{
 */

/** @addtogroup SensorDemo
 *  @{
 */
 
/** @defgroup SENSOR_SERVICE
 * @{
 */

/** @defgroup SENSOR_SERVICE_Private_Variables
 * @{
 */
/* Private variables ---------------------------------------------------------*/
__IO uint32_t connected = FALSE;
__IO uint8_t set_connectable = 1;
__IO uint16_t connection_handle = 0;
__IO uint8_t notification_enabled = FALSE;
uint16_t EmbeddedProtoServHandle, CommandCharHandle, SensorCharHandle;
/**
 * @}
 */
extern uint8_t bnrg_expansion_board;
/** @defgroup SENSOR_SERVICE_Private_Macros
 * @{
 */
/* Private macros ------------------------------------------------------------*/
#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

#define COPY_EMBEDDEDPROTO_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_COMMAND_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_SENSOR_UUID(uuid_struct)          COPY_UUID_128(uuid_struct,0x34,0x0a,0x1b,0x80, 0xcf,0x4b, 0x11,0xe1, 0xac,0x36, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )
/**
 * @}
 */

/** @defgroup SENSOR_SERVICE_Exported_Functions 
 * @{
 */ 
/**
 * @brief  Add EmbeddedProto service using a vendor specific profile.
 *
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_EmbeddedProto_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];
  
  //Add EmbeddedProto Service
  COPY_EMBEDDEDPROTO_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 7,
                          &EmbeddedProtoServHandle);
  if(ret == BLE_STATUS_SUCCESS) {
    //Add Command Characteristic
    COPY_COMMAND_UUID(uuid);
    ret =  aci_gatt_add_char(EmbeddedProtoServHandle, UUID_TYPE_128, uuid, 20,
                         CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE,
                 GATT_NOTIFY_ATTRIBUTE_WRITE ,
                             16, 0, &CommandCharHandle);
  }

  if(ret == BLE_STATUS_SUCCESS) {
    //Add Sensor Characteristic
    COPY_SENSOR_UUID(uuid);
    ret =  aci_gatt_add_char(EmbeddedProtoServHandle, UUID_TYPE_128, uuid, 20,
                             CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                             ATTR_PERMISSION_NONE,
                             GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                             16, 0, &SensorCharHandle);
  }
  
  if(ret == BLE_STATUS_SUCCESS) {
    PRINTF("Service EmbeddedProto added. Handle 0x%04X, Command Charac handle: 0x%04X, "
            "Sensor Charac handle: 0x%04X\n",
            EmbeddedProtoServHandle, CommandCharHandle, SensorCharHandle);
  }
  else {
    PRINTF("Error while adding EmbeddedProto service.\n");
  }

  return (ret == BLE_STATUS_SUCCESS) ? BLE_STATUS_SUCCESS : BLE_STATUS_ERROR;
}

/**
 * @brief  Update Sensor characteristic.
 *
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Sensor_Update(uint8_t *data, uint8_t len)
{  
  tBleStatus ret;
	

  ret = aci_gatt_update_char_value(EmbeddedProtoServHandle, SensorCharHandle, 0, len,
                                   data);
	
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating sensor characteristic.\n") ;
    return BLE_STATUS_ERROR ;
  }
  return BLE_STATUS_SUCCESS;
}


/**
 * @brief  Puts the device in connectable mode.
 *         If you want to specify a UUID list in the advertising data, those data can
 *         be specified as a parameter in aci_gap_set_discoverable().
 *         For manufacture data, aci_gap_update_adv_data must be called.
 * @param  None 
 * @retval None
 */
/* Ex.:
 *
 *  tBleStatus ret;    
 *  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G'};    
 *  const uint8_t serviceUUIDList[] = {AD_TYPE_16_BIT_SERV_UUID,0x34,0x12};    
 *  const uint8_t manuf_data[] = {4, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x05, 0x02, 0x01};
 *  
 *  ret = aci_gap_set_discoverable(ADV_DATA_TYPE, ADV_INTERV_MIN, ADV_INTERV_MAX, PUBLIC_ADDR, 
 *                                 NO_WHITE_LIST_USE, 8, sizeof(local_name), 3, serviceUUIDList, 0, 0);    
 *  ret = aci_gap_update_adv_data(5, manuf_data);
 *
 */
void setConnectable(void)
{  
  tBleStatus ret;
  
  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'E','m','b','e','d','d','e', 'd', 'P', 'r', 'o', 't', 'o'};
  
  /* disable scan response */
  hci_le_set_scan_resp_data(0,NULL);
  PRINTF("General Discoverable Mode.\n");
  
  ret = aci_gap_set_discoverable(ADV_DATA_TYPE, ADV_INTERV_MIN, ADV_INTERV_MAX, PUBLIC_ADDR, 
                                 NO_WHITE_LIST_USE, sizeof(local_name), local_name, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while setting discoverable mode (%d)\n", ret);    
  }  
}


/**
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  if(handle == CommandCharHandle+1){
      //receiveData(att_data, data_length);
	  PRINTF("Received data\n");
      ble_receive_command(att_data, data_length);
  }
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{  
  connected = TRUE;
  connection_handle = handle;
  
  PRINTF("Connected to device:");
  for(uint32_t i = 5; i > 0; i--){
    PRINTF("%02X-", addr[i]);
  }
  PRINTF("%02X\n", addr[0]);
}

/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None 
 * @retval None
 */
void GAP_DisconnectionComplete_CB(void)
{
  connected = FALSE;
  PRINTF("Disconnected\n");
  /* Make the device connectable again. */
  set_connectable = TRUE;
  notification_enabled = FALSE;
}

/**
 * @brief  Read request callback.
 * @param  uint16_t Handle of the attribute
 * @retval None
 */
void Read_Request_CB(uint16_t handle)
{  
  if(handle == SensorCharHandle + 1){
	ble_send_sensor();
  }  

  if(connection_handle != 0)
    aci_gatt_allow_read(connection_handle);
}

/**
 * @brief  This function is used to receive data related to the sample service
 *         (received over the air from the remote board).
 * @param  data_buffer : pointer to store in received data
 * @param  Nb_bytes : number of bytes to be received
 * @retval None
 */
void receiveData(uint8_t* data_buffer, uint8_t Nb_bytes)
{
  PRINTF("Received data\n");

  for(int i = 0; i < Nb_bytes; i++) {
    printf("%c", data_buffer[i]);
  }
  fflush(stdout);
}

/**
 * @brief  This function is called when there is a notification from the sever.
 * @param  attr_handle Handle of the attribute
 * @param  attr_len    Length of attribute value in the notification
 * @param  attr_value  Attribute value in the notification
 * @retval None
 */
void GATT_Notification_CB(uint16_t attr_handle, uint8_t attr_len, uint8_t *attr_value)
{
  if (attr_handle == CommandCharHandle+1) {
    receiveData(attr_value, attr_len);
  }
}

/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void user_notify(void * pData)
{
  hci_uart_pckt *hci_pckt = pData;
  /* obtain event packet */
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  
  if(hci_pckt->type != HCI_EVENT_PKT)
    return;
  
  switch(event_pckt->evt){
    
  case EVT_DISCONN_COMPLETE:
    {
      GAP_DisconnectionComplete_CB();
    }
    break;
    
  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      
      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
        }
        break;
      }
    }
    break;
    
  case EVT_VENDOR:
  {
	  evt_blue_aci *blue_evt = (void*)event_pckt->data;
	  switch(blue_evt->ecode){

	  case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
	  {
		  if (bnrg_expansion_board == IDB05A1) {
			  evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
			  Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
		  }
		  else {
			  evt_gatt_attr_modified_IDB04A1 *evt = (evt_gatt_attr_modified_IDB04A1*)blue_evt->data;
			  Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
		  }
	  }
	  break;
	  case EVT_BLUE_GATT_READ_PERMIT_REQ:
	  {
		  evt_gatt_read_permit_req *pr = (void*)blue_evt->data;
		  Read_Request_CB(pr->attr_handle);
	  }
	  break;

	  case EVT_BLUE_GATT_NOTIFICATION:
	  {
		  evt_gatt_attr_notification *evt = (evt_gatt_attr_notification*)blue_evt->data;
		  GATT_Notification_CB(evt->attr_handle, evt->event_data_length - 2, evt->attr_value);
	  }
	  break;
	  }
  }
  break;
  }    
}

/**
 * @}
 */
 
/**
 * @}
 */

/**
 * @}
 */

 /**
 * @}
 */
 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
