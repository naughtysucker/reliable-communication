#include "reliable_communication_receiver.h"

enum reliable_communication_error_t reliable_communication_receiver_initialize(struct reliable_communication_receiver_t *receiver, size_t buffer_size, void *buffer, reliable_communication_send_response_func_t send_response, reliable_communication_receive_packet_func_t receive_packet, reliable_communication_new_packet_received_callback callback, reliable_communication_new_packet_received_order_callback order_callback)
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

func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_receiver_receive(struct reliable_communication_receiver_t *receiver, void *object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;

    while (1)
    {
        uint32_t index = 0;
        enum reliable_communication_error_t err = receiver->receive_packet(&index, object);
        if (err == reliable_communication_error_got_one_packet)
        {
            size_t buffer_size;
            enum reliable_communication_error_t err = reliable_communication_get_buffer_size(&receiver->recorder, &buffer_size);
            if (err != reliable_communication_error_no)
            {
                continue;
            }

            enum reliable_communiaction_response_t response = reliable_communication_response_received;
            if (index < receiver->recorder.first_packet_index)
            {
            }
            else if (index >= (receiver->recorder.first_packet_index + buffer_size))
            {
                response = reliable_communication_response_overflow;
            }
            else
            {
				uint32_t record_data;
				err = reliable_communication_get_record(&sender->recorder, index, &record_data);
                assert(err == reliable_communication_error_no);
				if (record_data == reliable_communication_packet_have_not_received)
				{
					if (receiver->callback && object)
					{
						receiver->callback(index, object);
					}
				}
                err = reliable_communication_record_received(receiver, index);
                assert(err == reliable_communication_error_no);

                err = reliable_communication_walk(&receiver->recorder, order_callback, object);
                assert(err == reliable_communication_error_no);
            }
            receiver->send_response(index, response, object);
        }
    }

func_end:
    return func_res;
}
