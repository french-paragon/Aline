#ifndef EDITORFACTORY_H
#define EDITORFACTORY_H

#include "aline_global.h"

#include <QObject>
#include <QAbstractListModel>

namespace Aline {

class Editor;

class ALINE_EXPORT EditorFactory : public QObject
{
	Q_OBJECT
public:
	explicit EditorFactory(QObject *parent = nullptr);

	QString getItemTypeId() const;
	QString getItemTypeName() const;
	QStringList editableTypes() const;
	virtual Editor* createItem(QWidget* parent) const = 0;

signals:

public slots:

protected:

	mutable QString _typeId;
	mutable QString _typeName;
	mutable bool _hasEditableTypesCached;
	mutable QStringList _editableTypes;

};

} // namespace Aline

#endif // EDITORFACTORY_H
