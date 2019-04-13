
 
# Makefile for the KiwiLight program.
# Written By: Brach Knutson

CXX = g++
CV = `pkg-config --cflags --libs opencv`
GTK = `pkg-config --cflags --libs gtk+-2.0`

#UI 
bin/SubMenuItem.o: ui/SubMenuItem.cpp
	$(CXX) -c -o bin/SubMenuItem.o ui/SubMenuItem.cpp $(GTK)

bin/MenuItem.o: ui/MenuItem.cpp
	$(CXX) -c -o bin/MenuItem.o ui/MenuItem.cpp $(GTK)

bin/MenuBar.o: ui/MenuBar.cpp
	$(CXX) -c -o bin/MenuBar.o ui/MenuBar.cpp $(GTK)

bin/AlignmentContainer.o: ui/AlignmentContainer.cpp
	$(CXX) -c -o bin/AlignmentContainer.o ui/AlignmentContainer.cpp $(GTK)

bin/Button.o: ui/Button.cpp
	$(CXX) -c -o bin/Button.o ui/Button.cpp $(GTK)

bin/Label.o: ui/Label.cpp
	$(CXX) -c -o bin/Label.o ui/Label.cpp $(GTK)

bin/Panel.o: ui/Panel.cpp
	$(CXX) -c -o bin/Panel.o ui/Panel.cpp $(GTK)

bin/Frame.o: ui/Frame.cpp
	$(CXX) -c -o bin/Frame.o ui/Frame.cpp $(GTK)

bin/Separator.o: ui/Separator.cpp
	$(CXX) -c -o bin/Separator.o ui/Separator.cpp $(GTK)

bin/Slider.o: ui/Slider.cpp
	$(CXX) -c -o bin/Slider.o ui/Slider.cpp $(GTK)

bin/CheckBox.o: ui/CheckBox.cpp
	$(CXX) -c -o bin/CheckBox.o ui/CheckBox.cpp $(GTK)

bin/TextBox.o: ui/TextBox.cpp
	$(CXX) -c -o bin/TextBox.o ui/TextBox.cpp $(GTK)

bin/Window.o: ui/Window.cpp
	$(CXX) -c -o bin/Window.o ui/Window.cpp $(GTK)

bin/CameraSetting.o: ui/CameraSetting.cpp
	$(CXX) -c -o bin/CameraSetting.o ui/CameraSetting.cpp $(GTK)

lib/UI.a: bin/Button.o bin/Label.o bin/Panel.o bin/Frame.o bin/Separator.o bin/Slider.o bin/CheckBox.o bin/TextBox.o bin/Window.o bin/CameraSetting.o bin/AlignmentContainer.o bin/MenuBar.o bin/MenuItem.o bin/SubMenuItem.o
	ar rs lib/UI.a bin/Button.o bin/Label.o bin/Panel.o bin/Frame.o bin/Separator.o bin/Slider.o bin/CheckBox.o bin/TextBox.o bin/Window.o bin/CameraSetting.o bin/AlignmentContainer.o bin/MenuBar.o bin/MenuItem.o bin/SubMenuItem.o

#UTIL
bin/Flags.o: util/Flags.cpp
	$(CXX) -c -o bin/Flags.o util/Flags.cpp

bin/Shell.o: util/Shell.cpp
	$(CXX) -c -o bin/Shell.o util/Shell.cpp

bin/StringUtils.o: util/StringUtils.cpp
	$(CXX) -c -o bin/StringUtils.o util/StringUtils.cpp

bin/XMLDocument.o: util/XMLDocument.cpp
	$(CXX) -c -o bin/XMLDocument.o util/XMLDocument.cpp

bin/XMLTag.o: util/XMLTag.cpp
	$(CXX) -c -o bin/XMLTag.o util/XMLTag.cpp

bin/XMLTagAttribute.o: util/XMLTagAttribute.cpp
	$(CXX) -c -o bin/XMLTagAttribute.o util/XMLTagAttribute.cpp

lib/Util.a: bin/Flags.o bin/Shell.o bin/StringUtils.o bin/XMLDocument.o bin/XMLTag.o bin/XMLTagAttribute.o
	ar rs lib/Util.a bin/Flags.o bin/Shell.o bin/StringUtils.o bin/XMLDocument.o bin/XMLTag.o bin/XMLTagAttribute.o


#ROOT

bin/Kiwi.o: Kiwi.cpp
	$(CXX) -c -o bin/Kiwi.o Kiwi.cpp $(GTK)

bin/Camera.o: Camera.cpp
	$(CXX) -c -o bin/Camera.o Camera.cpp $(CV) $(GTK)

bin/Settings.o: Settings.cpp
	$(CXX) -c -o bin/Settings.o Settings.cpp $(GTK)

#MAIN FILE
KiwiLight: Main.cpp lib/UI.a lib/Util.a bin/Kiwi.o bin/Camera.o bin/Settings.o
	$(CXX) -o KiwiLight Main.cpp bin/Kiwi.o bin/Camera.o bin/Settings.o lib/UI.a lib/Util.a $(CV) $(GTK)