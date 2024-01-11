#include "reliable_communication_receiver.h"

enum reliable_communication_error_t reliable_communication_receiver_initialize(struct reliable_communication_receiver_t *receiver, size_t buffer_size, void *buffer, reliable_communication_new_packet_received_callback callback, reliable_communication_new_packet_received_order_callback order_callback)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    receiver->callback = callback;
    receiver->order_callback = order_callback;

func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_receiver_receive(struct reliable_communication_receiver_t *receiver, void *object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

func_end:
    return func_res;
}
