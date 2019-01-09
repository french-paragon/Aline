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
	/*!
	 * \brief installQmlEditorFactory install a qml editor factory
	 * \param shadowTypeEditor the shadow type
	 * \param shadowNameEditor the shadow name
	 * \param qmlSource the qml source
	 * \param editableTypes the editable types
	 * The manager will instance its own QmlEditorFactory based on the given parameters. Later, it might also store more information, if, for example, a distance program needs to acess the qml source.
	 */
	void installQmlEditorFactory(QString const& shadowTypeEditor,
								 QString const& shadowNameEditor,
								 QString const& qmlSource,
								 QStringList const& editableTypes);

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
