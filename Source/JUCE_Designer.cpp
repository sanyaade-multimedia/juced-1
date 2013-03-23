
#include "JUCE_Designer.h"

JUCE_Designer::JUCE_Designer ()
{
	//AlertWindow::showMessageBox(AlertWindow::NoIcon, "here we go", "commented alert for copy & paste");
	setSize (800, 600);

	addAndMakeVisible(&grid, 10);
	
	PropertyGroup *properties = new PropertyGroup();
	propertyView = new PropertyView(properties);
	propertyView->setBounds(100, 100, 200, 400);

	addAndMakeVisible(&mousePositionLabel);
	mousePositionLabel.setAlwaysOnTop(true);
	mousePositionLabel.setFont(Font(12.0f));
	mousePositionLabel.setBounds(getWidth() - 80, getHeight() - 25, 80, 25);
	mousePositionLabel.setInterceptsMouseClicks(false, false);
	
	Constructor *constructor = Constructor::getInstance();
	constructor->loadAttributesFromXmlFile(File(File::addTrailingSeparator(File::getCurrentWorkingDirectory().getFullPathName()) + "attributes.xml"));
	constructor->loadEnumerationsFromXmlFile(File(File::addTrailingSeparator(File::getCurrentWorkingDirectory().getFullPathName()) + "enumerations.xml"));
	constructor->setDesigner(this);

	SelectionArea *selectionBox = constructor->getSelectionBox();
	addAndMakeVisible(selectionBox);
	selectionBox->setVisible(false);
}

JUCE_Designer::~JUCE_Designer() { }

Component* JUCE_Designer::createToolbox (int itemsPerRow, int itemSize, int itemPadding) {
	Toolbox *toolbox = new Toolbox(itemsPerRow, itemSize, itemPadding);
	toolbox->setBounds(0, 0, 1, 1);
	toolbox->loadTooltips();
	toolboxes.add(toolbox);
	return toolbox;
}

void JUCE_Designer::addToolboxItem (Component* _toolbox, const String& name, const String& toolTip, const char* image, int imageSize) {
	Toolbox *toolbox = (Toolbox*) _toolbox;
	toolbox->addItem(name, toolTip, image, imageSize);
}

Viewport* JUCE_Designer::getPropertyView ()
{
	return propertyView;
}

String* JUCE_Designer::getSelectedToolName () {
	//for (int i = 0; i < toolboxes.size(); i++) {
	for (int i = toolboxes.size(); --i >= 0;) {
		if (toolboxes[i]->getSelectedToolName()->isNotEmpty()) {
			return toolboxes[i]->getSelectedToolName();
		}
	}
	return (new String(""));
}

void JUCE_Designer::deselectTool () {
	//for (int i = 0; i < toolboxes.size(); i++) {
	for (int i = toolboxes.size(); --i >= 0;) {	
		toolboxes[i]->deselectTool();
	}
}

void JUCE_Designer::addWindow (Component *parent, int x, int y, int width, int height)
{
	if (parent == this && bigTree != nullptr) return;	//only one main window allowed

	juced_Window *win = new juced_Window();
	parent->addAndMakeVisible(win);
	win->setBounds(x, y, (width >= win->minWidth) ? width : win->minWidth, (height >= win->minHeight) ? height : win->minHeight);
	
	win->setProperty(Attributes::x, x);
	win->setProperty(Attributes::y, y);
	win->setProperty(Attributes::width, (width >= win->minWidth) ? width : win->minWidth);
	win->setProperty(Attributes::height, (height >= win->minHeight) ? height : win->minHeight);

	

	juced_MainComponent *comp = new juced_MainComponent();
	comp->setProperty(Attributes::x, 0);
	comp->setProperty(Attributes::y, 0);
	comp->setProperty(Attributes::width, win->getWidth());
	comp->setProperty(Attributes::height, win->getHeight() - win->getTitleBarHeight());
	win->setContentOwned(comp, true);
	win->getContentComponent()->addMouseListener(this, true);
	BigTree *compTree = new BigTree(comp, comp->getProperty(Attributes::objectType));
	if (parent == this) {
		bigTree = new BigTree(win, win->getProperty(Attributes::objectType));
		Constructor::getInstance()->setBigTreeRoot(bigTree);
		bigTree->addChild(*compTree, -1, 0);
	} else {
		BigTree *objTree = new BigTree(win, win->getProperty(Attributes::objectType));
		BigTree parentTree(bigTree->getChildWithProperty(Attributes::ID, parent->getComponentID(), true));
		jassert (parentTree.isValid());		//this should not happen
		parentTree.addChild(*objTree, -1, 0);
		objTree->addChild(*compTree, -1, 0);
	}

	selectComponent(comp);

}

void JUCE_Designer::writeXmlToFile (String _filename)
{
	XmlElement *obj_xml = bigTree->createXml();

	//Create xml file from XmlElement
	File file = File(File::addTrailingSeparator(File::getCurrentWorkingDirectory().getFullPathName()) + _filename);
	file.create();
	obj_xml->writeToFile(file, "");
	file.revealToUser();
}

void JUCE_Designer::selectComponent (Component *componentToSelect)
{
	SelectionArea *selectionBox = Constructor::getInstance()->getSelectionBox();

	if (componentToSelect != this) {
		selectionBox->setVisible(false);

		/*AlertWindow::showMessageBox(AlertWindow::NoIcon, "here we go", "...");
		if (selectedComponent != nullptr)
			selectedComponent->removeComponentListener(selectionBox);
		AlertWindow::showMessageBox(AlertWindow::NoIcon, "here we go", "done");*/

		Point<int> pos = componentToSelect->getScreenPosition() - this->getScreenPosition();
		selectedComponentPositionDifference = pos - componentToSelect->getPosition();
		selectionBox->setSelectionBounds(pos.getX(), pos.getY(), componentToSelect->getWidth(), componentToSelect->getHeight());
		selectedComponent = componentToSelect;

		BigTree valueTree(bigTree->getChildWithProperty(Attributes::ID, selectedComponent->getComponentID(), true));

		PropertyGroup *properties = new PropertyGroup(&valueTree);
		propertyView->setViewedComponent(properties);
		selectionBox->setVisible(true);

		
		componentToSelect->addComponentListener(selectionBox);

	} else {
		//set selection box invisible and clean properties window since user clicked the editor itself
		selectionBox->setVisible(false);

		/*if (selectedComponent != nullptr)
			selectedComponent->removeComponentListener(selectionBox);*/

		PropertyGroup *properties = new PropertyGroup();
		propertyView->setViewedComponent(properties);
		//bring properties view to front (just in case)
		propertyView->toFront(false);
	}
}

void JUCE_Designer::paint (Graphics& g)
{
    g.fillAll (Colour((uint8) 49, (uint8) 124, (uint8) 205));
	#ifdef __JUCER_APPEARANCESETTINGS_H_34D762C7__
		IntrojucerLookAndFeel::fillWithBackgroundTexture (*this, g);
	#endif
}

void JUCE_Designer::resized()
{
	grid.setBounds(0, 0, getWidth(), getHeight());
	mousePositionLabel.setBounds(getWidth() - mousePositionLabel.getWidth(), getHeight() - mousePositionLabel.getHeight(), mousePositionLabel.getWidth(), mousePositionLabel.getHeight());
	
}

void JUCE_Designer::lookAndFeelChanged()
{

}

void JUCE_Designer::childrenChanged()
{

}

void JUCE_Designer::mouseMove (const MouseEvent& event)
{
	MouseEvent relativeEvent = event.getEventRelativeTo(this);
	mousePositionLabel.setText(String(relativeEvent.x) + " , " + String(relativeEvent.y), false);
}

void JUCE_Designer::mouseDown (const MouseEvent& event)
{
	bool ctrlKeyDown = event.mods.isCtrlDown();

	if (getSelectedToolName()->isNotEmpty()) {
		selectionArea = new SelectionArea();
		addAndMakeVisible(selectionArea);
		MouseEvent relativeEvent = event.getEventRelativeTo(this);
		selectionArea->setSelectionBounds(relativeEvent.getMouseDownX(), relativeEvent.getMouseDownY(), 1, 1, !ctrlKeyDown, !ctrlKeyDown);

	} else {
		if (event.originalComponent == selectedComponent) {
			//user is going to move the component using drag & drop
			componentPositionOnDragStart = event.originalComponent->getScreenPosition() - this->getScreenPosition();
		}
	}
	
}

void JUCE_Designer::mouseDrag (const MouseEvent& event)
{
	if (selectionArea != nullptr) {
		//creating a new component
		bool ctrlKeyDown = event.mods.isCtrlDown();
		MouseEvent relativeEvent = event.getEventRelativeTo(this);
		selectionArea->setSelectionBounds(relativeEvent.getMouseDownX(), relativeEvent.getMouseDownY(), relativeEvent.getDistanceFromDragStartX(), relativeEvent.getDistanceFromDragStartY(), !ctrlKeyDown, !ctrlKeyDown, !ctrlKeyDown, !ctrlKeyDown);

	} else if (event.originalComponent == selectedComponent) {
		//moving an existing component
		bool ctrlKeyDown = event.mods.isCtrlDown();
		Constructor::getInstance()->getSelectionBox()->setListenToChanges(false);
		
		Constructor::getInstance()->getSelectionBox()->setSelectionBounds(componentPositionOnDragStart.getX() + event.getDistanceFromDragStartX(), componentPositionOnDragStart.getY() + event.getDistanceFromDragStartY(), selectedComponent->getWidth(), selectedComponent->getHeight(), !ctrlKeyDown, !ctrlKeyDown);
	}
}

void JUCE_Designer::mouseUp (const MouseEvent& event)
{
	Constructor::getInstance()->getSelectionBox()->setListenToChanges(true);

	String *selectedToolName = getSelectedToolName();
	if (!event.mouseWasClicked()) {
		if (selectedToolName->isNotEmpty()) {

			//If user draw inside a component, let's find it's associated BigTree of the parent component
			BigTree parentTree;
			if (event.originalComponent != this) {
				BigTree bTree(bigTree->getChildWithProperty(Attributes::ID, event.originalComponent->getComponentID(), true));
				if (bTree.isValid()) parentTree = bTree;
			}

			MouseEvent relativeEvent = event.getEventRelativeTo(event.originalComponent);
			Constructor *constructor = Constructor::getInstance();

			if (selectedToolName->equalsIgnoreCase("juced_Window")) {
				addWindow(event.originalComponent, relativeEvent.getMouseDownX() - constructor->getDrawBoundsModX(), relativeEvent.getMouseDownY() - constructor->getDrawBoundsModY(), relativeEvent.getDistanceFromDragStartX() - constructor->getDrawBoundsModWidth(), relativeEvent.getDistanceFromDragStartY() - constructor->getDrawBoundsModHeight());
			} else {
				//Create a component of the selected tool name unless it is placed outside a window
				if (parentTree.isValid()) {
					Rectangle<int> bounds(relativeEvent.getMouseDownX() - constructor->getDrawBoundsModX(), relativeEvent.getMouseDownY()  - constructor->getDrawBoundsModY(), relativeEvent.getDistanceFromDragStartX() - constructor->getDrawBoundsModWidth(), relativeEvent.getDistanceFromDragStartY() - constructor->getDrawBoundsModHeight());
					Component* newComponent = constructor->createComponent(*selectedToolName, event.originalComponent->getComponentID(), bounds);
					selectComponent(newComponent);
				}
				/*if (parentTree.isValid()) {
					DynamicObject *dynamicObj;
					if ((dynamicObj = createObjectFromToolName(selectedToolName)) != nullptr) {

						event.originalComponent->addAndMakeVisible(dynamic_cast<Component *> (dynamicObj));
						(dynamic_cast<Component *> (dynamicObj))->addMouseListener(this, false);

						BigTree *objTree = new BigTree(dynamicObj, dynamicObj->getProperty(Attributes::objectType));
					
						parentTree.addChild(*objTree, -1, 0);

						objTree->setProperty(Attributes::x, relativeEvent.getMouseDownX() - constructor->getDrawBoundsModX(), 0);
						objTree->setProperty(Attributes::y, relativeEvent.getMouseDownY()  - constructor->getDrawBoundsModY(), 0);
						objTree->setProperty(Attributes::width, relativeEvent.getDistanceFromDragStartX() - constructor->getDrawBoundsModWidth(), 0);
						objTree->setProperty(Attributes::height, relativeEvent.getDistanceFromDragStartY() - constructor->getDrawBoundsModHeight(), 0);

						selectComponent(dynamic_cast<Component *> (dynamicObj));
						//PropertyGroup *properties = new PropertyGroup(objTree);
						//propertyView->setViewedComponent(properties);
						//activePropertyGroup = properties;
						//selectedComponentTree = objTree;
					}
				}*/
			}
			selectionArea = nullptr;
		}
	} else {
		if (selectedToolName->isEmpty()) {
			//Selecting an object
			selectComponent(event.originalComponent);
		}
	}
	deselectTool();
}

void JUCE_Designer::mouseDoubleClick (const MouseEvent& event)
{
	Time e = event.eventTime;	//useless - to avoid warnings
}

bool JUCE_Designer::keyPressed (const KeyPress& key)
{
	mousePositionLabel.setText(String(key.getKeyCode()), 0);
	if (key.getKeyCode() == 90 && key.getModifiers().isCtrlDown()) {
		Constructor::getInstance()->getUndoManager()->undo();
	} else if (key.getKeyCode() == 89 && key.getModifiers().isCtrlDown()) {
		Constructor::getInstance()->getUndoManager()->redo();
	} else if (key.getKeyCode() == 82 && key.getModifiers().isCtrlDown()) {
		int rand = 0;
		while ((rand = Random::getSystemRandom().nextInt() % 21) < 4);
		Constructor::getInstance()->setGridSize(rand);
		grid.repaint();
		mousePositionLabel.setText(String(Constructor::getInstance()->getGridSize()), 0);
	} else if (key.getKeyCode() == 83 && key.getModifiers().isCtrlDown()) {
		this->writeXmlToFile("save.xml");
	} else if (key.getKeyCode() == key.leftKey) {
		if (selectedComponent != nullptr) {
			BigTree valueTree(bigTree->getChildWithProperty(Attributes::ID, selectedComponent->getComponentID(), true));
			int currentPosX = valueTree.getProperty(Attributes::x);
			valueTree.setProperty(Attributes::x, currentPosX - 1, Constructor::getInstance()->getUndoManager());
		}
	} else if (key.getKeyCode() == key.rightKey) {
		if (selectedComponent != nullptr) {
			BigTree valueTree(bigTree->getChildWithProperty(Attributes::ID, selectedComponent->getComponentID(), true));
			int currentPosX = valueTree.getProperty(Attributes::x);
			valueTree.setProperty(Attributes::x, currentPosX + 1, Constructor::getInstance()->getUndoManager());
		}
	} else if (key.getKeyCode() == key.upKey) {
		if (selectedComponent != nullptr) {
			BigTree valueTree(bigTree->getChildWithProperty(Attributes::ID, selectedComponent->getComponentID(), true));
			int currentPosY = valueTree.getProperty(Attributes::y);
			valueTree.setProperty(Attributes::y, currentPosY - 1, Constructor::getInstance()->getUndoManager());
		}
	} else if (key.getKeyCode() == key.downKey) {
		if (selectedComponent != nullptr) {
			BigTree valueTree(bigTree->getChildWithProperty(Attributes::ID, selectedComponent->getComponentID(), true));
			int currentPosY = valueTree.getProperty(Attributes::y);
			valueTree.setProperty(Attributes::y, currentPosY + 1, Constructor::getInstance()->getUndoManager());
		}
	}
    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
}

void JUCE_Designer::focusOfChildComponentChanged (FocusChangeType cause)
{
	if (cause == 1) {}	//useless - to avoid warnings
}

void JUCE_Designer::focusGained (FocusChangeType cause)
{
	if (cause == 1) {}	//useless - to avoid warnings
}

void JUCE_Designer::childBoundsChanged (Component * child)
{
	SelectionArea *selectionBox = Constructor::getInstance()->getSelectionBox();

	if (child == selectionBox) {
		if (selectedComponent != nullptr && selectionBox->isReady()) {

			BigTree valueTree(bigTree->getChildWithProperty(Attributes::ID, selectedComponent->getComponentID(), true));
			valueTree.setProperty(Attributes::width, child->getWidth() - (selectionBox->getBoxSize() * 2), Constructor::getInstance()->getUndoManager());
			valueTree.setProperty(Attributes::height, child->getHeight() - (selectionBox->getBoxSize() * 2), Constructor::getInstance()->getUndoManager());
			valueTree.setProperty(Attributes::x, child->getX() + selectionBox->getBoxSize() - selectedComponentPositionDifference.getX(), Constructor::getInstance()->getUndoManager());
			valueTree.setProperty(Attributes::y, child->getY() + selectionBox->getBoxSize() - selectedComponentPositionDifference.getY(), Constructor::getInstance()->getUndoManager());

		}
	}

}


//====================private methods================================

DynamicObject* JUCE_Designer::createObjectFromToolName (String *selectedToolName)
{
	if (selectedToolName->equalsIgnoreCase("juced_Label")) {
		juced_Label *object = new juced_Label();
		return (DynamicObject *)object;
	} else if (selectedToolName->equalsIgnoreCase("juced_TextButton")) {
		juced_TextButton *object = new juced_TextButton();
		return (DynamicObject *)object;
	}
	return nullptr;
}

//===================================================================

void JUCE_Designer::Grid::paint (Graphics& g)
{
	float gridSize = (float) Constructor::getInstance()->getGridSize();
	int doubleLineInterval = 85 - (85 % (int) gridSize);
	g.fillAll (Colours::transparentWhite);
    g.setColour (Colours::white);
	for (float x = gridSize; x < (float) getWidth(); x += gridSize) {
		g.setOpacity((((int) x) % doubleLineInterval == 0) ? 0.15f : 0.06f);
		g.drawVerticalLine(x, 0.0f, (float) getHeight());
		g.drawHorizontalLine(x, 0.0f, (float) getWidth());
		if (((int) x) % doubleLineInterval == 0) {
			g.setOpacity(0.35f);
			for (float i = (float) (int)(getHeight() / doubleLineInterval + 1); --i > 0;) {
				g.drawVerticalLine(x, ((float) doubleLineInterval) * i - 3.0f, ((float) doubleLineInterval) * i + 4.0f);
				g.drawHorizontalLine(((float) doubleLineInterval) * i, x - 3.0f, x + 4.0f);
			}
		}
	}
}

JUCE_Designer::Grid::Grid ()
{
	setAlwaysOnTop(true);
	setInterceptsMouseClicks(false, false);
}