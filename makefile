app.exe: app.o
	g++ -o app.exe app.o -mwindows -lcomctl32 -lole32 -lshell32 -lcomdlg32

app.o: app.cpp
	g++ -c app.cpp -std=c++11
