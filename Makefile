target=main.cpp load.cpp score.cpp wav.cpp note.cpp errmsg.cpp
header=load.h score.h wav.h note.h errmsg.h
jackdaw: $(target) $(header)
	g++ $(target) -o jackdaw
