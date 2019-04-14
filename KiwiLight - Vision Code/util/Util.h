#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "opencv2/opencv.hpp"


using namespace cv;

namespace KiwiLight {
    
    /**
     * An easy event and variable flagging system
     */
    class Flags {
        public:
        static void RaiseFlag(std::string flagName);
        static void LowerFlag(std::string flagName);
        static void SetFlagState(std::string flagName, bool state);
        static bool GetFlag(std::string flagName);

        private:
        static std::vector<std::string> flagNames;
    };

    /**
     * Class that can execute shell commands and return their output.
     */
    class Shell {
        public:
        static std::string ExecuteCommand(std::string command);
    };

    class StringUtils {
        public:
        static std::vector<std::string> SplitString(std::string str, char character);
        static std::string Substring(std::string str, int begin, int end);
        static bool StringStartsWith(std::string str, std::string startsWith);
    };



    class XMLTagAttribute {
        public:
        XMLTagAttribute(std::string name, std::string value);
        std::string Name() { return this->name; };
        std::string Value() { return this->value; };
        std::string ReturnString();

        private:
        std::string name;
        std::string value;
    };

    /**
     * Represents a tag formatted in XML that may be found as a child of others (or contain others)
     */
    class XMLTag {
        public:
        XMLTag(std::string name);
        XMLTag(std::string name, std::vector<XMLTagAttribute> attributes);
        XMLTag(std::string name, std::vector<XMLTag> children);
        XMLTag(std::string name, std::vector<XMLTag> children, std::vector<XMLTagAttribute> attributes);
        XMLTag(std::string name, std::string content, std::vector<XMLTagAttribute> attributes);
        XMLTag(std::string name, std::string content);
        void AddTag(XMLTag tag);
        void AddAttribute(XMLTagAttribute attribute);
        void AddContent(std::string content);
        std::vector<XMLTag> GetTagsByName(std::string name);
        std::vector<XMLTag> Tags() { return this->children; };
        std::vector<XMLTagAttribute> GetAttributesByName(std::string name);
        std::vector<XMLTagAttribute> Attributes() { return this->attributes; };
        std::string Name() { return this->name; };
        std::string Content() { return this->content; };
        std::string ReturnString(std::string prefix);
        std::string ReturnString();

        private:
        bool containsChildren;
        std::string name;
        std::string content;
        std::vector<XMLTag> children;
        std::vector<XMLTagAttribute> attributes;
    };

    /**
     * Represents a text document formatted in XML
     */
    class XMLDocument {
        public:
        XMLDocument();
        XMLDocument(std::vector<XMLTag> tags);
        XMLDocument(std::string filePath);
        void AddTag(XMLTag tag);
        std::vector<XMLTag> GetTagsByName(std::string name);
        std::string ReturnString();
        void WriteFile(std::string filePath);

        private:
        std::vector<XMLTag> children;
    };

}

#endif