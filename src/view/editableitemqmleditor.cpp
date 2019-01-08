#include "editableitemqmleditor.h"
#include "ui_editableitemqmleditor.h"

#include "model/editableitem.h"
#include <QtQml/QQmlContext>

namespace Aline {

EditableItemQmlEditor::EditableItemQmlEditor(QWidget *parent) :
	EditableItemEditor(parent),
	ui(new Ui::EditableItemQmlEditor)
{
	ui->setupUi(this);

	_proxy = new EditableItemQmlEditorProxy(this);

	ui->quickWidget->rootContext()->setContextProperty("proxy", _proxy);
}

EditableItemQmlEditor::EditableItemQmlEditor(QUrl const& source, QWidget *parent) :
	EditableItemQmlEditor(parent)
{
	setQmlSource(source);
}

EditableItemQmlEditor::~EditableItemQmlEditor()
{
	delete ui;
}

void EditableItemQmlEditor::setQmlSource(const QUrl &source) {
	ui->quickWidget->setSource(source);
}

bool EditableItemQmlEditor::effectivelySetEditedItem(EditableItem* item) {

	return _proxy->setEditedItem(item);

}


EditableItemQmlEditorProxy::EditableItemQmlEditorProxy(EditableItemQmlEditor* parent) :
	QObject(parent),
	_editedItem(nullptr)
{

}

EditableItemQmlEditorProxy::~EditableItemQmlEditorProxy() {

}

bool EditableItemQmlEditorProxy::setEditedItem(EditableItem *editedItem)
{
	if (editedItem != _editedItem) {
		_editedItem = editedItem;
		emit editedItemChanged();
		//TODO: look if we want to react from qml, this should come here.
		return true;
	}

	return false;
}

QObject *EditableItemQmlEditorProxy::editedItem() const
{
	return qobject_cast<QObject*>(_editedItem);
}

} // namespace Aline
