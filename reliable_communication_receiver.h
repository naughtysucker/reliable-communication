#pragma once

#include "reliable_communication.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum reliable_communication_error_t (*reliable_communication_receive_reset_func_t)(void *object);
typedef enum reliable_communication_error_t (*reliable_communication_send_reset_response_func_t)(void *object);

typedef enum reliable_communication_error_t (*reliable_communication_receive_packet_func_t)(uint32_t *index, void *object);
typedef enum reliable_communication_error_t (*reliable_communication_send_response_func_t)(uint32_t index, enum reliable_communication_response_t response_data, void *object);

struct reliable_communication_receiver_t
{
    struct reliable_communication_t recorder;
    reliable_communication_new_packet_received_callback callback;
    reliable_communication_new_packet_received_order_callback order_callback;
    reliable_communication_receive_packet_func_t receive_packet;
    reliable_communication_send_response_func_t send_response;
    reliable_communication_receive_reset_func_t receive_reset_packet;
    reliable_communication_send_reset_response_func_t send_reset_response;
};

enum reliable_communication_error_t reliable_communication_receiver_initialize(struct reliable_communication_receiver_t *receiver, size_t buffer_size, void *buffer, reliable_communication_send_response_func_t send_response, reliable_communication_receive_packet_func_t receive_packet, reliable_communication_receive_reset_func_t receive_reset_func, reliable_communication_send_reset_response_func_t send_reset_response_func, reliable_communication_new_packet_received_callback callback, reliable_communication_new_packet_received_order_callback order_callback);

enum reliable_communication_error_t reliable_communication_receiver_receive(struct reliable_communication_receiver_t *receiver, void *object, reliable_communication_yield_condition_func_t yield_condition_func, void *yield_condition_object);

#ifdef __cplusplus
}
#endif
