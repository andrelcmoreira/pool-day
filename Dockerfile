FROM debian:latest

RUN apt update -y
RUN apt install cmake gcc g++ -y # building tools
RUN apt install doxygen git python3 python3-pip -y # utils
RUN apt install cppcheck gcovr valgrind -y # qa tools
RUN apt install libgmock-dev libgtest-dev -y # frameworks
RUN apt install curl zip unzip tar -y # vcpkg dependencies
RUN pip3 install --break-system-packages cpplint # python packages

# vcpkg stuff
RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg
RUN cd /opt/vcpkg && ./bootstrap-vcpkg.sh

ENV VCPKG_ROOT /opt/vcpkg
ENV PATH $VCPKG_ROOT:$PATH

WORKDIR /pool-day

ENTRYPOINT ["/bin/sh", "-c"]
