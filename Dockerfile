ARG CROSS_TC_IMAGE_ARCH=armhf
ARG CROSS_TC_DOCKER_REGISTRY=torizon
ARG BASE_NAME=debian
ARG IMAGE_ARCH=linux/arm/v7
ARG IMAGE_TAG=2-bullseye
ARG DOCKER_REGISTRY=torizon

# First stage, x86_64 build container
FROM $CROSS_TC_DOCKER_REGISTRY/debian-cross-toolchain-$CROSS_TC_IMAGE_ARCH:$IMAGE_TAG AS cross-container
ARG GCC_PREFIX=arm-linux-gnueabihf

# copy project source
WORKDIR /project
COPY adc/ /project

# compile
RUN mkdir build && cd build \
    && $GCC_PREFIX-gcc -Wall -g -D_GNU_SOURCE -o adc \
    ../adc.c

# Install required packages
RUN if [ ! -z "" ]; then \
    apt-get -q -y update \
    && apt-get -q -y install  \
    && rm -rf /var/lib/apt/lists/* ; \
    fi

# Second stage, container for target
FROM --platform=$IMAGE_ARCH $DOCKER_REGISTRY/$BASE_NAME:$IMAGE_TAG AS deploy-container

# get the compiled program from the Build stage
COPY --from=cross-container /project/build/* /usr/local/bin/

ENV adc_bus=0 module_family=apalis
CMD adc $module_family $adc_bus