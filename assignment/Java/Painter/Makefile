TARGET := Painter
SRC_DIR := src
SOURCES := $(shell find $(SRC_DIR) -name '*.java')
CLS_DIR := classes
CLASSES := $(subst $(SRC_DIR)/,,$(subst .java,.class,$(SOURCES)))

JAVA := java
JAVAC := javac
JFLAGS := -sourcepath $(SRC_DIR) -d $(CLS_DIR)
JAR := jar
JARFLAGS := cmf
MKDIR := mkdir -p

.PHONY: all
all: jar

dryrun:
	@echo $(SOURCES)
	@echo $(CLASSES)

class: $(SOURCES)
	@[ -d $(CLS_DIR) ] || $(MKDIR) $(CLS_DIR)
	$(JAVAC) $(JFLAGS) $^

jar: class
	$(JAR) $(JARFLAGS) manifest.mf $(TARGET).jar -C $(CLS_DIR) .

test: jar
	$(JAVA) -jar $(TARGET).jar
