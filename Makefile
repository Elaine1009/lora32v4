ENV ?= heltec_wifi_lora_32_V4

KNOWN_TARGETS = build upload monitor clean fullclean rebuild all
EXTRA := $(strip $(filter-out $(KNOWN_TARGETS), $(MAKECMDGOALS)))
FILE := $(if $(EXTRA),$(EXTRA).cpp,main.cpp)

build:
	PLATFORMIO_BUILD_SRC_FILTER="-<*.cpp> +<$(FILE)>" pio run -e $(ENV)

upload:
	PLATFORMIO_BUILD_SRC_FILTER="-<*.cpp> +<$(FILE)>" pio run -e $(ENV) -t upload

monitor:
	pio device monitor -b 115200

clean:
	pio run -t clean

fullclean:
	rm -rf .pio
	rm -f src/CMakeLists.txt CMakeLists.txt

test: clean
	pio test -vvv

rebuild: clean build

all: build upload monitor

call: fullclean build upload monitor

%:
	@:
