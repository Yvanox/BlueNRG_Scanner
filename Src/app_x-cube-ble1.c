/* Includes ------------------------------------------------------------------*/
#include "app_x-cube-ble1.h"
#include "bluenrg_gap.h"
#include "hci_const.h"
#include "bluenrg_hal_aci.h"
#include "hci.h"
#include "hci_le.h"
#include "hci_const.h"
#include "bluenrg_l2cap_aci.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_utils.h"
#include "bluenrg_aci_const.h"
#include <stdint.h>

#define BDADDR_SIZE 6

void receiveData(uint8_t * data_buffer, uint8_t * rssi_data, uint8_t no_bytes);
void user_notify(void *pData);


uint8_t rcv_data[30];
uint8_t RSSI[30];

void MX_BlueNRG_MS_Init(void)
{
  int ret;
  uint8_t CLIENT_BDADDR[] = {0X01,0X02,0X03,0X04,0X05,0X06};
  uint8_t bdaddr[BDADDR_SIZE];

  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

  hci_init(user_notify,NULL);
  hci_reset();
  HAL_Delay(100);
  
  //BLUENRG_memcpy(bdaddr,CLIENT_BDADDR,sizeof(CLIENT_BDADDR));
  
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,

                                 bdaddr);
  
  //IDB05A1
  ret = aci_gatt_init();
  ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1|GAP_CENTRAL_ROLE_IDB05A1|GAP_OBSERVER_ROLE_IDB05A1,0,0x07,&service_handle,&dev_name_char_handle,&appearance_char_handle);
  ret = aci_gap_set_auth_requirement(  0x00,
                                       0x00,
                                       NULL,
                                       7,
                                       16,
                                       0x00,
                                       123456,
                                       0x00);
}

void MX_BlueNRG_MS_Process(void)
{

  aci_gap_start_general_discovery_proc(0x4000, 0x4000,0x00,0x00);
  HAL_Delay(95);
  hci_user_evt_proc();

}


void receiveData(uint8_t * data_buffer, uint8_t * rssi_data, uint8_t no_bytes)
{

	int rssi;
	printf("DEVICE:");

        for(int i =0;i<no_bytes;i++)
        {
          rcv_data[i] = data_buffer[i];
          RSSI[30] = rssi_data[30];
          rssi = (0xff - RSSI[30])+1;
          printf("%p", rcv_data[i]);
        }

    printf("   ");
    printf("RSSI:");
    printf("%d",rssi);
    printf("   ");
    printf("\n\r");
    printf("\n\r");
}

void user_notify(void *pData)
{
                hci_uart_pckt *hci_pckt = pData;

                hci_event_pckt *event_pckt = (hci_event_pckt *)hci_pckt->data;

               if(hci_pckt->type != HCI_EVENT_PKT)
                             return;

               switch(event_pckt->evt)
              {
                             case EVT_LE_META_EVENT:
                             {
                                           evt_le_meta_event *evt = (void *)event_pckt->data;
                                           switch(evt->subevent)
                                           {
                                 case EVT_LE_ADVERTISING_REPORT:
                                 {

                                        le_advertising_info *pr = (le_advertising_info *)(((uint8_t*)evt->data)+1); /* evt->data[0] is number of reports (On BlueNRG-MS is always 1) */
                                        receiveData(pr->bdaddr,pr->data_RSSI, pr->data_length);
                                 }
                                 break;
                                           }
                                           }
                             break;

              }
}
