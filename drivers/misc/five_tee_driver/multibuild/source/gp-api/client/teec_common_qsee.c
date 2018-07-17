/**
 * @file teec_common_qsee.c
 * @brief Common client interface implementation based on qsee API
 * @author Iaroslav Makarchuk (i.makarchuk@samsung.com)
 * @date Created Oct 3, 2016
 * @par In Samsung Ukraine R&D Center (SURC) under a contract between
 * @par LLC "Samsung Electronics Ukraine Company" (Kiev, Ukraine) and
 * @par "Samsung Elecrtronics Co", Ltd (Seoul, Republic of Korea)
 * @par Copyright: (c) Samsung Electronics Co, Ltd 2015. All rights reserved.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied
 * or distributed, transmitted, transcribed, stored in a retrieval system
 * or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */

#include <linux/string.h>
#include <linux/bug.h>

#include "teec_common.h"

#include "protocol.h"
#include "qseecom_kernel.h"
#include "teec_param_utils.h"
#include "linux/errno.h"

#define MAX_QSEE_UUID_LEN 17
#define QSEE_SUCCESS      0

/* 1. Actually we don't use the response buffer. But RBUF_LEN can't be zero.
 * 2. The size which is passed to qseecom_start_app must be RBUF_LEN + SBUF_LEN.
 * 3. There is difference between user space and kernel QSEECOM_ALIGN
 *    implementation. User space one always adds 0x40 to the passed size if
 *    it's already aligned by 0x40 but kernel macro doesn't do it.
 *    QSEECOM_ALIGN_SIZE is 0x40. That's why we add QSEECOM_ALIGN_SIZE
 *    in RBUF_LEN.
 */
#define SBUF_LEN (sizeof(ProtocolCmd) + TEEC_CONFIG_SHAREDMEM_MAX_SIZE)
#define RBUF_LEN (QSEECOM_ALIGN(SBUF_LEN) - SBUF_LEN + QSEECOM_ALIGN_SIZE)

typedef struct QseeSessionStruct {
	struct qseecom_handle *qsee_com_handle;
	struct qseecom_handle *listener_handle;
	void *parent_context;
	TEEC_UUID uuid;
} QseeSession;

static TEEC_Result TeecUuidToQseeUuid(const TEEC_UUID *uuid, char *qsee_uuid) {
	uint32_t i = 0;
	uint32_t j = 0;
	TEEC_Result res = TEEC_SUCCESS;

	if (!uuid || !qsee_uuid) {
		res = TEEC_ERROR_BAD_PARAMETERS;
		goto exit;
	}

	qsee_uuid[i++] = (uuid->timeLow & 0xFF000000) >> 24;
	qsee_uuid[i++] = (uuid->timeLow & 0xFF0000) >> 16;
	qsee_uuid[i++] = (uuid->timeLow & 0xFF00) >> 8;
	qsee_uuid[i++] = (uuid->timeLow & 0xFF);

	qsee_uuid[i++] = (uuid->timeMid & 0xFF00) >> 8;
	qsee_uuid[i++] = (uuid->timeMid & 0xFF);

	qsee_uuid[i++] = (uuid->timeHiAndVersion & 0xFF00) >> 8;
	qsee_uuid[i++] = (uuid->timeHiAndVersion & 0xFF);

	for (j = 0; j < sizeof(uuid->clockSeqAndNode); j++) {
		qsee_uuid[i++] = uuid->clockSeqAndNode[j];
	}

exit:
	return res;
}

TEEC_Result QseeUnloadTA(void *ta_session)
{
	TEEC_Result status = TEEC_ERROR_GENERIC;
	int qsee_res;
	QseeSession *qsee_session = (QseeSession *)ta_session;

	if (qsee_session->qsee_com_handle == NULL)
		goto exit;

	qsee_res = qseecom_shutdown_app(&qsee_session->qsee_com_handle);
	if (qsee_res != QSEE_SUCCESS)
		goto exit;

	qsee_session->qsee_com_handle = NULL;

	status = TEEC_SUCCESS;
exit:
	return status;
}

TEEC_Result QseeLoadTA(void *ta_session, const TEEC_UUID *uuid)
{
	TEEC_Result status = TEEC_SUCCESS;
	int qsee_res;
	char qsee_uuid[MAX_QSEE_UUID_LEN] = {0};
	QseeSession *qsee_session = (QseeSession *)ta_session;

	if (!ta_session || !uuid ||
		TEEC_SUCCESS != TeecUuidToQseeUuid(uuid, qsee_uuid)) {
		status = TEEC_ERROR_BAD_PARAMETERS;
		goto exit;
	}

	memcpy(&qsee_session->uuid, uuid, sizeof(TEEC_UUID));

	BUILD_BUG_ON((SBUF_LEN + RBUF_LEN) & QSEECOM_ALIGN_MASK);

	qsee_res = qseecom_start_app(&qsee_session->qsee_com_handle,
				 qsee_uuid,
				 SBUF_LEN + RBUF_LEN);
	if (qsee_res != QSEE_SUCCESS) {
		status = qsee_res == -EINVAL ? TEEC_ERROR_TARGET_DEAD :
							    TEEC_ERROR_GENERIC;
	}

exit:
	return status;
}

TEEC_Result QseeSendCommand(void *ta_session,
			    uint32_t command_id,
			    TEEC_Operation *operation,
			    uint32_t *return_origin)
{
	int32_t qsee_res = QSEE_SUCCESS;
	QseeSession *qsee_session = (QseeSession *)ta_session;
	ProtocolCmd *command = NULL;
	TEEC_Result ret = TEEC_ERROR_BAD_PARAMETERS;

	if (!qsee_session)
		goto exit;

	command = (ProtocolCmd *)qsee_session->qsee_com_handle->sbuf;

	/*
	 * Return origin: "COMMS" by default because of possible
	 * communication errors.
	 */
	command->return_origin = TEEC_ORIGIN_COMMS;

	command->cmd_id = command_id;
	command->param_types = 0;

	if (operation) {
		command->param_types = operation->paramTypes;
		ret = FillCommandArgs(command, operation, ta_session);

		if (ret != TEEC_SUCCESS) {
			command->return_origin = TEEC_ORIGIN_API;
			goto exit;
		}
	}

	/* Call */
	qsee_res = qseecom_set_bandwidth(qsee_session->qsee_com_handle, true);

	if (qsee_res != QSEE_SUCCESS) {
		ret = TEEC_ERROR_COMMUNICATION;
		goto exit;
	}

	/* Prepare for silent TA death */
	command->cmd_ret = TEEC_ERROR_TARGET_DEAD;

	/* Send command to TA
	 * Passing zero as response size isn't allowed
	 * that's why it's used QSEECOM_ALIGN_SIZE instead
	 */
	qsee_res = qseecom_send_command(qsee_session->qsee_com_handle,
			   qsee_session->qsee_com_handle->sbuf,
			   SBUF_LEN,
			   qsee_session->qsee_com_handle->sbuf,
			   RBUF_LEN);

	if (qsee_res != QSEE_SUCCESS) {
		ret = qsee_res == -EINVAL ?  TEEC_ERROR_TARGET_DEAD :
						      TEEC_ERROR_COMMUNICATION;
		goto exit;
	}

	qsee_res = qseecom_set_bandwidth(qsee_session->qsee_com_handle, false);

	if (qsee_res != QSEE_SUCCESS) {
		ret = (qsee_res == -EINVAL ? TEEC_ERROR_TARGET_DEAD :
						    TEEC_ERROR_COMMUNICATION);
		goto exit;
	}

	/*
	 * Return origin:
	 * "TEE" or "TRUSTED_APP" after communication with TEE.
	 */
	ret = command->cmd_ret;

	if (ret == TEEC_SUCCESS && operation) {
		ret = FillOperationArgs(operation, command);
		if (ret != TEEC_SUCCESS)
			command->return_origin = TEEC_ORIGIN_API;
	}

exit:
	if (return_origin && command)
		*return_origin = command->return_origin;

	return ret;
}

PlatformClientImpl g_qsee_platform_impl = {
	QseeLoadTA,
	QseeUnloadTA,
	QseeSendCommand,
	NULL,
	NULL,
	NULL,
	NULL,
};

PlatformClientImpl *GetPlatformClientImpl(void)
{
	return &g_qsee_platform_impl;
}

size_t GetPlatformClientImplSize(void)
{
	return sizeof(QseeSession);
}

void ClientImplSetParentContext(void *session, void *context)
{
	QseeSession *qsee_session = (QseeSession *)session;

	if (qsee_session)
		qsee_session->parent_context = context;
}

void *ClientImplGetParentContext(void *session)
{
	void *ret = NULL;
	QseeSession *qsee_session = (QseeSession *)session;

	if (qsee_session)
		ret = qsee_session->parent_context;

	return ret;
}
