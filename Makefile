ENV=heltec_wifi_lora_32_V4

build:
	pio run -e $(ENV)

upload:
	pio run -e $(ENV) -t upload

monitor:
	pio device monitor -b 115200

clean:
	pio run -t clean

fullclean:
	rm -rf .pio
	rm src/CMakeLists.txt CMakelists.txt 

test: clean
	pio test -vvv

remake: clean all

rebuild: clean build

all: build upload monitor

call: fullclean build upload monitor

