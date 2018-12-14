FROM lasote/conangcc54 AS build 

ADD https://raw.githubusercontent.com/labviros/is-wire/v1.1.4/bootstrap.sh .
RUN sudo chmod +x bootstrap.sh && sudo ./bootstrap.sh

ADD conanfile.py .
RUN sudo conan install . -s compiler.libcxx=libstdc++11 --build=missing

ADD . /project
WORKDIR /project

RUN sudo bash build.sh
RUN mkdir -v -p /tmp/deploy                                      \
 && libs=`find build/ -type f -name '*.bin' -exec ldd {} \;      \
    | cut -d '(' -f 1 | cut -d '>' -f 2 | sort | uniq`           \
 && for lib in $libs; do                                         \
        cp --verbose --parents $lib /tmp/deploy;                 \
        libdir=`dirname $lib`;                                   \
        find $libdir -type f -name '*.xml' -exec                 \
            cp --verbose --parents {}  /tmp/deploy \;;           \
    done                                                         \
 && cp --verbose `find build/ -type f -name '*.bin'` /tmp/deploy \
 && cp --verbose options.json /tmp/deploy                        \
 && sudo rm -rf build/

# Deployment container
FROM ubuntu:16.04
COPY --from=build /tmp/deploy /