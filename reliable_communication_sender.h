#pragma once

#include "reliable_communication.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum reliable_communication_error_t (*reliable_communication_get_receiver_received_response_func_t)(uint32_t *index, uint32_t *response, void *object);
typedef enum reliable_communication_error_t (*reliable_communication_send_packet_func_t)(uint32_t index, void *object);

struct reliable_communication_sender_t
{
    struct reliable_communication_t recorder;
    reliable_communication_get_receiver_received_response_func_t get_recved_response;
    reliable_communication_send_packet_func_t send_packet;
};

enum reliable_communication_error_t reliable_communication_sender_initialize(struct reliable_communication_sender_t *sender, size_t buffer_size, void *buffer, reliable_communication_get_receiver_received_response_func_t get_recved_response, reliable_communication_send_packet_func_t send_packet);
enum reliable_communication_error_t reliable_communication_sender_send_packets(struct reliable_communication_sender_t *sender, void *object, reliable_communication_yield_condition_func_t yield_condition_func, void *yield_condition_object);

#ifdef __cplusplus
}
#endif
