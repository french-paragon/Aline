#ifndef ALINE_EDITABLEITEMMANAGER_H
#define ALINE_EDITABLEITEMMANAGER_H

#include <QAbstractItemModel>

#include "../aline_global.h"

namespace Aline {

class EditableItem;
class EditableItemFactoryManager;
class LabelsTree;
class EditorFactoryManager;

class ALINE_EXPORT EditableItemManager : public QAbstractItemModel
{
	Q_OBJECT
public:

	static const QChar RefSeparator;
	static const QString RefRoot;

	static const QString RefMimeType;

	enum InternalDataRole{
		ItemRefRole = Qt::UserRole + 1,
		ItemAcceptChildrenRole = Qt::UserRole + 2,
		ItemTypeRefRole = Qt::UserRole + 3,
		EditableItemManagerUserRole = Qt::UserRole + 4,
	};

	/*!
	 * \brief The treeStruct struct allow for the EditableItemManager to store the structure of loadable items in tree.
	 */
	struct treeStruct {
		QString _ref;
		QString _name;
		QString _type_ref;
		bool _acceptChildrens;
	};

	struct loadedItem {
		treeStruct* _treeStruct;
		EditableItem* _item;
	};

	explicit EditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const = 0;

	QModelIndex indexFromRef(QString const& ref) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

	EditableItem* loadItem(QString const& ref);
	QStringList loadedItems(QString const& type = "") const;
	QStringList loadedItemsTypes() const;
	void forceUnloadItem(QString ref);
	bool isItemLoaded(const QString &ref) const;
	bool containItem(const QString & ref) const;

	bool createItem(QString typeRef, QString ref);
	bool createItem(QString typeRef, QString ref, QString* createdItemRef);
	bool clearItem(QString itemRef);
	bool clearItems(QStringList itemRefs);

	virtual bool saveItem(QString ref);
	virtual bool saveAll();
	virtual bool saveStruct() = 0;
	virtual bool saveLabels() = 0;
	virtual bool loadStruct() = 0;

	EditableItemFactoryManager *factoryManager() const;

	bool makeRefUniq(QString &ref) const;

	/*!
	 * \brief isNetworkExtended indicate if the manager is shared on some network
	 * \return true if it's shared. false otherwise.
	 *
	 * Some item may have to refer to external datas, usually local files,
	 * by knowing their datas will have to be shared along with they might propose a special mechanism
	 * to store those files using some serialization mechanism of the item manager (yet to be implemented).
	 */
	virtual bool isNetworkShared() const = 0;

	virtual bool hasDistantFile(QString fileName);
	/*!
	 * \brief loadFile get a distant file from the manager
	 * \param fileName the distant file name
	 * \return the local path of the loaded file.
	 */
	virtual QString loadFile(QString fileName);
	virtual bool storeProjectFile(QString localFile);

	EditableItem* activeItem() const;

	LabelsTree* labelsTree();

	void setFactoryManager(EditableItemFactoryManager *factoryManager, bool takeOwnership = false);

	/*!
	 * \brief setEditorManager indicate to the model that a gui might want to edit the items and will want to use this editor factory manager.
	 * \param editorManager the factory manager.
	 */
	void setEditorManager(EditorFactoryManager *editorManager);

	Q_INVOKABLE QAbstractItemModel* getSubTreeFromItemType(QString typeRef, QObject* modelParent = nullptr);

	virtual QHash<int, QByteArray> roleNames() const;

Q_SIGNALS:

	void loadingItemSucessFull(QString ref);

	void itemAboutToBeUnloaded(QString ref);
	void itemUnloaded(QString ref);

	void activeItemChanged();

public Q_SLOTS:

	virtual void reset() = 0;
	virtual void closeAll();

	void setActiveItem(QString ref);

protected:

	void cleanTreeStruct();

	QModelIndex indexFromLeaf(treeStruct* leaf) const;
	QModelIndex indexFromType(QString typeRef) const;

	void itemVisibleStateChanged(QString ref);

	/*!
	 * \brief effectivelyLoadItem load an item without checking if it is cached before.
	 * \param ref the ref of the item to load.
	 */
	virtual EditableItem* effectivelyLoadItem(QString const& ref) = 0;

	virtual bool clearItemData(QString itemRef) = 0;

	virtual bool effectivelySaveItem(QString const& ref) = 0;

	/*!
	 * \brief insertItem insert an item in the manager, the manager take ownership of the item.
	 * \param item the item to insert
	 */
	virtual bool insertItem(EditableItem* item);

	/*!
	 * \brief effectivelyLoadLabels load the labels from the datasource.
	 */
	virtual void effectivelyLoadLabels() = 0;

	LabelsTree* _labels;

	EditableItem* _activeItem;

	QMap<QString, treeStruct*> _treeIndex; //build an index of the tree.
	QMap<QString, QVector<treeStruct*>> _itemsByTypes;
	QMap<QString, loadedItem> _loadedItems;

	EditableItemFactoryManager* _factoryManager;
	EditorFactoryManager* _editorManager;
};

} // namespace Aline

#endif // ALINE_EDITABLEITEMMANAGER_H
