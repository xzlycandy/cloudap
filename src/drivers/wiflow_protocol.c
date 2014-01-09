/********************************************************************/
/* Copyright (C) SSE-USTC, 2014                                     */
/*                                                                  */
/*  FILE NAME             :  wiflow_protocol.c                      */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  driver_nl80211                         */
/*  MODULE NAME           :  WiFlow                                 */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2014/01/08                             */
/*  DESCRIPTION           :  implement of WiFlow PDU parser         */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2014/01/08 
 *
 */
#include<stdio.h> 			/* perror */
#include<stdlib.h>			/* exit	*/
#include<sys/types.h>		/* WNOHANG */
#include<sys/wait.h>		/* waitpid */
#include<string.h>			/* memset */

#include "common.h"
#include "driver.h"
#include "wiflow_protocol.h"


typedef struct wpa_init_params_pdu{
	void *global_priv; //NOT used,use local global_priv
	u8 bssid;   //ETH_ALEN length
	char ifname;//sizeof(bss->ifname) or char ifname[IFNAMSIZ + 1]; ex.wlan0
	u8 ssid;    //ssid_len
	size_t ssid_len;    
	char *test_socket;//NOT used
	int use_pae_group_addr;//NOT used
	char bridge; //ex. br0
	size_t num_bridge;

	u8 *own_addr; // ETH_ALENlength,buffer for writing own MAC address 
}wpa_init_params_pdu;

int wpa_init_params_parser(char * pdu, int pdu_size,struct wpa_init_params *params)
{
	struct wpa_init_params_pdu *p = (wpa_init_params_pdu *)pdu;
	params->bssid = p->bssid;
	params->ifname = p->ifname;
	params->ssid = p->ssid;
	params->ssid_len = p->ssid_len;
	params->bridge = &(p->bridge);
	params->num_bridge = params->num_bridge;
        return 0;
}

int wpa_init_params_format(char * pdu, int pdu_size,struct wpa_init_params *params)
{
	struct wpa_init_params_pdu *p = (wpa_init_params_pdu *)pdu;
	p->bssid = *(params->bssid);
	p->ifname = *(params->ifname);
	p->ssid = *(params->ssid);
	p->ssid_len = params->ssid_len;
	p->bridge = **(params->bridge);
	p->num_bridge = params->num_bridge;
        return 0;   
}

typedef struct i802_bss_pdu {
//	struct wpa_driver_nl80211_data *drv;//NOT used
	struct i802_bss *next;//multi-record flag
	int ifindex;
	char ifname[IFNAMSIZ + 1];
	char brname[IFNAMSIZ];
	unsigned int beacon_set:1;
	unsigned int added_if_into_bridge:1;
	unsigned int added_bridge:1;
	unsigned int in_deinit:1;

	u8 addr[ETH_ALEN];

	int freq;

//	void *ctx;//NOT used
//	struct nl_handle *nl_preq, *nl_mgmt;//NOT used
//	struct nl_cb *nl_cb;//NOT used

//	struct nl80211_wiphy_data *wiphy_data;//NOT used
//	struct dl_list wiphy_list;//NOT used
}i802_bss_pdu;
int i802_bss_parser(char * pdu, int pdu_size,struct i802_bss *bss)
{
	struct i802_bss_pdu *p = (i802_bss_pdu *)pdu;
	bss->next = p->next;
	bss->ifindex = p->ifindex;
	memcpy(bss->ifname,p->ifname,17*sizeof(char));
	memcpy(bss->brname,p->brname,16*sizeof(char));
	bss->beacon_set = p->beacon_set;
	bss->added_if_into_bridge = p->added_if_into_bridge;
	bss->in_deinit = p->in_deinit;
	memcpy(bss->addr,p->addr,6*sizeof(char));
	bss->freq = p->freq;
    return 0;   
}

int i802_bss_format(char * pdu, int pdu_size,struct i802_bss *bss)
{
	struct i802_bss_pdu *p = (i802_bss_pdu *)pdu;
	//p->next = *(bss->next);
	p->ifindex = bss->ifindex;
        memcpy(p->ifname,bss->ifname,17*sizeof(char));
	memcpy(p->brname,bss->brname,16*sizeof(char));
	p->beacon_set = bss->beacon_set;
	p->added_if_into_bridge = bss->added_if_into_bridge;
	p->in_deinit = bss->in_deinit;
	memcpy(p->addr,bss->addr,6*sizeof(u8));
	p->freq = bss->freq;
    return 0;   
}
