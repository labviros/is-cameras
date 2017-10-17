COMPILER = g++
FLAGS = -g -std=c++14 -Wall -Wextra -Werror

SO_DEPS = $(shell pkg-config --libs --cflags libSimpleAmqpClient msgpack librabbitmq opencv theoradec theoraenc)
# SO_DEPS += -lboost_program_options -lboost_system -lboost_filesystem -lpthread -larmadillo
SO_DEPS += -lflycapture -lpthread

MAINTAINER = picoreti
SERVICE = camera-gateway
VERSION = 1
LOCAL_REGISTRY = git.is:5000

all: $(SERVICE) test

clean:
	rm -f $(SERVICE) test

test: test.cpp
	$(COMPILER) $^ -o $@ $(FLAGS) $(SO_DEPS)

$(SERVICE): $(SERVICE).cpp
	$(COMPILER) $^ -o $@ $(FLAGS) $(SO_DEPS)

docker: $(SERVICE)
	rm -rf libs/
	mkdir libs/
	lddcp $(SERVICE) libs/
	docker build -t $(MAINTAINER)/$(SERVICE):$(VERSION) .
	rm -rf libs/

push_local: docker
	docker tag $(MAINTAINER)/$(SERVICE):$(VERSION) $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)
	docker push $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)

push_cloud: docker
	docker push $(MAINTAINER)/$(SERVICE):$(VERSION)