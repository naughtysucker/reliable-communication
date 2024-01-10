#include "reliable_communication_sender.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum reliable_communication_error_t reliable_communication_sender_initialize(struct reliable_communication_sender_t *sender, size_t buffer_size, void *buffer, reliable_communication_get_receiver_received_response_func_t get_recved_response, reliable_communication_send_packet_func_t send_packet)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    func_res = reliable_communication_fifo_initialize(&sender->recorder, buffer_size, buffer);
    if (func_res != reliable_communication_error_no)
    {
        goto func_end;
    }

    sender->get_recved_response = get_recved_response;
    sender->send_packet = send_packet;

func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_sender_send_packets(struct reliable_communication_sender_t *sender, void *object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    void *packet = NULL;
    size_t packet_size = 0;
    uint32_t end_index = 0;

    while (1)
    {
        size_t buffer_size;
        enum reliable_communication_error_t err = reliable_communication_get_buffer_size(&sender->recorder, &buffer_size);
        if (err != reliable_communication_error_no)
        {
            continue;
        }
        for (uint32_t i = 0; i < buffer_size; i++)
        {
            uint32_t recved_index;
            uint32_t record_data;
            while (1)
            {
                err = sender->get_recved_response(&recved_index, object);
                if (err == reliable_communication_error_got_one_response)
                {
                    err = reliable_communication_record_received(&sender->recorder, recved_index);
                }
                else if (err != reliable_communication_error_no)
                {
                    func_res = reliable_communication_error_unknown;
                    goto func_end;
                }
                else
                {
                    break;
                }
            }
            err = reliable_communication_get_record(&sender->recorder, i, &record_data);
            assert(err == reliable_communication_error_no);
            if (record_data == reliable_communication_packet_have_not_received)
            {
                err = sender->send_packet(sender->recorder.first_packet_index + i, object);
                if (err == reliable_communication_error_overflow)
                {
                    end_index = i;
                    break;
                }
                else if (err != reliable_communication_error_no)
                {
                    func_res = reliable_communication_error_unknown;
                    goto func_end;
                }
            }
        }
        err = reliable_communication_walk(&sender->recorder);
        if (end_index)
        {
            if (sender->recorder.first_packet_index == end_index)
            {
                break;
            }
        }
        // Delay Begin
        // Delay End
    }
func_end:
    return func_res;
}

#ifdef __cplusplus
}
#endif
