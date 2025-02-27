#include "reliable_communication_sender.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum reliable_communication_error_t reliable_communication_sender_reset(struct reliable_communication_sender_t *sender, void *object, reliable_communication_yield_condition_func_t yield_condition_func, void *yield_condition_object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    uint32_t response = 0;

    func_res = reliable_communication_reset_recorder(&sender->recorder);
    if (func_res != reliable_communication_error_no)
    {
        goto func_end;
    }

    while (1)
    {
        if (yield_condition_func)
        {
            int32_t if_yield = yield_condition_func(yield_condition_object);
            if (if_yield)
            {
                func_res = reliable_communication_error_not_completed;
                break;
            }
        }

        enum reliable_communication_error_t reset_send_res = sender->send_reset(object);

        enum reliable_communication_error_t reset_response_res = sender->get_reset_response(&response, object);
        if (reset_response_res == reliable_communication_error_got_one_response)
        {
            break;
        }
    }

func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_sender_initialize(struct reliable_communication_sender_t *sender, size_t buffer_size, void *buffer, reliable_communication_get_receiver_received_response_func_t get_recved_response, reliable_communication_send_packet_func_t send_packet, reliable_communication_send_reset_func_t send_reset_func, reliable_communication_get_receiver_reset_response_func_t get_reset_response_func)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    func_res = reliable_communication_fifo_initialize(&sender->recorder, buffer_size, buffer);
    if (func_res != reliable_communication_error_no)
    {
        goto func_end;
    }

    sender->get_recved_response = get_recved_response;
    sender->send_packet = send_packet;
    sender->send_reset = send_reset_func;
    sender->get_reset_response = get_reset_response_func;

func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_sender_send_packets(struct reliable_communication_sender_t *sender, void *object, reliable_communication_yield_condition_func_t yield_condition_func, void *yield_condition_object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    uint32_t end_index = 0;
    size_t tuned_buffer_size = 0;

    size_t buffer_size;
    enum reliable_communication_error_t err = reliable_communication_get_buffer_size(&sender->recorder, &buffer_size);
    if (err != reliable_communication_error_no)
    {
        goto func_end;
    }

    while (1)
    {
        if (yield_condition_func)
        {
            int32_t if_yield = yield_condition_func(yield_condition_object);
            if (if_yield)
            {
                break;
            }
        }

        if (tuned_buffer_size)
        {
            buffer_size = tuned_buffer_size;
        }
        for (uint32_t i = 0; i < buffer_size; i++)
        {
            uint32_t recved_index;
            uint32_t record_data;
            while (1)
            {
				uint32_t response = 0;
                err = sender->get_recved_response(&recved_index, &response, object);
                if (err == reliable_communication_error_got_one_response)
                {
					if (response == reliable_communication_response_received)
					{
						err = reliable_communication_record_received(&sender->recorder, recved_index);
						if (err != reliable_communication_error_no)
						{
							continue;
						}
					}
					else if (response == reliable_communication_response_overflow)
					{
                        tuned_buffer_size = recved_index - sender->recorder.first_packet_index;
					}
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
            err = reliable_communication_get_record(&sender->recorder, i + sender->recorder.first_packet_index, &record_data);
            assert(err == reliable_communication_error_no);
            if (record_data == reliable_communication_packet_have_not_received)
            {
                err = sender->send_packet(sender->recorder.first_packet_index + i, object);
                if (err == reliable_communication_error_overflow)
                {
                    end_index = sender->recorder.first_packet_index + i;
                    break;
                }
                else if (err != reliable_communication_error_no)
                {
                    func_res = reliable_communication_error_unknown;
                    goto func_end;
                }
            }
        }
        err = reliable_communication_walk(&sender->recorder, NULL, NULL);
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
