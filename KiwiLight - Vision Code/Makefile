
 
# Makefile for the KiwiLight program.
# Written By: Brach Knutson

CXX=g++
FLAGS=-c -o
CV=`pkg-config --cflags --libs opencv`
GTK=`pkg-config --cflags --libs gtk+-3.0`

#UI 
bin/ui/Widget.o: ui/Widget.cpp
	$(CXX) $(FLAGS) bin/ui/Widget.o ui/Widget.cpp $(GTK)

bin/ui/ConfirmationDialog.o: ui/ConfirmationDialog.cpp
	$(CXX) $(FLAGS) bin/ui/ConfirmationDialog.o ui/ConfirmationDialog.cpp $(GTK)

bin/ui/SubMenuItem.o: ui/SubMenuItem.cpp
	$(CXX) $(FLAGS) bin/ui/SubMenuItem.o ui/SubMenuItem.cpp $(GTK)

bin/ui/MenuItem.o: ui/MenuItem.cpp
	$(CXX) $(FLAGS) bin/ui/MenuItem.o ui/MenuItem.cpp $(GTK)

bin/ui/MenuBar.o: ui/MenuBar.cpp
	$(CXX) $(FLAGS) bin/ui/MenuBar.o ui/MenuBar.cpp $(GTK)

bin/ui/Button.o: ui/Button.cpp
	$(CXX) $(FLAGS) bin/ui/Button.o ui/Button.cpp $(GTK)

bin/ui/Label.o: ui/Label.cpp
	$(CXX) $(FLAGS) bin/ui/Label.o ui/Label.cpp $(GTK)

bin/ui/LabeledSlider.o: ui/LabeledSlider.cpp
	$(CXX) $(FLAGS) bin/ui/LabeledSlider.o ui/LabeledSlider.cpp $(GTK)

bin/ui/AboutWindow.o: ui/AboutWindow.cpp
	$(CXX) $(FLAGS) bin/ui/AboutWindow.o ui/AboutWindow.cpp $(GTK)

bin/ui/HelpWindow.o: ui/HelpWindow.cpp
	$(CXX) $(FLAGS) bin/ui/HelpWindow.o ui/HelpWindow.cpp $(GTK)

bin/ui/CronWindow.o: ui/CronWindow.cpp
	$(CXX) $(FLAGS) bin/ui/CronWindow.o ui/CronWindow.cpp $(GTK)

bin/ui/LogViewer.o: ui/LogViewer.cpp
	$(CXX) $(FLAGS) bin/ui/LogViewer.o ui/LogViewer.cpp $(GTK)

bin/ui/Panel.o: ui/Panel.cpp
	$(CXX) $(FLAGS) bin/ui/Panel.o ui/Panel.cpp $(GTK)

bin/ui/Separator.o: ui/Separator.cpp
	$(CXX) $(FLAGS) bin/ui/Separator.o ui/Separator.cpp $(GTK)

bin/ui/Slider.o: ui/Slider.cpp
	$(CXX) $(FLAGS) bin/ui/Slider.o ui/Slider.cpp $(GTK)

bin/ui/CheckBox.o: ui/CheckBox.cpp
	$(CXX) $(FLAGS) bin/ui/CheckBox.o ui/CheckBox.cpp $(GTK)

bin/ui/TextBox.o: ui/TextBox.cpp
	$(CXX) $(FLAGS) bin/ui/TextBox.o ui/TextBox.cpp $(GTK)

bin/ui/NumberBox.o: ui/NumberBox.cpp
	$(CXX) $(FLAGS) bin/ui/NumberBox.o ui/NumberBox.cpp $(GTK)

bin/ui/FileChooser.o: ui/FileChooser.cpp
	$(CXX) $(FLAGS) bin/ui/FileChooser.o ui/FileChooser.cpp $(GTK)

bin/ui/TabView.o: ui/TabView.cpp
	$(CXX) $(FLAGS) bin/ui/TabView.o ui/TabView.cpp $(GTK)

bin/ui/Image.o: ui/Image.cpp
	$(CXX) $(FLAGS) bin/ui/Image.o ui/Image.cpp $(GTK) $(CV)

bin/ui/Window.o: ui/Window.cpp
	$(CXX) $(FLAGS) bin/ui/Window.o ui/Window.cpp $(GTK)

bin/ui/ConfigPanel.o: ui/ConfigPanel.cpp
	$(CXX) $(FLAGS) bin/ui/ConfigPanel.o ui/ConfigPanel.cpp $(GTK)

bin/ui/OverviewPanel.o: ui/OverviewPanel.cpp
	$(CXX) $(FLAGS) bin/ui/OverviewPanel.o ui/OverviewPanel.cpp $(GTK)

bin/ui/Settings.o: ui/Settings.cpp
	$(CXX) $(FLAGS) bin/ui/Settings.o ui/Settings.cpp $(GTK)

bin/ui/PreprocessorEditor.o: ui/PreprocessorEditor.cpp
	$(CXX) $(FLAGS) bin/ui/PreprocessorEditor.o ui/PreprocessorEditor.cpp $(GTK) $(CV)

bin/ui/PostprocessorEditor.o: ui/PostprocessorEditor.cpp
	$(CXX) $(FLAGS) bin/ui/PostprocessorEditor.o ui/PostprocessorEditor.cpp $(GTK) $(CV)

bin/ui/RunnerEditor.o: ui/RunnerEditor.cpp
	$(CXX) $(FLAGS) bin/ui/RunnerEditor.o ui/RunnerEditor.cpp $(GTK) $(CV)

bin/ui/ConfigEditor.o: ui/ConfigEditor.cpp
	$(CXX) $(FLAGS) bin/ui/ConfigEditor.o ui/ConfigEditor.cpp $(GTK) $(CV)

bin/ui/CameraSetting.o: ui/CameraSetting.cpp
	$(CXX) $(FLAGS) bin/ui/CameraSetting.o ui/CameraSetting.cpp $(GTK)

bin/ui/UDPPanel.o: ui/UDPPanel.cpp
	$(CXX) $(FLAGS) bin/ui/UDPPanel.o ui/UDPPanel.cpp $(GTK)

lib/UI.a: bin/ui/Widget.o bin/ui/ConfirmationDialog.o bin/ui/SubMenuItem.o bin/ui/MenuItem.o bin/ui/MenuBar.o bin/ui/Button.o bin/ui/Label.o bin/ui/LabeledSlider.o bin/ui/AboutWindow.o bin/ui/HelpWindow.o bin/ui/CronWindow.o bin/ui/LogViewer.o bin/ui/Panel.o bin/ui/Separator.o bin/ui/Slider.o bin/ui/CheckBox.o bin/ui/TextBox.o bin/ui/NumberBox.o bin/ui/FileChooser.o bin/ui/TabView.o bin/ui/Image.o bin/ui/Window.o bin/ui/ConfigPanel.o bin/ui/OverviewPanel.o bin/ui/Settings.o bin/ui/PreprocessorEditor.o bin/ui/PostprocessorEditor.o bin/ui/RunnerEditor.o bin/ui/ConfigEditor.o bin/ui/CameraSetting.o bin/ui/UDPPanel.o
	ar rs lib/UI.a bin/ui/Widget.o bin/ui/ConfirmationDialog.o bin/ui/SubMenuItem.o bin/ui/MenuItem.o bin/ui/MenuBar.o bin/ui/Button.o bin/ui/Label.o bin/ui/LabeledSlider.o bin/ui/AboutWindow.o bin/ui/HelpWindow.o bin/ui/CronWindow.o bin/ui/LogViewer.o bin/ui/Panel.o bin/ui/Separator.o bin/ui/Slider.o bin/ui/CheckBox.o bin/ui/TextBox.o bin/ui/NumberBox.o bin/ui/FileChooser.o bin/ui/TabView.o bin/ui/Image.o bin/ui/Window.o bin/ui/ConfigPanel.o bin/ui/OverviewPanel.o bin/ui/Settings.o bin/ui/PreprocessorEditor.o bin/ui/PostprocessorEditor.o bin/ui/RunnerEditor.o bin/ui/ConfigEditor.o bin/ui/CameraSetting.o bin/ui/UDPPanel.o

#UTIL
bin/util/Shell.o: util/Shell.cpp
	$(CXX) $(FLAGS) bin/util/Shell.o util/Shell.cpp

bin/util/Util.o: util/Util.cpp
	$(CXX) $(FLAGS) bin/util/Util.o util/Util.cpp

bin/util/UDP.o: util/UDP.cpp
	$(CXX) $(FLAGS) bin/util/UDP.o util/UDP.cpp

bin/util/XMLDocument.o: util/XMLDocument.cpp
	$(CXX) $(FLAGS) bin/util/XMLDocument.o util/XMLDocument.cpp

bin/util/XMLTag.o: util/XMLTag.cpp
	$(CXX) $(FLAGS) bin/util/XMLTag.o util/XMLTag.cpp

bin/util/XMLTagAttribute.o: util/XMLTagAttribute.cpp
	$(CXX) $(FLAGS) bin/util/XMLTagAttribute.o util/XMLTagAttribute.cpp

bin/util/SettingPair.o: util/SettingPair.cpp
	$(CXX) $(FLAGS) bin/util/SettingPair.o util/SettingPair.cpp

bin/util/Color.o: util/Color.cpp
	$(CXX) $(FLAGS) bin/util/Color.o util/Color.cpp

bin/util/Clock.o: util/Clock.cpp
	$(CXX) $(FLAGS) bin/util/Clock.o util/Clock.cpp

bin/util/LogEvent.o: util/LogEvent.cpp
	$(CXX) $(FLAGS) bin/util/LogEvent.o util/LogEvent.cpp 

lib/Util.a: bin/util/Shell.o bin/util/Util.o bin/util/UDP.o bin/util/XMLDocument.o bin/util/XMLTag.o bin/util/XMLTagAttribute.o bin/util/SettingPair.o bin/util/Color.o bin/util/Clock.o bin/util/LogEvent.o
	ar rs lib/Util.a bin/util/Shell.o bin/util/Util.o bin/util/UDP.o bin/util/XMLDocument.o bin/util/XMLTag.o bin/util/XMLTagAttribute.o bin/util/SettingPair.o bin/util/Color.o bin/util/Clock.o bin/util/LogEvent.o

#RUNNER
bin/runner/Contour.o: runner/Contour.cpp
	$(CXX) $(FLAGS) bin/runner/Contour.o runner/Contour.cpp $(CV)

bin/runner/ExampleContour.o: runner/ExampleContour.cpp
	$(CXX) $(FLAGS) bin/runner/ExampleContour.o runner/ExampleContour.cpp $(CV)

bin/runner/ExampleTarget.o: runner/ExampleTarget.cpp
	$(CXX) $(FLAGS) bin/runner/ExampleTarget.o runner/ExampleTarget.cpp $(CV)

bin/runner/PostProcessor.o: runner/PostProcessor.cpp
	$(CXX) $(FLAGS) bin/runner/PostProcessor.o runner/PostProcessor.cpp $(CV)

bin/runner/PreProcessor.o: runner/PreProcessor.cpp
	$(CXX) $(FLAGS) bin/runner/PreProcessor.o runner/PreProcessor.cpp $(CV)

bin/runner/CameraFrame.o: runner/CameraFrame.cpp
	$(CXX) $(FLAGS) bin/runner/CameraFrame.o runner/CameraFrame.cpp $(CV)

bin/runner/Logger.o: runner/Logger.cpp
	$(CXX) $(FLAGS) bin/runner/Logger.o runner/Logger.cpp $(CV)

bin/runner/ConfigLearner.o: runner/ConfigLearner.cpp
	$(CXX) $(FLAGS) bin/runner/ConfigLearner.o runner/ConfigLearner.cpp $(GTK) $(CV)

bin/runner/Runner.o: runner/Runner.cpp
	$(CXX) $(FLAGS) bin/runner/Runner.o runner/Runner.cpp $(GTK) $(CV)

bin/runner/TargetDistanceLearner.o: runner/TargetDistanceLearner.cpp
	$(CXX) $(FLAGS) bin/runner/TargetDistanceLearner.o runner/TargetDistanceLearner.cpp $(CV)

bin/runner/Target.o: runner/Target.cpp
	$(CXX) $(FLAGS) bin/runner/Target.o runner/Target.cpp $(CV)

bin/runner/TargetTroubleshooter.o: runner/TargetTroubleshooter.cpp
	$(CXX) $(FLAGS) bin/runner/TargetTroubleshooter.o runner/TargetTroubleshooter.cpp $(CV)

bin/runner/RunnerSettings.o: runner/RunnerSettings.cpp
	$(CXX) $(FLAGS) bin/runner/RunnerSettings.o runner/RunnerSettings.cpp

lib/Runner.a: bin/runner/Contour.o bin/runner/ExampleContour.o bin/runner/ExampleTarget.o bin/runner/PostProcessor.o bin/runner/PreProcessor.o bin/runner/CameraFrame.o bin/runner/Logger.o bin/runner/ConfigLearner.o bin/runner/Runner.o bin/runner/Target.o bin/runner/TargetDistanceLearner.o bin/runner/TargetTroubleshooter.o bin/runner/RunnerSettings.o
	ar rs lib/Runner.a bin/runner/Runner.o bin/runner/ConfigLearner.o bin/runner/Contour.o bin/runner/ExampleContour.o bin/runner/ExampleTarget.o bin/runner/PostProcessor.o bin/runner/Logger.o bin/runner/PreProcessor.o bin/runner/CameraFrame.o bin/runner/Target.o bin/runner/TargetDistanceLearner.o bin/runner/TargetTroubleshooter.o bin/runner/RunnerSettings.o

#MAIN FILE
bin/KiwiLight.o: KiwiLight.cpp
	$(CXX) $(FLAGS) bin/KiwiLight.o KiwiLight.cpp $(GTK) $(CV)

KiwiLight: Main.cpp lib/UI.a lib/Util.a lib/Runner.a bin/KiwiLight.o
	$(CXX) -o KiwiLight Main.cpp bin/KiwiLight.o lib/UI.a  lib/Runner.a lib/Util.a $(GTK) $(CV)

#SET UP THE FILES AND FOLDERS
setup:
	mkdir lib
	mkdir bin
	mkdir bin/ui
	mkdir bin/util
	mkdir bin/runner

#UNINSTALL
clean:
	$(info Uninstalling KiwiLight Executables...)
	rm -r bin
	rm -r lib
	make setup
	rm KiwiLight
