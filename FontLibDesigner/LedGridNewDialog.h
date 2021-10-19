#pragma once

#include <QDialog>
#include "ui_LedGridNewDialog.h"

class LedGridNewDialog : public QDialog
{
	Q_OBJECT
		Q_PROPERTY(QSize pixelSize READ pixelSize WRITE setPixelSize)
public:
	LedGridNewDialog(QWidget *parent = Q_NULLPTR);
	
	QSize pixelSize() const;

public slots:
	void setHorizontalPixelCount(int);
	void setVerticalPixelCount(int);
	void setPixelSize(QSize);

protected:
	QSize m_pixelSize{ 4, 8 };

private:
	Ui::LedGridNewDialog ui;
};
