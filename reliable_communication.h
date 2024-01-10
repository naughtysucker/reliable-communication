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
    reliable_communication_error_got_one_response,
};

enum reliable_communication_packet_record_status_t
{
    reliable_communication_packet_have_not_received,
    reliable_communication_packet_received_already,
};

enum reliable_communication_error_t reliable_communication_fifo_initialize(struct reliable_communication_t *ins, size_t buffer_size, void *buffer);
enum reliable_communication_error_t reliable_communication_walk(struct reliable_communication_t *ins);
enum reliable_communication_error_t reliable_communication_record_received(struct reliable_communication_t *ins, uint32_t index);
enum reliable_communication_error_t reliable_communication_get_record(struct reliable_communication_t *ins, uint32_t index, uint32_t **record_data);
enum reliable_communication_error_t reliable_communication_get_buffer_size(struct reliable_communication_t *ins, size_t *buffer_size);

#ifdef __cplusplus
}
#endif
