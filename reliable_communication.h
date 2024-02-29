#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "naughty_fifo.h"

struct reliable_communication_t
{
    uint32_t first_packet_index;
    struct naughty_fifo fifo;
};

enum reliable_communication_error_t
{
    reliable_communication_error_no,
    reliable_communication_error_unknown,
    reliable_communication_error_overflow,
    reliable_communication_error_received_before,
    reliable_communication_error_got_one_packet,
    reliable_communication_error_got_one_response,
    reliable_communication_error_end,
    reliable_communication_error_not_completed,
};

enum reliable_communication_packet_record_status_t
{
    reliable_communication_packet_have_not_received,
    reliable_communication_packet_received_already,
};

enum reliable_communication_response_t
{
    reliable_communication_response_received,
    reliable_communication_response_overflow,
};

typedef void (*reliable_communication_new_packet_received_callback)(uint32_t index, void *object);
typedef void (*reliable_communication_new_packet_received_order_callback)(uint32_t index, void *object);

enum reliable_communication_error_t reliable_communication_fifo_initialize(struct reliable_communication_t *ins, size_t buffer_size, void *buffer);
enum reliable_communication_error_t reliable_communication_walk(struct reliable_communication_t *ins, reliable_communication_new_packet_received_order_callback order_callback, void *object);
enum reliable_communication_error_t reliable_communication_record_received(struct reliable_communication_t *ins, uint32_t index);
enum reliable_communication_error_t reliable_communication_get_record(struct reliable_communication_t *ins, uint32_t index, uint32_t *record_data);
enum reliable_communication_error_t reliable_communication_get_buffer_size(struct reliable_communication_t *ins, size_t *buffer_size);
enum reliable_communication_error_t reliable_communication_reset_recorder(struct reliable_communication_t *recorder);

typedef int32_t (*reliable_communication_yield_condition_func_t)(void *object);

#ifdef __cplusplus
}
#endif
