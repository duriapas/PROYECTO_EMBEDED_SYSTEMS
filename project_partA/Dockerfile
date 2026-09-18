# imagen for intro-cpp module
FROM ubuntu:noble

# init package installation
ARG DEBIAN_FRONTEND="noninteractive"
RUN apt update -y && apt upgrade -y

#configure repository from kitware
RUN apt install -y --no-install-recommends ca-certificates gpg wget
RUN test -f /usr/share/doc/kitware-archive-keyring/copyright || wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null

# Install compilation environment
RUN apt update -y && apt install -y --no-install-recommends gcc-14 g++-14 ninja-build gdb make cmake git libgtest-dev
RUN apt -y purge gcc-13 g++-13

#configure g++14 by default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 50 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 50

# Install GTest
WORKDIR /usr/src/gtest
RUN cmake . && make
