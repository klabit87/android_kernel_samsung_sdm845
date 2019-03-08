#ifndef _UAPI_QBT1000_H_
#define _UAPI_QBT1000_H_

#define MAX_NAME_SIZE					 32

/*
 * enum qbt1000_fw_event -
 *      enumeration of firmware events
 * @FW_EVENT_FINGER_DOWN - finger down detected
 * @FW_EVENT_FINGER_UP - finger up detected
 * @FW_EVENT_INDICATION - an indication IPC from the firmware is pending
 */
enum qbt1000_fw_event {
	FW_EVENT_FINGER_DOWN = 1,
	FW_EVENT_FINGER_UP = 2,
	FW_EVENT_CBGE_REQUIRED = 3,
};

#endif /* _UAPI_QBT1000_H_ */
