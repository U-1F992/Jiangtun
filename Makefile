.PHONY: all clean

ifeq ($(OS),Windows_NT)
    RM = powershell -Command Remove-Item -Recurse -Force
	CP = copy
    FIXPATH = $(subst /,\,$1)
	PIO = $(USERPROFILE)\.platformio\penv\Scripts\platformio.exe
else
    RM = rm -rf
	CP = cp
    FIXPATH = $1
	PIO = $(HOME)/.platformio/penv/bin/platformio
endif

.pio/build/pico/firmware.uf2: src/main.cpp src/jiangtun.cpp src/internal.h include/jiangtun.h
	$(PIO) run --environment pico

.pio/build/dol-pico/firmware.uf2: src/main.cpp src/jiangtun.cpp src/internal.h include/jiangtun.h
	$(PIO) run --environment dol-pico

.pio/build/xiao-rp2040/firmware.uf2: src/main.cpp src/jiangtun.cpp src/internal.h include/jiangtun.h
	$(PIO) run --environment xiao-rp2040

.pio/build/dol-xiao-rp2040/firmware.uf2: src/main.cpp src/jiangtun.cpp src/internal.h include/jiangtun.h
	$(PIO) run --environment dol-xiao-rp2040

dist:
	mkdir dist

dist/firmware-pico.uf2: .pio/build/pico/firmware.uf2 dist
	$(CP) $(call FIXPATH,$<) $(call FIXPATH,$@)

dist/firmware-dol-pico.uf2: .pio/build/dol-pico/firmware.uf2 dist
	$(CP) $(call FIXPATH,$<) $(call FIXPATH,$@)

dist/firmware-xiao-rp2040.uf2: .pio/build/xiao-rp2040/firmware.uf2 dist
	$(CP) $(call FIXPATH,$<) $(call FIXPATH,$@)

dist/firmware-dol-xiao-rp2040.uf2: .pio/build/dol-xiao-rp2040/firmware.uf2 dist
	$(CP) $(call FIXPATH,$<) $(call FIXPATH,$@)

all: dist/firmware-pico.uf2 dist/firmware-dol-pico.uf2 dist/firmware-xiao-rp2040.uf2 dist/firmware-dol-xiao-rp2040.uf2

clean:
	$(RM) $(call FIXPATH,dist/*)
	$(RM) $(call FIXPATH,.pio/build/*)