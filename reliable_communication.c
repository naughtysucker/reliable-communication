#include "reliable_communication.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum reliable_communication_error_t reliable_communication_get_buffer_size(struct reliable_communication_t *ins, size_t *buffer_size)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    naughty_exception res = naughty_fifo_get_fulled_size(&ins->fifo, buffer_size);
    if (res != naughty_exception_no)
    {
        func_res = reliable_communication_error_unknown;
        goto func_end;
    }
func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_get_record(struct reliable_communication_t *ins, uint32_t index, uint32_t *record_data_ptr)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    naughty_exception res;

	if (index < ins->first_packet_index)
	{
		*record_data_ptr = reliable_communication_packet_received_already;
		goto func_end;
	}

    void *data_ptr;
    res = naughty_fifo_get_data(&ins->fifo, index - ins->first_packet_index, &data_ptr);
    if (res != naughty_exception_no)
    {
        if (res == naughty_exception_outofrange)
        {
            func_res = reliable_communication_error_overflow;
            goto func_end;
        }
    }

    *record_data_ptr = *(uint32_t *)data_ptr;

func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_fifo_initialize(struct reliable_communication_t *ins, size_t buffer_size, void *buffer)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    ins->first_packet_index = 0;
    naughty_exception res = naughty_fifo_initialize(&ins->fifo, sizeof(uint32_t), buffer_size, buffer);
    if (res != naughty_exception_no)
    {
        func_res = reliable_communication_error_unknown;
        goto func_end;
    }
    for (size_t i = 0; i < buffer_size; i++)
    {
        uint32_t data = reliable_communication_packet_have_not_received;
        res = naughty_fifo_push_back(&ins->fifo, &data);
        assert(res == naughty_exception_no);
    }
func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_walk(struct reliable_communication_t *ins, reliable_communication_new_packet_received_order_callback order_callback, void *object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    uint32_t *data_ptr = NULL;
    while (1)
    {
        naughty_exception res = naughty_fifo_get_front(&ins->fifo, (void **)&data_ptr);
        assert(res == naughty_exception_no);
        if ((enum reliable_communication_packet_record_status_t) * data_ptr == reliable_communication_packet_received_already)
        {
			if (order_callback)
			{
				order_callback(ins->first_packet_index, object);
			}

            res = naughty_fifo_pop_front(&ins->fifo);
            assert(res == naughty_exception_no);
            ins->first_packet_index++;
            uint32_t record = reliable_communication_packet_have_not_received;
            res = naughty_fifo_push_back(&ins->fifo, &record);
            assert(res == naughty_exception_no);

        }
        else
        {
            break;
        }
    }
func_end:
    return func_res;
}

enum reliable_communication_error_t reliable_communication_record_received(struct reliable_communication_t *ins, uint32_t index)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    size_t fulled_size = 0;
    naughty_exception res = naughty_fifo_get_fulled_size(&ins->fifo, &fulled_size);
    assert(res == naughty_exception_no);
    uint32_t record_ptr = reliable_communication_packet_received_already;
    if (index >= ins->first_packet_index + fulled_size)
    {
        func_res = reliable_communication_error_overflow;
        goto func_end;
    }
    if (index < ins->first_packet_index)
    {
        func_res = reliable_communication_error_received_before;
        goto func_end;
    }
    res = naughty_fifo_set_data(&ins->fifo, index - ins->first_packet_index, &record_ptr);
    assert(res == naughty_exception_no);
func_end:
    return func_res;
}

#ifdef __cplusplus
}
#endif
