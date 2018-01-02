CXX = clang++
CXXFLAGS += -std=c++14 -Wall -Werror
LDFLAGS += -L/usr/local/lib -I/usr/local/include \
			`pkg-config --libs protobuf librabbitmq libSimpleAmqpClient`\
			-lopencv_imgproc -lopencv_core -lopencv_imgcodecs -lopencv_highgui\
			-lflycapture -lpthread -lboost_system -lboost_program_options -lismsgs\
			-lprometheus-cpp -lopentracing -lzipkin -lzipkin_opentracing 
PROTOC = protoc

LOCAL_PROTOS_PATH = ./msgs/
vpath %.proto $(LOCAL_PROTOS_PATH)

MAINTAINER = viros
SERVICE = camera-gateway
VERSION = 1.1
LOCAL_REGISTRY = git.is:5000

all: debug

debug: CXXFLAGS += -g 
debug: LDFLAGS += -fsanitize=address -fno-omit-frame-pointer
debug: $(SERVICE) test

release: CXXFLAGS += -Wall -Werror -O2
release: $(SERVICE)

clean:
	rm -f *.o *.pb.cc *.pb.h $(SERVICE) test

$(SERVICE): $(SERVICE).o
	$(CXX) $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

test: test.o
	$(CXX) $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(LOCAL_PROTOS_PATH) --cpp_out=. $<

docker:
	docker build -t $(MAINTAINER)/$(SERVICE):$(VERSION) --build-arg=SERVICE=$(SERVICE) --build-arg=BINARY=$(SERVICE) .

push_local: docker
	docker tag $(MAINTAINER)/$(SERVICE):$(VERSION) $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)
	docker push $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)

push_cloud: docker
	docker push $(MAINTAINER)/$(SERVICE):$(VERSION)