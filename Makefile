all:
	gcc udp_srv.c -l ncurses -o forza

clean:
	rm forza

run:
	./forza 20777

require:
	sudo apt-get install libncurses5-dev
