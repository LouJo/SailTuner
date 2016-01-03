# helper to build for Sailfish with sdk engine

# to add pulseaudio dev to MerSDK:
# sudo zypper in pulseaudio-devel
# sb2 -t SailfishOS-i486-x86 -m sdk-install -R zypper in pulseaudio-devel
# sb2 -t SailfishOS-armv7hl -m sdk-install -R zypper in pulseaudio-devel

SAILFISHOS_DIR = $(HOME)/SailfishOS

SSH_KEYS_DIR = $(SAILFISHOS_DIR)/vmshare/ssh/private_keys
SSH_KEY_ENGINE = $(SSH_KEYS_DIR)/engine/mersdk
SSH_KEY_EMULATOR = $(SSH_KEYS_DIR)/SailfishOS_Emulator/nemo

SSH_LOGIN_ENGINE = mersdk@localhost
SSH_LOGIN_EMULATOR = nemo@localhost
SSH_LOGIN_JOLLA = nemo@jolla

HOME_ENGINE = /home/mersdk/share

BUILD_DIR_LOCAL = $(HOME)/merBuild/sailtuner
BUILD_DIR_ENGINE = $(HOME_ENGINE)/merBuild/sailtuner

SOURCE_DIR_LOCAL = $(PWD)/..
#SOURCE_DIR_ENGINE = $(subst $(HOME),$(HOME_ENGINE),$(SOURCE_DIR_LOCAL))
SOURCE_DIR_ENGINE = /home/src1/SailTuner

PRO_FILE = harbour-sailtuner.pro

I486_TARGET = SailfishOS-i486
JOLLA_TARGET = SailfishOS-armv7hl

ENGINE_PORT = 2222
EMULATOR_PORT = 2223

BUILD_DIR_LOCAL_I486 = $(BUILD_DIR_LOCAL)/$(I486_TARGET)
BUILD_DIR_LOCAL_JOLLA = $(BUILD_DIR_LOCAL)/$(JOLLA_TARGET)

BUILD_DIR_ENGINE_I486 = $(BUILD_DIR_ENGINE)/$(I486_TARGET)
BUILD_DIR_ENGINE_JOLLA = $(BUILD_DIR_ENGINE)/$(JOLLA_TARGET)

SSH_ENGINE = ssh -p 2222 -i $(SSH_KEY_ENGINE) $(SSH_LOGIN_ENGINE)
SSH_EMULATOR = ssh -p 2223 -i $(SSH_KEY_EMULATOR) $(SSH_LOGIN_EMULATOR)

SCP_EMULATOR = scp -P 2223 -i $(SSH_KEY_EMULATOR)

RPM_I486 = harbour-sailtuner-0.1-1.i486.rpm
RPM_JOLLA = harbour-sailtuner-0.1-1.armv7hl.rpm

MB2 = mb2 -p $(SOURCE_DIR_ENGINE) 

all: $(BUILD_DIR_LOCAL_I486)/Makefile

$(BUILD_DIR_LOCAL_I486):
	mkdir -p $@

$(BUILD_DIR_LOCAL_I486)/Makefile: $(BUILD_DIR_LOCAL_I486)
	$(SSH_ENGINE) "cd $(BUILD_DIR_ENGINE_I486) && $(MB2) qmake $(SOURCE_DIR_ENGINE)/$(PRO_FILE)"

rpm-i486: $(BUILD_DIR_LOCAL_I486)/Makefile
	$(SSH_ENGINE) "cd $(BUILD_DIR_ENGINE_I486) && $(MB2) rpm"

install-i486: rpm-i486
	$(SCP_EMULATOR) $(BUILD_DIR_LOCAL_I486)/RPMS/$(RPM_I486) $(SSH_LOGIN_EMULATOR):
	$(SSH_EMULATOR) "sudo rpm -i $(RPM_I486) --force"

$(BUILD_DIR_LOCAL_JOLLA):
	mkdir -p $@

$(BUILD_DIR_LOCAL_JOLLA)/Makefile: $(BUILD_DIR_LOCAL_JOLLA)
	$(SSH_ENGINE) "cd $(BUILD_DIR_ENGINE_JOLLA) && $(MB2) -t $(JOLLA_TARGET) qmake $(SOURCE_DIR_ENGINE)/$(PRO_FILE)"

rpm-jolla: $(BUILD_DIR_LOCAL_JOLLA)/Makefile
	$(SSH_ENGINE) "cd $(BUILD_DIR_ENGINE_JOLLA) && $(MB2) -t $(JOLLA_TARGET) rpm"

install-jolla: rpm-jolla
	scp $(BUILD_DIR_LOCAL_JOLLA)/RPMS/$(RPM_JOLLA) $(SSH_LOGIN_JOLLA):
	ssh $(SSH_LOGIN_JOLLA) "devel-su rpm -i $(RPM_JOLLA) --force"
