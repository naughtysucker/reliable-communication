#pragma once

#include "reliable_communication.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*reliable_communication_new_packet_received_callback)(uint32_t index, void *object);
typedef void (*reliable_communication_new_packet_received_order_callback)(uint32_t index, void *object);

struct reliable_communication_receiver_t
{
    struct reliable_communication_t recorder;
    reliable_communication_new_packet_received_callback callback;
    reliable_communication_new_packet_received_order_callback order_callback;
};

#ifdef __cplusplus
}
#endif
