FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# 1. Update the system and install all packages and dependencies needed
# (bison, flex, texinfo, python3-dev, python2-dev, autoconf, git, build-essential, libtool, qemu-system, cmake, wget, nano, etc.)
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
        bison \
        flex \
        texinfo \
        python3-dev \
        python2-dev \
        python2 \
        autoconf \
        git \
        build-essential \
        libtool \
        qemu-system \
        cmake \
        wget \
        tar \
        gzip \
        unzip \
        nano \
        socat \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

# 2. Set Python 2.7 as the default python version using update-alternatives
RUN update-alternatives --install /usr/bin/python python /usr/bin/python2.7 10

# 3. Create the workspace and directory structure for the RTEMS framework (i386)
#ENV HOME=/root
#WORKDIR $HOME/rtems-dev

#RUN mkdir -p compiler/i386/5 && \
#    mkdir -p rtems-lib/i386/5/build

# 4. Configure environment variables so RTEMS cross-compilation tools are in PATH
#ENV PATH="$HOME/rtems-dev/compiler/i386/5/bin:${PATH}"

CMD ["/bin/bash"]
