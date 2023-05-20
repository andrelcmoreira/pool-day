FROM debian:latest

RUN apt update -y
RUN apt install cmake gcc g++ -y # building tools
RUN apt install doxygen python3 python3-pip -y # utils
RUN apt install cppcheck gcovr valgrind -y # qa tools
RUN apt install libgmock-dev libgtest-dev -y # frameworks
RUN pip3 install cpplint # python packages

WORKDIR /pool-day

ENTRYPOINT ["/bin/sh", "-c"]
