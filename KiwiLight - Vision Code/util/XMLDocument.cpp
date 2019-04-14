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
        // std::cout << "wh size: " << workingHierarchy.size() << std::endl;
        std::string line = fileLines[i];
        if(StringUtils::StringStartsWith(line, "<") && !StringUtils::StringStartsWith(line, "<!--")) {
            // std::cout << "2" << std::endl;
            int closingBracket = line.find('>');
            std::string tag = StringUtils::Substring(line, 1, closingBracket);
            std::string content = StringUtils::Substring(line, closingBracket + 1, line.length());
            // std::cout << "tag: \"" << tag << "\"" << std::endl;
            // std::cout << tag << std::endl;
            // std::cout << "3" << std::endl;
            if(StringUtils::StringStartsWith(tag, "/")) {
                //ending tag, rebuild the hierarchy without this tag
                // std::cout << "4" << std::endl;
                if(workingHierarchy.size() > 1) {
                    // std::cout << "5a" << std::endl;
                    XMLTag addTag = workingHierarchy[workingHierarchy.size() - 1];
                    workingHierarchy[workingHierarchy.size() - 2].AddTag(addTag);

                    // std::cout << "5b" << std::endl;
                } else {
                    // std::cout << "5c" << std::endl;
                    // std::cout << "adding tag to master" << std::endl;
                    this->AddTag(workingHierarchy[0]);
                    // std::cout << "5d" << std::endl;
                }
                // std::cout << "5e" << std::endl;
                std::vector<XMLTag> newHierarchy = std::vector<XMLTag>();
                for(int k=0; k<workingHierarchy.size() - 1; k++) {
                    // std::cout << "5f" << std::endl;
                    newHierarchy.push_back(workingHierarchy[k]);
                    // std::cout << "5g" << std::endl;
                }
                // std::cout << "5h" << std::endl;
                workingHierarchy = newHierarchy;
            } else {
                // std::cout << "6" << std::endl;
                std::vector<std::string> nameAndAttrs = StringUtils::SplitString(tag, ' ');
                std::vector<XMLTagAttribute> attrs = std::vector<XMLTagAttribute>();
                std::string name = nameAndAttrs[0];

                // std::cout << "7" << std::endl;
                for(int k=1; k<nameAndAttrs.size(); k++) {
                    // std::cout << "8" << std::endl;
                    std::string attrString = nameAndAttrs[k];
                    std::string attrName = StringUtils::SplitString(attrString, '=')[0];
                    std::string attrValue = StringUtils::SplitString(attrString, '=')[1];

                    //take off quotes 
                    attrValue = StringUtils::Substring(attrValue, 1, attrValue.length()-1);
                    XMLTagAttribute newAttr = XMLTagAttribute(attrName, attrValue);
                    attrs.push_back(newAttr);
                    // std::cout << "9" << std::endl;
                }
                // std::cout << "10" << std::endl;
                XMLTag newTag = XMLTag(name, attrs);
                // std::cout << "11" << std::endl;
                if(content.length() > 0) {
                    // std::cout << "12" << std::endl;
                    //get the index of the starting bracket of the closing tag
                    int closingTag = content.find("</" + newTag.Name() + ">");
                    content = StringUtils::Substring(content, 0, closingTag);
                    newTag.AddContent(content);
                    
                    if(workingHierarchy.size() > 0) {
                        workingHierarchy[workingHierarchy.size() - 1].AddTag(newTag);
                    } else {
                        this->AddTag(newTag);
                    }

                    // std::cout << "13" << std::endl;
                } else {
                    workingHierarchy.push_back(newTag);
                }
            }
        } else {
            // std::cout << "12" << std::endl;
            if(!StringUtils::StringStartsWith(line, "<!--")) {
                // std::cout << "13" << std::endl;
                //line is not comment, parse as content for the current tag
                workingHierarchy[workingHierarchy.size()-1].AddContent(line);
                // std::cout << "14" << std::endl;
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