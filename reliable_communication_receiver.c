#include "reliable_communication_receiver.h"

enum reliable_communication_error_t reliable_communication_receiver_initialize(struct reliable_communication_receiver_t *receiver, size_t buffer_size, void *buffer, reliable_communication_send_response_func_t send_response, reliable_communication_receive_packet_func_t receive_packet, reliable_communication_receive_reset_func_t receive_reset_func, reliable_communication_send_reset_response_func_t send_reset_response_func, reliable_communication_new_packet_received_callback callback, reliable_communication_new_packet_received_order_callback order_callback)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    func_res = reliable_communication_fifo_initialize(&receiver->recorder, buffer_size, buffer);
    if (func_res != reliable_communication_error_no)
    {
        goto func_end;
    }

    receiver->callback = callback;
    receiver->order_callback = order_callback;
    receiver->receive_packet = receive_packet;
    receiver->send_response = send_response;
    receiver->receive_reset_packet = receive_reset_func;
    receiver->send_reset_response = send_reset_response_func;

func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_receiver_receive(struct reliable_communication_receiver_t *receiver, void *object, reliable_communication_yield_condition_func_t yield_condition_func, void *yield_condition_object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    while (1)
    {
        uint32_t index = 0;

        if (yield_condition_func)
        {
            int32_t if_yield = yield_condition_func(yield_condition_object);
            if (if_yield)
            {
                func_res = reliable_communication_error_no;
                break;
            }
        }

        if (receiver->receive_reset_packet)
        {
            func_res = receiver->receive_reset_packet(object);
            if (func_res == reliable_communication_error_got_one_packet)
            {
                func_res = reliable_communication_reset_recorder(&receiver->recorder);
                if (func_res != reliable_communication_error_no)
                {
                    break;
                }
                func_res = receiver->send_reset_response(object);
                if (func_res != reliable_communication_error_no)
                {
                    break;
                }
            }
        }

        enum reliable_communication_error_t err = receiver->receive_packet(&index, object);
        if (err == reliable_communication_error_got_one_packet)
        {
            size_t buffer_size;
            enum reliable_communication_error_t err = reliable_communication_get_buffer_size(&receiver->recorder, &buffer_size);
            if (err != reliable_communication_error_no)
            {
                continue;
            }

            enum reliable_communication_response_t response_data = reliable_communication_response_received;
            if (index < receiver->recorder.first_packet_index)
            {
            }
            else if (index >= (receiver->recorder.first_packet_index + buffer_size))
            {
                response_data = reliable_communication_response_overflow;
            }
            else
            {
				uint32_t record_data;
				err = reliable_communication_get_record(&receiver->recorder, index, &record_data);
                assert(err == reliable_communication_error_no);
                uint32_t have_error = 0;
				if (record_data == reliable_communication_packet_have_not_received)
				{
					if (receiver->callback)
					{
                        enum reliable_communication_callback_result_t callback_result;
						callback_result = receiver->callback(index, object);
                        if (callback_result != reliable_communication_callback_result_ok)
                        {
                            response_data = reliable_communication_response_abort;
                            have_error = 1;
                        }
					}
				}
                if (!have_error)
                {
                    err = reliable_communication_record_received(&receiver->recorder, index);
                    assert(err == reliable_communication_error_no);
                }

                err = reliable_communication_walk(&receiver->recorder, receiver->order_callback, object);
                if (err == reliable_communication_error_callback_execute_failed)
                {
                    response_data = reliable_communication_response_abort;
                }
                else 
                {
                    assert(err == reliable_communication_error_no);
                }
            }
            receiver->send_response(index, response_data, object);
        }
    }

func_end:
    return func_res;
}
