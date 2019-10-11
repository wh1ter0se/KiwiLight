
 
# Makefile for the KiwiLight program.
# Written By: Brach Knutson

CXX = g++
CV = `pkg-config --cflags --libs opencv`
GTK = `pkg-config --cflags --libs gtk+-3.0`

#UI 
bin/ui/ConfirmationDialog.o: ui/ConfirmationDialog.cpp
	$(CXX) -c -o bin/ui/ConfirmationDialog.o ui/ConfirmationDialog.cpp $(GTK)

bin/ui/SubMenuItem.o: ui/SubMenuItem.cpp
	$(CXX) -c -o bin/ui/SubMenuItem.o ui/SubMenuItem.cpp $(GTK)

bin/ui/MenuItem.o: ui/MenuItem.cpp
	$(CXX) -c -o bin/ui/MenuItem.o ui/MenuItem.cpp $(GTK)

bin/ui/MenuBar.o: ui/MenuBar.cpp
	$(CXX) -c -o bin/ui/MenuBar.o ui/MenuBar.cpp $(GTK)

bin/ui/Button.o: ui/Button.cpp
	$(CXX) -c -o bin/ui/Button.o ui/Button.cpp $(GTK)

bin/ui/Label.o: ui/Label.cpp
	$(CXX) -c -o bin/ui/Label.o ui/Label.cpp $(GTK)

bin/ui/LabeledSlider.o: ui/LabeledSlider.cpp
	$(CXX) -c -o bin/ui/LabeledSlider.o ui/LabeledSlider.cpp $(GTK)

bin/ui/AboutWindow.o: ui/AboutWindow.cpp
	$(CXX) -c -o bin/ui/AboutWindow.o ui/AboutWindow.cpp $(GTK)

bin/ui/HelpWindow.o: ui/HelpWindow.cpp
	$(CXX) -c -o bin/ui/HelpWindow.o ui/HelpWindow.cpp $(GTK)

bin/ui/Panel.o: ui/Panel.cpp
	$(CXX) -c -o bin/ui/Panel.o ui/Panel.cpp $(GTK)

bin/ui/Scrollable.o: ui/Scrollable.cpp
	$(CXX) -c -o bin/ui/Scrollable.o ui/Scrollable.cpp $(GTK)

bin/ui/Frame.o: ui/Frame.cpp
	$(CXX) -c -o bin/ui/Frame.o ui/Frame.cpp $(GTK)

bin/ui/Separator.o: ui/Separator.cpp
	$(CXX) -c -o bin/ui/Separator.o ui/Separator.cpp $(GTK)

bin/ui/Slider.o: ui/Slider.cpp
	$(CXX) -c -o bin/ui/Slider.o ui/Slider.cpp $(GTK)

bin/ui/CheckBox.o: ui/CheckBox.cpp
	$(CXX) -c -o bin/ui/CheckBox.o ui/CheckBox.cpp $(GTK)

bin/ui/TextBox.o: ui/TextBox.cpp
	$(CXX) -c -o bin/ui/TextBox.o ui/TextBox.cpp $(GTK)

bin/ui/NumberBox.o: ui/NumberBox.cpp
	$(CXX) -c -o bin/ui/NumberBox.o ui/NumberBox.cpp $(GTK)

bin/ui/FileChooser.o: ui/FileChooser.cpp
	$(CXX) -c -o bin/ui/FileChooser.o ui/FileChooser.cpp $(GTK)

bin/ui/TabView.o: ui/TabView.cpp
	$(CXX) -c -o bin/ui/TabView.o ui/TabView.cpp $(GTK)

bin/ui/PopupTextBox.o: ui/PopupTextBox.cpp
	$(CXX) -c -o bin/ui/PopupTextBox.o ui/PopupTextBox.cpp $(GTK)

bin/ui/Image.o: ui/Image.cpp
	$(CXX) -c -o bin/ui/Image.o ui/Image.cpp $(GTK) $(CV)

bin/ui/Window.o: ui/Window.cpp
	$(CXX) -c -o bin/ui/Window.o ui/Window.cpp $(GTK)

bin/ui/ConfigPanel.o: ui/ConfigPanel.cpp
	$(CXX) -c -o bin/ui/ConfigPanel.o ui/ConfigPanel.cpp $(GTK)

bin/ui/OverviewPanel.o: ui/OverviewPanel.cpp
	$(CXX) -c -o bin/ui/OverviewPanel.o ui/OverviewPanel.cpp $(GTK) $(CV)

bin/ui/Settings.o: ui/Settings.cpp
	$(CXX) -c -o bin/ui/Settings.o ui/Settings.cpp $(GTK)

bin/ui/PreprocessorEditor.o: ui/PreprocessorEditor.cpp
	$(CXX) -c -o bin/ui/PreprocessorEditor.o ui/PreprocessorEditor.cpp $(CV) $(GTK)

bin/ui/PostprocessorEditor.o: ui/PostprocessorEditor.cpp
	$(CXX) -c -o bin/ui/PostprocessorEditor.o ui/PostprocessorEditor.cpp $(CV) $(GTK)

bin/ui/RunnerEditor.o: ui/RunnerEditor.cpp
	$(CXX) -c -o bin/ui/RunnerEditor.o ui/RunnerEditor.cpp $(CV) $(GTK)

bin/ui/ConfigEditor.o: ui/ConfigEditor.cpp
	$(CXX) -c -o bin/ui/ConfigEditor.o ui/ConfigEditor.cpp $(CV) $(GTK)

bin/ui/CameraSetting.o: ui/CameraSetting.cpp
	$(CXX) -c -o bin/ui/CameraSetting.o ui/CameraSetting.cpp $(GTK)

lib/UI.a: bin/ui/ConfirmationDialog.o bin/ui/SubMenuItem.o bin/ui/MenuItem.o bin/ui/MenuBar.o bin/ui/Button.o bin/ui/Label.o bin/ui/LabeledSlider.o bin/ui/AboutWindow.o bin/ui/HelpWindow.o bin/ui/Panel.o bin/ui/Scrollable.o bin/ui/Frame.o bin/ui/Separator.o bin/ui/Slider.o bin/ui/CheckBox.o bin/ui/TextBox.o bin/ui/NumberBox.o bin/ui/FileChooser.o bin/ui/TabView.o bin/ui/PopupTextBox.o bin/ui/Image.o bin/ui/Window.o bin/ui/ConfigPanel.o bin/ui/OverviewPanel.o bin/ui/Settings.o bin/ui/PreprocessorEditor.o bin/ui/PostprocessorEditor.o bin/ui/RunnerEditor.o bin/ui/ConfigEditor.o bin/ui/CameraSetting.o
	ar rs lib/UI.a bin/ui/ConfirmationDialog.o bin/ui/SubMenuItem.o bin/ui/MenuItem.o bin/ui/MenuBar.o bin/ui/Button.o bin/ui/Label.o bin/ui/LabeledSlider.o bin/ui/AboutWindow.o bin/ui/HelpWindow.o bin/ui/Panel.o bin/ui/Scrollable.o bin/ui/Frame.o bin/ui/Separator.o bin/ui/Slider.o bin/ui/CheckBox.o bin/ui/TextBox.o bin/ui/NumberBox.o bin/ui/FileChooser.o bin/ui/TabView.o bin/ui/PopupTextBox.o bin/ui/Image.o bin/ui/Window.o bin/ui/ConfigPanel.o bin/ui/OverviewPanel.o bin/ui/Settings.o bin/ui/PreprocessorEditor.o bin/ui/PostprocessorEditor.o bin/ui/RunnerEditor.o bin/ui/ConfigEditor.o bin/ui/CameraSetting.o

#UTIL
bin/util/Flags.o: util/Flags.cpp
	$(CXX) -c -o bin/util/Flags.o util/Flags.cpp $(GTK)

bin/util/Shell.o: util/Shell.cpp
	$(CXX) -c -o bin/util/Shell.o util/Shell.cpp $(GTK)

bin/util/StringUtils.o: util/StringUtils.cpp
	$(CXX) -c -o bin/util/StringUtils.o util/StringUtils.cpp $(GTK)

bin/util/DataUtils.o: util/DataUtils.cpp
	$(CXX) -c -o bin/util/DataUtils.o util/DataUtils.cpp $(GTK)

bin/util/UDP.o: util/UDP.cpp
	$(CXX) -c -o bin/util/UDP.o util/UDP.cpp $(GTK)

bin/util/XMLDocument.o: util/XMLDocument.cpp
	$(CXX) -c -o bin/util/XMLDocument.o util/XMLDocument.cpp $(GTK)

bin/util/XMLTag.o: util/XMLTag.cpp
	$(CXX) -c -o bin/util/XMLTag.o util/XMLTag.cpp $(GTK)

bin/util/XMLTagAttribute.o: util/XMLTagAttribute.cpp
	$(CXX) -c -o bin/util/XMLTagAttribute.o util/XMLTagAttribute.cpp $(GTK)

bin/util/SettingPair.o: util/SettingPair.cpp
	$(CXX) -c -o bin/util/SettingPair.o util/SettingPair.cpp $(GTK)

bin/util/Color.o: util/Color.cpp
	$(CXX) -c -o bin/util/Color.o util/Color.cpp $(GTK) $(CV)

lib/Util.a: bin/util/Flags.o bin/util/Shell.o bin/util/StringUtils.o bin/util/DataUtils.o bin/util/UDP.o bin/util/XMLDocument.o bin/util/XMLTag.o bin/util/XMLTagAttribute.o bin/util/SettingPair.o bin/util/Color.o
	ar rs lib/Util.a bin/util/Flags.o bin/util/Shell.o bin/util/StringUtils.o bin/util/DataUtils.o bin/util/UDP.o bin/util/XMLDocument.o bin/util/XMLTag.o bin/util/XMLTagAttribute.o bin/util/SettingPair.o bin/util/Color.o

#RUNNER
bin/runner/Contour.o: runner/Contour.cpp
	$(CXX) -c -o bin/runner/Contour.o runner/Contour.cpp $(GTK) $(CV)

bin/runner/ExampleContour.o: runner/ExampleContour.cpp
	$(CXX) -c -o bin/runner/ExampleContour.o runner/ExampleContour.cpp $(GTK) $(CV)

bin/runner/ExampleTarget.o: runner/ExampleTarget.cpp
	$(CXX) -c -o bin/runner/ExampleTarget.o runner/ExampleTarget.cpp $(GTK) $(CV)

bin/runner/PostProcessor.o: runner/PostProcessor.cpp
	$(CXX) -c -o bin/runner/PostProcessor.o runner/PostProcessor.cpp $(GTK) $(CV)

bin/runner/PreProcessor.o: runner/PreProcessor.cpp
	$(CXX) -c -o bin/runner/PreProcessor.o runner/PreProcessor.cpp $(GTK) $(CV)

bin/runner/CameraFrame.o: runner/CameraFrame.cpp
	$(CXX) -c -o bin/runner/CameraFrame.o runner/CameraFrame.cpp $(GTK) $(CV)

bin/runner/ConfigLearner.o: runner/ConfigLearner.cpp
	$(CXX) -c -o bin/runner/ConfigLearner.o runner/ConfigLearner.cpp $(GTK) $(CV)

bin/runner/Runner.o: runner/Runner.cpp
	$(CXX) -c -o bin/runner/Runner.o runner/Runner.cpp $(GTK) $(CV)

bin/runner/TargetDistanceLearner.o: runner/TargetDistanceLearner.cpp
	$(CXX) -c -o bin/runner/TargetDistanceLearner.o runner/TargetDistanceLearner.cpp $(GTK) $(CV)

bin/runner/Target.o: runner/Target.cpp
	$(CXX) -c -o bin/runner/Target.o runner/Target.cpp $(GTK) $(CV)

bin/runner/TargetTroubleshooter.o: runner/TargetTroubleshooter.cpp
	$(CXX) -c -o bin/runner/TargetTroubleshooter.o runner/TargetTroubleshooter.cpp $(GTK) $(CV)

bin/runner/RunnerSettings.o: runner/RunnerSettings.cpp
	$(CXX) -c -o bin/runner/RunnerSettings.o runner/RunnerSettings.cpp

lib/Runner.a: bin/runner/Contour.o bin/runner/ExampleContour.o bin/runner/ExampleTarget.o bin/runner/PostProcessor.o bin/runner/PreProcessor.o bin/runner/CameraFrame.o bin/runner/ConfigLearner.o bin/runner/Runner.o bin/runner/Target.o bin/runner/TargetDistanceLearner.o bin/runner/TargetTroubleshooter.o bin/runner/RunnerSettings.o
	ar rs lib/Runner.a bin/runner/Runner.o bin/runner/ConfigLearner.o bin/runner/Contour.o bin/runner/ExampleContour.o bin/runner/ExampleTarget.o bin/runner/PostProcessor.o bin/runner/PreProcessor.o bin/runner/CameraFrame.o bin/runner/Target.o bin/runner/TargetDistanceLearner.o bin/runner/TargetTroubleshooter.o bin/runner/RunnerSettings.o

#MAIN FILE
bin/KiwiLight.o: KiwiLight.cpp
	$(CXX) -c -o bin/KiwiLight.o KiwiLight.cpp $(CV) $(GTK)

KiwiLight: Main.cpp bin/KiwiLight.o lib/UI.a lib/Util.a lib/Runner.a
	$(CXX) -o KiwiLight Main.cpp bin/KiwiLight.o lib/UI.a  lib/Runner.a lib/Util.a $(CV) $(GTK)

#UNINSTALL
clean:
	$(info Uninstalling KiwiLight Executables...)
	rm bin/ui/*
	rm bin/util/*
	rm bin/runner/*
	rm lib/*
	rm KiwiLight
