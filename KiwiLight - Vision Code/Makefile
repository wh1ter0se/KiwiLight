
 
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

bin/SettingPair.o: util/SettingPair.cpp
	$(CXX) -c -o bin/SettingPair.o util/SettingPair.cpp

bin/Color.o: util/Color.cpp
	$(CXX) -c -o bin/Color.o util/Color.cpp $(CV)

bin/Camera.o: util/Camera.cpp
	$(CXX) -c -o bin/Camera.o util/Camera.cpp $(CV) $(GTK)

lib/Util.a: bin/Flags.o bin/Shell.o bin/StringUtils.o bin/XMLDocument.o bin/XMLTag.o bin/XMLTagAttribute.o bin/SettingPair.o bin/Color.o bin/Camera.o 
	ar rs lib/Util.a bin/Flags.o bin/Shell.o bin/StringUtils.o bin/XMLDocument.o bin/XMLTag.o bin/XMLTagAttribute.o bin/SettingPair.o bin/Color.o bin/Camera.o 

#RUNNER
bin/ConfigurationSetting.o: runner/ConfigurationSetting.cpp
	$(CXX) -c -o bin/ConfigurationSetting.o runner/ConfigurationSetting.cpp $(CV)

bin/ConfigurationSettingsList.o: runner/ConfigurationSettingsList.cpp
	$(CXX) -c -o bin/ConfigurationSettingsList.o runner/ConfigurationSettingsList.cpp $(CV)

bin/Contour.o: runner/Contour.cpp
	$(CXX) -c -o bin/Contour.o runner/Contour.cpp $(CV)

bin/ExampleContour.o: runner/ExampleContour.cpp
	$(CXX) -c -o bin/ExampleContour.o runner/ExampleContour.cpp $(CV)

bin/ExampleTarget.o: runner/ExampleTarget.cpp
	$(CXX) -c -o bin/ExampleTarget.o runner/ExampleTarget.cpp $(CV)

bin/PostProcessor.o: runner/PostProcessor.cpp
	$(CXX) -c -o bin/PostProcessor.o runner/PostProcessor.cpp $(CV)

bin/PreProcessor.o: runner/PreProcessor.cpp
	$(CXX) -c -o bin/PreProcessor.o runner/PreProcessor.cpp $(CV)

bin/Runner.o: runner/Runner.cpp
	$(CXX) -c -o bin/Runner.o runner/Runner.cpp $(CV)

bin/Target.o: runner/Target.cpp
	$(CXX) -c -o bin/Target.o runner/Target.cpp $(CV)

lib/Runner.a: bin/ConfigurationSetting.o bin/ConfigurationSettingsList.o bin/Contour.o bin/ExampleContour.o bin/ExampleTarget.o bin/PostProcessor.o bin/PreProcessor.o bin/Runner.o bin/Target.o
	ar rs lib/Runner.a bin/Runner.o bin/ConfigurationSetting.o bin/ConfigurationSettingsList.o bin/Contour.o bin/ExampleContour.o bin/ExampleTarget.o bin/PostProcessor.o bin/PreProcessor.o bin/Target.o

#ROOT

bin/Kiwi.o: Kiwi.cpp
	$(CXX) -c -o bin/Kiwi.o Kiwi.cpp $(GTK)

bin/Settings.o: Settings.cpp
	$(CXX) -c -o bin/Settings.o Settings.cpp $(GTK)

#MAIN FILE
KiwiLight: Main.cpp lib/UI.a lib/Util.a lib/Runner.a bin/Kiwi.o bin/Camera.o bin/Settings.o
	$(CXX) -o KiwiLight Main.cpp bin/Kiwi.o bin/Settings.o lib/UI.a lib/Util.a lib/Runner.a $(CV) $(GTK)

RunnerTest: RunnerTest.cpp lib/UI.a lib/Util.a lib/Runner.a bin/Kiwi.o bin/Settings.o
	$(CXX) -o RunnerTest RunnerTest.cpp lib/Runner.a lib/Util.a lib/UI.a $(CV) $(GTK)

#UNINSTALL
clean:
	$(info Uninstalling KiwiLight Executables...)
	rm lib/*
	rm bin/*
	rm KiwiLight
