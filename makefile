# Project specifid variables
SRC = src
INC = include/nnet
BUILD = build
LIB = lib
INC_INSTALL = /usr/local/include
LIB_INSTALL = /usr/local/lib
LIB_NAME = nnet
CXXFLAGS=-pedantic -Wall -fPIC -std=c++11 -I include
LDLIBS=-lPocoFoundation -lPocoXML

# Patterned variables
SRCS = $(wildcard $(SRC)/*.cpp)
OBJECTS = $(patsubst $(SRC)/%.cpp,$(BUILD)/%.o,$(SRCS))
DEPDIR = .dep
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
COMPILE = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: $(LIB) $(BUILD) $(LIB)/lib$(LIB_NAME).so.1.0

$(LIB)/lib$(LIB_NAME).so.1.0: $(OBJECTS)
	@echo Building lib$(LIB_NAME)
	@$(CXX) -shared -Wl,-soname,lib$(LIB_NAME).so.1 -o \
		$(LIB)/lib$(LIB_NAME).so.1.0 $(OBJECTS) $(LDLIBS)
	@ln -sf $(LIB)/lib$(LIB_NAME).so.1.0 $(LIB)/lib$(LIB_NAME).so
	@ln -sf $(LIB)/lib$(LIB_NAME).so.1.0 $(LIB)/lib$(LIB_NAME).so.1

# Generic patterns targets
$(BUILD)/%.o: $(SRC)/%.cpp
$(BUILD)/%.o: $(SRC)/%.cpp $(DEPDIR)/%.d
	@echo Building \($@\)
	@$(COMPILE) -o $@ $<
	@$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst $(SRC)/%.cpp,$(DEPDIR)/%.d,$(SRCS))

$(BUILD):
	@mkdir -p $@ 

$(LIB):
	@mkdir -p $@

clean:
	@rm -rf $(LIB)
	@rm -rf $(BUILD)

install:
	@mkdir -p $(LIB_INSTALL)
	@cp $(LIB)/lib$(LIB_NAME).so.1.0  $(LIB_INSTALL)
	@ln -sf $(LIB_INSTALL)/lib$(LIB_NAME).so.1.0 \
		$(LIB_INSTALL)/lib$(LIB_NAME).so.1
	@ln -sf $(LIB_INSTALL)/lib$(LIB_NAME).so.1.0 \
		$(LIB_INSTALL)/lib$(LIB_NAME).so
	@mkdir -p $(INC_INSTALL)/nnet
	@cp $(INC)/* $(INC_INSTALL)/nnet

uninstall:
	@rm -rf $(INC_INSTALL)/nnet
	@rm $(LIB_INSTALL)/lib$(LIB_NAME).so*

# -------TESTING---------

# Test variables
TEST_SRC = test/src
TEST_BUILD = test/build
TEST_BIN = test/bin

atest: $(TEST_BUILD) $(TEST_BIN) $(TEST_BUILD)/Main.o $(OBJECTS)
	@echo Compiling acceptance test
	@$(CXX) $(CXXFLAGS) $(TEST_BUILD)/Main.o $(OBJECTS) -o $(TEST_BIN)/$@ $(LDLIBS)

$(TEST_BUILD)/Main.o: $(TEST_SRC)/Main.cpp $(INC)/NeuralNetwork2.hpp \
	$(INC)/TanhInputNeuron.hpp $(INC)/
	@$(CXX) $(CXXFLAGS) -c $(TEST_SRC)/Main.cpp -o $(TEST_BUILD)/Main.o

$(TEST_BUILD)/%.o: $(SRC)/%.cpp
$(TEST_BUILD)/%.o: $(SRC)/%.cpp $(DEPDIR)/%.d
	@echo Building \($@\)
	@$(COMPILE) -o $@ $<
	@$(POSTCOMPILE)

$(TEST_BUILD):
	@mkdir -p $@

$(TEST_BIN):
	@mkdir -p $@