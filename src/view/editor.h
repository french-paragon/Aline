#ifndef EDITOR_H
#define EDITOR_H

/*This file is part of the project Aline

Copyright (C) 2022 Paragon <french.paragon@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../aline_global.h"

#include <QWidget>
#include <QString>

#include <functional>

namespace Aline {

class MainWindow;
class EditableItem;

class ALINE_EXPORT Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Editor(QWidget *parent = nullptr);

	QString title() const;

	/*!
	 * \brief getTypeId return a string id for the editor type.
	 * \return the editor type id.
	 */
	virtual QString getTypeId() const = 0;

	/*!
	 * \brief getTypeName allow to get the name of the editor item type as a string.
	 * \return the name
	 */
	virtual QString getTypeName() const = 0;

	/*!
	 * \brief editableType allow to get the types the editor might support
	 * \return the type id string, or empty string if none is supported.
	 */
	virtual QStringList editableTypes() const;

	/*!
	 * \brief setEditedItem set the item to edit (by default will do nothing, as generally speaking editor might not support editable item edition).
	 * \param item the item to edit
	 */
	virtual void setEditedItem(EditableItem* item);

	/*!
	 * \brief getEditorMainWindow search the editor parents hierarchy to find the main windows
	 * \return the main windows, or nullptr if none could be found.
	 */
	MainWindow* getEditorMainWindow() const;

	/*!
	 * \brief getContextActions build actions that are currently available for the editor
	 * \return a list of actions
	 *
	 * Editor can have a contextual list of actions, presented as a toolbar.
	 * To do so, they need to override this function (default implementation return an empty list).
	 */
	virtual QList<QAction*> getContextActions();

	/*!
	 * \brief isStateRestorable indicate if the editor can be restored next time the project is opened
	 * \return true if the editor state can be restored, false otherwise (default implementation does nothing).
	 */
	virtual bool isStateRestorable() const;

	/*!
	 * \brief restoreFromState attempt to restore the editor from a state representation
	 * \param stateRep the state representation as a string
	 * \return true if the state could be restored, false otherwise.
	 */
	virtual bool restoreFromState(QString const& stateRep);

	/*!
	 * \brief getEditorState give the editor state
	 * \return a state which can be used to restore the editor
	 */
	virtual QString getEditorState() const;

	/*!
	 * \brief getEditorContentClue gives a clue about the content of the editor ot avoid opening duplicated editors
	 * \return the clue as a string
	 *
	 * when trying to open an editor, if the windows find two editors with the same clue and the same type,
	 * it will instead switch to the previously opened editor rather than adding a new one.
	 *
	 * default implementation return nothing, so no two editors of the same type can be open at the same time.
	 */
	virtual QString getEditorNoDuplicateClue() const;

Q_SIGNALS:

	void titleChanged(Aline::Editor* himself, QString title);
	void contextActionListChanged();

public Q_SLOTS:

	void setTitle(QString title);
	void setHasUnsavedChanges(bool saveState);

	virtual void saveAction() = 0;

protected:

	void updateEffectiveTitle();

	QString _title;
	bool _saveState;
};

/*!
 * \brief The EditorPersistentStateSaveInterface class is an app interface to restore editors states per projects
 */
class ALINE_EXPORT EditorPersistentStateSaveInterface : public QObject {
	Q_OBJECT
public:

	static const char* EditorPersistentStateSaveInterfaceCode;

	EditorPersistentStateSaveInterface(QObject *parent = nullptr);

	/*!
	 * \brief saveEditorStates save the state of the editors for the current project in the windows
	 * \param mw the windows to treate
	 */
	void saveEditorStates(MainWindow* mw);

	/*!
	 * \brief restoreEditorStates retore the editors for the active project
	 * \param mw the window to treate
	 */
	void restoreEditorStates(MainWindow* mw);

protected:

};

} // namespace Aline

#endif // EDITOR_H
