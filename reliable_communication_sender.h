#pragma once

#include "reliable_communication.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum reliable_communication_error_t (*reliable_communication_get_receiver_received_response_func_t)(uint32_t *index, void *object);
typedef enum reliable_communication_error_t (*reliable_communication_send_packet_func_t)(uint32_t index, void *object);

struct reliable_communication_sender_t
{
    struct reliable_communication_t recorder;
    reliable_communication_get_receiver_received_response_func_t get_recved_response;
    reliable_communication_send_packet_func_t send_packet;
};

#ifdef __cplusplus
}
#endif
