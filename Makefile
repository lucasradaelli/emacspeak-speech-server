
CC=g++
CXXFLAGS+= -g -std=c++11 -m32 -Wall
LIBS+= -lboost_regex -ldl -lasound
OBJECTS= speech_server.o server_state.o  command_generator.o commands.o tts.o alsa_player.o messages.o text_formatter.o

main: server_main.cc  $(OBJECTS)
	$(CC) -o speech_server server_main.cc $(OBJECTS) $(CXXFLAGS) $(LIBS)

test: test.cpp
	$(CC) -o test test.cpp $(OBJECTS) $(CXXFLAGS) $(LIBS)


speech_server.o: speech_server.cc speech_server.h
	$(CC) -c -o speech_server.o speech_server.cc $(CXXFLAGS)

server_state.o: server_state.cc server_state.h
	$(CC) -c -o server_state.o server_state.cc $(CXXFLAGS)

command_generator.o: command_generator.cc command_generator.h 
	$(CC) -c -o command_generator.o command_generator.cc  $(CXXFLAGS)

commands.o: commands.cc commands.h
	$(CC) -c -o commands.o commands.cc $(CXXFLAGS)

alsa_player.o: alsa_player.cc alsa_player.h
	$(CC) -c -o alsa_player.o alsa_player.cc  $(CXXFLAGS)

tts.o: tts.cc tts.h
	$(CC) -c -o tts.o tts.cc   $(CXXFLAGS)

messages.o: messages.cc messages.h
	$(CC) -c -o messages.o messages.cc   $(CXXFLAGS)


text_formatter.o: text_formatter.cc text_formatter.h
	$(CC) -c -o text_formatter.o text_formatter.cc   $(CXXFLAGS)

clean:
	rm *.o speech_server test
