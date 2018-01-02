FROM git.is:5000/is-cpp:flycapture-1
ARG SERVICE=local
ARG BINARY=local
COPY . ${SERVICE}
RUN cd ${SERVICE} && \
    make clean && \
    make release && \
    mkdir lib && \
    for lib in `ldd camera-gateway | awk 'BEGIN{ORS=" "}$1~/^\//{print $1}$3~/^\//{print $3}' | sed 's/,$/\n/'`; do cp $lib lib/; done 

FROM ubuntu:16.04
ARG SERVICE=local
ARG BINARY=local
COPY --from=0 ${SERVICE}/${BINARY} .
COPY --from=0 ${SERVICE}/ptgrey-test .
COPY --from=0 ${SERVICE}/lib /usr/local/lib/
RUN ldconfig
CMD ["./camera-gateway"]