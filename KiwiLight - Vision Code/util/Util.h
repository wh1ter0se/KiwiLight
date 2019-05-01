#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "gtk-3.0/gtk/gtk.h"
#include "opencv2/opencv.hpp"
#include "netdb.h"
#include "unistd.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"


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
        static int CountCharacters(std::string str, char character);
    };

    /**
     * A UDP sender utility that sends and recieves information to and from the RIO.
     */
    class UDP {
        public:
        UDP(){};
        UDP(std::string dest_ip, int port);
        void Send(std::string msg);
        std::string Recieve();
        void Close();

        private:
        int sock; //sock fd returned by socket() call
        sockaddr_in server_address; //address of the server
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
        bool HasContents() { return this->hasContents; };
        void AddTag(XMLTag tag);
        std::vector<XMLTag> GetTagsByName(std::string name);
        std::string ReturnString();
        void WriteFile(std::string filePath);

        private:
        bool hasContents;
        std::vector<XMLTag> children;
    };

    /**
     * Represents a single value with an error.
     */
    class SettingPair {
        public:
        SettingPair() {};
        SettingPair(double value, double error);
        double UpperBound();
        double LowerBound();
        double Value() { return this->value; };
        double Error() { return this->error; };

        private:
        double value,
               error; 
    };

    /**
     * Represents a color in the HSV colorspace
     */
    class Color {
        public:
        Color() {};
        Color(int h, int s, int v, int hError, int sError, int vError);

        cv::Scalar ReturnScalar();
        cv::Scalar GetLowerBound();
        cv::Scalar GetUpperBound();
        int GetH() { return this->h; };
        int GetS() { return this->s; };
        int GetV() { return this->v; };
        int GetHError() { return this->hError; };
        int GetSError() { return this->sError; };
        int GetVError() { return this->vError; };

        private:
        int h,
            s,
            v,
            hError,
            sError,
            vError;
    };

    /**
     * Represents an image which can be displayed in an ImageFrame
     */
    class Image {
        public:
        Image(std::string fileName);
        Image(cv::Mat img);
        GdkPixbuf *ReturnImage() { return this->img; };

        private:
        GdkPixbuf *img;
    };
}

#endif