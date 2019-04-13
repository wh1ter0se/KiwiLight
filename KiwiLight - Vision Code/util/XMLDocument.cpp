#include "Util.h"

/**
 * Source file for the XMLDocument class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a blank XML document.
 */
XMLDocument::XMLDocument() {
}

/**
 * Creates an XML document starting with the given tags.
 */
XMLDocument::XMLDocument(std::vector<XMLTag> tags) {
    this->children = tags;
}

/**
 * Creates a new XML document using the formatted file passed.
 */
XMLDocument::XMLDocument(std::string fileName) {
    std::fstream log = std::fstream(fileName);
    std::vector<std::string> fileLines = std::vector<std::string>();

    //loop through and read every line, formatting into std::string separated by newline characters
    std::string line;
    while(std::getline(log, line)) {
        //get rid of spaces at beginning of each line
        int beginningOfContent = 0;
        std::string trimmedContent = "";
        for(int i=0; i<line.length(); i++) {
            if(line.at(i) != ' ') {
                beginningOfContent = i;
                break;
            }
        }

        for(int k=beginningOfContent; k<line.length(); k++) {
            trimmedContent += line.at(k);
        }
        
        fileLines.push_back(trimmedContent);
    }

    //parse the vector of lines and build a hierarchy of xml tags for this document
    std::vector<XMLTag> workingHierarchy = std::vector<XMLTag>(); //the hierarchy of tags to be completed
    for(int i=0; i<fileLines.size(); i++) {
        // std::cout << "1" << std::endl;
        std::string line = fileLines[i];
        if(StringUtils::StringStartsWith(line, "<") && !StringUtils::StringStartsWith(line, "<!--")) {
            //beginning of tag, but not a comment, parse as new tag
            // std::cout << "2" << std::endl;
            std::string tagName = "";
            std::vector<XMLTagAttribute> attrs = std::vector<XMLTagAttribute>();
            line = StringUtils::Substring(line, 1, line.length() - 1); //take off brackets

            // std::cout << "3" << std::endl;
            if(StringUtils::StringStartsWith(line, "/")) {
                // std::cout << "4" << std::endl;
                //ending tag, do things
                if(workingHierarchy.size() <= 1) {
                    this->AddTag(workingHierarchy[workingHierarchy.size() - 1]);
                }
                else {
                    workingHierarchy[workingHierarchy.size() - 2].AddTag(workingHierarchy[workingHierarchy.size() - 1]);
                }

                std::vector<XMLTag> newHierarchy = std::vector<XMLTag>();

                // std::cout << "5" << std::endl;

                int size = workingHierarchy.size();
                // std::cout << "working hierarchy size: " << std::to_string(size) << std::endl;

                for(int k=0; k<workingHierarchy.size() - 1; k++) {
                    // std::cout << "6" << std::endl;
                    //reconstruct the hierarchy without the most recent object
                    newHierarchy.push_back(workingHierarchy[k]);
                }

                // std::cout << "7" << std::endl;
                workingHierarchy = newHierarchy;
            } 
            else {
                // std::cout << "8" << std::endl;
                std::vector<std::string> nameAndAttrs = StringUtils::SplitString(line, ' ');
                tagName = nameAndAttrs[0];

                // std::cout << "9" << std::endl;

                std::vector<XMLTagAttribute> newAttrs = std::vector<XMLTagAttribute>();
                for(int k=1; k<nameAndAttrs.size(); k++) {
                    // std::cout << "10" << std::endl;
                    std::string attr = nameAndAttrs[k];
                    std::string attrName = StringUtils::SplitString(attr, '=')[0];
                    std::string attrValue = StringUtils::SplitString(attr, '=')[1];
                    attrValue = StringUtils::Substring(attrValue, 1, attrValue.size() - 1); //get rid of quotes

                    // std::cout << "11" << std::endl;
                    XMLTagAttribute newAttr = XMLTagAttribute(attrName, attrValue);
                    newAttrs.push_back(newAttr);
                }
                // std::cout << "12" << std::endl;
                XMLTag newTag = XMLTag(tagName, newAttrs);
                workingHierarchy.push_back(newTag);
            }
        } else if(!StringUtils::StringStartsWith(line, "<!--")) {
            // std::cout << "13" << std::endl;
            if(workingHierarchy.size() > 0) {
                // std::cout << "14" << std::endl;
                workingHierarchy[workingHierarchy.size() - 1].AddContent(line);
            }
        }
        
    }
}

/**
 * Adds the passed tag to the XML document.
 */
void XMLDocument::AddTag(XMLTag tag) {
    this->children.push_back(tag);
}

/**
 * Returns a list of XML tags with the given name.
 */
std::vector<XMLTag> XMLDocument::GetTagsByName(std::string name) {
    std::vector<XMLTag> results = std::vector<XMLTag>();

    for(int i=0; i<this->children.size(); i++) {
        if(this->children[i].Name() == name) {
            results.push_back(this->children[i]);
        }
    }

    return results;
}

/**
 * Returns a string formatted in XML representing the current object.
 */
std::string XMLDocument::ReturnString() {
    std::string results = "";

    for(int i=0; i<this->children.size(); i++) {
        results += this->children[i].ReturnString();
        results += "\n";
    }

    return results;
}

/**
 * Writes the XML document to the given file.
 */
void XMLDocument::WriteFile(std::string fileName) {
    std::ofstream file = std::ofstream(fileName.c_str(), std::ofstream::out);
    std::string toWrite = this->ReturnString();
    file.write(toWrite.c_str(), toWrite.length());

    file.close();
}