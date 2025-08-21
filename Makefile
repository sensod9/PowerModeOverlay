clean:
	del obj

exe: obj/Main.o obj/ConfigParser.o obj/StringToVKey.o
	g++ obj/Main.o obj/ConfigParser.o obj/StringToVKey.o -o PowerModeOverlay.exe -lgdi32 -luser32 -lkernel32 -mwindows

obj/Main.o: src/Main.cpp | obj
	g++ -c src/Main.cpp -o obj/Main.o

obj/ConfigParser.o: src/ConfigParser.cpp | obj
	g++ -c src/ConfigParser.cpp -o obj/ConfigParser.o

obj/StringToVKey.o: src/StringToVKey.cpp | obj
	g++ -c src/StringToVKey.cpp -o obj/StringToVKey.o

obj:
	if not exist obj mkdir obj
