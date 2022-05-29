objects = main.o tq_gen.o track_schedule.o magnetic_track.o

main: $(objects)
	cc -o main $(objects)

main.o : main.c track_schedule.o magnetic_track.o tq_gen.o
	cc -c -g main.c

track_schedule.o: track_schedule.c magnetic_track.h
	cc -c -g track_schedule.c

magnetic_track.o: magnetic_track.c 
	cc -c -g magnetic_track.c

tq_gen.o: tq_gen.c magnetic_track.h
	cc -c -g tq_gen.c

clean :
	del main.exe $(objects)