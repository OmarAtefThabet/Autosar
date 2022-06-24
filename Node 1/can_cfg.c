#include "can_cfg.h"


//map ports to can peipherals


//Needs to be changed to match port config layout (layered arch)

volatile uint32_t error_type = 0;
volatile bool errFlag=0; //error flag for any errors in the CAN bus
tCANMsgObject TXmsg; //message object for the sent message
tCANMsgObject RXmsg; //message object for the recieved message
//message data of maximum size 8 bytes (64 bits) as per CAn protocol
unsigned char TXmsg_Data[8]; 
unsigned char RXmsg_Data[8];
volatile bool rxFlag=0;

// CAN interrupt handler
void CANIntHandler(void) {
	// read interrupt status
	unsigned long status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE); 
	// controller status interrupt
	if(status == CAN_INT_INTID_STATUS) { 
		status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
		errFlag = 1;
		error_type |= status ; 
	} 
	// msg object 1
	else if(status == RXOBJECT) { 
		// clear interrupt
		CANIntClear(CAN0_BASE, RXOBJECT);
		// set rx flag
		rxFlag = 1; 
		// clear any error flags	
		errFlag = 0; 
	}

	else if(status == TXOBJECT) { 
		// clear interrupt
		CANIntClear(CAN0_BASE, TXOBJECT);
		// clear any error flags	
		errFlag = 0; 
	}
}
void CAN_Init(void){
	// Set up CAN0
	Port_ConfigType CAN_Config;
	CAN_Config.PinMode = Port_PinMode_CAN;
	CAN_Config.Pin = 9;	//Add any channel in the GPIO Port needed Eg: Port B --> Channels (8 - 15)
	Port_Init(&CAN_Config);
	CANIntRegister(CAN0_BASE, CANIntHandler);
	CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
	IntEnable(INT_CAN0);
}

//encrypt the message, set the message object
//to the encrypted data andn send iver the can bus
void CAN_Send(void){
	
	// Set up msg object
	TXmsg.pui8MsgData = (uint8_t*)&TXmsg_Data;

	// send as msg object 1
	CANMessageSet(CAN0_BASE, TXOBJECT, &TXmsg, MSG_OBJ_TYPE_TX); 
}

//if the can flag is raised receive the message
//decrypt the message and display it on pc over uart
void CAN_recieve(void){
			if(rxFlag && !errFlag){ 			
				 // set pointer to rx buffer
				RXmsg.pui8MsgData = RXmsg_Data ; 
				// read CAN message object 1 from CAN peripheral
				CANMessageGet(CAN0_BASE, RXOBJECT, &RXmsg, 0); 
				// clear rx flag
				Error_Handling();	
				rxFlag = 0; 
				}
}

void sendingDone(void){
	unsigned long status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
	while(status != CAN_STS_TXOK);
}
// handles the errors than can occur
// outputs a message to the user with the error type
void Error_Handling (void){
		
	if (errFlag){
	  // bus off error occurred.
		if(error_type & CAN_STATUS_BUS_OFF){
		  	error_type &= ~(CAN_STATUS_BUS_OFF);
    }

    // bit stuffing error occurred.
    else if(error_type & CAN_STATUS_LEC_STUFF){
        error_type &= ~(CAN_STATUS_LEC_STUFF);
    }
	 // other error occurred
		else{
				// Error - Check the Cable
		}
	}

}