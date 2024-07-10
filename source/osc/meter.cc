#include <iostream>
#include <syncstream>
#include <chrono>
#include <thread>
#include <lo/lo.h>
#include <lo/lo_cpp.h>

#define XR18_IP "192.168.1.100" // Replace with your XR18's IP address
#define XR18_PORT "10024"

void error(int num, const char *msg, const char *path) {
    std::cerr << "Error " << num << " in path " << path << ": " << msg << std::endl;
}
                //    int (const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
int handle_meter_values(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data) {
    if (argc < 2) return 1; // Check if we have enough arguments

    int size = argv[0]->i; // Size of the blob
    const int16_t *meters = (const int16_t *)&argv[1]->blob;
    
    // Assuming we get 8 meter values per channel, channel 11 starts at index 88 (11 * 8)
    int channel_index = 10; // channel 11 is index 10 in 0-based array
    int meter_index = channel_index * 8; // 8 meters per channel

    int16_t pre_fader_l = meters[meter_index];
    int16_t pre_fader_r = meters[meter_index + 1];
    // Add other meters if needed

    std::cout << "Channel 11 Pre-Fader Left: " << pre_fader_l / 256.0 << " dB" << std::endl;
    std::cout << "Channel 11 Pre-Fader Right: " << pre_fader_r / 256.0 << " dB" << std::endl;

    return 0;
}

int main() {
    lo::ServerThread server(9000, error); // Local port to receive messages

    // Add method to handle incoming meter values
    server.add_method("/meters/0", "b", handle_meter_values);

    // Start the server
    server.start();

    // Create an OSC client to send requests to XR18
    lo::Address xr18(XR18_IP, XR18_PORT);

    // Send subscription request
    xr18.send("/meters", "si", "/meters/0", 8);

    std::cout << "Subscribed to meter values. Listening on port 9000..." << std::endl;

    // Keep the program running to receive messages
    while (true) {
        // Sleep to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
