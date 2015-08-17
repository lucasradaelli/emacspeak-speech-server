
CC=g++
CXXFLAGS+= -std=c++11 -m32 -Wall
OBJECTS=command_generator.o command_handler.o tts.o alsa_player.o

speech_server: server_main.cc  $(OBJECTS)
	$(CC) -o speech_server server_main.cc $(OBJECTS) $(CXXFLAGS)


command_generator.o: command_generator.cc command_generator.h 
	$(CC) -c -o command_generator.o command_generator.cc  $(CXXFLAGS)

command_handler.o: command_handler.cc command_handler.h
	$(CC) -c -o command_handler.o command_handler.cc $(CXXFLAGS)

alsa_player.o: alsa_player.cc alsa_player.h
	$(CC) -c -o alsa_player.o alsa_player.cc $(CXXFLAGS)

tts.o: tts.cc tts.h
	$(CC) -c -o tts.o tts.cc $(CXXFLAGS)



clean:
	rm *.o speech_server
