/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_COMBOBOX_JUCEHEADER__
#define __JUCE_COMBOBOX_JUCEHEADER__

#include "juce_Label.h"
#include "../../../text/juce_StringArray.h"
#include "../../../containers/juce_Value.h"
class ComboBox;


//==============================================================================
/**
    A class for receiving events from a ComboBox.

    You can register a ComboBoxListener with a ComboBox using the ComboBox::addListener()
    method, and it will be called when the selected item in the box changes.

    @see ComboBox::addListener, ComboBox::removeListener
*/
class JUCE_API  ComboBoxListener
{
public:
    /** Destructor. */
    virtual ~ComboBoxListener() {}

    /** Called when a ComboBox has its selected item changed.
    */
    virtual void comboBoxChanged (ComboBox* comboBoxThatHasChanged) = 0;
};


//==============================================================================
/**
    A component that lets the user choose from a drop-down list of choices.

    The combo-box has a list of text strings, each with an associated id number,
    that will be shown in the drop-down list when the user clicks on the component.

    The currently selected choice is displayed in the combo-box, and this can
    either be read-only text, or editable.

    To find out when the user selects a different item or edits the text, you
    can register a ComboBoxListener to receive callbacks.

    @see ComboBoxListener
*/
class JUCE_API  ComboBox  : public Component,
                            public SettableTooltipClient,
                            private LabelListener,
                            private AsyncUpdater,
                            private Value::Listener
{
public:
    //==============================================================================
    /** Creates a combo-box.

        On construction, the text field will be empty, so you should call the
        setSelectedId() or setText() method to choose the initial value before
        displaying it.

        @param componentName    the name to set for the component (see Component::setName())
    */
    explicit ComboBox (const String& componentName = String::empty);

    /** Destructor. */
    ~ComboBox();

    //==============================================================================
    /** Sets whether the test in the combo-box is editable.

        The default state for a new ComboBox is non-editable, and can only be changed
        by choosing from the drop-down list.
    */
    void setEditableText (bool isEditable);

    /** Returns true if the text is directly editable.
        @see setEditableText
    */
    bool isTextEditable() const throw();

    /** Sets the style of justification to be used for positioning the text.

        The default is Justification::centredLeft. The text is displayed using a
        Label component inside the ComboBox.
    */
    void setJustificationType (const Justification& justification) throw();

    /** Returns the current justification for the text box.
        @see setJustificationType
    */
    const Justification getJustificationType() const throw();

    //==============================================================================
    /** Adds an item to be shown in the drop-down list.

        @param newItemText      the text of the item to show in the list
        @param newItemId        an associated ID number that can be set or retrieved - see
                                getSelectedId() and setSelectedId()
        @see setItemEnabled, addSeparator, addSectionHeading, removeItem, getNumItems, getItemText, getItemId
    */
    void addItem (const String& newItemText,
                  int newItemId) throw();

    /** Adds a separator line to the drop-down list.

        This is like adding a separator to a popup menu. See PopupMenu::addSeparator().
    */
    void addSeparator() throw();

    /** Adds a heading to the drop-down list, so that you can group the items into
        different sections.

        The headings are indented slightly differently to set them apart from the
        items on the list, and obviously can't be selected. You might want to add
        separators between your sections too.

        @see addItem, addSeparator
    */
    void addSectionHeading (const String& headingName) throw();

    /** This allows items in the drop-down list to be selectively disabled.

        When you add an item, it's enabled by default, but you can call this
        method to change its status.

        If you disable an item which is already selected, this won't change the
        current selection - it just stops the user choosing that item from the list.
    */
    void setItemEnabled (int itemId,
                         bool shouldBeEnabled) throw();

    /** Changes the text for an existing item.
    */
    void changeItemText (int itemId,
                         const String& newText) throw();

    /** Removes all the items from the drop-down list.

        If this call causes the content to be cleared, then a change-message
        will be broadcast unless dontSendChangeMessage is true.

        @see addItem, removeItem, getNumItems
    */
    void clear (bool dontSendChangeMessage = false);

    /** Returns the number of items that have been added to the list.

        Note that this doesn't include headers or separators.
    */
    int getNumItems() const throw();

    /** Returns the text for one of the items in the list.

        Note that this doesn't include headers or separators.

        @param index    the item's index from 0 to (getNumItems() - 1)
    */
    const String getItemText (int index) const throw();

    /** Returns the ID for one of the items in the list.

        Note that this doesn't include headers or separators.

        @param index    the item's index from 0 to (getNumItems() - 1)
    */
    int getItemId (int index) const throw();

    /** Returns the index in the list of a particular item ID.
        If no such ID is found, this will return -1.
    */
    int indexOfItemId (int itemId) const throw();

    //==============================================================================
    /** Returns the ID of the item that's currently shown in the box.

        If no item is selected, or if the text is editable and the user
        has entered something which isn't one of the items in the list, then
        this will return 0.

        @see setSelectedId, getSelectedItemIndex, getText
    */
    int getSelectedId() const throw();

    /** Returns a Value object that can be used to get or set the selected item's ID.

        You can call Value::referTo() on this object to make the combo box control
        another Value object.
    */
    Value& getSelectedIdAsValue() throw()                   { return currentId; }

    /** Sets one of the items to be the current selection.

        This will set the ComboBox's text to that of the item that matches
        this ID.

        @param newItemId                the new item to select
        @param dontSendChangeMessage    if set to true, this method won't trigger a
                                        change notification
        @see getSelectedId, setSelectedItemIndex, setText
    */
    void setSelectedId (int newItemId,
                        bool dontSendChangeMessage = false) throw();

    //==============================================================================
    /** Returns the index of the item that's currently shown in the box.

        If no item is selected, or if the text is editable and the user
        has entered something which isn't one of the items in the list, then
        this will return -1.

        @see setSelectedItemIndex, getSelectedId, getText
    */
    int getSelectedItemIndex() const throw();

    /** Sets one of the items to be the current selection.

        This will set the ComboBox's text to that of the item at the given
        index in the list.

        @param newItemIndex             the new item to select
        @param dontSendChangeMessage    if set to true, this method won't trigger a
                                        change notification
        @see getSelectedItemIndex, setSelectedId, setText
    */
    void setSelectedItemIndex (int newItemIndex,
                               bool dontSendChangeMessage = false) throw();

    //==============================================================================
    /** Returns the text that is currently shown in the combo-box's text field.

        If the ComboBox has editable text, then this text may have been edited
        by the user; otherwise it will be one of the items from the list, or
        possibly an empty string if nothing was selected.

        @see setText, getSelectedId, getSelectedItemIndex
    */
    const String getText() const throw();

    /** Sets the contents of the combo-box's text field.

        The text passed-in will be set as the current text regardless of whether
        it is one of the items in the list. If the current text isn't one of the
        items, then getSelectedId() will return -1, otherwise it wil return
        the approriate ID.

        @param newText                  the text to select
        @param dontSendChangeMessage    if set to true, this method won't trigger a
                                        change notification
        @see getText
    */
    void setText (const String& newText,
                  bool dontSendChangeMessage = false) throw();

    /** Programmatically opens the text editor to allow the user to edit the current item.

        This is the same effect as when the box is clicked-on.
        @see Label::showEditor();
    */
    void showEditor();

    /** Pops up the combo box's list. */
    void showPopup();

    //==============================================================================
    /** Registers a listener that will be called when the box's content changes. */
    void addListener (ComboBoxListener* listener) throw();

    /** Deregisters a previously-registered listener. */
    void removeListener (ComboBoxListener* listener) throw();

    //==============================================================================
    /** Sets a message to display when there is no item currently selected.

        @see getTextWhenNothingSelected
    */
    void setTextWhenNothingSelected (const String& newMessage) throw();

    /** Returns the text that is shown when no item is selected.

        @see setTextWhenNothingSelected
    */
    const String getTextWhenNothingSelected() const throw();


    /** Sets the message to show when there are no items in the list, and the user clicks
        on the drop-down box.

        By default it just says "no choices", but this lets you change it to something more
        meaningful.
    */
    void setTextWhenNoChoicesAvailable (const String& newMessage) throw();

    /** Returns the text shown when no items have been added to the list.
        @see setTextWhenNoChoicesAvailable
    */
    const String getTextWhenNoChoicesAvailable() const throw();

    //==============================================================================
    /** Gives the ComboBox a tooltip. */
    void setTooltip (const String& newTooltip);


    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the combo box.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        To change the colours of the menu that pops up

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId  = 0x1000b00,    /**< The background colour to fill the box with. */
        textColourId        = 0x1000a00,    /**< The colour for the text in the box. */
        outlineColourId     = 0x1000c00,    /**< The colour for an outline around the box. */
        buttonColourId      = 0x1000d00,    /**< The base colour for the button (a LookAndFeel class will probably use variations on this). */
        arrowColourId       = 0x1000e00,    /**< The colour for the arrow shape that pops up the menu */
    };

    //==============================================================================
    /** @internal */
    void labelTextChanged (Label*);
    /** @internal */
    void enablementChanged();
    /** @internal */
    void colourChanged();
    /** @internal */
    void focusGained (Component::FocusChangeType cause);
    /** @internal */
    void focusLost (Component::FocusChangeType cause);
    /** @internal */
    void handleAsyncUpdate();
    /** @internal */
    const String getTooltip()                                       { return label->getTooltip(); }
    /** @internal */
    void mouseDown (const MouseEvent&);
    /** @internal */
    void mouseDrag (const MouseEvent&);
    /** @internal */
    void mouseUp (const MouseEvent&);
    /** @internal */
    void lookAndFeelChanged();
    /** @internal */
    void paint (Graphics&);
    /** @internal */
    void resized();
    /** @internal */
    bool keyStateChanged (bool isKeyDown);
    /** @internal */
    bool keyPressed (const KeyPress&);
    /** @internal */
    void valueChanged (Value&);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    struct ItemInfo
    {
        String name;
        int itemId;
        bool isEnabled : 1, isHeading : 1;

        bool isSeparator() const throw();
        bool isRealItem() const throw();
    };

    class Callback;
    friend class Callback;

    OwnedArray <ItemInfo> items;
    Value currentId;
    int lastCurrentId;
    bool isButtonDown, separatorPending, menuActive, textIsCustom;
    ListenerList <ComboBoxListener> listeners;
    ScopedPointer<Label> label;
    String textWhenNothingSelected, noChoicesMessage;

    ItemInfo* getItemForId (int itemId) const throw();
    ItemInfo* getItemForIndex (int index) const throw();

    ComboBox (const ComboBox&);
    ComboBox& operator= (const ComboBox&);
};

#endif   // __JUCE_COMBOBOX_JUCEHEADER__
