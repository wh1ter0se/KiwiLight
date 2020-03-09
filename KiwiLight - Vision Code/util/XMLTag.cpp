#include "Util.h"

/**
 * Source file for the XMLTag class.
 * Written by: Brach Knutson
 */

using namespace KiwiLight;


XMLTag::XMLTag(std::string name) {
    this->containsChildren = true;
    this->name = name;
}


XMLTag::XMLTag(std::string name, std::vector<XMLTagAttribute> attributes) {
    this->containsChildren = true;
    this->name = name;
    this->attributes = attributes;
}


XMLTag::XMLTag(std::string name, std::vector<XMLTag> children) {
    this->containsChildren = true;
    this->name = name;
    this->children = children;
}


XMLTag::XMLTag(std::string name, std::vector<XMLTag> children, std::vector<XMLTagAttribute> attributes) {
    this->containsChildren = true;
    this->name = name;
    this->children = children;
    this->attributes = attributes;
}


XMLTag::XMLTag(std::string name, std::string content, std::vector<XMLTagAttribute> attributes) {
    this->containsChildren = false;
    this->name = name;
    this->content = content;
    this->attributes = attributes;
}


XMLTag::XMLTag(std::string name, std::string content) {
    this->containsChildren = false;
    this->name = name;
    this->content = content;
}


void XMLTag::AddTag(XMLTag tag) {
    this->containsChildren = true;
    this->children.push_back(tag);
}


void XMLTag::AddAttribute(XMLTagAttribute attribute) {
    this->attributes.push_back(attribute);
}


void XMLTag::AddContent(std::string content) {
    this->containsChildren = false;
    this->content += content;
}


std::vector<XMLTag> XMLTag::GetTagsByName(std::string name) {
    std::vector<XMLTag> results = std::vector<XMLTag>();

    for(int i=0; i<this->children.size(); i++) {
        if(this->children[i].Name() == name) {
            results.push_back(this->children[i]);
        }
    }

    if(results.size() == 0) {
        std::cout << "There were no XML Tag results for \"" << name << "\"" << std::endl;
    }

    return results;
}


std::vector<XMLTagAttribute> XMLTag::GetAttributesByName(std::string name) {
    std::vector<XMLTagAttribute> results = std::vector<XMLTagAttribute>();

    for(int i=0; i<this->attributes.size(); i++) {
        if(this->attributes[i].Name() == name) {
            results.push_back(this->attributes[i]);
        }
    }

    if(results.size() == 0) {
        std::cout << "There were no XML Attribute results for \"" << name << "\"" << std::endl;
    }

    return results;
}


std::string XMLTag::ReturnString(std::string prefix) {
    std::string finalString = prefix + "<" + this->name;
    for(int i=0; i<this->attributes.size(); i++) {
        //add a space to buffer between the name / other attributes, or this attribute.
        finalString += " ";
        finalString += this->attributes[i].ReturnString();
    }

    finalString += ">";

    if(this->containsChildren) {
        for(int i=0; i<this->children.size(); i++) {
            finalString += "\n";
            finalString += this->children[i].ReturnString(prefix + "    ");
        }

        finalString += "\n" + prefix + "</" + this->name + ">";
    } else {
        finalString += this->content;
        finalString += "</" + this->name + ">";
    }

    return finalString;
}


std::string XMLTag::ReturnString() {
    return this->ReturnString("");
}
