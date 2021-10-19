#pragma once

#include <QMainWindow>
#include "ui_FileNewWindow.h"

class FileNewWindow : public QMainWindow
{
	Q_OBJECT

public:
	FileNewWindow(QWidget *parent = Q_NULLPTR);
	~FileNewWindow();

private:
	Ui::FileNewWindow ui;
};
