#include "netTopoAutoCatch.h"

//初始化路由器
int init_router(ROUTER **pRouter) {

	*pRouter = NULL;
	*pRouter = (ROUTER *)malloc(sizeof(ROUTER));
	
	if(*pRouter == NULL) {
		printf("[ERROR] malloc the router pointer failed\n");
		return 1;
	}
	(*pRouter)->routerID = 0;
	(*pRouter)->routerName = NULL;
	(*pRouter)->flagIPAddr = NULL;
	(*pRouter)->ipAddrTable = NULL;
	(*pRouter)->subnetVector = NULL;
	(*pRouter)->processFlag = UNDO_ROUTER_FLAG;
	(*pRouter)->ipAddrTableNum = 0;
	(*pRouter)->subnetVectorNum = 0;
	
	return 0;
}

//初始化路由器队列
void init_router_queue(ROUTER *pRouterQueue[], unsigned router_queue_len) {
	unsigned int count;
	
	for(count = 0; count < router_queue_len; count++) {
		pRouterQueue[count] = NULL;
	}
}

//判断路由器的进程标签是否被标记processFlag == UNDO_ROUTER_FLAG，并找出该路由器在队列中的位置
int judage_undo_router_flag(ROUTER *pRouterQueue[], unsigned router_queue_len, unsigned int *position) {
	unsigned int count;
	unsigned int flag = 0;
	
	for(count = 0; count < router_queue_len; count++) {
		if(pRouterQueue[count]->processFlag == UNDO_ROUTER_FLAG) {//UNDO_ROUTER_FLAG=0
			flag = 1;
			*position = count;
			break;
		}
	}
	
	return flag;
}

//删除无效的路由表，return  count  ，如果路由器的ipRouteTableType不符合则删除
int delete_invalid_router_table_info(IP_ROUTE_TABLE_INFO *curRouteTableInfo, unsigned int *curRouterTableNum) {
	unsigned int count;
	IP_ROUTE_TABLE_INFO tmp[MAX_MY_OID_RESULT_LEN];
	
	for(count = 0; count < *curRouterTableNum; count++) {
		if(curRouteTableInfo[count].ipRouteTableType != 3 && curRouteTableInfo[count].ipRouteTableType != 4) {
			
		}
	}
	
}

//判断ip是否在子网内，subMask子网掩码
int judage_ip_in_subnet(SUBNET *pSubNer, unsigned int pSubNerNum, unsigned char *ip, unsigned char *subMask) {
	unsigned int count;
	
	for(count = 0; count < pSubNerNum; count++) {
		if(!strcmp(pSubNer[count].sunnetIP, ip) && !strcmp(pSubNer[count].subnetMask, subMask)) {
			return 1;
		}
	}
	return 0;
}
//判断某个ip的路由器是否在路由器队列中
int judage_router_ip_in_router_queue(ROUTER *pRouter[], unsigned int pRouterNum, unsigned char *ip) {
	unsigned int count;
	
	for(count = 0; count < pRouterNum; count++) {
		if(!strcmp(pRouter[count]->flagIPAddr, ip)) {
			return 1;
		}
	}
	return 0;
}

int judage_ip_router_in_connect_vector(ROUTER_CONNECT *pRouterConnectVector[],
									   const unsigned int routerConnectVectorLen,
									   const char currentRouterIp[],
									   const char routerNextHopIp[]) {
	unsigned int count = 0;
	
	for(count = 0; count < routerConnectVectorLen; count++) {
		if(!strncmp(pRouterConnectVector[count]->localRaddr, currentRouterIp, 16) && 
		   !strncmp(pRouterConnectVector[count]->peerRAddr, routerNextHopIp, 16)) {
			return 1;
		}
		
		if(!strncmp(pRouterConnectVector[count]->localRaddr, routerNextHopIp, 16) && 
		   !strncmp(pRouterConnectVector[count]->peerRAddr, currentRouterIp, 16)) {
			return 1;
		}
	}
	return 0;
}



void free_router_queue(ROUTER *pRouterQueue[], const unsigned int routerQueueLen) {
	unsigned int count;
	
	for(count = 0; count < routerQueueLen; count++) {
		if(pRouterQueue[count]->routerName != NULL) {
			free(pRouterQueue[count]->routerName);
		}
		if(pRouterQueue[count]->flagIPAddr != NULL) {
			free(pRouterQueue[count]->flagIPAddr);
		}
		
		if(pRouterQueue[count]->subnetVector != NULL) {
			free(pRouterQueue[count]->subnetVector);
		}

		free(pRouterQueue[count]);
	}
}

void free_router_connect_queue(ROUTER_CONNECT *pRouterConnectVector[], const unsigned int routerConnectVectorLen) {
	unsigned int count;
	
	for(count = 0; count < routerConnectVectorLen; count++) {
		if(pRouterConnectVector[count] != NULL) {
			free(pRouterConnectVector[count]);
		}
	}
}


int network_topo_auto_catch(const unsigned char *initIP, const unsigned int initRouterID) {


	char comm[32];

	ROUTER *pRouter;
	ROUTER *pCurrentRouter;
	ROUTER *pRouterQueue[MAX_ROUTE_QUEUE_LEN];
	ROUTER_CONNECT *pRouterConnectVector[MAX_ROUTE_CONNECT_LEN];
	ROUTER_CONNECT *pRouterConnect;
	
	
	
	unsigned int count;
	unsigned int curPosition = 0;
	unsigned int routerQueueLen = 0;
	unsigned int routerConnectVectorLen = 0;
	unsigned int curRouterID = initRouterID;
	unsigned int curRouteTableNum = 0;
	unsigned int ipForWardingFlag = 0;
	IP_ROUTE_TABLE_INFO *curRouteTableInfo = (IP_ROUTE_TABLE_INFO *)malloc(sizeof(IP_ROUTE_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
	
	init_router_queue(pRouterQueue, MAX_ROUTE_QUEUE_LEN);
	init_router(&pRouter);
	
	pRouter->routerID = curRouterID++;
	pRouter->flagIPAddr = (char *)malloc(sizeof(char) * MAX_IP_ADDRESS_LEN);
	strncpy(pRouter->flagIPAddr, initIP, MAX_IP_ADDRESS_LEN);
	pRouter->processFlag = UNDO_ROUTER_FLAG;
	
	pRouterQueue[routerQueueLen++] = pRouter;
	/*connect to database*/
	MYSQL mysql;
	Status conn_db=Database_Connect(&mysql);
	while(judage_undo_router_flag(pRouterQueue, routerQueueLen, &curPosition) && (routerQueueLen <= MAX_ROUTE_QUEUE_LEN)) {
	

		pCurrentRouter = pRouterQueue[curPosition];
		memset(comm, 0, 32);
		printf("[DEBUG] Please Input The %s Route Community: ", pCurrentRouter->flagIPAddr);
		scanf("%s", comm);
		printf("[DEBUG] You Input The Community: %s, Start Get Info By Snmp\n", comm);
	
		/* get the router name info by snmp */
		pCurrentRouter->routerName = (char *)malloc(sizeof(char) * MAX_ROUTER_NAME_LEN);
		memset(pCurrentRouter->routerName, 0, MAX_ROUTER_NAME_LEN);
		if(get_ip_route_name_from_device_by_snmp(pCurrentRouter->flagIPAddr, comm, pCurrentRouter->routerName) != 0) {
			printf("[ERROR] the %s router get router name failed, please check it\n", pCurrentRouter->flagIPAddr);
			pCurrentRouter->processFlag = DONE_ROUTER_FLAG;
			continue;
		}
		
		printf("router name: %s\n", pCurrentRouter->routerName);
	


		/* get the router ipForWardingFlag value */
		ipForWardingFlag = 0;
		if(get_ip_forwarding_info_from_device_by_snmp(pCurrentRouter->flagIPAddr, comm, &ipForWardingFlag) != 0) {
			printf("[ERROR] the %s router get router ipForWardingFlag failed, please check it\n", pCurrentRouter->flagIPAddr);
			pCurrentRouter->processFlag = DONE_ROUTER_FLAG;
			continue;
		}	
		
		if(ipForWardingFlag != 1) {
			pCurrentRouter->processFlag = DONE_ROUTER_FLAG;
			continue;
		}
	
		/* get the router ipAddrTable info by snmp */
		pCurrentRouter->ipAddrTable = (IP_ADDR_TABLE_INFO *)malloc(sizeof(IP_ADDR_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
		memset(pCurrentRouter->ipAddrTable, 0, sizeof(IP_ADDR_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
		if(get_ip_addr_table_info_from_device_by_snmp(pCurrentRouter->flagIPAddr, comm, pCurrentRouter->ipAddrTable, &(pCurrentRouter->ipAddrTableNum)) != 0 ) {
			printf("[ERROR] the %s router get the address table failed, please check it\n", pCurrentRouter->flagIPAddr);
			pCurrentRouter->processFlag = DONE_ROUTER_FLAG;
			continue;
		}
	
		
		printf("entip           entifindex      entmask\n");
		for(count = 0; count < pRouterQueue[curPosition]->ipAddrTableNum; count++) {
			printf("%-16s%-16d%-16s\n", pRouterQueue[curPosition]->ipAddrTable[count].ipAddrTableEntIp,
									    pRouterQueue[curPosition]->ipAddrTable[count].ipAddrTableEntIfIndex,
									    pRouterQueue[curPosition]->ipAddrTable[count].ipAddrTableEntMask);
		}
	


		/* get the router ip router table info by snmp */
		memset(curRouteTableInfo, 0, sizeof(IP_ROUTE_TABLE_INFO) * MAX_MY_OID_RESULT_LEN);
		if(get_ip_route_table_info_from_device_by_snmp(pCurrentRouter->flagIPAddr, comm, curRouteTableInfo, &curRouteTableNum) != 0 ) {
			printf("[DEBUG] the %s router get the router table failed, please check it\n", pCurrentRouter->flagIPAddr);
			pCurrentRouter->processFlag = DONE_ROUTER_FLAG;
			continue;
		}
	
	
		printf("dest            nexthop         mask            ifindex type\n");
		for(count = 0; count < curRouteTableNum; count++) {
			printf("%-16s%-16s%-16s%-8d%-8d\n", curRouteTableInfo[count].ipRouteTableDest,
												curRouteTableInfo[count].ipRouteTableNextHop,
												curRouteTableInfo[count].ipRouteTableMask,
												curRouteTableInfo[count].ipRouteTableIfIndex,
												curRouteTableInfo[count].ipRouteTableType);
		}
		
	
		/* start to compare */
		/* init subnet address */
		pCurrentRouter->subnetVector = (SUBNET *)malloc(sizeof(SUBNET) * MAX_ROUTER_NAME_LEN);
		pCurrentRouter->subnetVectorNum = 0;
		for(count = 0; count < curRouteTableNum; count++) {
		
			if(curRouteTableInfo[count].ipRouteTableType == 3) {
				if(judage_ip_in_subnet(pCurrentRouter->subnetVector, 
									   pCurrentRouter->subnetVectorNum, 
									   curRouteTableInfo[count].ipRouteTableDest,
									   curRouteTableInfo[count].ipRouteTableMask) == 0) {
					pCurrentRouter->subnetVector[pCurrentRouter->subnetVectorNum].subnetIfIndex = 
													curRouteTableInfo[count].ipRouteTableIfIndex;
					strncpy(pCurrentRouter->subnetVector[pCurrentRouter->subnetVectorNum].sunnetIP, 
													curRouteTableInfo[count].ipRouteTableDest, MAX_IP_ADDRESS_LEN);
					strncpy(pCurrentRouter->subnetVector[pCurrentRouter->subnetVectorNum].subnetMask, 
													curRouteTableInfo[count].ipRouteTableMask, MAX_IP_ADDRESS_LEN);
					pCurrentRouter->subnetVector[pCurrentRouter->subnetVectorNum].hostVector = NULL;
					pCurrentRouter->subnetVector[pCurrentRouter->subnetVectorNum].hostVectorNum = 0;
					pCurrentRouter->subnetVectorNum++;

				}
			}
		
			if(curRouteTableInfo[count].ipRouteTableType == 4) {
			
				if(judage_router_ip_in_router_queue(pRouterQueue, 
													routerQueueLen, 
													curRouteTableInfo[count].ipRouteTableNextHop) == 1) {
					continue;
				}
			
				/* add a new route into the queue */
				init_router(&pRouter);
				pRouter->routerID = curRouterID++;
				pRouter->flagIPAddr = (char *)malloc(sizeof(char) * MAX_IP_ADDRESS_LEN);
				strncpy(pRouter->flagIPAddr, curRouteTableInfo[count].ipRouteTableNextHop, MAX_IP_ADDRESS_LEN);
				pRouter->processFlag = UNDO_ROUTER_FLAG;
				pRouterQueue[routerQueueLen++] = pRouter;
				
				/* add router to the router_connect structure*/
				if(judage_ip_router_in_connect_vector(pRouterConnectVector,
													  routerConnectVectorLen,
													  pCurrentRouter->flagIPAddr, 
													  pRouter->flagIPAddr) == 1) {
					continue;
				}
				pRouterConnect = (ROUTER_CONNECT *)malloc(sizeof(ROUTER_CONNECT));
				
				pRouterConnect->localRID = pCurrentRouter->routerID;
				strncpy(pRouterConnect->localRaddr, pCurrentRouter->flagIPAddr, 16);
				pRouterConnect->peerRID = pRouter->routerID;
				strncpy(pRouterConnect->peerRAddr, pRouter->flagIPAddr, 16);
				
				pRouterConnectVector[routerConnectVectorLen++] = pRouterConnect;
			}
			
		}	
	
		pCurrentRouter->processFlag = DONE_ROUTER_FLAG;
	}
	
	
	int sift;
	for(count = 0; count < routerQueueLen; count++) {			
		/*insert pCurrentRouter into Table Router*/
		printf("routerID: %d\n", pRouterQueue[count]->routerID);	
		Status ins_Rou = Router_Info_Table_Insert(&mysql, pRouterQueue[count]->routerID, pRouterQueue[count]->routerName, 
			pRouterQueue[count]->flagIPAddr, pRouterQueue[count]->ipAddrTableNum,  pRouterQueue[count]->subnetVectorNum );
	
		printf("11111\n");	
		/*insert pCurrentRouter into Table ipAddrTable*/
		unsigned int addr_count;
		for(addr_count = 0; addr_count < pRouterQueue[count]->ipAddrTableNum; addr_count++) {
		
			Status ins_pCurr_IpAddr = IpAddrTable_Info_Table_Insert(&mysql, pRouterQueue[count]->routerID, 
				pRouterQueue[count]->ipAddrTable[addr_count].ipAddrTableEntIfIndex, 
				pRouterQueue[count]->ipAddrTable[addr_count].ipAddrTableEntIp, 
				pRouterQueue[count]->ipAddrTable[addr_count].ipAddrTableEntMask );

		}
		
		printf("22222\n"); 

		unsigned int count_sub;
		/*insert into Subnet*/
		printf("subnetVectorNum: %d\n", pRouterQueue[count]->subnetVectorNum);
		for(count_sub = 0; count_sub < pRouterQueue[count]->subnetVectorNum; count_sub++) {
			sift = strncmp(pRouterQueue[count]->subnetVector[count_sub].sunnetIP,"127",3);
			if(sift != 0)
			{
				Status ins_Sub = Subnet_Info_Table_Insert(&mysql, pRouterQueue[count]->routerID, 
					pRouterQueue[count]->subnetVector[count_sub].subnetIfIndex,
					pRouterQueue[count]->subnetVector[count_sub].sunnetIP, 
					pRouterQueue[count]->subnetVector[count_sub].subnetMask,  0);


			printf("%d %d %s %s\n",pRouterQueue[count]->routerID,pRouterQueue[count]->subnetVector[count_sub].subnetIfIndex,pRouterQueue[count]->subnetVector[count_sub].sunnetIP,pRouterQueue[count]->subnetVector[count_sub].subnetMask);
			}		
		}
	}
	/*insert into Table RouterConnect*/
	printf("routerConnectVectorLen: %d\n", routerConnectVectorLen);
	for(count = 0; count < routerConnectVectorLen; count++) {
		Status ins_RouConn = RouterConnect_Info_Table_Insert(&mysql, pRouterConnectVector[count]->localRID, 
			pRouterConnectVector[count]->peerRID, pRouterConnectVector[count]->localRaddr, 
			pRouterConnectVector[count]->peerRAddr);

		printf("%d %d %s %s\n",pRouterConnectVector[count]->localRID,pRouterConnectVector[count]->peerRID,pRouterConnectVector[count]->localRaddr,pRouterConnectVector[count]->peerRAddr);
	}
	
	free_router_queue(pRouterQueue, routerQueueLen);
	free_router_connect_queue(pRouterConnectVector, routerConnectVectorLen);
}



int main(int argc, char *argv[]) {
	
	if(argc < 2) {
		printf("[DEBUG] Please Input The IP and Community\n");
		return 1;
	}

	network_topo_auto_catch(argv[1], 1);
	return 0;
}
