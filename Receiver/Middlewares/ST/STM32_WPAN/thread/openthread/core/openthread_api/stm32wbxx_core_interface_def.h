/**
  ******************************************************************************
  * @file    stm32wbxx_core_interface_def.h
  * @author  MCD Application Team
  * @brief   This file contains all the defines and structures used for the
  *          communication between the two core M0 and M4.
  *          This file is shared between the code running on M4 and the code
  *          running on M0.
  *
  ******************************************************************************
  * @attention
 *
 * Copyright (c) 2018-2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WBxx_CORE_INTERFACE_DEF_H
#define STM32WBxx_CORE_INTERFACE_DEF_H

#include "stm32_wpan_common.h"
#include "coap.h"
#include "coap_secure.h"
#include "udp.h"

#ifdef __cplusplus
extern "C" {
#endif
 
typedef struct  {
  void * mContext;
  otCoapRequestHandler mHandler;
} STCoapRequestContextType;

typedef struct  {
  void * mContext;
  otCoapResponseHandler mHandler;
} STCoapResponseContextType;

typedef struct  {
  otCoapResource *          mResource;
  STCoapRequestContextType  mSpecificContext; /* Contains context and handler */
} STCoapResourceType;

typedef struct  {
  void * mContext;
  otHandleCoapSecureClientConnect mHandler;
} STCoapSecureSpecificContextType;

typedef struct  {
  const otSockAddr *               mSocketAddr;         /* The Socket Addr pointer */
  STCoapSecureSpecificContextType  mSpecificContext;    /* Contains context and handler */
} STCoapSecureConnectType;

typedef struct  {
  void * mContext;
  otUdpHandler mHandler;
} STUdpHandlerContextType;

typedef struct  {
  otUdpReceiver *           mReceiver;
  STUdpHandlerContextType   mSpecificContext; /* Contains context and handler */
} STUdpReceiverType;

/* Structure of the messages exchanged between M0 and M4 */
#define OT_CMD_BUFFER_SIZE 20U
typedef PACKED_STRUCT
{
  uint32_t  ID;
  uint32_t  Size;
  uint32_t  Data[OT_CMD_BUFFER_SIZE];
}Thread_OT_Cmd_Request_t;

/* Structure of the messages exchanged between M0 and M4 in RCP case */
#ifdef OPENTHREAD_RCP
#define RCP_PACKET_BUFFER_SIZE 256
typedef PACKED_STRUCT
{
  uint32_t  RCP_packetCnt;
  uint32_t  RCP_packetLength;
  uint8_t  RCP_packetPayload[RCP_PACKET_BUFFER_SIZE];
} RCP_Packet_t;
#endif

/* List of messages sent by the M4 to the M0 */
typedef enum
{
  /* LINK */
  MSG_M4TOM0_OT_LINK_ACTIVE_SCAN,
  MSG_M4TOM0_OT_LINK_IS_ACTIVE_SCAN_IN_PROGRESS,
  MSG_M4TOM0_OT_LINK_ENERGY_SCAN,
  MSG_M4TOM0_OT_LINK_IS_ENERGY_SCAN_IN_PROGRESS,
  MSG_M4TOM0_OT_LINK_SEND_DATA_REQUEST,
  MSG_M4TOM0_OT_LINK_IS_IN_TRANSMIT_STATE,
  MSG_M4TOM0_OT_LINK_GET_CHANNEL,
  MSG_M4TOM0_OT_LINK_SET_CHANNEL,
  MSG_M4TOM0_OT_LINK_GET_SUPPORTED_CHANNEL_MASK,
  MSG_M4TOM0_OT_LINK_SET_SUPPORTED_CHANNEL_MASK,
  MSG_M4TOM0_OT_LINK_GET_EXTENDED_ADDRESS,
  MSG_M4TOM0_OT_LINK_SET_EXTENDED_ADDRESS,
  MSG_M4TOM0_OT_LINK_GET_FACTORY_ASSIGNED_EUI64,
  MSG_M4TOM0_OT_LINK_GET_PANID,
  MSG_M4TOM0_OT_LINK_SET_PANID,
  MSG_M4TOM0_OT_LINK_GET_POLL_PERIOD,
  MSG_M4TOM0_OT_LINK_SET_POLL_PERIOD,
  MSG_M4TOM0_OT_LINK_GET_MAX_FRAME_RETRIES_DIRECT,
  MSG_M4TOM0_OT_LINK_SET_MAX_FRAME_RETRIES_DIRECT,
  MSG_M4TOM0_OT_LINK_GET_MAX_FRAME_RETRIES_INDIRECT,
  MSG_M4TOM0_OT_LINK_SET_MAX_FRAME_RETRIES_INDIRECT,
  MSG_M4TOM0_OT_LINK_GET_SHORT_ADDRESS,
  MSG_M4TOM0_OT_LINK_FILTER_GET_ADRESS_MODE,
  MSG_M4TOM0_OT_LINK_FILTER_SET_ADDRESS_MODE,
  MSG_M4TOM0_OT_LINK_FILTER_ADD_ADDRESS,
  MSG_M4TOM0_OT_LINK_FILTER_REMOVE_ADDRESS,
  MSG_M4TOM0_OT_LINK_FILTER_CLEAR_ADDRESSES,
  MSG_M4TOM0_OT_LINK_FILTER_GET_NEXT_ADDRESS,
  MSG_M4TOM0_OT_LINK_FILTER_ADD_RSS_IN,
  MSG_M4TOM0_OT_LINK_FILTER_REMOVE_RSS_IN,
  MSG_M4TOM0_OT_LINK_FILTER_SET_DEFAULT_RSS_IN,
  MSG_M4TOM0_OT_LINK_FILTER_CLEAR_DEFAULT_RSS_IN,
  MSG_M4TOM0_OT_LINK_FILTER_CLEAR_ALL_RSS_IN,
  MSG_M4TOM0_OT_LINK_GET_NEXT_RSS_IN,
  MSG_M4TOM0_OT_LINK_SET_RADIO_FILTER_ENABLED,
  MSG_M4TOM0_OT_LINK_IS_RADIO_FILTER_ENABLED,
  MSG_M4TOM0_OT_LINK_CONVERT_RSS_TO_LINK_QUALITY,
  MSG_M4TOM0_OT_LINK_CONVERT_LINK_QUALITY_TO_RSS,
  MSG_M4TOM0_OT_LINK_GET_TX_DIRECT_RETRY_SUCCESS_HISTOGRAM,
  MSG_M4TOM0_OT_LINK_GET_TX_INDIRECT_RETRY_SUCCESS_HISTOGRAM,
  MSG_M4TOM0_OT_LINK_RESET_TX_RETRY_SUCCESS_HISTOGRAM,
  MSG_M4TOM0_OT_LINK_GET_COUNTERS,
  MSG_M4TOM0_OT_LINK_RESET_COUNTERS,
  MSG_M4TOM0_OT_LINK_SET_PCAP_CALLBACK,
  MSG_M4TOM0_OT_LINK_IS_PROMISCUOUS,
  MSG_M4TOM0_OT_LINK_SET_PROMISCUOUS,
  MSG_M4TOM0_OT_LINK_GET_CCA_FAILURE_RATE,
  MSG_M4TOM0_OT_LINK_CSL_GET_CHANNEL,
  MSG_M4TOM0_OT_LINK_CSL_SET_CHANNEL,
  MSG_M4TOM0_OT_LINK_CSL_GET_PERIOD,
  MSG_M4TOM0_OT_LINK_CSL_SET_PERIOD,
  MSG_M4TOM0_OT_LINK_CSL_GET_TIMEOUT,
  MSG_M4TOM0_OT_LINK_CSL_SET_TIMEOUT,
  MSG_M4TOM0_OT_LINK_SEND_EMPTY_DATA,
  MSG_M4TOM0_OT_LINK_SET_ENABLED,
  MSG_M4TOM0_OT_LINK_IS_ENABLED,
  /* LINK METRICS */
  MSG_M4TOM0_OT_LINK_METRICS_QUERY,
  MSG_M4TOM0_OT_LINK_METRICS_CONFIG_FORWARD_TRACKING_SERIES,
  MSG_M4TOM0_OT_LINK_METRICS_CONFIG_ENH_ACK_PROBING,
  MSG_M4TOM0_OT_LINK_METRICS_SEND_LINK_PROBE,
  /* IP6 */
  MSG_M4TOM0_OT_IP6_SET_ENABLED,
  MSG_M4TOM0_OT_IP6_IS_ENABLED,
  MSG_M4TOM0_OT_IP6_ADD_UNICAST_ADDRESS,
  MSG_M4TOM0_OT_IP6_REMOVE_UNICAST_ADDRESS,
  MSG_M4TOM0_OT_IP6_GET_UNICAST_ADDRESSES,
  MSG_M4TOM0_OT_IP6_SUBSCRIBE_MULTICAST_ADDRESS,
  MSG_M4TOM0_OT_IP6_UNSUBSCRIBE_MULTICAST_ADDRESS,
  MSG_M4TOM0_OT_IP6_GET_MULTICAST_ADDRESSES,
  MSG_M4TOM0_OT_IP6_IS_MULTICAST_PROMISCUOUS_ENABLED,
  MSG_M4TOM0_OT_IP6_SET_MULTICAST_PROMISCUOUS_ENABLED,
  MSG_M4TOM0_OT_IP6_NEW_MESSAGE,
  MSG_M4TOM0_OT_IP6_NEW_MESSAGE_FROM_BUFFER,
  MSG_M4TOM0_OT_IP6_SET_RECEIVE_CALLBACK,
  MSG_M4TOM0_OT_IP6_SET_ADDRESS_CALLBACK,
  MSG_M4TOM0_OT_IP6_IS_RECEIVE_FILTER_ENABLED,
  MSG_M4TOM0_OT_IP6_SET_RECEIVE_FILTER_ENABLED,
  MSG_M4TOM0_OT_IP6_SEND,
  MSG_M4TOM0_OT_IP6_ADD_UNSECURE_PORT,
  MSG_M4TOM0_OT_IP6_REMOVE_UNSECURE_PORT,
  MSG_M4TOM0_OT_IP6_REMOVE_ALL_UNSECURE_PORTS,
  MSG_M4TOM0_OT_IP6_GET_UNSECURE_PORTS,
  MSG_M4TOM0_OT_IP6_IS_ADDRESS_EQUAL,
  MSG_M4TOM0_OT_IP6_ARE_PREFIXES_EQUAL,
  MSG_M4TOM0_OT_IP6_ADDRESS_FROM_STRING,
  MSG_M4TOM0_OT_IP6_ADDRESS_TO_STRING,
  MSG_M4TOM0_OT_IP6_SOCK_ADDR_TO_STRING,
  MSG_M4TOM0_OT_IP6_PREFIX_TO_STRING,
  MSG_M4TOM0_OT_IP6_PREFIX_MATCH,
  MSG_M4TOM0_OT_IP6_IS_ADDRESS_UNSPECIFIED,
  MSG_M4TOM0_OT_IP6_SELECT_SOURCE_ADDRESS,
  MSG_M4TOM0_OT_IP6_REGISTER_MULTICAST_LISTENERS,
  MSG_M4TOM0_OT_IP6_IS_SLAAC_ENABLED,
  MSG_M4TOM0_OT_IP6_SET_SLAAC_ENABLED,
  MSG_M4TOM0_OT_IP6_SET_SLAAC_PREFIX_FILTER,
  MSG_M4TOM0_OT_IP6_SET_MESH_LOCAL_IID,
  MSG_M4TOM0_OT_IP6_PROTO_TO_STRING,
  /* THREAD */
  MSG_M4TOM0_OT_THREAD_SET_ENABLED,
  MSG_M4TOM0_OT_THREAD_GET_VERSION,
  MSG_M4TOM0_OT_THREAD_IS_SINGLETON,
  MSG_M4TOM0_OT_THREAD_DISCOVER,
  MSG_M4TOM0_OT_THREAD_SET_JOINER_ADVERTISEMENT,
  MSG_M4TOM0_OT_THREAD_IS_DISCOVER_IN_PROGRESS,
  MSG_M4TOM0_OT_THREAD_GET_CHILD_TIMEOUT,
  MSG_M4TOM0_OT_THREAD_SET_CHILD_TIMEOUT,
  MSG_M4TOM0_OT_THREAD_GET_EXTPANID,
  MSG_M4TOM0_OT_THREAD_SET_EXTPANID,
  MSG_M4TOM0_OT_THREAD_GET_LEADER_RLOC,
  MSG_M4TOM0_OT_THREAD_GET_LINK_MODE,
  MSG_M4TOM0_OT_THREAD_SET_LINK_MODE,
  MSG_M4TOM0_OT_THREAD_GET_NETWORK_KEY,
  MSG_M4TOM0_OT_THREAD_GET_NETWORK_KEY_REF,
  MSG_M4TOM0_OT_THREAD_SET_NETWORK_KEY,
  MSG_M4TOM0_OT_THREAD_SET_NETWORK_KEY_REF,
  MSG_M4TOM0_OT_THREAD_GET_RLOC,
  MSG_M4TOM0_OT_THREAD_GET_MESH_LOCAL_EID,
  MSG_M4TOM0_OT_THREAD_GET_MESH_LOCAL_PREFIX,
  MSG_M4TOM0_OT_THREAD_SET_MESH_LOCAL_PREFIX,
  MSG_M4TOM0_OT_THREAD_GET_LINK_LOCAL_IP6_ADDRESS,
  MSG_M4TOM0_OT_THREAD_GET_LINK_LOCAL_ALL_THREAD_NODES_MULTICAST_ADDRESS,
  MSG_M4TOM0_OT_THREAD_GET_REALM_LOCAL_ALL_THREAD_NODES_MULTICAST_ADDRESS,
  MSG_M4TOM0_OT_THREAD_GET_SERVICE_ALOC,
  MSG_M4TOM0_OT_THREAD_GET_NETWORK_NAME,
  MSG_M4TOM0_OT_THREAD_SET_NETWORK_NAME,
  MSG_M4TOM0_OT_THREAD_GET_DOMAIN_NAME,
  MSG_M4TOM0_OT_THREAD_SET_DOMAIN_NAME,
  MSG_M4TOM0_OT_THREAD_SET_FIXED_DUA_INTERFACE_IDENTIFIER,
  MSG_M4TOM0_OT_THREAD_GET_FIXED_DUA_INTERFACE_IDENTIFIER,
  MSG_M4TOM0_OT_THREAD_GET_KEY_SEQUENCE_COUNTER,
  MSG_M4TOM0_OT_THREAD_SET_KEY_SEQUENCE_COUNTER,
  MSG_M4TOM0_OT_THREAD_GET_KEY_SWITCH_GUARD_TIME,
  MSG_M4TOM0_OT_THREAD_SET_KEY_SWITCH_GUARD_TIME,
  MSG_M4TOM0_OT_THREAD_BECOME_DETACHED,
  MSG_M4TOM0_OT_THREAD_BECOME_CHILD,
  MSG_M4TOM0_OT_THREAD_GET_NEXT_NEIGHBOR_INFO,
  MSG_M4TOM0_OT_THREAD_GET_DEVICE_ROLE,
  MSG_M4TOM0_OT_THREAD_GET_DEVICE_ROLE_TO_STRING,
  MSG_M4TOM0_OT_THREAD_GET_LEADER_DATA,
  MSG_M4TOM0_OT_THREAD_GET_LEADER_ROUTER_ID,
  MSG_M4TOM0_OT_THREAD_GET_LEADER_WEIGHT,
  MSG_M4TOM0_OT_THREAD_GET_PARTITION_ID,
  MSG_M4TOM0_OT_THREAD_GET_RLOC_16,
  MSG_M4TOM0_OT_THREAD_GET_PARENT_INFO,
  MSG_M4TOM0_OT_THREAD_GET_PARENT_AVERAGE_RSSI,
  MSG_M4TOM0_OT_THREAD_GET_PARENT_LAST_RSSI,
  MSG_M4TOM0_OT_THREAD_SET_RECEIVE_DIAGNOSTIC_GET_CALLBACK,
  MSG_M4TOM0_OT_THREAD_GET_IP6_COUNTERS,
  MSG_M4TOM0_OT_THREAD_RESET_IP6_COUNTERS,
  MSG_M4TOM0_OT_THREAD_GET_MLE_COUNTERS,
  MSG_M4TOM0_OT_THREAD_RESET_MLE_COUNTERS,
  MSG_M4TOM0_OT_THREAD_REGISTER_PARENT_RESPONSE_CALLBACK,
  MSG_M4TOM0_OT_THREAD_LOCATE_ANYCAST_DESTINATION,
  MSG_M4TOM0_OT_THREAD_IS_ANYCAST_LOCATE_IN_PROGRESS,
  /* THREAD FTD */
  MSG_M4TOM0_OT_THREAD_FTD_GET_MAX_ALLOWED_CHILDREN,
  MSG_M4TOM0_OT_THREAD_FTD_SET_MAX_ALLOWED_CHILDREN,
  MSG_M4TOM0_OT_THREAD_GET_MAX_CHILD_IP_ADDRESS,
  MSG_M4TOM0_OT_THREAD_SET_MAX_CHILD_IP_ADDRESS,
  MSG_M4TOM0_OT_THREAD_IS_ROUTER_ELIGIBLE,
  MSG_M4TOM0_OT_THREAD_SET_ROUTER_ELIGIBLE,
  MSG_M4TOM0_OT_THREAD_FTD_SET_PREFERRED_ROUTER_ID,
  MSG_M4TOM0_OT_THREAD_FTD_GET_LOCAL_LEADER_WEIGHT,
  MSG_M4TOM0_OT_THREAD_FTD_SET_LOCAL_LEADER_WEIGHT,
  MSG_M4TOM0_OT_THREAD_GET_PREFERRED_LEADER_PARTITION_ID,
  MSG_M4TOM0_OT_THREAD_SET_PREFERRED_LEADER_PARTITION_ID,
  MSG_M4TOM0_OT_THREAD_FTD_GET_JOINER_UDP_PORT,
  MSG_M4TOM0_OT_THREAD_FTD_SET_JOINER_UDP_PORT,
  MSG_M4TOM0_OT_THREAD_FTD_SET_STEERING_DATA,
  MSG_M4TOM0_OT_THREAD_FTD_GET_CONTEXT_ID_REUSE_DELAY,
  MSG_M4TOM0_OT_THREAD_FTD_SET_CONTEXT_ID_REUSE_DELAY,
  MSG_M4TOM0_OT_THREAD_FTD_GET_NETWORK_ID_TIMEOUT,
  MSG_M4TOM0_OT_THREAD_FTD_SET_NETWORK_ID_TIMEOUT,
  MSG_M4TOM0_OT_THREAD_FTD_GET_ROUTER_UPGRADE_THRESHOLD,
  MSG_M4TOM0_OT_THREAD_FTD_SET_ROUTER_UPGRADE_THRESHOLD,
  MSG_M4TOM0_OT_THREAD_FTD_RELEASE_ROUTER_ID,
  MSG_M4TOM0_OT_THREAD_FTD_BECOME_ROUTER,
  MSG_M4TOM0_OT_THREAD_FTD_BECOME_LEADER,
  MSG_M4TOM0_OT_THREAD_FTD_GET_ROUTER_DOWNGRADE_THRESHOLD,
  MSG_M4TOM0_OT_THREAD_FTD_SET_ROUTER_DOWNGRADE_THRESHOLD,
  MSG_M4TOM0_OT_THREAD_FTD_GET_ROUTER_SELECTION_JITTER,
  MSG_M4TOM0_OT_THREAD_FTD_SET_ROUTER_SELECTION_JITTER,
  MSG_M4TOM0_OT_THREAD_FTD_GET_CHILD_INFO_BY_ID,
  MSG_M4TOM0_OT_THREAD_FTD_GET_CHILD_INFO_BY_INDEX,
  MSG_M4TOM0_OT_THREAD_FTD_GET_CHILD_NEXT_IP6_ADDRESS,
  MSG_M4TOM0_OT_THREAD_FTD_GET_ROUTER_ID_SEQUENCE,
  MSG_M4TOM0_OT_THREAD_FTD_GET_MAX_ROUTER_ID,
  MSG_M4TOM0_OT_THREAD_FTD_GET_ROUTER_INFO,
  MSG_M4TOM0_OT_THREAD_FTD_GET_NEXT_CACHE_ENTRY,
  MSG_M4TOM0_OT_THREAD_FTD_GET_PSKC,
  MSG_M4TOM0_OT_THREAD_FTD_GET_PSKC_REF,
  MSG_M4TOM0_OT_THREAD_FTD_SET_PSKC,
  MSG_M4TOM0_OT_THREAD_FTD_SET_PSKC_REF,
  MSG_M4TOM0_OT_THREAD_FTD_GET_PARENT_PRIORITY,
  MSG_M4TOM0_OT_THREAD_FTD_SET_PARENT_PRIORITY,
  MSG_M4TOM0_OT_THREAD_REGISTER_NEIGHBOR_TABLE_CALLBACK,
  MSG_M4TOM0_OT_THREAD_SET_DISCOVERY_REQUEST_CALLBACK,
  MSG_M4TOM0_OT_THREAD_SEND_ADDRESS_NOTIFICATION,
  MSG_M4TOM0_OT_THREAD_SEND_PROACTIVE_BACKBONE_NOTIFICATION,
  MSG_M4TOM0_OT_THREAD_SET_CCM_ENABLED,
  MSG_M4TOM0_OT_THREAD_SET_THREAD_VERSION_CHECK_ENABLED,
  MSG_M4TOM0_OT_THREAD_GET_ROUTER_ID_RANGE,
  MSG_M4TOM0_OT_THREAD_SET_ROUTER_ID_RANGE,
  /* INSTANCE */
  MSG_M4TOM0_OT_INSTANCE_INIT,
  MSG_M4TOM0_OT_INSTANCE_INIT_SINGLE,
  MSG_M4TOM0_OT_INSTANCE_IS_INITIALIZED,
  MSG_M4TOM0_OT_INSTANCE_FINALIZE,
  MSG_M4TOM0_OT_SET_STATE_CHANGED_CALLBACK,
  MSG_M4TOM0_OT_REMOVE_STATE_CHANGED_CALLBACK,
  MSG_M4TOM0_OT_INSTANCE_RESET,
  MSG_M4TOM0_OT_INSTANCE_GET_UP_TIME,
  MSG_M4TOM0_OT_INSTANCE_GET_UP_TIME_AS_STRING,
  MSG_M4TOM0_OT_INSTANCE_FACTORY_RESET,
  MSG_M4TOM0_OT_INSTANCE_ERASE_PERSISTENT_INFO,
  MSG_M4TOM0_OT_INSTANCE_RESET_RADIO_STACK,
  MSG_M4TOM0_OT_OPENTHREAD_GET_VERSION_STRING,
  MSG_M4TOM0_OT_OPENTHREAD_GET_RADIO_VERSION_STRING,
  /* COAP */
  MSG_M4TOM0_OT_COAP_MESSAGE_INIT,
  MSG_M4TOM0_OT_COAP_MESSAGE_INIT_RESPONSE,
  MSG_M4TOM0_OT_COAP_MESSAGE_SET_TOKEN,
  MSG_M4TOM0_OT_COAP_MESSAGE_GENERATE_TOKEN,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_CONTENT_FORMAT_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_UINT_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_OBSERVE_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_URI_PATH_OPTIONS,
  MSG_M4TOM0_OT_COAP_BLOCK_SIZE_FROM_EXPONENT,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_BLOCK2_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_BLOCK1_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_PROXY_URI_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_MAX_AGE_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_APPEND_URI_QUERY_OPTION,
  MSG_M4TOM0_OT_COAP_MESSAGE_SET_PAYLOAD_MARKER,
  MSG_M4TOM0_OT_COAP_MESSAGE_GET_TYPE,
  MSG_M4TOM0_OT_COAP_MESSAGE_GET_CODE,
  MSG_M4TOM0_OT_COAP_MESSAGE_SET_CODE,
  MSG_M4TOM0_OT_COAP_MESSAGE_CODE_TO_STRING,
  MSG_M4TOM0_OT_COAP_MESSAGE_GET_MESSAGE_ID,
  MSG_M4TOM0_OT_COAP_MESSAGE_GET_TOKEN_LENGTH,
  MSG_M4TOM0_OT_COAP_MESSAGE_GET_TOKEN,
  MSG_M4TOM0_OT_COAP_OPTION_ITERATOR_INIT,
  MSG_M4TOM0_OT_COAP_OPTION_ITERATOR_GET_FIRST_OPTION_MATCHING,
  MSG_M4TOM0_OT_COAP_OPTION_ITERATOR_GET_FIRST_OPTION,
  MSG_M4TOM0_OT_COAP_OPTION_ITERATOR_GET_NEXT_OPTION_MATCHING,
  MSG_M4TOM0_OT_COAP_OPTION_ITERATOR_GET_NEXT_OPTION,
  MSG_M4TOM0_OT_COAP_OPTION_ITERATOR_GET_OPTION_UINT_VALUE,
  MSG_M4TOM0_OT_COAP_OPTION_ITERATOR_GET_OPTION_VALUE,
  MSG_M4TOM0_OT_COAP_SEND_REQUEST_BLOCK_WISE_WITH_PARAMETERS,
  MSG_M4TOM0_OT_COAP_SEND_REQUEST_WITH_PARAMETERS,
  MSG_M4TOM0_OT_COAP_NEW_MESSAGE,
  MSG_M4TOM0_OT_COAP_START,
  MSG_M4TOM0_OT_COAP_STOP,
  MSG_M4TOM0_OT_COAP_ADD_BLOCK_WISE_RESOURCE,
  MSG_M4TOM0_OT_COAP_REMOVE_BLOCK_WISE_RESOURCE,
  MSG_M4TOM0_OT_COAP_ADD_RESOURCE,
  MSG_M4TOM0_OT_COAP_REMOVE_RESOURCE,
  MSG_M4TOM0_OT_COAP_SET_DEFAULT_HANDLER,
  MSG_M4TOM0_OT_COAP_SEND_RESPONSE_BLOCK_WISE_WITH_PARAMETERS,
  MSG_M4TOM0_OT_COAP_SEND_RESPONSE_WITH_PARAMETERS,
  /* COAP SECURE */
  MSG_M4TOM0_OT_COAP_SECURE_START,
  MSG_M4TOM0_OT_COAP_SECURE_STOP,
  MSG_M4TOM0_OT_COAP_SECURE_SET_PSK,
  MSG_M4TOM0_OT_COAP_SECURE_GET_PEER_CERTIFICATE_BASE_64,
  MSG_M4TOM0_OT_COAP_SECURE_SET_SSL_AUTH_MODE,
  MSG_M4TOM0_OT_COAP_SECURE_SET_CERTIFICATE,
  MSG_M4TOM0_OT_COAP_SECURE_SET_CA_CERTIFICATE_CHAIN,
  MSG_M4TOM0_OT_COAP_SECURE_CONNECT,
  MSG_M4TOM0_OT_COAP_SECURE_DISCONNECT,
  MSG_M4TOM0_OT_COAP_SECURE_IS_CONNECTED,
  MSG_M4TOM0_OT_COAP_SECURE_IS_CONNECTION_ALIVE,
  MSG_M4TOM0_OT_COAP_SECURE_SEND_REQUEST_BLOCK_WISE,
  MSG_M4TOM0_OT_COAP_SECURE_SEND_REQUEST,
  MSG_M4TOM0_OT_COAP_SECURE_ADD_BLOCK_WISE_RESOURCE,
  MSG_M4TOM0_OT_COAP_SECURE_REMOVE_BLOCK_WISE_RESOURCE,
  MSG_M4TOM0_OT_COAP_SECURE_ADD_RESOURCE,
  MSG_M4TOM0_OT_COAP_SECURE_REMOVE_RESOURCE,
  MSG_M4TOM0_OT_COAP_SECURE_SET_DEFAULT_HANDLER,
  MSG_M4TOM0_OT_COAP_SECURE_SET_CLIENT_CONNECTED_CALLBACK,
  MSG_M4TOM0_OT_COAP_SECURE_SEND_RESPONSE_BLOCK_WISE,
  MSG_M4TOM0_OT_COAP_SECURE_SEND_RESPONSE,
  /* MESSAGE */
  MSG_M4TOM0_OT_MESSAGE_FREE,
  MSG_M4TOM0_OT_MESSAGE_GET_LENGTH,
  MSG_M4TOM0_OT_MESSAGE_SET_LENGTH,
  MSG_M4TOM0_OT_MESSAGE_GET_OFFSET,
  MSG_M4TOM0_OT_MESSAGE_SET_OFFSET,
  MSG_M4TOM0_OT_MESSAGE_IS_LINK_SECURITY_ENABLED,
  MSG_M4TOM0_OT_MESSAGE_SET_DIRECT_TRANSMISSION,
  MSG_M4TOM0_OT_MESSAGE_GET_RSS,
  MSG_M4TOM0_OT_MESSAGE_APPEND,
  MSG_M4TOM0_OT_MESSAGE_READ,
  MSG_M4TOM0_OT_MESSAGE_WRITE,
  MSG_M4TOM0_OT_MESSAGE_QUEUE_INIT,
  MSG_M4TOM0_OT_MESSAGE_QUEUE_ENQUEUE,
  MSG_M4TOM0_OT_MESSAGE_QUEUE_ATHEAD,
  MSG_M4TOM0_OT_MESSAGE_QUEUE_DEQUEUE,
  MSG_M4TOM0_OT_MESSAGE_QUEUE_GET_HEAD,
  MSG_M4TOM0_OT_MESSAGE_QUEUE_GET_NEXT,
  MSG_M4TOM0_OT_MESSAGE_BUFFER_INFO,
  /* MULTI RADIO */
  MSG_M4TOM0_OT_MULTI_RADIO_GET_NEIGHBOR_INFO,
  /* COMMISSIONER */
  MSG_M4TOM0_OT_COMMISSIONER_START,
  MSG_M4TOM0_OT_COMMISSIONER_STOP,
  MSG_M4TOM0_OT_COMMISSIONER_ADD_JOINER,
  MSG_M4TOM0_OT_COMMISSIONER_ADD_JOINER_WITH_DISCERNER,
  MSG_M4TOM0_OT_COMMISSIONER_REMOVE_JOINER,
  MSG_M4TOM0_OT_COMMISSIONER_REMOVE_JOINER_WITH_DISCERNER,
  MSG_M4TOM0_OT_COMMISSIONER_GET_NEXT_JOINER_INFO,
  MSG_M4TOM0_OT_COMMISSIONER_GET_PROVISIONING_URL,
  MSG_M4TOM0_OT_COMMISSIONER_SET_PROVISIONING_URL,
  MSG_M4TOM0_OT_COMMISSIONER_ANNOUNCE_BEGIN,
  MSG_M4TOM0_OT_COMMISSIONER_ENERGY_SCAN,
  MSG_M4TOM0_OT_COMMISSIONER_PANID_QUERY,
  MSG_M4TOM0_OT_COMMISSIONER_SEND_MGMT_GET,
  MSG_M4TOM0_OT_COMMISSIONER_SEND_MGMT_SET,
  MSG_M4TOM0_OT_COMMISSIONER_GET_SESSION_ID,
  MSG_M4TOM0_OT_COMMISSIONER_GET_STATE,
  /* DATASET_FTD */
  MSG_M4TOM0_OT_DATASET_CREATE_NEW_NETWORK,
  MSG_M4TOM0_OT_DATASET_GET_DELAY_TIMER_MINIMAL,
  MSG_M4TOM0_OT_DATASET_SET_DELAY_TIMER_MINIMAL,
  /* DATASET UPDATER */
  MSG_M4TOM0_OT_DATASET_UPDATER_REQUEST_UPDATE,
  MSG_M4TOM0_OT_DATASET_UPDATER_CANCEL_UPDATE,
  MSG_M4TOM0_OT_DATASET_UPDATER_IS_UPDATE_ON_GOING,
  /* DATASET */
  MSG_M4TOM0_OT_DATASET_IS_COMMISSIONED,
  MSG_M4TOM0_OT_DATASET_GET_ACTIVE,
  MSG_M4TOM0_OT_DATASET_GET_ACTIVE_TLVS,
  MSG_M4TOM0_OT_DATASET_SET_ACTIVE,
  MSG_M4TOM0_OT_DATASET_SET_ACTIVE_TLVS,
  MSG_M4TOM0_OT_DATASET_GET_PENDING,
  MSG_M4TOM0_OT_DATASET_GET_PENDING_TLVS,
  MSG_M4TOM0_OT_DATASET_SET_PENDING,
  MSG_M4TOM0_OT_DATASET_SET_PENDING_TLVS,
  MSG_M4TOM0_OT_DATASET_SEND_MGMT_ACTIVE_GET,
  MSG_M4TOM0_OT_DATASET_SEND_MGMT_ACTIVE_SET,
  MSG_M4TOM0_OT_DATASET_SEND_MGMT_PENDING_GET,
  MSG_M4TOM0_OT_DATASET_SEND_MGMT_PENDING_SET,
  MSG_M4TOM0_OT_DATASET_GENERATE_PSKC,
  MSG_M4TOM0_OT_NETWORK_NAME_FROM_STRING,
  MSG_M4TOM0_OT_DATASET_PARSE_TLVS,
  /* DIAG */
  MSG_M4TOM0_OT_DIAG_PROCESS_CMD,
  MSG_M4TOM0_OT_DIAG_PROCESS_CMD_LINE,
  MSG_M4TOM0_OT_DIAG_IS_ENABLED,
  /* DNS */
  MSG_M4TOM0_OT_DNS_INIT_TX_ENTRY_ITERATOR,
  MSG_M4TOM0_OT_DNS_GET_NEXT_TX_ENTRY,
  MSG_M4TOM0_OT_DNS_SET_NAME_COMPRESSION_ENABLED,
  MSG_M4TOM0_OT_DNS_IS_NAME_COMPRESSION_ENABLED,
  MSG_M4TOM0_OT_DNS_CLIENT_GET_DEFAULT_CONFIG,
  MSG_M4TOM0_OT_DNS_CLIENT_SET_DEFAULT_CONFIG,
  MSG_M4TOM0_OT_DNS_CLIENT_RESOLVE_ADDRESS,
  MSG_M4TOM0_OT_DNS_ADDRESS_RESPONSE_GET_HOST_NAME,
  MSG_M4TOM0_OT_DNS_ADDRESS_RESPONSE_GET_ADDRESS,
  MSG_M4TOM0_OT_DNS_CLIENT_BROWSE,
  MSG_M4TOM0_OT_BROWSE_RESPONSE_GET_SERVICE_NAME,
  MSG_M4TOM0_OT_BROWSE_RESPONSE_GET_SERVICE_INSTANCE,
  MSG_M4TOM0_OT_BROWSE_RESPONSE_GET_SERVICE_INFO,
  MSG_M4TOM0_OT_BROWSE_RESPONSE_GET_HOST_ADDRESS,
  MSG_M4TOM0_OT_DNS_CLIENT_RESOLVE_SERVICE,
  MSG_M4TOM0_OT_DNS_SERVICE_RESPONSE_GET_SERVICE_NAME,
  MSG_M4TOM0_OT_DNS_SERVICE_RESPONSE_GET_SERVICE_INFO,
  MSG_M4TOM0_OT_DNS_SERVICE_RESPONSE_GET_HOST_ADDRESS,
  /* DNSSD SERVER */
  MSG_M4TOM0_OT_DNSSD_QUERY_SET_CALLBACKS,
  MSG_M4TOM0_OT_DNSSD_QUERY_HANDLE_DISCOVERED_SERVICE_INSTANCE,
  MSG_M4TOM0_OT_DNSSD_QUERY_DISCOVERED_HOST,
  MSG_M4TOM0_OT_DNSSD_GET_NEXT_QUERY,
  MSG_M4TOM0_OT_DNSSD_GET_QUERY_TYPE_AND_NAME,
  /* HISTORY TRACKER */
  MSG_M4TOM0_OT_HISTORY_TRACKER_INIT_ITERATOR,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_NET_INFO_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_UNICAST_ADDRESS_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_MULTICAST_ADDRESS_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_RX_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_TX_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_NEIGHBOR_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_ON_MESH_PREFIX_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ITERATE_EXTERNAL_ROUTE_HISTORY,
  MSG_M4TOM0_OT_HISTORY_TRACKER_ENTRY_AGE_TO_STRING,
  /* ICMP6 */
  MSG_M4TOM0_OT_ICMP6_GET_ECHO_MODE,
  MSG_M4TOM0_OT_ICMP6_SET_ECHO_MODE,
  MSG_M4TOM0_OT_ICMP6_REGISTER_HANDLER,
  MSG_M4TOM0_OT_ICMP6_SEND_ECHO_REQUEST,
  /* JOINER */
  MSG_M4TOM0_OT_JOINER_START,
  MSG_M4TOM0_OT_JOINER_STOP,
  MSG_M4TOM0_OT_JOINER_GET_STATE,
  MSG_M4TOM0_OT_JOINER_GET_ID,
  MSG_M4TOM0_OT_JOINER_SET_DISCERNER,
  MSG_M4TOM0_OT_JOINER_GET_DISCERNER,
  MSG_M4TOM0_OT_JOINER_STATE_TO_STRING,
  /* LINK RAW */
  MSG_M4TOM0_OT_LINK_RAW_SET_RECEIVE_DONE,
  MSG_M4TOM0_OT_LINK_RAW_IS_ENABLED,
  MSG_M4TOM0_OT_LINK_RAW_SET_PANID,
  MSG_M4TOM0_OT_LINK_RAW_SET_EXTENDED_ADDRESS,
  MSG_M4TOM0_OT_LINK_RAW_SET_SHORT_ADDRESS,
  MSG_M4TOM0_OT_LINK_RAW_GET_PROMISCUOUS,
  MSG_M4TOM0_OT_LINK_RAW_SET_PROMISCUOUS,
  MSG_M4TOM0_OT_LINK_RAW_SLEEP,
  MSG_M4TOM0_OT_LINK_RAW_RECEIVE,
  MSG_M4TOM0_OT_LINK_RAW_GET_TRANSMIT_BUFFER,
  MSG_M4TOM0_OT_LINK_RAW_TRANSMIT,
  MSG_M4TOM0_OT_LINK_RAW_GET_RSSI,
  MSG_M4TOM0_OT_LINK_RAW_GET_CAPS,
  MSG_M4TOM0_OT_LINK_RAW_ENERGY_SCAN,
  MSG_M4TOM0_OT_LINK_RAW_SRC_MATCH_ENABLE,
  MSG_M4TOM0_OT_LINK_RAW_SRC_MATCH_ADD_SHORT_ENTRY,
  MSG_M4TOM0_OT_LINK_RAW_SRC_MATCH_ADD_EXT_ENTRY,
  MSG_M4TOM0_OT_LINK_RAW_SRC_MATCH_CLEAR_SHORT_ENTRY,
  MSG_M4TOM0_OT_LINK_RAW_SRC_MATCH_CLEAR_EXT_ENTRY,
  MSG_M4TOM0_OT_LINK_RAW_SRC_MATCH_CLEAR_SHORT_ENTRIES,
  MSG_M4TOM0_OT_LINK_RAW_SRC_MATCH_CLEAR_EXT_ENTRIES,
  MSG_M4TOM0_OT_LINK_RAW_SET_MAC_KEY,
  MSG_M4TOM0_OT_LINK_RAW_SET_MAC_FRAME_COUNTER,
  MSG_M4TOM0_OT_LINK_RAW_GET_RADIO_TIME,
  /* NET_DATA */
  MSG_M4TOM0_OT_NET_DATA_GET,
  MSG_M4TOM0_OT_NET_DATA_GET_NEXT_ON_MESH_PREFIX,
  MSG_M4TOM0_OT_NET_DATA_GET_NEXT_ROUTE,
  MSG_M4TOM0_OT_NET_DATA_GET_NEXT_SERVICE,
  MSG_M4TOM0_OT_NET_DATA_GET_VERSION,
  MSG_M4TOM0_OT_NET_DATA_GET_STABLE_VERSION,
  MSG_M4TOM0_OT_NET_DATA_STEERING_DATA_CHECK_JOINER,
  MSG_M4TOM0_OT_NET_DATA_STEERING_DATA_CHECK_JOINER_WITH_DISCERNER,
  /* NET_DATA PUBLISHER */
  MSG_M4TOM0_OT_NET_DATA_PUBLISH_DNS_SRP_SERVICE_ANYCAST,
  MSG_M4TOM0_OT_NET_DATA_PUBLISH_DNS_SRP_SERVICE_UNICAST,
  MSG_M4TOM0_OT_NET_DATA_PUBLISH_DNS_UNICAST_MESH_LOCAL_EID,
  MSG_M4TOM0_OT_NET_DATA_IS_DNS_SRP_SERVICE_ADDED,
  MSG_M4TOM0_OT_NET_DATA_SET_DNS_SRP_SERVICE_PUBLISHER_CALLBACK,
  MSG_M4TOM0_OT_NET_DATA_UNPUBLISH_DNS_SRP_SERVICE,
  MSG_M4TOM0_OT_NET_DATA_PUBLISH_ON_MESH_PREFIX,
  MSG_M4TOM0_OT_NET_DATA_PUBLISH_EXTERNAL_ROUTE,
  MSG_M4TOM0_OT_NET_DATA_IS_PREFIX_ADDED,
  MSG_M4TOM0_OT_NET_DATA_SET_PREFIX_PUBLISHER_CALLBACK,
  MSG_M4TOM0_OT_NET_DATA_UNPUBLISH_PREFIX,
  /* NET DIAG */
  MSG_M4TOM0_OT_THREAD_NEXT_DIAGNOSTIC_TLV,
  MSG_M4TOM0_OT_THREAD_SEND_DIAGNOSTIC_GET,
  MSG_M4TOM0_OT_THREAD_SEND_DIAGNOSTIC_RESET,
  /* PING SENDER */
  MSG_M4TOM0_OT_PING_SENDER_PING,
  MSG_M4TOM0_OT_PING_SENDER_STOP,
  /* OPENTHREAD */
  MSG_M4TOM0_OT_OPENTHREAD_ERROR_TO_STRING,             //TODO : UNUSED, replaced by MSG_M4TOM0_OT_THREAD_ERROR_TO_STRING
  /* TASKLET */
  MSG_M4TOM0_OT_TASKLETS_PROCESS,
  MSG_M4TOM0_OT_TASKLETS_ARE_PENDING,
  /* TMF_PROXY */
  MSG_M4TOM0_OT_TMF_PROXY_START,
  MSG_M4TOM0_OT_TMF_PROXY_STOP,
  MSG_M4TOM0_OT_TMF_PROXY_SEND,
  MSG_M4TOM0_OT_TMF_PROXY_IS_ENABLED,
  /* UDP */
  MSG_M4TOM0_OT_UDP_ADD_RECEIVER,
  MSG_M4TOM0_OT_UDP_REMOVE_RECEIVER,
  MSG_M4TOM0_OT_UDP_SEND_DATAGRAM,
  MSG_M4TOM0_OT_UDP_NEW_MESSAGE,
  MSG_M4TOM0_OT_UDP_OPEN,
  MSG_M4TOM0_OT_UDP_IS_OPEN,
  MSG_M4TOM0_OT_UDP_IS_PORT_IN_USE,
  MSG_M4TOM0_OT_UDP_CLOSE,
  MSG_M4TOM0_OT_UDP_BIND,
  MSG_M4TOM0_OT_UDP_CONNECT,
  MSG_M4TOM0_OT_UDP_SEND,
  MSG_M4TOM0_OT_UDP_FORWARD_SET_FORWARDER,
  /* SRP_CLIENT */
  MSG_M4TOM0_OT_SRP_CLIENT_START,
  MSG_M4TOM0_OT_SRP_CLIENT_STOP,
  MSG_M4TOM0_OT_SRP_CLIENT_IS_RUNNING,
  MSG_M4TOM0_OT_SRP_CLIENT_GET_SERVER_ADDRESS,
  MSG_M4TOM0_OT_SRP_CLIENT_SET_CALLBACK,
  MSG_M4TOM0_OT_SRP_CLIENT_ENABLE_AUTO_START_MODE,
  MSG_M4TOM0_OT_SRP_CLIENT_DISABLE_AUTO_START_MODE,
  MSG_M4TOM0_OT_SRP_CLIENT_IS_AUTO_START_MODE_ENABLED,
  MSG_M4TOM0_OT_SRP_CLIENT_GET_LEASE_INTERVAL,
  MSG_M4TOM0_OT_SRP_CLIENT_SET_LEASE_INTERVAL,
  MSG_M4TOM0_OT_SRP_CLIENT_GET_KEY_LEASE_INTERVAL,
  MSG_M4TOM0_OT_SRP_CLIENT_SET_KEY_LEASE_INTERVAL,
  MSG_M4TOM0_OT_SRP_CLIENT_GET_HOST_INFO,
  MSG_M4TOM0_OT_SRP_CLIENT_SET_HOST_NAME,
  MSG_M4TOM0_OT_SRP_CLIENT_SET_HOST_ADDRESSES,
  MSG_M4TOM0_OT_SRP_CLIENT_ADD_SERVICE,
  MSG_M4TOM0_OT_SRP_CLIENT_REMOVE_SERVICE,
  MSG_M4TOM0_OT_SRP_CLIENT_CLEAR_SERVICE,
  MSG_M4TOM0_OT_SRP_CLIENT_GET_SERVICES,
  MSG_M4TOM0_OT_SRP_CLIENT_REMOVE_HOST_AND_SERVICES,
  MSG_M4TOM0_OT_SRP_CLIENT_CLEAR_HOST_AND_SERVICES,
  MSG_M4TOM0_OT_SRP_CLIENT_GET_DOMAIN_NAME,
  MSG_M4TOM0_OT_SRP_CLIENT_SET_DOMAIN_NAME,
  MSG_M4TOM0_OT_SRP_CLIENT_ITEM_STATE,
  MSG_M4TOM0_OT_SRP_CLIENT_SET_SERVICE_KEY_RECORD_ENABLED,
  MSG_M4TOM0_OT_SRP_CLIENT_IS_SERVICE_KEY_RECORD,
  /* SRP_CLIENT_BUFFERS */
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_GET_HOST_NAME_STRING,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_GET_HOST_ADDRESSES_ARRAY,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_ALLOCATE_SERVICE,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_FREE_SERVICE,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_FREE_ALL_SERVICES,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_GET_SERVICE_ENTRY_SERVICE_NAME_STRING,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_GET_SERVICE_ENTRY_INSTANCE_NAME_STRING,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_GET_SERVICE_ENTRY_TX_BUFFER,
  MSG_M4TOM0_OT_SRP_CLIENT_BUFFERS_GET_SUB_TYPE_LABELS_ARRAY,
  /* SRP_SERVER */
  MSG_M4TOM0_OT_SRP_SERVER_GET_DOMAIN,
  MSG_M4TOM0_OT_SRP_SERVER_SET_DOMAIN,
  MSG_M4TOM0_OT_SRP_SERVER_GET_STATE,
  MSG_M4TOM0_OT_SRP_SERVER_GET_ADDRESS_MODE,
  MSG_M4TOM0_OT_SRP_SERVER_SET_ADDRESS_MODE,
  MSG_M4TOM0_OT_SRP_SERVER_GET_ANYCAST_MODE_SEQUENCE_NUMBER,
  MSG_M4TOM0_OT_SRP_SERVER_SET_ANYCAST_MODE_SEQUENCE_NUMBER,
  MSG_M4TOM0_OT_SRP_SERVER_SET_ENABLED,
  MSG_M4TOM0_OT_SRP_SERVER_GET_LEASE_CONFIG,
  MSG_M4TOM0_OT_SRP_SERVER_SET_LEASE_CONFIG,
  MSG_M4TOM0_OT_SRP_SERVER_SET_SERVICE_UPDATE_HANDLER,
  MSG_M4TOM0_OT_SRP_SERVER_HANDLE_SERVICE_UPDATE_RESULT,
  MSG_M4TOM0_OT_SRP_SERVER_GET_NEXT_HOST,
  MSG_M4TOM0_OT_SRP_SERVER_HOST_IS_DELETED,
  MSG_M4TOM0_OT_SRP_SERVER_HOST_GET_FULL_NAME,
  MSG_M4TOM0_OT_SRP_SERVER_HOST_GET_ADDRESSES,
  MSG_M4TOM0_OT_SRP_SERVER_HOST_GET_NEXT_SERVICE,
  MSG_M4TOM0_OT_SRP_SERVER_HOST_FIND_NEXT_SERVICE,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_IS_DELETED,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_IS_SUB_TYPE,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_FULL_NAME,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_INSTANCE_NAME,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_SERVICE_NAME,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_SERVICE_SUB_TYPE_LABEL,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_PORT,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_WEIGHT,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_PRIORITY,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_TX_DATA,
  MSG_M4TOM0_OT_SRP_SERVER_SERVICE_GET_HOST,
  /* CHANNEL_MANAGER */
  MSG_M4TOM0_OT_CHANNEL_MANAGER_REQUEST_CHANNEL_CHANGE,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_GET_REQUESTED_CHANNEL,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_GET_DELAY,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_SET_DELAY,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_REQUEST_CHANNEL_SELECT,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_SET_AUTO_CHANNEL_SELECTION_ENABLED,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_GET_AUTO_CHANNEL_SELECTION_ENABLED,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_SET_AUTO_CHANNEL_SELECTION_INTERVAL,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_GET_AUTO_CHANNEL_SELECTION_INTERVAL,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_GET_SUPPORTED_CHANNELS,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_SET_SUPPORTED_CHANNELS,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_GET_FAVORED_CHANNELS,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_SET_FAVORED_CHANNELS,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_GET_CCA_FAILURE_RATE_THRESHOLD,
  MSG_M4TOM0_OT_CHANNEL_MANAGER_SET_CCA_FAILURE_RATE_THRESHOLD,
  /* CHANNEL_MONITOR */
  MSG_M4TOM0_OT_CHANNEL_MONITOR_SET_ENABLED,
  MSG_M4TOM0_OT_CHANNEL_MONITOR_IS_ENABLED,
  MSG_M4TOM0_OT_CHANNEL_MONITOR_GET_SAMPLE_INTERVAL,
  MSG_M4TOM0_OT_CHANNEL_MONITOR_GET_RSSI_THRESHOLD,
  MSG_M4TOM0_OT_CHANNEL_MONITOR_GET_SAMPLE_WINDOW,
  MSG_M4TOM0_OT_CHANNEL_MONITOR_GET_SAMPLE_COUNT,
  MSG_M4TOM0_OT_CHANNEL_MONITOR_GET_CHANNEL_OCCUPANCY,
  /* CHILD_SUPERVISION */
  MSG_M4TOM0_OT_CHILD_SUPERVISION_GET_INTERVAL,
  MSG_M4TOM0_OT_CHILD_SUPERVISION_SET_INTERVAL,
  MSG_M4TOM0_OT_CHILD_SUPERVISION_GET_CHECK_TIMEOUT,
  MSG_M4TOM0_OT_CHILD_SUPERVISION_SET_CHECK_TIMEOUT,
  /* JAM_DETECTION */
  MSG_M4TOM0_OT_JAM_DETECTION_SET_RSSI_THRESHOLD,
  MSG_M4TOM0_OT_JAM_DETECTION_GET_RSSI_THRESHOLD,
  MSG_M4TOM0_OT_JAM_DETECTION_SET_WINDOW,
  MSG_M4TOM0_OT_JAM_DETECTION_GET_WINDOW,
  MSG_M4TOM0_OT_JAM_DETECTION_SET_BUSY_PERIOD,
  MSG_M4TOM0_OT_JAM_DETECTION_GET_BUSY_PERIOD,
  MSG_M4TOM0_OT_JAM_DETECTION_START,
  MSG_M4TOM0_OT_JAM_DETECTION_STOP,
  MSG_M4TOM0_OT_JAM_DETECTION_IS_ENABLED,
  MSG_M4TOM0_OT_JAM_DETECTION_GET_STATE,
  MSG_M4TOM0_OT_JAM_DETECTION_GET_HISTORY_BITMAP,
  /* SERVER */
  MSG_M4TOM0_OT_SERVER_GET_NET_DATA_LOCAL,
  MSG_M4TOM0_OT_SERVER_ADD_SERVICE,
  MSG_M4TOM0_OT_SERVER_REMOVE_SERVICE,
  MSG_M4TOM0_OT_SERVER_GET_NEXT_SERVICE,
  MSG_M4TOM0_OT_SERVER_REGISTER,
  /* NETWORK */
  MSG_M4TOM0_OT_NETWORK_TIME_GET,
  MSG_M4TOM0_OT_NETWORK_TIME_SET_SYNC_PERIOD,
  MSG_M4TOM0_OT_NETWORK_TIME_GET_SYNC_PERIOD,
  MSG_M4TOM0_OT_NETWORK_TIME_SET_XTAL_THRESHOLD,
  MSG_M4TOM0_OT_NETWORK_TIME_GET_XTAL_THRESHOLD,
  MSG_M4TOM0_OT_NETWORK_SYNC_SET_CALLBACK,
  /* Crypto */
  MSG_M4TOM0_OT_CRYPTO_HMAC_SHA256,
  MSG_M4TOM0_OT_CRYPTO_AES_CCM,
  MSG_M4TOM0_OT_CRYPTO_ECDSA_SIGN,
  /* BORDER AGENT */
  MSG_M4TOM0_OT_BORDER_AGENT_GET_STATE,
  MSG_M4TOM0_OT_BORDER_AGENT_GET_UDP_PORT,
  /* BORDER ROUTING */
  MSG_M4TOM0_OT_BORDER_ROUTING_INIT,
  MSG_M4TOM0_OT_BORDER_ROUTING_SET_ENABLED,
  MSG_M4TOM0_OT_BORDER_ROUTING_GET_OMR_PREFIX,
  MSG_M4TOM0_OT_BORDER_ROUTING_GET_ONLINK_PREFIX,
  MSG_M4TOM0_OT_BORDER_ROUTING_GET_NAT64_PREFIX,
  /* BORDER ROUTER */
  MSG_M4TOM0_OT_BORDER_ROUTER_GET_NET_DATA,
  MSG_M4TOM0_OT_BORDER_ROUTER_ADD_ON_MESH_PREFIX,
  MSG_M4TOM0_OT_BORDER_ROUTER_REMOVE_ON_MESH_PREFIX,
  MSG_M4TOM0_OT_BORDER_ROUTER_GET_NEXT_ON_MESH_PREFIX,
  MSG_M4TOM0_OT_BORDER_ROUTER_ADD_ROUTE,
  MSG_M4TOM0_OT_BORDER_ROUTER_REMOVE_ROUTE,
  MSG_M4TOM0_OT_BORDER_ROUTER_GET_NEXT_ROUTE,
  MSG_M4TOM0_OT_BORDER_ROUTER_REGISTER,
  /* ENTROPY */
  MSG_M4TOM0_OT_ENTROPY_MBED_TLS_CONTEXT_GET,
  /* ERROR */
  MSG_M4TOM0_OT_THREAD_ERROR_TO_STRING,
  /* LOGGING */
  MSG_M4TOM0_OT_LOGGING_GET_LEVEL,
  MSG_M4TOM0_OT_LOGGING_SET_LEVEL,
  /* RANDOM CRYPTO */
  MSG_M4TOM0_OT_RANDOM_CRYPTO_FILL_BUFFER,
  MSG_M4TOM0_OT_RANDOM_CRYPTO_MBDED_TLS_CONTEXT_GET,
  /* RANDOM NON CRYPTO */
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_GET_UINT_32,
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_GET_UINT8,
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_GET_UINT16,
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_GET_UINT8_IN_RANGE,
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_GET_UINT16_IN_RANGE,
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_GET_UINT32_IN_RANGE,
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_FILL_BUFFER,
  MSG_M4TOM0_OT_RANDOM_NON_CRYPTO_ADD_JITTER,
  /* SNTP */
  MSG_M4TOM0_OT_SNTP_CLIENT_QUERY,
  MSG_M4TOM0_OT_SNTP_CLIENT_SET_UNIX_ERA,
  /* Radio platform */
  MSG_M4TOM0_OT_RADIO_SET_TRANSMIT_POWER,
  MSG_M4TOM0_OT_RADIO_GET_TRANSMIT_POWER,
  /* BACKBONE ROUTER */
  MSG_M4TOM0_OT_BACKBONE_ROUTER_GET_PRIMARY,
  /* BACKBONE ROUTER FTD */
  MSG_M4TOM0_OT_BACKBONE_ROUTER_SET_ENABLED,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_GET_STATE,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_GET_CONFIG,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_SET_CONFIG,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_REGISTER,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_GET_REGISTRATION,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_GET_REGISTRATION_JITTER,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_SET_REGISTRATION_JITTER,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_GET_DOMAIN_PREFIX,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_SET_DOMAIN_PREFIX,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_SET_ND_PROXY_CALLBACK,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_GET_ND_PROXY_INFO,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_SET_MULTICAST_LISTENER_CALLBACK,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_MULTICAST_LISTENER_GET_NEXT,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_SET_MULTICAST_LISTENER_GET_NEXT,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_CONFIG_NEXT_DUA_REGISTTRATION_RESPONSE,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_CONFIG_NEXT_MULTICAST_LISTENER_REGISTRATION,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_MULTICAST_LISTENER_CLEAR,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ADD,
  MSG_M4TOM0_OT_BACKBONE_ROUTER_CONFIG_SKIP_SEQ_NUM_INCREASE,
  /* TCP */
  MSG_M4TOM0_OT_TCP_ENDPOINT_INITIALIZE,
  MSG_M4TOM0_OT_TCP_ENDPOINT_GET_INSTANCE,
  MSG_M4TOM0_OT_TCP_ENDPOINT_GET_CONTEXT,
  MSG_M4TOM0_OT_TCP_GET_LOCAL_ADDRESS,
  MSG_M4TOM0_OT_TCP_GET_PEER_ADDRESS,
  MSG_M4TOM0_OT_TCP_BIND,
  MSG_M4TOM0_OT_TCP_CONNECT,
  MSG_M4TOM0_OT_TCP_SEND_BY_REFERENCE,
  MSG_M4TOM0_OT_TCP_SEND_BY_EXTENSION,
  MSG_M4TOM0_OT_TCP_RECEIVE_BY_REFERENCE,
  MSG_M4TOM0_OT_TCP_RECEIVE_CONTIGUITY,
  MSG_M4TOM0_OT_TCP_COMMIT_RECEIVE,
  MSG_M4TOM0_OT_TCP_SEND_END_OF_STREAM,
  MSG_M4TOM0_OT_TCP_ABORT,
  MSG_M4TOM0_OT_TCP_ENDPOINT_DEINITIALIZE,
  MSG_M4TOM0_OT_TCP_LISTENER_INITIALIZE,
  MSG_M4TOM0_OT_TCP_LISTENER_GET_INSTANCE,
  MSG_M4TOM0_OT_TCP_LISTENER_GET_CONTEXT,
  MSG_M4TOM0_OT_TCP_LISTEN,
  MSG_M4TOM0_OT_TCP_STOP_LISTENING,
  MSG_M4TOM0_OT_TCP_LISTENER_DEINITIALIZE,
  /* TREL */
  MSG_M4TOM0_OT_TREL_ENABLE,
  MSG_M4TOM0_OT_TREL_DISABLE,
  MSG_M4TOM0_OT_TREL_IS_ENABLED,
  MSG_M4TOM0_OT_TREL_INIT_PEER_ITERATOR,
  MSG_M4TOM0_OT_TREL_GET_NEXT_PEER,
  MSG_M4TOM0_OT_TREL_SET_FILTER_ENABLED,
  MSG_M4TOM0_OT_TREL_IS_FILTER_ENABLED
} MsgId_M4toM0_Enum_t;

/* List of messages sent by the M0 to the M4 */
typedef enum
{
  MSG_M0TOM4_SYNCHRO_INIT,
  MSG_M0TOM4_NOTIFY_STATE_CHANGE,
  MSG_M0TOM4_COAP_REQUEST_HANDLER,
  MSG_M0TOM4_COAP_RESPONSE_HANDLER,
  MSG_M0TOM4_NOTIFY_STACK_RESET,
  MSG_M0TOM4_IP6_RECEIVE,
  MSG_M0TOM4_IP6_ADDRESS,
  MSG_M0TOM4_IP6_SLAAC_IID_CREATE,
  MSG_M0TOM4_IP6_SLAAC_PREFIX_FILTER,
  MSG_M0TOM4_IP6_REGISTER_MULTICAST_LISTENERS_CB,
  MSG_M0TOM4_HANDLE_ACTIVE_SCAN_RESULT,
  MSG_M0TOM4_HANDLE_ENERGY_SCAN_RESULT,
  MSG_M0TOM4_HANDLE_LINK_PCAP,
  MSG_M0TOM4_RECEIVE_DIAGNOSTIC_GET_CALLBACK,
  MSG_M0TOM4_THREAD_FTD_NEIGHBOR_TABLE_CALLBACK,
  MSG_M0TOM4_COMMISSIONER_ENERGY_REPORT_CALLBACK,
  MSG_M0TOM4_COMMISSIONER_PANID_CONFLICT_CALLBACK,
  MSG_M0TOM4_COMMISSIONER_STATE_CALLBACK,
  MSG_M0TOM4_COMMISSIONER_JOINER_CALLBACK,
  MSG_M0TOM4_DNS_RESPONSE_HANDLER,
  MSG_M0TOM4_ICMP6_RECEIVE_CALLBACK,
  MSG_M0TOM4_JOINER_CALLBACK,
  MSG_M0TOM4_LINK_RAW_RECEIVE_DONE,
  MSG_M0TOM4_LINK_RAW_TRANSMIT_DONE,
  MSG_M0TOM4_LINK_RAW_ENERGY_SCAN_DONE,
  MSG_M0TOM4_LINK_METRICS_MGMT_RESPONSE_ENHACK_PROBING_CB,
  MSG_M0TOM4_LINK_METRICS_MGMT_RESPONSE_CB,
  MSG_M0TOM4_LINK_METRICS_ENHACK_PROBING_IE_REPORT_CB,
  MSG_M0TOM4_LINK_METRICS_REPORT_CB,
  MSG_M0TOM4_TMF_PROXY_STREAM_HANDLER,
  MSG_M0TOM4_UDP_RECEIVE,
  MSG_M0TOM4_UDP_FORWARDER,
  MSG_M0TOM4_UDP_FORWARD_RECEIVE,
  MSG_M0TOM4_UDP_GET_SOCKETS,
  MSG_M0TOM4_JAM_DETECTION_CALLBACK,
  MSG_M0TOM4_TRACE_SEND,
  MSG_M0TOM4_DEFAULT_COAP_REQUEST_HANDLER,
  MSG_M0TOM4_COAP_SECURE_CLIENT_CONNECT,
  MSG_M0TOM4_COAP_SECURE_SET_CLIENT_CONNECT,
  MSG_M0TOM4_COAP_SECURE_DEFAULT_REQUEST_HANDLER,
  MSG_M0TOM4_UDP_HANDLER,
  MSG_M0TOM4_NETWORK_TIME_SYNC_CALLBACK_FN,
  MSG_M0TOM4_SNTP_RESPONSE_HANDLER,
  MSG_M0TOM4_THREAD_PARENT_RESPONSE_HANDLER,
  MSG_M0TOM4_THREAD_ANYCAST_LOCATOR_CB,
  MSG_M0TOM4_DATASET_MGMT_SET_CALLBACK_ACTIVE,
  MSG_M0TOM4_DATASET_MGMT_SET_CALLBACK_PENDING,
  MSG_M0TOM4_DATASET_UPDATER_CB,
  MSG_M0TOM4_DNS_BROWSE_CB,
  MSG_M0TOM4_DNS_ADDRESS_CB,
  MSG_M0TOM4_DNS_SERVICE_CB,
  MSG_M0TOM4_DNSSD_QUERY_SUBSCRIBE_CB,
  MSG_M0TOM4_DNSSD_QUERY_UNSUBSCRIBE_CB,
  MSG_M0TOM4_BACKBONE_ROUTER_DOMAIN_PREFIX_CB,
  MSG_M0TOM4_BACKBONE_ROUTER_MULTICAST_LISTENER_CB,
  MSG_M0TOM4_BACKBONE_ROUTER_ND_PROXY_CB,
  MSG_M0TOM4_NETDATA_DNS_SRP_SERVICE_PUBLISHER_CB,
  MSG_M0TOM4_NETDATA_PREFIX_PUBLISHER_CB,
  MSG_M0TOM4_SRP_CLIENT_AUTO_START_CB,
  MSG_M0TOM4_SRP_CLIENT_CB,
  MSG_M0TOM4_SRP_SERVER_SERVICE_UPDATE_HANDLER_CB,
  MSG_M0TOM4_RECEIVE_DIAGNOSTIC_GET_CB,
  MSG_M0TOM4_THREAD_DISCOVERY_REQUEST_CB,
} MsgId_M0toM4_Enum_t;

/* List of modes available for UART configuration */
typedef enum
{
  SYS_LPUART1_CLI,
  SYS_USART1_CLI,
} Sys_ConfigUart_Enum_t;

/* List of modes available on the overall device  */
typedef enum
{
  SYS_PROTOCOL_BLE,
  SYS_PROTOCOL_THREAD,
} Sys_ConfigProtocol_Enum_t;

/* List of errors returned by the interface  */
typedef enum
{
  ERR_INTERFACE_IPCC_INIT_FAIL = 100U,
  ERR_INTERFACE_IPCC_REMOTE_FAIL = 101U,
  ERR_INTERFACE_IPCC_SEND_ACK_TO_M0 = 102U,
} Error_Interface_Id_Enum_t;

/* Gravity error level */
typedef enum
{
  ERR_INTERFACE_FATAL= 1U,
  ERR_INTERFACE_WARNING = 2U
} Error_Interface_Level_Enum_t;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STM32WBxx_CORE_INTERFACE_DEF_H */


