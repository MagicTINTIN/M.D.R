#include <lo/lo.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

// Constants
const char* XR18_IP = "192.168.1.69";
const int XR18_PORT = 10024;  // Default OSC port for XR18

// Global variables
lo_server_thread st;
lo_address t;

void error(int num, const char *msg, const char *path) {
    std::cerr << "liblo server error " << num << " in path " << path << ": " << msg << std::endl;
    exit(1);
}

int fft_callback(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data) {
    std::cout << "Received OSC message on path: " << path << std::endl;
    
    for (int i = 0; i < argc; ++i) {
        if (types[i] == 'f') {
            std::cout << "Argument " << i << ": " << argv[i]->f << std::endl;
        } else {
            std::cout << "Argument " << i << ": (unsupported type)" << std::endl;
        }
    }

    return 0;  // Return value is not used in this context
}

int main() {
    // Initialize liblo address
    t = lo_address_new(XR18_IP, std::to_string(XR18_PORT).c_str());
    if (!t) {
        std::cerr << "Failed to create lo_address" << std::endl;
        return 1;
    }

    // Create a new server to listen for messages
    st = lo_server_thread_new("10028", error);  // Port to listen on
    if (!st) {
        std::cerr << "Failed to create lo_server_thread" << std::endl;
        return 1;
    }

    // Add method to handle OSC messages
    lo_server_thread_add_method(st, "/ch/11/mix/fader", "f*", fft_callback, NULL);

    // Start the server
    lo_server_thread_start(st);

    std::cout << "Listening for OSC messages on /ch/11/mix/fader..." << std::endl;

    // Keep the program running to listen for messages
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // Adjust sleep time as needed
    }

    // Clean up
    lo_server_thread_free(st);
    lo_address_free(t);

    return 0;
}
