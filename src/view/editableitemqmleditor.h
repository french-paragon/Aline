#ifndef ALINE_EDITABLEITEMQMLEDITOR_H
#define ALINE_EDITABLEITEMQMLEDITOR_H

#include "editableitemeditor.h"

namespace Aline {

namespace Ui {
class EditableItemQmlEditor;
}

class EditableItemQmlEditorProxy;

class EditableItemQmlEditor : public EditableItemEditor
{
	Q_OBJECT

public:
	explicit EditableItemQmlEditor(QWidget *parent = nullptr);
	explicit EditableItemQmlEditor(QUrl const& source, QWidget *parent = nullptr);
	~EditableItemQmlEditor();

	void setQmlSource(QUrl const& source);

private:
	Ui::EditableItemQmlEditor *ui;

protected:

	virtual bool effectivelySetEditedItem(EditableItem* item);

	EditableItemQmlEditorProxy* _proxy;
};

class EditableItemQmlEditorProxy : public QObject {

	Q_OBJECT

public:
	explicit EditableItemQmlEditorProxy(EditableItemQmlEditor* parent);
	~EditableItemQmlEditorProxy();

	Q_PROPERTY(QObject* editedItem READ editedItem NOTIFY editedItemChanged)

	bool setEditedItem(EditableItem *editedItem);

	QObject* editedItem() const;

signals:

	void editedItemChanged();

protected:

	EditableItem* _editedItem;

};


} // namespace Aline
#endif // ALINE_EDITABLEITEMQMLEDITOR_H
