#include <lo/lo.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

void error(int num, const char *msg, const char *path) {
    std::cerr << "liblo server error " << num << " in path " << path << ": " << msg << std::endl;
    exit(1);
}

int channel1_volume_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data) {
    float volume = argv[0]->f;
    std::cout << "Channel 1 volume changed: " << volume << " dB" << std::endl;
    return 0;
}

int main() {
    // Create a new address to send messages to the XR18 mixer
    lo_address t = lo_address_new("192.168.22.64", "10024");  // Replace with your XR18 IP and port

    // Check if address is valid
    if (!t) {
        std::cerr << "Failed to create lo_address" << std::endl;
        return 1;
    }

    // Send a message to set channel 11 level to -20dB
    int result;
    result = lo_send(t, "/ch/11/mix/fader", "f", 0.1);  // -20dB in XR18 fader value
    if (result == -1) {
        std::cerr << "Failed to send OSC message: " << lo_address_errstr(t) << std::endl;
        return 1;
    }
    // result = lo_send(t, "/ch/11/config/color", "i", 3);  // -20dB in XR18 fader value
    // if (result == -1) {
    //     std::cerr << "Failed to send OSC message: " << lo_address_errstr(t) << std::endl;
    //     return 1;
    // }


    // Create a new server to listen for messages
    lo_server_thread st = lo_server_thread_new("9000", error);  // Port to listen on
    if (!st) {
        std::cerr << "Failed to create lo_server_thread" << std::endl;
        return 1;
    }

    // Add method to handle volume changes for channel 1
    lo_server_thread_add_method(st, "/ch/11/mix/fader", "f", channel1_volume_handler, NULL);

    // Start the server
    lo_server_thread_start(st);

    std::cout << "Listening for volume changes on channel 11..." << std::endl;

    // Keep the program running to listen for messages
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Clean up (unreachable in this example)
    lo_server_thread_free(st);
    lo_address_free(t);

    return 0;
}
