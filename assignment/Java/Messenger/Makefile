TARGET := Messenger
SRC_DIR := src
SOURCES := $(shell find $(SRC_DIR) -name '*.java')
CLS_DIR := classes

JAVA := java
JAVAC := javac
JFLAGS := -sourcepath $(SRC_DIR) -d $(CLS_DIR)
JAR := jar
JARFLAGS := cmf

MKDIR := mkdir -p
RM := rm -rf

.PHONY: all
all: test

class: $(SOURCES)
	@[ -d $(CLS_DIR) ] || $(MKDIR) $(CLS_DIR)
	$(JAVAC) $(JFLAGS) $^

jar: class
	$(JAR) $(JARFLAGS) manifest.mf $(TARGET).jar -C $(CLS_DIR) .

test: jar
	$(JAVA) -jar $(TARGET).jar

.PHONY: clean
clean:
	$(RM) $(CLS_DIR)

distclean: clean
	$(RM) $(TARGET).jar
