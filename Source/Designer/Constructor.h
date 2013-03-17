/*
  ==============================================================================

    Constructor.h
    Created: 16 Mar 2013 4:58:15pm
    Author:  admin

  ==============================================================================
*/

#ifndef __CONSTRUCTOR_H_E78AE279__
#define __CONSTRUCTOR_H_E78AE279__

#include "Globals.h"
#include "../Modules/juced_Label.h"
#include "../Modules/juced_TextButton.h"
#include "../Modules/juced_Window.h"
#include "../Modules/juced_MainComponent.h"

/** Struct to hold all parameters of a given attribute identifier.

	@see getAttributeOf
	@see Attributes
*/
struct Attribute
{
	Identifier name;
	String group;
	bool visible;
	Identifier type;
	String display;
	Identifier valueType;
};

/**
    Object required to build components and show its properties.

    Only one instance of this object is allowed.

    To use, simply get an instance of this class using the getInstance() static method.
	Example: Constructor *constructor = Constructor::getInstance();
*/
class Constructor {
public:
	Array<Attribute*> _attributes;

	/** This function is called to create an instance of the class.

		Calling the constructor publicly is not allowed. The constructor
		is private and is only called by this Instance function.
	*/
	static Constructor* getInstance();

	/** Function called within JUCE_Designer constructor.
		
		Loads all parameters from a list of attribute types in a XML file.

		@param xmlFile File object that points to XML file in disc.
		@see getAttributeOf
	*/
	void loadAttributesFromXmlFile(const File &xmlFile);

	/** Returns a struct pointer of attribute parameters given it's Identifier.

		@param _name Attribute identifier.
		@see Attribute
		@see Attributes
	*/
	Attribute* getAttributeOf(Identifier _name);

	/** Returns a pointer to an UndoManager.

		@see UndoManager, BigTree, ValueTree
	*/
	UndoManager* getUndoManager();

private:
	ScopedPointer<UndoManager> undoManager;
	Constructor(){};  // Private so that it can  not be called
	Constructor(Constructor const&){};             // copy constructor is private
	Constructor& operator=(Constructor const&){};  // assignment operator is private
	static Constructor* m_pInstance;
};



#endif  // __CONSTRUCTOR_H_E78AE279__
