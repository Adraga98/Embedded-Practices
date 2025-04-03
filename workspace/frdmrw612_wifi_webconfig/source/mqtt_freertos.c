/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "mqtt_freertos.h"

#include "board.h"
#include "fsl_silicon_id.h"

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/apps/mqtt.h"
#include "lwip/tcpip.h"
#include <stdio.h>
// FIXME cleanup

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief MQTT server host name or IP address. */
#ifndef EXAMPLE_MQTT_SERVER_HOST
#define EXAMPLE_MQTT_SERVER_HOST "broker.emqx.io"
#endif

/*! @brief MQTT server port number. */
#ifndef EXAMPLE_MQTT_SERVER_PORT
#define EXAMPLE_MQTT_SERVER_PORT 1883
#endif

/*! @brief Stack size of the temporary lwIP initialization thread. */
#define INIT_THREAD_STACKSIZE 1024

/*! @brief Priority of the temporary lwIP initialization thread. */
#define INIT_THREAD_PRIO DEFAULT_THREAD_PRIO

/*! @brief Stack size of the temporary initialization thread. */
#define APP_THREAD_STACKSIZE 1024

/*! @brief Priority of the temporary initialization thread. */
#define APP_THREAD_PRIO DEFAULT_THREAD_PRIO

#define sensorLight "myDomoticHome/sensor/switchLight" //subs
#define actuatorLight "myDomoticHome/actuator/light"	//publish

#define sensorDoorLock "myDomoticHome/sensor/doorLock"  //subs
#define actuatorDoorLock "myDomoticHome/actuator/doorLock"  //pub

#define sensorWaterTank "myDomoticHome/sensor/waterTank" //publish
#define actuatorWaterPump "myDomoticHome/actuator/waterPump" //publish

#define sensorAirQ "myDomoticHome/sensor/airQ" //publish
#define sensorGasTank "myDomoticHome/sensor/gasTank" //publish

#define sensorFan "myDomoticHome/sensor/sliderFan"	//subs
#define actuatorFan "myDomoticHome/actuator/fan"	//publish

#define level0	"0"
#define level1	"1"
#define level2	"2"
#define level3	"3"
#define level4	"4"
#define level5	"5"

#define empty 0
#define minLevel 300
#define maxLevel 600
#define full 100

#define good 0
#define regular 50
#define bad 100
#define veryBad 150
#define dangerous 200

#define lightOn "1"
#define lightOff "0"

#define pumpOn "1"
#define pumpOff "0"

#define unlock "1"
#define lock "0"
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void publish_updateData(void);
static void callFunctions(void);
static void lightCtrl(char *payload, u16_t len);
static void gasCtrl(void);
static void waterCtrl(void);
static void airQuality(void);
static void doorCtrl(char *payload, u16_t len);
static void fanCtrl(char *payload, u16_t len);

static void connect_to_mqtt(void *ctx);

/*******************************************************************************
 * Variables
 ******************************************************************************/
char topicCurrent[100];

static int volumeWater = 200;// Initial value of gasTank
static int gasLevel = 45;// Initial value of gasTank

const char *lightState = lightOff;
const char *levelFan = level0;
const char *lockDoor = unlock;
const char *airQ = level1;

char payGas[10];
char payWater[10];
char payPump[10];
/*! @brief MQTT client data. */
static mqtt_client_t *mqtt_client;

/*! @brief MQTT client ID string. */
static char client_id[(SILICONID_MAX_LENGTH * 2) + 5];

/*! @brief MQTT client information. */
static const struct mqtt_connect_client_info_t mqtt_client_info = {
    .client_id   = (const char *)&client_id[0],
    .client_user = NULL,
    .client_pass = NULL,
    .keep_alive  = 100,
    .will_topic  = NULL,
    .will_msg    = NULL,
    .will_qos    = 0,
    .will_retain = 0,
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    .tls_config = NULL,
#endif
};

/*! @brief MQTT broker IP address. */
static ip_addr_t mqtt_addr;

/*! @brief Indicates connection to MQTT broker. */
static volatile bool connected = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Called when subscription request finishes.
 */
static void mqtt_topic_subscribed_cb(void *arg, err_t err)
{
    const char *topic = (const char *)arg;

    if (err == ERR_OK)
    {
        PRINTF("Subscribed to the topic \"%s\".\r\n", topic);
    }
    else
    {
        PRINTF("Failed to subscribe to the topic \"%s\": %d.\r\n", topic, err);
    }
}

/*!
 * @brief Called when there is a message on a subscribed topic.
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    LWIP_UNUSED_ARG(arg);
    strcpy(topicCurrent, topic);
    PRINTF("Received %u bytes from the topic \"%s\": \"", tot_len, topic);
}

/*!
 * @brief Called when recieved incoming published message fragment.
 */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    LWIP_UNUSED_ARG(arg);

    char payload[2] = {0}; //Buffer to store the received payload

	if(len >= sizeof(payload)){
		PRINTF("Payload too long\r\n");
		return;
	}

	memcpy(payload, data, len);//Copy the payload into a local buffer
	payload[len] = '\0'; //Ensure null-termination

	if (strcmp(topicCurrent, sensorLight) == 0) {
		lightCtrl(payload,len);
	}
	if (strcmp(topicCurrent, sensorDoorLock) == 0) {
		doorCtrl(payload, len);
	}
	if (strcmp(topicCurrent, sensorFan) == 0) {
		fanCtrl(payload, len);
	}

	if (flags & MQTT_DATA_FLAG_LAST)
	{
		PRINTF("\"\r\n");
		publish_updateData();
	}
}

/*!
 * @brief Subscribe to MQTT topics.
 */
static void mqtt_subscribe_topics(mqtt_client_t *client)
{
    static const char *topics[] = {sensorLight, sensorDoorLock, sensorFan};
    int qos[]                   = {1, 1, 1};
    err_t err;
    int i;

    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb,
                            LWIP_CONST_CAST(void *, &mqtt_client_info));

    for (i = 0; i < ARRAY_SIZE(topics); i++)
    {
        err = mqtt_subscribe(client, topics[i], qos[i], mqtt_topic_subscribed_cb, LWIP_CONST_CAST(void *, topics[i]));

        if (err == ERR_OK)
        {
            PRINTF("Subscribing to the topic \"%s\" with QoS %d...\r\n", topics[i], qos[i]);
        }
        else
        {
            PRINTF("Failed to subscribe to the topic \"%s\" with QoS %d: %d.\r\n", topics[i], qos[i], err);
        }
    }
}

/*!
 * @brief Called when connection state changes.
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t *)arg;

    connected = (status == MQTT_CONNECT_ACCEPTED);

    switch (status)
    {
        case MQTT_CONNECT_ACCEPTED:
            PRINTF("MQTT client \"%s\" connected.\r\n", client_info->client_id);
            mqtt_subscribe_topics(client);
            break;

        case MQTT_CONNECT_DISCONNECTED:
            PRINTF("MQTT client \"%s\" not connected.\r\n", client_info->client_id);
            /* Try to reconnect 1 second later */
            sys_timeout(1000, connect_to_mqtt, NULL);
            break;

        case MQTT_CONNECT_TIMEOUT:
            PRINTF("MQTT client \"%s\" connection timeout.\r\n", client_info->client_id);
            /* Try again 1 second later */
            sys_timeout(1000, connect_to_mqtt, NULL);
            break;

        case MQTT_CONNECT_REFUSED_PROTOCOL_VERSION:
        case MQTT_CONNECT_REFUSED_IDENTIFIER:
        case MQTT_CONNECT_REFUSED_SERVER:
        case MQTT_CONNECT_REFUSED_USERNAME_PASS:
        case MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_:
            PRINTF("MQTT client \"%s\" connection refused: %d.\r\n", client_info->client_id, (int)status);
            /* Try again 10 seconds later */
            sys_timeout(10000, connect_to_mqtt, NULL);
            break;

        default:
            PRINTF("MQTT client \"%s\" connection status: %d.\r\n", client_info->client_id, (int)status);
            /* Try again 10 seconds later */
            sys_timeout(10000, connect_to_mqtt, NULL);
            break;
    }
}

/*!
 * @brief Starts connecting to MQTT broker. To be called on tcpip_thread.
 */
static void connect_to_mqtt(void *ctx)
{
    LWIP_UNUSED_ARG(ctx);

    PRINTF("Connecting to MQTT broker at %s...\r\n", ipaddr_ntoa(&mqtt_addr));

    mqtt_client_connect(mqtt_client, &mqtt_addr, EXAMPLE_MQTT_SERVER_PORT, mqtt_connection_cb,
                        LWIP_CONST_CAST(void *, &mqtt_client_info), &mqtt_client_info);
}

/*!
 * @brief Called when publish request finishes.
 */
static void mqtt_message_published_cb(void *arg, err_t err)
{
    const char *topic = (const char *)arg;

    if (err == ERR_OK)
    {
        PRINTF("Published to the topic \"%s\".\r\n", topic);
    }
    else
    {
        PRINTF("Failed to publish to the topic \"%s\": %d.\r\n", topic, err);
    }
}

/*!
 * @brief Publishes a message. To be called on tcpip_thread.
 */
static void publish_message(void *ctx)
{
	callFunctions();
	LWIP_UNUSED_ARG(ctx);

	PRINTF("Going to publish to the topic \"%s\"...\r\n", sensorWaterTank);
	mqtt_publish(mqtt_client, sensorWaterTank, payWater, strlen(payWater), 2, 0, mqtt_message_published_cb, (void *)sensorWaterTank);
	PRINTF("Going to publish to the topic \"%s\"...\r\n", sensorGasTank);
	mqtt_publish(mqtt_client, sensorGasTank, payGas, strlen(payGas), 2, 0, mqtt_message_published_cb, (void *)sensorGasTank);
	PRINTF("Going to publish to the topic \"%s\"...\r\n", actuatorWaterPump);
	mqtt_publish(mqtt_client, actuatorWaterPump, payPump, strlen(payPump), 2, 0, mqtt_message_published_cb, (void *)actuatorWaterPump);
	PRINTF("Going to publish to the topic \"%s\"...\r\n", sensorAirQ);
	mqtt_publish(mqtt_client, sensorAirQ, airQ, strlen(airQ), 2, 0, mqtt_message_published_cb, (void *)sensorAirQ);
}

/*!
 * @brief Publishes a message of subscribed topics. To be called on tcpip_thread.
 */
static void publish_msgOfSuscribedTopics(void *ctx)
{
    LWIP_UNUSED_ARG(ctx);

    PRINTF("Going to publish to the topic \"%s\"...\r\n", actuatorLight);
    mqtt_publish(mqtt_client, actuatorLight, lightState, strlen(lightState), 1, 0, mqtt_message_published_cb, (void *)actuatorLight);
    PRINTF("Going to publish to the topic \"%s\"...\r\n", actuatorDoorLock);
    mqtt_publish(mqtt_client, actuatorDoorLock, lockDoor, strlen(lockDoor), 1, 0, mqtt_message_published_cb, (void *)actuatorDoorLock);
    PRINTF("Going to publish to the topic \"%s\"...\r\n", actuatorFan);
    mqtt_publish(mqtt_client, actuatorFan, levelFan, strlen(levelFan), 1, 0, mqtt_message_published_cb, (void *)actuatorFan);
}

/*!
 * @brief Application thread.
 */
static void app_thread(void *arg)
{
    struct netif *netif = (struct netif *)arg;
    err_t err;
    int i;

    PRINTF("\r\nIPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
    PRINTF("IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
    PRINTF("IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));

    /*
     * Check if we have an IP address or host name string configured.
     * Could just call netconn_gethostbyname() on both IP address or host name,
     * but we want to print some info if goint to resolve it.
     */
    if (ipaddr_aton(EXAMPLE_MQTT_SERVER_HOST, &mqtt_addr) && IP_IS_V4(&mqtt_addr))
    {
        /* Already an IP address */
        err = ERR_OK;
    }
    else
    {
        /* Resolve MQTT broker's host name to an IP address */
        PRINTF("Resolving \"%s\"...\r\n", EXAMPLE_MQTT_SERVER_HOST);
        err = netconn_gethostbyname(EXAMPLE_MQTT_SERVER_HOST, &mqtt_addr);
    }

    if (err == ERR_OK)
    {
        /* Start connecting to MQTT broker from tcpip_thread */
        err = tcpip_callback(connect_to_mqtt, NULL);
        if (err != ERR_OK)
        {
            PRINTF("Failed to invoke broker connection on the tcpip_thread: %d.\r\n", err);
        }
    }
    else
    {
        PRINTF("Failed to obtain IP address: %d.\r\n", err);
    }

    /* Publish some messages */
    while(1)
    {
    	if (connected)
       	{
       		err = tcpip_callback(publish_message, NULL);
   			if (err != ERR_OK)
   			{
   				PRINTF("Failed to invoke publishing of a message on the tcpip_thread: %d.\r\n", err);
   			}
   		}
       	sys_msleep(5000U);
   	}

    vTaskDelete(NULL);
}

static void generate_client_id(void)
{
    uint8_t silicon_id[SILICONID_MAX_LENGTH];
    const char *hex = "0123456789abcdef";
    status_t status;
    uint32_t id_len = sizeof(silicon_id);
    int idx         = 0;
    int i;
    bool id_is_zero = true;

    /* Get unique ID of SoC */
    status = SILICONID_GetID(&silicon_id[0], &id_len);
    assert(status == kStatus_Success);
    assert(id_len > 0U);
    (void)status;

    /* Covert unique ID to client ID string in form: nxp_hex-unique-id */

    /* Check if client_id can accomodate prefix, id and terminator */
    assert(sizeof(client_id) >= (5U + (2U * id_len)));

    /* Fill in prefix */
    client_id[idx++] = 'n';
    client_id[idx++] = 'x';
    client_id[idx++] = 'p';
    client_id[idx++] = '_';

    /* Append unique ID */
    for (i = (int)id_len - 1; i >= 0; i--)
    {
        uint8_t value    = silicon_id[i];
        client_id[idx++] = hex[value >> 4];
        client_id[idx++] = hex[value & 0xFU];

        if (value != 0)
        {
            id_is_zero = false;
        }
    }

    /* Terminate string */
    client_id[idx] = '\0';

    if (id_is_zero)
    {
        PRINTF(
            "WARNING: MQTT client id is zero. (%s)"
#ifdef OCOTP
            " This might be caused by blank OTP memory."
#endif
            "\r\n",
            client_id);
    }
}

/*!
 * @brief Create and run example thread
 *
 * @param netif  netif which example should use
 */
void mqtt_freertos_run_thread(struct netif *netif)
{
    LOCK_TCPIP_CORE();
    mqtt_client = mqtt_client_new();
    UNLOCK_TCPIP_CORE();
    if (mqtt_client == NULL)
    {
        PRINTF("mqtt_client_new() failed.\r\n");
        while (1)
        {
        }
    }

    generate_client_id();

    if (sys_thread_new("app_task", app_thread, netif, APP_THREAD_STACKSIZE, APP_THREAD_PRIO) == NULL)
    {
        LWIP_ASSERT("mqtt_freertos_start_thread(): Task creation failed.", 0);
    }
}

static void publish_updateData(void){
	tcpip_callback(publish_msgOfSuscribedTopics, NULL);
}

static void lightCtrl(char *payload, u16_t len){

	if (strcmp(payload, lightOn) == 0){
		lightState = "1";
		PRINTF("%c",lightOn);
	}
	else if(strcmp(payload, lightOff) == 0){
		lightState = "0";
		PRINTF("%c",lightOff);
	}
	else{
		PRINTF("Unrecognized payload: %s\r\n", payload);
		return;
	}
}

static void gasCtrl(void){

	int change = (rand()%25)-10;
	gasLevel += change;

	if(gasLevel < 1){
		gasLevel = 1;
	}
	if(gasLevel > 100){
		gasLevel = 100;
	}
	snprintf(payGas, sizeof(payGas), "%d", gasLevel);

	PRINTF("Publishing Level of Gas: %s\r\n", payGas);
}

static void waterCtrl(void){

	int sensorValue = rand()%1001;
	int change = (rand()%21) - 10; //-10 y 10
	int statePump = 0;

	sensorValue += change;

	if(sensorValue < 0){
		sensorValue = 0;
	}
	if(sensorValue > 1000){
		sensorValue = 1000;
	}

	if(sensorValue>= empty && sensorValue <= minLevel){
		statePump = 1;
	}
	else if(sensorValue> minLevel && sensorValue <= maxLevel){
		statePump = 1;
	}else if(sensorValue > maxLevel && sensorValue <= full){
		statePump = 0;
	}
	else{
		//Nothing to do
	}
	snprintf(payPump, sizeof(payPump), "%d", statePump);
	snprintf(payWater, sizeof(payWater), "%d", sensorValue);

	PRINTF("Publishing Level of Water: %s\r\n", payWater);
}

static void airQuality(void){
	int sensorValue = (rand()%501);
	int change = (rand()%21) - 10; //-10 y 10
	int quality = 0;

	sensorValue += change;

	if(sensorValue < 0){
		sensorValue = 0;
	}
	if(sensorValue > 500){
		sensorValue = 500;
	}

	if(sensorValue>= good && sensorValue <= regular){
		airQ = level1;
	}
	else if(sensorValue> regular && sensorValue <= bad){
		airQ = level2;
	}
	else if(sensorValue > bad && sensorValue <= veryBad){
		airQ = level3;
	}
	else if(sensorValue > veryBad && sensorValue <= dangerous){
		airQ = level4;
	}
	else if(sensorValue > dangerous && sensorValue <= 500){
		airQ = level5;

	}
	else{
		//Nothing to do
	}

	PRINTF("Publishing Level of AirQuality: %s\r\n", airQ);

}

static void doorCtrl(char *payload, u16_t len){
	if (strcmp(payload, lock) == 0){
		lockDoor = "0";
		PRINTF("%c",lock);
	}
	else if(strcmp(payload, unlock) == 0){
		lockDoor = "1";
		PRINTF("%c",unlock);
	}
	else{
		PRINTF("Unrecognized payload: %s\r\n", payload);
		return;
	}
}
static void fanCtrl(char *payload, u16_t len){
	if (strcmp(payload, level0) == 0){
		levelFan = "0";
		PRINTF("%c",level0);
	}
	else if(strcmp(payload, level1) == 0){
		levelFan = "1";
		PRINTF("%c",level1);
	}
	else if(strcmp(payload, level2) == 0){
		levelFan = "2";
		PRINTF("%c",level2);
	}
	else if(strcmp(payload, level3) == 0){
		levelFan = "3";
		PRINTF("%c",level3);
	}
	else if(strcmp(payload, level4) == 0){
		levelFan = "4";
		PRINTF("%c",level4);
	}
	else if(strcmp(payload, level5) == 0){
		levelFan = "5";
		PRINTF("%c",level5);
	}
	else{
		PRINTF("Unrecognized payload: %s\r\n", payload);
		return;
	}
}

static void callFunctions(void){
	gasCtrl();
	waterCtrl();
	airQuality();
}


