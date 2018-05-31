#ifndef EDITORFACTORYMANAGER_H
#define EDITORFACTORYMANAGER_H

#include "../aline_global.h"

#include <QAbstractListModel>

namespace Aline {

class Editor;
class EditorFactory;
class EditableItem;

class ALINE_EXPORT EditorFactoryManager : public QAbstractListModel
{
	Q_OBJECT
public:

	enum InternalDataRole{
		ItemRefRole = Qt::UserRole + 1
	};

	static EditorFactoryManager GlobalEditorFactoryManager;

	explicit EditorFactoryManager(QObject *parent = nullptr);

	void installFactory(EditorFactory* factory, bool takeOwnership = true);

	bool hasFactoryInstalled(QString type_id) const;
	bool hasFactoryInstalledForItem(QString type_id) const;
	Editor* createItem(QString type_id, QWidget* parent) const;
	Editor* createItemForEditableItem(EditableItem* item, QWidget* parent) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:

	QMap<QString, QString> _editableTypes;
	QMap<QString, EditorFactory*> _installedFactories;
	QVector<QString> _installedFactoriesKeys;
};

} // namespace Aline

#endif // EDITORFACTORYMANAGER_H
