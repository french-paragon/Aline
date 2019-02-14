#include "editableitemqmleditor.h"
#include "ui_editableitemqmleditor.h"

#include "model/editableitem.h"
#include "model/editableitemmanager.h"
#include <QtQml/QQmlContext>
#include <QQmlEngine>

namespace Aline {

int EditableItemQmlEditor::registerEditableItemCode = qRegisterMetaType<Aline::EditableItem*>("Aline::EditableItem*") + qmlRegisterType<Aline::EditableItem>() + qRegisterMetaType<Aline::EditableItemManager*>("Aline::EditableItemManager*") + qmlRegisterType<Aline::EditableItemManager>();

const QString EditableItemQmlEditor::GENERIC_QMLBASED_EDITOR_TYPE = "aline_qml_generic";

EditableItemQmlEditor::EditableItemQmlEditor(QWidget *parent) :
	EditableItemEditor(parent),
	ui(new Ui::EditableItemQmlEditor),
	_shadowEditorType(GENERIC_QMLBASED_EDITOR_TYPE),
	_shadowEditorName(tr("QML based generic editor"))
{
	ui->setupUi(this);

	_proxy = new EditableItemQmlEditorProxy(this);

	ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
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

QString EditableItemQmlEditor::getTypeId() const {
	return _shadowEditorType;
}
QString EditableItemQmlEditor::getTypeName() const {
	return _shadowEditorName;
}

QStringList EditableItemQmlEditor::editableTypes() const {
	return _editableTypes;
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

EditableItem *EditableItemQmlEditorProxy::editedItem() const
{
	return _editedItem;
}

} // namespace Aline
