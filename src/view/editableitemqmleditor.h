#ifndef ALINE_EDITABLEITEMQMLEDITOR_H
#define ALINE_EDITABLEITEMQMLEDITOR_H

#include "editableitemeditor.h"

namespace Aline {

namespace Ui {
class EditableItemQmlEditor;
}

class EditableItemQmlEditorProxy;
class QmlEditorFactory;

class EditableItemQmlEditor : public EditableItemEditor
{
	Q_OBJECT

public:

	static const QString GENERIC_QMLBASED_EDITOR_TYPE;

	friend class QmlEditorFactory;

	explicit EditableItemQmlEditor(QWidget *parent = nullptr);
	explicit EditableItemQmlEditor(QUrl const& source, QWidget *parent = nullptr);
	~EditableItemQmlEditor();

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual QStringList editableTypes() const;

	void setQmlSource(QUrl const& source);

private:
	Ui::EditableItemQmlEditor *ui;

protected:

	virtual bool effectivelySetEditedItem(EditableItem* item);

	EditableItemQmlEditorProxy* _proxy;

	QString _shadowEditorType;
	QString _shadowEditorName;
	QStringList _editableTypes;

private:

	static int registerEditableItemCode;
};

class EditableItemQmlEditorProxy : public QObject {

	Q_OBJECT

public:
	explicit EditableItemQmlEditorProxy(EditableItemQmlEditor* parent);
	~EditableItemQmlEditorProxy();

	Q_PROPERTY(Aline::EditableItem* editedItem READ editedItem NOTIFY editedItemChanged)

	bool setEditedItem(EditableItem *editedItem);

	EditableItem* editedItem() const;

signals:

	void editedItemChanged();

protected:

	EditableItem* _editedItem;

};


} // namespace Aline
#endif // ALINE_EDITABLEITEMQMLEDITOR_H
