#pragma once

#include <QtWidgets/QMainWindow>
#include "LedGridNewDialog.h"
#include "ui_FontLibDesigner.h"

class FontLibDesigner : public QMainWindow
{
	Q_OBJECT

public:
	FontLibDesigner(QWidget* parent = Q_NULLPTR);

public slots:
	void updateCodeBrowser();

protected:
	QTimer codeBrowserUpdateTimer{ this };
	void doUpdateCodeBrowser() const;

	static QString fmtColorMap(const QVector<QRgb>& colors, QSize size, QString innerJoin, QString outerJoin,
		QString prefix, QString suffix, std::function<QString(QRgb)> fmtColorFunc);

protected:
	LedGridNewDialog d{ this };

private:
	Ui::FontLibDesignerClass ui{};
};
