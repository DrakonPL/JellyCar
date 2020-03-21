TARGET		:= JellyCar
TITLE		:= JELY00001

OBJS   = 	../../Libs/tinyxml/tinystr.o \
			../../Libs/tinyxml/tinyxmlerror.o \
			../../Libs/tinyxml/tinyxmlparser.o \
			../../Libs/tinyxml/tinyxml.o \
			../../JellyPhysics/AABB.o \
			../../JellyPhysics/Body.o \
			../../JellyPhysics/ClosedShape.o \
			../../JellyPhysics/InternalSpring.o \
			../../JellyPhysics/PointMass.o \
			../../JellyPhysics/PressureBody.o \
			../../JellyPhysics/SpringBody.o \
			../../JellyPhysics/Vector2.o \
			../../JellyPhysics/VectorTools.o \
			../../JellyPhysics/World.o \
			../../JellyCar/Car/Car.o \
			../../JellyCar/Car/Chassis.o \
			../../JellyCar/Car/GameBallonBody.o \
			../../JellyCar/Car/SuspensionShock.o \
			../../JellyCar/Car/Tire.o \
			../../JellyCar/Levels/KinematicControl.o \
			../../JellyCar/Levels/KinematicMotor.o \
			../../JellyCar/Levels/KinematicPlatform.o \
			../../JellyCar/Levels/LevelManager.o \
			../../JellyCar/Levels/LevelSoftBody.o \
			../../JellyCar/Levels/GamePressureBody.o \
			../../JellyCar/Levels/GameSpringBody.o \
			../../JellyCar/Utils/JellyHelper.o \
			../../JellyCar/Utils/InputHelper.o \
			../../JellyCar/Utils/AudioHelper.o \
			../../JellyCar/Game/JellyCore.o \
			../../JellyCar/Game/JellyGame.o \
			../../JellyCar/Game/JellyOptions.o \
			../../JellyCar/Game/JellyMenuBetter.o \
			../../JellyCar/Game/JellyIntro.o \
			../../JellyCar/Game/JellySplash.o \
			../../JellyCar/JellyGameManager.o \
			../../JellyCar/JellyCar.o
			
LIBS =  -lvorbisfile -lvorbisenc -lvorbis -lspeexdsp -logg -lSceDisplay_stub -lSceGxm_stub -lSceCommonDialog_stub -lSceCtrl_stub -lSceSysmodule_stub -lSceTouch_stub -lSceAudio_stub -lSceNet_stub -lSceNetCtl_stub -lSceAvPlayer_stub -lfreetype -lm -lc -lpng -lz -L../../../Andromeda-Lib/Build/Vita/ -lAndromedaLib

PREFIX  	= arm-vita-eabi
CC      	= $(PREFIX)-gcc
CXX      	= $(PREFIX)-g++
STRIP 		= $(PREFIX)-strip
CXXFLAGS  	= -Wl,-q -O3 -fpermissive -ffast-math -mtune=cortex-a9 -mfloat-abi=hard -mfpu=neon -std=gnu++11 -Wall -Werror=return-type -DANDROMEDA_VITA -DHAVE_OGGVORBIS -DWANT_FASTWAV -DHAVE_LIBSPEEXDSP -DUSE_AUDIO_RESAMPLER 
CXXFLAGS	+= -I../../ -I../../../Andromeda-Lib -I../../../Libs -I../../../Andromeda-Lib/Libs/glm -I../../../Andromeda-Lib/Libs/freetype/include -I../../../Andromeda-Lib/Libs/freetype_gl -I../../JellyPhysics -I../../Libs/tinyxml -I../../../Andromeda-Lib/Libs/stb -I../../../Libs/nogg -I../../../Libs/tiny_obj_loader
CFLAGS  	= -Wl,-q -O3 -std=c99 -ffast-math -mtune=cortex-a9 -mfloat-abi=hard -mfpu=neon -Wall -Werror=return-type -DANDROMEDA_VITA -DHAVE_OGGVORBIS -DWANT_FASTWAV  -DHAVE_LIBSPEEXDSP -DUSE_AUDIO_RESAMPLER 
CFLAGS  	+= -I../../ -I../../../Andromeda-Lib -I../../../Libs -I../../../Andromeda-Lib/Libs/glm -I../../../Andromeda-Lib/Libs/freetype/include -I../../../Andromeda-Lib/Libs/freetype_gl -I../../JellyPhysics -I../../Libs/tinyxml -I../../../Andromeda-Lib/Libs/stb -I../../../Libs/nogg -I../../../Libs/tiny_obj_loader
ASFLAGS 	= $(CFLAGS)

all: $(TARGET).velf

%.velf: %.elf
	$(STRIP) -g $< 
	vita-elf-create $< $@
	vita-mksfoex -s TITLE_ID=$(TITLE) "Jelly Car" param.sfo
	vita-make-fself -s $@ eboot.bin

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CFLAGS)  $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS)
