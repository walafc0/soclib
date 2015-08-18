N_INITS?=1

all: compile clean

compile:
	./mjpeg$(N_INITS)proc.py 32 32
	cp muteks/soft/bin.soft ../bin$(N_INITS)proc.soft

clean:
	@rm -rf muteks/
	@rm -rf exe.muteks_hard
	@rm -rf *pyc
	@rm -rf exe.posix
	@rm -rf posix/
	@rm -rf semantic.cache
	@rm -rf srl_trace.txt
	@rm -rf *~
