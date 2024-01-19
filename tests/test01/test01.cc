#include <cstdint>
#include <cstdio>
#include "reliable_communication_sender.h"
#include "reliable_communication_receiver.h"
#include <deque>
#include <utility>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>

std::random_device rd;
std::default_random_engine re(rd());
std::uniform_int_distribution<uint32_t> uid(0, 99);

uint32_t loss_threshold = 50;

struct reliable_communication_sender_t g_sender;
struct reliable_communication_receiver_t g_receiver;

uint32_t g_sender_buffer[1] = {0};
uint32_t g_receiver_buffer[1] = {0};

std::chrono::steady_clock::time_point start_time;

// channel begin
std::mutex mtx_send_channel;
std::deque<uint32_t> send_channel;
std::mutex mtx_response_channel;
std::deque<std::pair<uint32_t, uint32_t>> response_channel;
// channel end

// typedef enum reliable_communication_error_t (*reliable_communication_get_receiver_received_response_func_t)(uint32_t *index, uint32_t *response, void *object);
// typedef enum reliable_communication_error_t (*reliable_communication_send_packet_func_t)(uint32_t index, void *object);

// for sender
enum reliable_communication_error_t test_get_received_response(uint32_t *index, uint32_t *response, void *object)
{
    // printf("test_receive_response \n");
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    {
        std::lock_guard<std::mutex> lck_grd(mtx_response_channel);
        if (response_channel.size())
        {
            *index = response_channel[0].first;
            *response = response_channel[0].second;
            // response_channel.erase(response_channel.begin());
            response_channel.pop_front();
            func_res = reliable_communication_error_got_one_response;
            // printf("test_receive_response: %d \n", *index);
        }
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(15));
func_end:
    return func_res;
}

enum reliable_communication_error_t test_send_packet(uint32_t index, void *object)
{
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    size_t size = 0;

    uint32_t ran_num = uid(re);

    if (ran_num < loss_threshold)
    {
        goto func_end;
    }

    {
        std::lock_guard<std::mutex> lck_grd(mtx_send_channel);
        send_channel.push_back(index);
        size = send_channel.size();
    }
    if (size > 10000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
func_end:
    return func_res;
}

// typedef enum reliable_communication_error_t (*reliable_communication_receive_packet_func_t)(uint32_t *index, void *object);
// typedef enum reliable_communication_error_t (*reliable_communication_send_response_func_t)(uint32_t index, enum reliable_communication_response_t response_data, void *object);

// for receiver

enum reliable_communication_error_t test_receive_packet(uint32_t *index, void *object)
{
    // printf("test_receive_packet \n");
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    {
        std::lock_guard<std::mutex> lck_grd(mtx_send_channel);
        if (send_channel.size())
        {
            *index = send_channel[0];
            func_res = reliable_communication_error_got_one_packet;
            // send_channel.erase(send_channel.begin());
            send_channel.pop_front();
            // printf("test_receive_packet: %d \n", *index);
        }
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(15));
func_end:
    return func_res;
}

enum reliable_communication_error_t test_send_response(uint32_t index, enum reliable_communication_response_t response_data, void *object)
{
    // printf("test_send_response: %d \n", index);
    enum reliable_communication_error_t func_res = reliable_communication_error_no;
    size_t size = 0;

    uint32_t ran_num = uid(re);

    if (ran_num < loss_threshold)
    {
        goto func_end;
    }

    {
        std::lock_guard<std::mutex> lck_grd(mtx_response_channel);
        response_channel.push_back({index, response_data});
        size = response_channel.size();
    }
#if 0
    if (response_data != reliable_communication_response_received)
    {
        printf("overflow: %d \n", index);
    }
#endif
    if (size > 10000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
func_end:
    return func_res;
}

// typedef void (*reliable_communication_new_packet_received_callback)(uint32_t index, void *object);
// typedef void (*reliable_communication_new_packet_received_order_callback)(uint32_t index, void *object);

void test_recved_callback(uint32_t index, void *object)
{
    // printf("test_recved callback: %d \n", index);
}

void test_ordered_recved_callback(uint32_t index, void *object)
{
    if (index == 1000000)
    {
        std::chrono::steady_clock::time_point cur_time = std::chrono::steady_clock::now();
        std::chrono::milliseconds dur = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - start_time);
        printf("1000000 Packets Received\n");
        printf("Spends %d ms \n", (int)dur.count());
    }
    else if (index < 1000000)
    {
        // printf("received: %d \n", index);
    }
}

// main 

int main(int argc, char **argv, char **env)
{
    reliable_communication_sender_initialize(&g_sender, sizeof(g_sender_buffer) / sizeof(uint32_t), g_sender_buffer, test_get_received_response, test_send_packet);
    reliable_communication_receiver_initialize(&g_receiver, sizeof(g_receiver_buffer) / sizeof(uint32_t), g_receiver_buffer, test_send_response, test_receive_packet, test_recved_callback, test_ordered_recved_callback);

    std::thread(
        [](){
            reliable_communication_receiver_receive(&g_receiver, NULL);
        }
    ).detach();

    start_time = std::chrono::steady_clock::now();

    reliable_communication_sender_send_packets(&g_sender, NULL);

    return 0;
}