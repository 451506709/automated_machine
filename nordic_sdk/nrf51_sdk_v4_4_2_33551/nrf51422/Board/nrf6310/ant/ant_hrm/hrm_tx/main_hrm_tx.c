/*
This software is subject to the license described in the license.txt file included with this software distribution. 
You may not use this file except in compliance with this license. 
Copyright � Dynastream Innovations Inc. 2012
All rights reserved.
*/

#include "main_hrm_tx.h"
#include <stdint.h>
#include <stdio.h>
#include "hrm_tx.h"
#include "nrf_soc.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"

#define ANT_EVENT_MSG_BUFFER_MIN_SIZE 32u                 /**< Minimum size of ANT event message buffer. */

#define HRMTX_DEVICE_TYPE             0x78u               /**< Channel ID. */
#define HRMTX_MSG_PERIOD              0x1F86u             /**< Decimal 8070 (4.06Hz). */
#define HRMTX_NETWORK_KEY             {0,0,0,0,0,0,0,0}   /**< The default network key used. */

static const uint8_t m_network_key[] = HRMTX_NETWORK_KEY; /**< ANT PLUS network key. */

 
/**@brief Function for setting up the ANT module for TX broadcast.
 */
static __INLINE void ant_channel_tx_broadcast_setup(void)
{
    printf("+ant_channel_tx_broadcast_setup\n");   

    uint32_t err_code = sd_ant_network_address_set(ANTPLUS_NETWORK_NUMBER, (uint8_t*)m_network_key);
    APP_ERROR_CHECK(err_code);    

    err_code = hrm_tx_open();
    APP_ERROR_CHECK(err_code);        

    err_code = sd_ant_channel_id_set(HRMTX_ANT_CHANNEL, 
                                     HRM_DEVICE_NUMBER, 
                                     HRMTX_DEVICE_TYPE, 
                                     HRM_TRANSMISSION_TYPE);
    APP_ERROR_CHECK(err_code);    
  
    err_code = sd_ant_channel_radio_freq_set(HRMTX_ANT_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);    
  
    err_code = sd_ant_channel_period_set(HRMTX_ANT_CHANNEL, HRMTX_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);    
  
    err_code = sd_ant_channel_open(HRMTX_ANT_CHANNEL);
    APP_ERROR_CHECK(err_code);    

    printf("-ant_channel_tx_broadcast_setup\n");   
}


/**@brief Function for processing application specific events.
 */
static __INLINE void application_event_process(void)
{
    uint8_t  event;
    uint8_t  ant_channel;  
    uint8_t  event_message_buffer[ANT_EVENT_MSG_BUFFER_MIN_SIZE];     
    uint32_t err_code;

    // Extract and process all pending ANT events.
    do
    {
        err_code = sd_ant_event_get(&ant_channel, &event, event_message_buffer);        
        if (err_code == NRF_SUCCESS)
        {
            err_code = hrm_tx_channel_event_handle(event);         
            APP_ERROR_CHECK(err_code);
        }
    } 
    while (err_code == NRF_SUCCESS);
}


void main_hrm_tx_run(void)
{
    printf("+main_hrmtx\n");
  
    ant_channel_tx_broadcast_setup();   
  
    // Main event processing loop. Sleep until interrupt is received. Upon wakeup process all 
    // application events that require processing and after that return to the sleep mode. 
    uint32_t err_code;   
    for (;;)
    {
        err_code = sd_app_event_wait();  
        APP_ERROR_CHECK(err_code);
    
        application_event_process();          
    }
} 


/**@brief Function for handling protocol stack IRQ.
 *
 * Interrupt is generated by the ANT stack upon sending event to the application. 
 */
void PROTOCOL_EVENT_IRQHandler(void)
{

}
