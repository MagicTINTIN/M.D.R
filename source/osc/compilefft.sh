# g++ -o xr18_fft fft.cc -lportaudio -lfftw3 -std=c++11
# g++ -o xr18_fft fft.cc -lportaudio -lfftw3f -lsfml-graphics -lsfml-window -lsfml-system

g++ -o xr18_fft fft.cc -llo -lfftw3 -lm
