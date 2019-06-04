#ifndef ALINE_QMLEDITORFACTORY_H
#define ALINE_QMLEDITORFACTORY_H

#include "editorfactory.h"

namespace Aline {

class QmlEditorFactory : public EditorFactory
{
	Q_OBJECT
public:
	explicit QmlEditorFactory(QString const& shadowTypeEditor,
							  QString const& shadowNameEditor,
							  QString const& qmlSource,
							  QStringList const& editableTypes,
							  QObject *parent = nullptr);

	virtual Editor* createItem(QWidget* parent) const;

Q_SIGNALS:

public Q_SLOTS:

protected:

	QString _shadowType;
	QString _shadowName;

	QString _qmlSource;
};

} // namespace Aline

#endif // ALINE_QMLEDITORFACTORY_H
