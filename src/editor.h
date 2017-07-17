#ifndef EDITOR_H
#define EDITOR_H

#include "aline_global.h"

#include <QWidget>
#include <QString>

namespace Aline {

class MainWindow;

class ALINE_EXPORT Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Editor(QWidget *parent = nullptr);

	QString title() const;

signals:

	void titleChanged(Editor* himself, QString title);

public slots:

	void setTitle(QString title);

protected:

	QString _title;
};

} // namespace Aline

#endif // EDITOR_H
