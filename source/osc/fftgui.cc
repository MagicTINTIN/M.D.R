#include <SFML/Graphics.hpp>
#include <portaudio.h>
#include <fftw3.h>
#include <iostream>
#include <cmath>

// Constants for audio settings
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 512;
const int NUM_CHANNELS = 2; // Stereo
const int FFT_SIZE = 512;   // Size of FFT (should be power of 2)

// Global variables
float audioBuffer[FRAMES_PER_BUFFER * NUM_CHANNELS];
fftwf_complex fftInput[FFT_SIZE];
fftwf_complex fftOutput[FFT_SIZE];
fftwf_plan fftPlan;

void initializeFFT() {
    fftPlan = fftwf_plan_dft_1d(FFT_SIZE, fftInput, fftOutput, FFTW_FORWARD, FFTW_ESTIMATE);
}

void processAudio(const float* inputBuffer) {
    // Convert inputBuffer to mono (take only left channel)
    for (int i = 0; i < FRAMES_PER_BUFFER; ++i) {
        audioBuffer[i] = inputBuffer[i * NUM_CHANNELS]; // Left channel
    }

    // Apply window function (optional)

    // Copy audioBuffer to fftInput and apply FFT
    for (int i = 0; i < FFT_SIZE; ++i) {
        fftInput[i][0] = audioBuffer[i]; // Real part
        fftInput[i][1] = 0.0f;           // Imaginary part
    }

    fftwf_execute(fftPlan);

    // Calculate magnitude spectrum
    float spectrum[FFT_SIZE];
    for (int i = 0; i < FFT_SIZE; ++i) {
        float re = fftOutput[i][0];
        float im = fftOutput[i][1];
        spectrum[i] = sqrt(re * re + im * im);
    }

    // Display FFT graphically using SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "FFT Visualization");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        // Draw FFT plot
        sf::VertexArray vertices(sf::LinesStrip, FFT_SIZE);
        for (int i = 0; i < FFT_SIZE; ++i) {
            float x = static_cast<float>(i) * 800.0f / FFT_SIZE;
            float y = 600.0f - spectrum[i] * 300.0f; // Scale spectrum amplitude for display
            vertices[i] = sf::Vector2f(x, y);
        }

        window.draw(vertices);

        window.display();
    }
}

int audioCallback(const void* inputBuffer, void* outputBuffer,
                  unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo* timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void* userData) {
    const float* in = static_cast<const float*>(inputBuffer);
    processAudio(in);
    return paContinue;
}

int main() {
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Open audio stream
    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream, NUM_CHANNELS, 0, paFloat32,
                               SAMPLE_RATE, FRAMES_PER_BUFFER, audioCallback, nullptr);
    if (err != paNoError) {
        std::cerr << "PortAudio error opening stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return 1;
    }

    // Start audio stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error starting stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    // Initialize FFT
    initializeFFT();

    // Wait for user to close window
    std::cout << "Press Ctrl+C to exit..." << std::endl;
    while (true) {
        // Keep main thread running
        sf::sleep(sf::milliseconds(100));
    }

    // Cleanup
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    fftwf_destroy_plan(fftPlan);
    fftwf_cleanup();

    return 0;
}
