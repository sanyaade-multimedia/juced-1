/*
  ==============================================================================

    juced_ListBox.cpp
    Created: 30 Apr 2013 12:39:51am
    Author:  admin

  ==============================================================================
*/

#include "juced_ListBox.h"

int juced_ListBox::_numObjects = 0; 

juced_ListBox::juced_ListBox()  : ListBox ()
{
	//setText("List Box", false);
	setName("ListBox");
	setProperty(Attributes::objectType, Modules::ListBox.toString());
	//setProperty(Attributes::text, getText());
	setProperty(Attributes::backgroundColour, findColour(backgroundColourId).toDisplayString(true));
	setProperty(Attributes::textColour, findColour(textColourId).toDisplayString(true));
	//setProperty(Attributes::showEditor, false);
	//setProperty(Attributes::editable, false);
	setProperty(Attributes::height, getHeight());
	setProperty(Attributes::width, getWidth());
	setProperty(Attributes::name, getName());
	++_numObjects;
	setProperty(Attributes::varName, getName() + String(_numObjects));
	setProperty(Attributes::className, "ListBox");
	setProperty(Attributes::toolName, "juced_ListBox");
	setProperty(Attributes::declareExtended, false);
	setComponentID(Uuid().toString());
	setProperty(Attributes::ID, getComponentID());
}

void juced_ListBox::mouseUp (const MouseEvent& event) {}
void juced_ListBox::mouseDrag (const MouseEvent& event) {}
void juced_ListBox::mouseDoubleClick (const MouseEvent& event) {}
void juced_ListBox::mouseMove (const MouseEvent& event) {}
void juced_ListBox::mouseDown (const MouseEvent& event) {}
void juced_ListBox::mouseEnter (const MouseEvent& event) {}
void juced_ListBox::mouseExit (const MouseEvent& event) {}
void juced_ListBox::mouseWheelMove (const MouseEvent &event, const MouseWheelDetails &wheel) {}