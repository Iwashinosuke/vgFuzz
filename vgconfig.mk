# テスト対象のMakefileにインクルードしてください
# 起動方法: make [options] TEST=1
# -----------------------------------

# テスト用変数 
VGTEST ?= 0
VGIJON ?= 0
VGIN ?= 0

ifeq ($(VGTEST),1)
$(info --- VGFUZZ TEST ACTIVATED ---)
$(info IJON = $(VGIJON))
$(info VGIN = $(VGIN))

# 動的ライブラリの追加
VGTEST_UTILS = -L./vgFuzz -lvgFuzz

# フラグ追加
VGTEST_UTILS += -DVGTEST=$(VGTEST)

ifeq ($(VGIJON),1)
VGTEST_UTILS += -DVGIJON=$(VGIJON)
endif
ifeq ($(VGIN),1)
VGTEST_UTILS += -DVGIN=$(VGIN)
endif


else
$(info --- vgFuzz installed, but not in test mode ---)
$(info --- if you want to test, please run make VGTEST=1 ---)
endif