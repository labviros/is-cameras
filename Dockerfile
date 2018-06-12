FROM is-cameras/dev

RUN conan search opencv/3.3.1@is/stable 
ADD . /project
WORKDIR /project
RUN sudo bash build.sh
RUN mkdir -v -p /tmp/deploy                                          \
 && libs=`find build/bin/ -type f -name '*.bin' -exec ldd {} \;      \
    | cut -d '(' -f 1 | cut -d '>' -f 2 | sort | uniq`               \
 && for lib in $libs; do                                             \
        cp --verbose --parents $lib /tmp/deploy;                     \
        libdir=`dirname $lib`;                                       \
        find $libdir -type f -name '*.xml' -exec                     \
            cp --verbose --parents {}  /tmp/deploy \;;               \
    done                                                             \
 && cp --verbose `find build/bin/ -type f -name '*.bin'` /tmp/deploy \
 && cp --verbose options.json /tmp/deploy                            \ 
 && sudo rm -rf build/

# Deployment container
FROM ubuntu:16.04
COPY --from=0 /tmp/deploy /