#ifndef APP_H
#define APP_H

#include "aline_global.h"

#include <QApplication>

namespace Aline {

class ALINE_EXPORT App : public QApplication
{
public:
	App(int argc, char** argv);

	virtual bool start(QString appCode = "default");

protected:

	QString _appCode;
};

} // namespace Aline

#endif // APP_H
