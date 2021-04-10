#include "Util.h"

/**
 * Source file for the XMLTag class.
 * Written by: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new XMLTag with the given name.
 */
XMLTag::XMLTag(std::string name) {
    this->containsChildren = true;
    this->name = name;
}

/**
 * Creates a new XMLTag with the given name and attributes.
 */
XMLTag::XMLTag(std::string name, std::vector<XMLTagAttribute> attributes) {
    this->containsChildren = true;
    this->name = name;
    this->attributes = attributes;
}

/**
 * Creates a new XMLTag with the given name and children.
 */
XMLTag::XMLTag(std::string name, std::vector<XMLTag> children) {
    this->containsChildren = true;
    this->name = name;
    this->children = children;
}

/**
 * Creates a new XMLTag with the given name, children, and attributes.
 */
XMLTag::XMLTag(std::string name, std::vector<XMLTag> children, std::vector<XMLTagAttribute> attributes) {
    this->containsChildren = true;
    this->name = name;
    this->children = children;
    this->attributes = attributes;
}

/**
 * Creates a new XMLTag with the given name, content, and attributes.
 */
XMLTag::XMLTag(std::string name, std::string content, std::vector<XMLTagAttribute> attributes) {
    this->containsChildren = false;
    this->name = name;
    this->content = content;
    this->attributes = attributes;
}

/**
 * Creates a new XMLTag with the given name and content.
 */
XMLTag::XMLTag(std::string name, std::string content) {
    this->containsChildren = false;
    this->name = name;
    this->content = content;
}

/**
 * Adds a child tag to this XMLTag.
 */
void XMLTag::AddTag(XMLTag tag) {
    this->containsChildren = true;
    this->children.push_back(tag);
}

/**
 * Add an attribute to this XMLTag.
 */
void XMLTag::AddAttribute(XMLTagAttribute attribute) {
    this->attributes.push_back(attribute);
}

/**
 * Adds content to this XMLTag.
 */
void XMLTag::AddContent(std::string content) {
    this->containsChildren = false;
    this->content += content;
}

/**
 * Searches existing children and returns all child tags with names matching "name."
 * @param name The XML Tag name to search for.
 */
std::vector<XMLTag> XMLTag::GetTagsByName(std::string name) {
    std::vector<XMLTag> results = std::vector<XMLTag>();

    for(int i=0; i<this->children.size(); i++) {
        if(this->children[i].Name() == name) {
            results.push_back(this->children[i]);
        }
    }

    if(results.size() == 0) {
        std::cout << "There were no XML Tag results for \"" << name << "\"" << std::endl;
        XMLTag placeholder = XMLTag(name, "NULL");
        results.push_back(placeholder);
    }

    return results;
}

/**
 * Searches existing attributes and returns all attributes with names matching "name"
 * @name The XML Tag Attribute name to search for.
 */
std::vector<XMLTagAttribute> XMLTag::GetAttributesByName(std::string name) {
    std::vector<XMLTagAttribute> results = std::vector<XMLTagAttribute>();

    for(int i=0; i<this->attributes.size(); i++) {
        if(this->attributes[i].Name() == name) {
            results.push_back(this->attributes[i]);
        }
    }

    if(results.size() == 0) {
        std::cout << "There were no XML Attribute results for \"" << name << "\"" << std::endl;
        XMLTagAttribute placeholder = XMLTagAttribute(name, "0");
        results.push_back(placeholder);
    }

    return results;
}

/**
 * Returns a std::string encoded in XML representing this tag.
 * @param prefix A string prefix to insert before the tag. This is used to add tabs to the document.
 */
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

/**
 * Returns a std::string encoded in XML representing this tag.
 */
std::string XMLTag::ReturnString() {
    return this->ReturnString("");
}
