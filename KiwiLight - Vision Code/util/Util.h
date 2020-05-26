#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include "opencv2/opencv.hpp"
#include "netdb.h"
#include "unistd.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "arpa/inet.h"

using namespace cv;

namespace KiwiLight {

    struct Distance {
        Distance(double x, double y) {
            this->x = x;
            this->y = y;
        }
        double x;
        double y;
    };

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


    class DataUtils {
        public:
        static std::vector<double> SortLeastGreatestDouble(std::vector<double> data);
        static double MaxWithoutOutliers(std::vector<double> data, double allowableDeviation);
        static double Total(std::vector<double> data);
        static double Average(std::vector<double> data);
        static double Median(std::vector<double> data);
        static double Greatest(std::vector<double> data);
        static double Least(std::vector<double> data);
        static double AverageDifference(std::vector<double> data);
        static int NumberOfOccurrances(std::vector<double> data, double value);
        static double MostCommonValue(std::vector<double> data);
        static bool IsOutlier(std::vector<double> data, int indexOfValue, double allowableError);
        static std::vector<double> RemoveOutliers(std::vector<double> data, double allowableError);
        static std::vector<double> RemoveOccurances(std::vector<double> data, double occurance);
        static std::string VectorToString(std::vector<double> data);

        std::vector<int> VectorDoubleToInt(std::vector<double> data);
        std::vector<double> VectorIntToDouble(std::vector<int> data);
    };


    /**
     * A UDP sender utility that sends and recieves information to and from the RIO.
     */
    class UDP {
        public:
        UDP(){};
        UDP(std::string dest_ip, int port, bool blockUntilConnected);
        UDP(std::string this_ip, std::string dest_ip, int port, bool blockUntilConnected);
        bool AttemptToConnect();
        bool Connected() { return this->connected; };
        void Send(std::string msg);
        std::string Recieve();
        void Close();
        std::string GetAddress() { return this->address; };
        int GetPort() { return this->port; };

        private:
        int sock; //sock fd returned by socket() call
        sockaddr_in client_address; //address of the server

        bool connected;

        std::string address;
        int port;
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
        XMLTag() {};
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
        std::string FileName() { return this->fileName; };
        std::string ReturnString();
        void WriteFile(std::string filePath);

        private:
        std::string fileName;
        bool hasContents;
        std::vector<XMLTag> children;
    };

    /**
     * General util class containing useful methods used by multiple classes
     */
    class Util {
        public:
        static XMLTag SearchCameraSettingsByID(std::vector<XMLTag> settings, int id);
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
     * Basic timer utility, which increments time since start() is called. 
     */
    class Clock {
        public:
        Clock();
        void Start();
        long GetTime();
        static long GetSystemTime();
        static std::string GetDateString();

        private:
        long startTime;
    };

    /**
     * Logger Event, such as a general update, or a record time or distance.
     */
    class LogEvent {
        public:
        static const std::string
            RECORD_LOW_FPS,
            RECORD_HIGH_FPS,
            RECORD_LOW_DIST,
            RECORD_HIGH_DIST,
            GENERAL_UPDATE;

        LogEvent() {};
        LogEvent(const std::string evName, long timestamp, double record); //for RECORD events only
        LogEvent(const std::string evName, long timestamp, double fps, int distance, bool targetSeen); // for GENERAL_UPDATE event only
        std::string GetEventType();
        long GetTimestamp();
        double GetRecord();
        double GetFPS();
        int GetDistance();
        bool GetTargetSeen();
        XMLTag EncodeXMLTag();

        private:
        void InitBlank(const std::string evName, long timetamp);
        std::string evName;
        long timestamp;
        
        //record events
        double record;

        //generate update events
        double fps;
        int distance;
        bool targetSeen;
    };
}

#endif
