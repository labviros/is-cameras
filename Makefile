CXX = clang++
CXXFLAGS += -std=c++14 -Wall
LDFLAGS += -I/usr/local/include -L/usr/local/lib -pthread -lpthread \
	-lprotobuf -lismsgs -lrabbitmq -lSimpleAmqpClient \
	-lopencv_core -lopencv_imgcodecs -lflycapture \
	-lboost_system -lboost_program_options \
	-lopentracing -lzipkin -lzipkin_opentracing
PROTOC = protoc

LOCAL_PROTOS_PATH = ./msgs/
vpath %.proto $(LOCAL_PROTOS_PATH)

MAINTAINER = viros
SERVICE = camera-gateway
TEST = test
VERSION = 1.1
LOCAL_REGISTRY = ninja.local:5000

all: debug

debug: CXXFLAGS += -g
debug: LDFLAGS += -fsanitize=address -fno-omit-frame-pointer -lopencv_highgui
debug: $(SERVICE) $(TEST)

release: CXXFLAGS += -Werror -O2
release: $(SERVICE)

clean:
	rm -f *.o *.pb.cc *.pb.h $(SERVICE) $(TEST)

$(SERVICE): $(SERVICE).o
	$(CXX)  $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

$(TEST): $(TEST).o
	$(CXX) $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(LOCAL_PROTOS_PATH) --cpp_out=. $<

docker:
	docker build -t $(MAINTAINER)/$(SERVICE):$(VERSION) --build-arg=SERVICE=$(SERVICE) .

push_local: docker
	docker tag $(MAINTAINER)/$(SERVICE):$(VERSION) $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)
	docker push $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)

push_cloud: docker
	docker push $(MAINTAINER)/$(SERVICE):$(VERSION)