#include <iostream>
#include <cmath>
#include <portaudio.h>
#include <fftw3.h>

// Constants
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 512;
const int NUM_CHANNELS = 1;
const int FFT_SIZE = 512;  // Size of FFT

// Global variables
fftw_complex *in, *out;
fftw_plan plan;

// Callback function for PortAudio
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    float *inBuffer = (float *)inputBuffer;
    
    // Copy input data to FFT input buffer
    for (int i = 0; i < FFT_SIZE; ++i) {
        in[i][0] = inBuffer[i];  // Real part
        in[i][1] = 0;            // Imaginary part (zero for real input)
    }
    
    // Perform FFT
    fftw_execute(plan);
    
    // Display FFT result (magnitude spectrum)
    for (int i = 0; i < FFT_SIZE / 2; ++i) {
        float mag = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        std::cout << i * (SAMPLE_RATE / FFT_SIZE) << " Hz: " << mag << std::endl;
    }
    
    return paContinue;
}

int main()
{
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }
    
    // Allocate FFT buffers
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_SIZE);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_SIZE);
    plan = fftw_plan_dft_1d(FFT_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    // Open PortAudio stream
    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, NUM_CHANNELS, 0, paFloat32, SAMPLE_RATE,
                               FRAMES_PER_BUFFER, paCallback, NULL);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }
    
    // Start audio stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }
    
    std::cout << "Listening for XR18 channel 1 audio input. Press Enter to stop..." << std::endl;
    std::cin.get();  // Wait for Enter key
    
    // Stop and close the audio stream
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }
    
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }
    
    // Clean up
    Pa_Terminate();
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    
    return 0;
}
