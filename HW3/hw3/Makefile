TA_HOME = /RAID2/COURSE/mlchip/mlchipTA01
LIB_DIR=$(TA_HOME)/systemc-2.3.3/lib-linux64
INC_DIR=$(TA_HOME)/systemc-2.3.3/include
LIB=-lsystemc-2.3.3
RPATH=-Wl,-rpath,$(TA_HOME)/systemc-2.3.3/lib-linux64

# export SYSTEMC_HOME=/usr/local/systmec-2.3.4
# export LD_LIBRARY_PATH=$(SYSTEMC_HOME)/lib-linux64

O = run
C = *.cpp *.o

all:
	g++ -I . -I  $(INC_DIR) -L . -L $(LIB_DIR) -o $(O) $(C) $(LIB) $(RPATH)
	./run

nWave_run:
	nWave -ssf *.vcd

clean:
	rm -rf $(O)
	rm -rf *.vcd *.fsdb
	rm -rf ./nWaveLog ./vfastLog ./verdiLog *.conf *.log
