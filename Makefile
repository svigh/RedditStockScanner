CXX=g++
APP=reddit_scrape.cpp
REST=-std=c++17
OUT_BIN_NAME=reddit_scrape
INCLUDE=-I./include/curlpp-0.8.1/include/
LD=-lm -lcurl -lcurlpp
# DEFINES=-DVERBOSE

build: clean
	${CXX} ${REST} ${APP} -o ${OUT_BIN_NAME} ${INCLUDE} ${LD} ${DEFINES}

run:
	./${OUT_BIN_NAME}

all: build run

clean:
	-rm ${OUT_BIN_NAME}

PHONY: build run all