
 
# Makefile for the KiwiLight program.
# Written By: Brach Knutson

CXX = g++
CV = `pkg-config --cflags --libs opencv`
GTK = `pkg-config --cflags --libs gtk+-3.0`

#UI 
bin/ConfirmationDialog.o: ui/ConfirmationDialog.cpp
	$(CXX) -c -o bin/ConfirmationDialog.o ui/ConfirmationDialog.cpp $(GTK)

bin/SubMenuItem.o: ui/SubMenuItem.cpp
	$(CXX) -c -o bin/SubMenuItem.o ui/SubMenuItem.cpp $(GTK)

bin/MenuItem.o: ui/MenuItem.cpp
	$(CXX) -c -o bin/MenuItem.o ui/MenuItem.cpp $(GTK)

bin/MenuBar.o: ui/MenuBar.cpp
	$(CXX) -c -o bin/MenuBar.o ui/MenuBar.cpp $(GTK)

bin/Button.o: ui/Button.cpp
	$(CXX) -c -o bin/Button.o ui/Button.cpp $(GTK)

bin/Label.o: ui/Label.cpp
	$(CXX) -c -o bin/Label.o ui/Label.cpp $(GTK)

bin/LabeledSlider.o: ui/LabeledSlider.cpp
	$(CXX) -c -o bin/LabeledSlider.o ui/LabeledSlider.cpp $(GTK)

bin/AboutWindow.o: ui/AboutWindow.cpp
	$(CXX) -c -o bin/AboutWindow.o ui/AboutWindow.cpp $(GTK)

bin/HelpWindow.o: ui/HelpWindow.cpp
	$(CXX) -c -o bin/HelpWindow.o ui/HelpWindow.cpp $(GTK)

bin/Panel.o: ui/Panel.cpp
	$(CXX) -c -o bin/Panel.o ui/Panel.cpp $(GTK)

bin/Scrollable.o: ui/Scrollable.cpp
	$(CXX) -c -o bin/Scrollable.o ui/Scrollable.cpp $(GTK)

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

bin/NumberBox.o: ui/NumberBox.cpp
	$(CXX) -c -o bin/NumberBox.o ui/NumberBox.cpp $(GTK)

bin/FileChooser.o: ui/FileChooser.cpp
	$(CXX) -c -o bin/FileChooser.o ui/FileChooser.cpp $(GTK)

bin/PopupTextBox.o: ui/PopupTextBox.cpp
	$(CXX) -c -o bin/PopupTextBox.o ui/PopupTextBox.cpp $(GTK)

bin/Image.o: ui/Image.cpp
	$(CXX) -c -o bin/Image.o ui/Image.cpp $(GTK) $(CV)

bin/Window.o: ui/Window.cpp
	$(CXX) -c -o bin/Window.o ui/Window.cpp $(GTK)

bin/ConfigRunnerEditor.o: ui/ConfigRunnerEditor.cpp
	$(CXX) -c -o bin/ConfigRunnerEditor.o ui/ConfigRunnerEditor.cpp $(CV) $(GTK)

bin/ConfigTargetEditor.o: ui/ConfigTargetEditor.cpp
	$(CXX) -c -o bin/ConfigTargetEditor.o ui/ConfigTargetEditor.cpp $(CV) $(GTK)

bin/Settings.o: ui/Settings.cpp
	$(CXX) -c -o bin/Settings.o ui/Settings.cpp $(GTK)

bin/ConfigEditor.o: ui/ConfigEditor.cpp
	$(CXX) -c -o bin/ConfigEditor.o ui/ConfigEditor.cpp $(CV) $(GTK)

bin/CameraSetting.o: ui/CameraSetting.cpp
	$(CXX) -c -o bin/CameraSetting.o ui/CameraSetting.cpp $(GTK)

lib/UI.a: bin/ConfirmationDialog.o bin/SubMenuItem.o bin/MenuItem.o bin/MenuBar.o bin/Button.o bin/Label.o bin/LabeledSlider.o bin/AboutWindow.o bin/HelpWindow.o bin/Panel.o bin/Scrollable.o bin/Frame.o bin/Separator.o bin/Slider.o bin/CheckBox.o bin/TextBox.o bin/NumberBox.o bin/FileChooser.o bin/PopupTextBox.o bin/Image.o bin/Window.o bin/Settings.o bin/ConfigRunnerEditor.o bin/ConfigTargetEditor.o bin/ConfigEditor.o bin/CameraSetting.o
	ar rs lib/UI.a bin/ConfirmationDialog.o bin/SubMenuItem.o bin/MenuItem.o bin/MenuBar.o bin/Button.o bin/Label.o bin/LabeledSlider.o bin/AboutWindow.o bin/HelpWindow.o bin/Panel.o bin/Scrollable.o bin/Frame.o bin/Separator.o bin/Slider.o bin/CheckBox.o bin/TextBox.o bin/NumberBox.o bin/FileChooser.o bin/PopupTextBox.o bin/Image.o bin/Window.o bin/Settings.o bin/ConfigRunnerEditor.o bin/ConfigTargetEditor.o bin/ConfigEditor.o bin/CameraSetting.o

#UTIL
bin/Flags.o: util/Flags.cpp
	$(CXX) -c -o bin/Flags.o util/Flags.cpp $(GTK)

bin/Shell.o: util/Shell.cpp
	$(CXX) -c -o bin/Shell.o util/Shell.cpp $(GTK)

bin/StringUtils.o: util/StringUtils.cpp
	$(CXX) -c -o bin/StringUtils.o util/StringUtils.cpp $(GTK)

bin/DataUtils.o: util/DataUtils.cpp
	$(CXX) -c -o bin/DataUtils.o util/DataUtils.cpp $(GTK)

bin/UDP.o: util/UDP.cpp
	$(CXX) -c -o bin/UDP.o util/UDP.cpp $(GTK)

bin/XMLDocument.o: util/XMLDocument.cpp
	$(CXX) -c -o bin/XMLDocument.o util/XMLDocument.cpp $(GTK)

bin/XMLTag.o: util/XMLTag.cpp
	$(CXX) -c -o bin/XMLTag.o util/XMLTag.cpp $(GTK)

bin/XMLTagAttribute.o: util/XMLTagAttribute.cpp
	$(CXX) -c -o bin/XMLTagAttribute.o util/XMLTagAttribute.cpp $(GTK)

bin/SettingPair.o: util/SettingPair.cpp
	$(CXX) -c -o bin/SettingPair.o util/SettingPair.cpp $(GTK)

bin/Color.o: util/Color.cpp
	$(CXX) -c -o bin/Color.o util/Color.cpp $(GTK) $(CV)

lib/Util.a: bin/Flags.o bin/Shell.o bin/StringUtils.o bin/DataUtils.o bin/UDP.o bin/XMLDocument.o bin/XMLTag.o bin/XMLTagAttribute.o bin/SettingPair.o bin/Color.o
	ar rs lib/Util.a bin/Flags.o bin/Shell.o bin/StringUtils.o bin/DataUtils.o bin/UDP.o bin/XMLDocument.o bin/XMLTag.o bin/XMLTagAttribute.o bin/SettingPair.o bin/Color.o

#RUNNER
bin/ConfigurationSetting.o: runner/ConfigurationSetting.cpp
	$(CXX) -c -o bin/ConfigurationSetting.o runner/ConfigurationSetting.cpp $(GTK) $(CV)

bin/ConfigurationSettingsList.o: runner/ConfigurationSettingsList.cpp
	$(CXX) -c -o bin/ConfigurationSettingsList.o runner/ConfigurationSettingsList.cpp $(GTK) $(CV)

bin/Contour.o: runner/Contour.cpp
	$(CXX) -c -o bin/Contour.o runner/Contour.cpp $(GTK) $(CV)

bin/ExampleContour.o: runner/ExampleContour.cpp
	$(CXX) -c -o bin/ExampleContour.o runner/ExampleContour.cpp $(GTK) $(CV)

bin/ExampleTarget.o: runner/ExampleTarget.cpp
	$(CXX) -c -o bin/ExampleTarget.o runner/ExampleTarget.cpp $(GTK) $(CV)

bin/PostProcessor.o: runner/PostProcessor.cpp
	$(CXX) -c -o bin/PostProcessor.o runner/PostProcessor.cpp $(GTK) $(CV)

bin/PreProcessor.o: runner/PreProcessor.cpp
	$(CXX) -c -o bin/PreProcessor.o runner/PreProcessor.cpp $(GTK) $(CV)

bin/CameraFrame.o: runner/CameraFrame.cpp
	$(CXX) -c -o bin/CameraFrame.o runner/CameraFrame.cpp $(GTK) $(CV)

bin/ConfigLearner.o: runner/ConfigLearner.cpp
	$(CXX) -c -o bin/ConfigLearner.o runner/ConfigLearner.cpp $(GTK) $(CV)

bin/Runner.o: runner/Runner.cpp
	$(CXX) -c -o bin/Runner.o runner/Runner.cpp $(GTK) $(CV)

bin/TargetDistanceLearner.o: runner/TargetDistanceLearner.cpp
	$(CXX) -c -o bin/TargetDistanceLearner.o runner/TargetDistanceLearner.cpp $(GTK) $(CV)

bin/Target.o: runner/Target.cpp
	$(CXX) -c -o bin/Target.o runner/Target.cpp $(GTK) $(CV)

bin/TargetTroubleshooter.o: runner/TargetTroubleshooter.cpp
	$(CXX) -c -o bin/TargetTroubleshooter.o runner/TargetTroubleshooter.cpp $(GTK) $(CV)

bin/RunnerSettings.o: runner/RunnerSettings.cpp
	$(CXX) -c -o bin/RunnerSettings.o runner/RunnerSettings.cpp

lib/Runner.a: bin/ConfigurationSetting.o bin/ConfigurationSettingsList.o bin/Contour.o bin/ExampleContour.o bin/ExampleTarget.o bin/PostProcessor.o bin/PreProcessor.o bin/CameraFrame.o bin/ConfigLearner.o bin/Runner.o bin/Target.o bin/TargetDistanceLearner.o bin/TargetTroubleshooter.o bin/RunnerSettings.o
	ar rs lib/Runner.a bin/Runner.o bin/ConfigLearner.o bin/ConfigurationSetting.o bin/ConfigurationSettingsList.o bin/Contour.o bin/ExampleContour.o bin/ExampleTarget.o bin/PostProcessor.o bin/PreProcessor.o bin/CameraFrame.o bin/Target.o bin/TargetDistanceLearner.o bin/TargetTroubleshooter.o bin/RunnerSettings.o

#ROOT

bin/ConfigPanel.o: ConfigPanel.cpp
	$(CXX) -c -o bin/ConfigPanel.o ConfigPanel.cpp $(GTK)

#MAIN FILE
KiwiLight: Main.cpp lib/UI.a lib/Util.a lib/Runner.a bin/ConfigPanel.o
	$(CXX) -o KiwiLight Main.cpp bin/ConfigPanel.o lib/UI.a  lib/Runner.a lib/Util.a $(CV) $(GTK)

RunnerTest: RunnerTest.cpp lib/UI.a lib/Util.a lib/Runner.a bin/Settings.o
	$(CXX) -o RunnerTest RunnerTest.cpp lib/Runner.a lib/Util.a lib/UI.a $(CV) $(GTK)

Test: Test.cpp lib/UI.a lib/Util.a lib/Runner.a bin/ConfigPanel.o
	$(CXX) -o Test Test.cpp bin/ConfigPanel.o lib/UI.a  lib/Runner.a lib/Util.a $(CV) $(GTK)

#UNINSTALL
clean:
	$(info Uninstalling KiwiLight Executables...)
	rm bin/*
	rm lib/*
	rm KiwiLight